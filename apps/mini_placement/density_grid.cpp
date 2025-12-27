/**
 * @file density_grid.cpp
 * @brief Density Grid Implementation for Electrostatic Placement
 */

#include "density_grid.h"
#include "../../lib/include/placer_db.h"
#include <iomanip>
#include <numeric>

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

DensityGrid::DensityGrid() {
    // Empty constructor - initialization done in init()
}

// ============================================================================
// Grid Initialization
// ============================================================================

bool DensityGrid::init(double core_w, double core_h, double core_x_min, double core_y_min, double bin_dim) {
    if (core_w <= 0.0 || core_h <= 0.0 || bin_dim <= 0.0) {
        std::cerr << "Error: Invalid dimensions for density grid initialization" << std::endl;
        return false;
    }

    // Store core area parameters
    core_w_ = core_w;
    core_h_ = core_h;
    core_x_min_ = core_x_min;
    core_y_min_ = core_y_min;

    // Calculate bin dimensions
    bin_w_ = bin_dim;
    bin_h_ = bin_dim;

    // Calculate number of bins (ensure at least 1x1)
    num_bins_x_ = static_cast<int>(std::ceil(core_w_ / bin_w_));
    num_bins_y_ = static_cast<int>(std::ceil(core_h_ / bin_h_));

    // Initialize bins array
    bins_.resize(num_bins_x_ * num_bins_y_);

    // Initialize each bin
    for (int y = 0; y < num_bins_y_; ++y) {
        for (int x = 0; x < num_bins_x_; ++x) {
            int idx = y * num_bins_x_ + x;
            Bin& bin = bins_[idx];
            
            // Set bin center coordinates
            bin.x = core_x_min_ + (x + 0.5) * bin_w_;
            bin.y = core_y_min_ + (y + 0.5) * bin_h_;
            
            // Initialize other fields
            bin.density = 0.0;
            bin.electro_potential = 0.0;
            bin.electro_gradient_x = 0.0;
            bin.electro_gradient_y = 0.0;
        }
    }

    initialized_ = true;

    debugLog("Density grid initialized: " + std::to_string(num_bins_x_) + "x" + 
             std::to_string(num_bins_y_) + " bins");
    debugLog("Bin dimensions: " + std::to_string(bin_w_) + "x" + std::to_string(bin_h_));
    debugLog("Core area: (" + std::to_string(core_x_min_) + "," + std::to_string(core_y_min_) + 
             ") to (" + std::to_string(core_x_min_ + core_w_) + "," + 
             std::to_string(core_y_min_ + core_h_) + ")");

    return true;
}

// ============================================================================
// Core Density Update Algorithm
// ============================================================================

void DensityGrid::updateDensity(const std::vector<Cell*>& cells, PlacerDB* placer_db) {
    if (!initialized_) {
        std::cerr << "Error: Density grid not initialized" << std::endl;
        return;
    }
    
    if (!placer_db) {
        std::cerr << "Error: PlacerDB is required for density calculation" << std::endl;
        return;
    }

    // Step 1: Clear all bin densities
    for (auto& bin : bins_) {
        bin.density = 0.0;
    }

    double bin_area = bin_w_ * bin_h_;

    // Step 2: Project each cell onto bins it overlaps with
    for (const Cell* cell : cells) {
        if (!cell) continue;

        // Get cell physical dimensions from PlacerDB
        double cell_x = cell->getX();
        double cell_y = cell->getY();
        const auto& cell_info = placer_db->getCellInfo(const_cast<Cell*>(cell));
        double cell_w = cell_info.width;
        double cell_h = cell_info.height;

        // Calculate bin range that this cell overlaps with
        int start_x = std::max(0, static_cast<int>((cell_x - core_x_min_) / bin_w_));
        int end_x   = std::min(num_bins_x_ - 1, static_cast<int>((cell_x + cell_w - core_x_min_) / bin_w_));
        int start_y = std::max(0, static_cast<int>((cell_y - core_y_min_) / bin_h_));
        int end_y   = std::min(num_bins_y_ - 1, static_cast<int>((cell_y + cell_h - core_y_min_) / bin_h_));

        // Step 3: For each overlapped bin, calculate exact overlap area
        for (int bin_x = start_x; bin_x <= end_x; ++bin_x) {
            for (int bin_y = start_y; bin_y <= end_y; ++bin_y) {
                double overlap_area = calculateCellBinOverlap(cell, bin_x, bin_y, placer_db);
                
                // Add overlap area to bin density (as raw area for now)
                int idx = bin_y * num_bins_x_ + bin_x;
                bins_[idx].density += overlap_area;
            }
        }
    }

    // Step 4: Normalize densities (convert area to density ratio)
    for (auto& bin : bins_) {
        bin.density /= bin_area;
    }

    debugLog("Density updated for " + std::to_string(cells.size()) + " cells");
}

// ============================================================================
// Bin Access Methods
// ============================================================================

Bin& DensityGrid::getBin(int grid_x, int grid_y) {
    if (grid_x < 0 || grid_x >= num_bins_x_ || grid_y < 0 || grid_y >= num_bins_y_) {
        throw std::out_of_range("Bin index out of bounds");
    }
    return bins_[grid_y * num_bins_x_ + grid_x];
}

const Bin& DensityGrid::getBin(int grid_x, int grid_y) const {
    if (grid_x < 0 || grid_x >= num_bins_x_ || grid_y < 0 || grid_y >= num_bins_y_) {
        throw std::out_of_range("Bin index out of bounds");
    }
    return bins_[grid_y * num_bins_x_ + grid_x];
}

std::pair<int, int> DensityGrid::getBinIndex(double x, double y) const {
    // Clamp to core area bounds
    x = std::max(core_x_min_, std::min(core_x_min_ + core_w_, x));
    y = std::max(core_y_min_, std::min(core_y_min_ + core_h_, y));

    // Calculate grid indices
    int grid_x = static_cast<int>((x - core_x_min_) / bin_w_);
    int grid_y = static_cast<int>((y - core_y_min_) / bin_h_);

    // Clamp to valid range
    grid_x = std::max(0, std::min(num_bins_x_ - 1, grid_x));
    grid_y = std::max(0, std::min(num_bins_y_ - 1, grid_y));

    return {grid_x, grid_y};
}

Bin& DensityGrid::getBinByCoord(double x, double y) {
    auto [grid_x, grid_y] = getBinIndex(x, y);
    return getBin(grid_x, grid_y);
}

const Bin& DensityGrid::getBinByCoord(double x, double y) const {
    auto [grid_x, grid_y] = getBinIndex(x, y);
    return getBin(grid_x, grid_y);
}

// ============================================================================
// Private Helper Methods
// ============================================================================

double DensityGrid::calculateCellBinOverlap(const Cell* cell, int bin_x, int bin_y, PlacerDB* placer_db) const {
    if (!cell || !placer_db) return 0.0;

    // Get cell boundaries
    double cell_x1 = cell->getX();
    double cell_y1 = cell->getY();
    const auto& cell_info = placer_db->getCellInfo(const_cast<Cell*>(cell));
    double cell_x2 = cell_x1 + cell_info.width;
    double cell_y2 = cell_y1 + cell_info.height;

    // Get bin boundaries
    double bin_x1, bin_x2, bin_y1, bin_y2;
    getBinBounds(bin_x, bin_y, bin_x1, bin_x2, bin_y1, bin_y2);

    // Calculate overlap rectangle
    double overlap_x1 = std::max(cell_x1, bin_x1);
    double overlap_y1 = std::max(cell_y1, bin_y1);
    double overlap_x2 = std::min(cell_x2, bin_x2);
    double overlap_y2 = std::min(cell_y2, bin_y2);

    // Calculate overlap area
    double overlap_w = std::max(0.0, overlap_x2 - overlap_x1);
    double overlap_h = std::max(0.0, overlap_y2 - overlap_y1);

    return overlap_w * overlap_h;
}

void DensityGrid::getBinBounds(int bin_x, int bin_y, 
                              double& x_min, double& x_max,
                              double& y_min, double& y_max) const {
    x_min = core_x_min_ + bin_x * bin_w_;
    x_max = x_min + bin_w_;
    y_min = core_y_min_ + bin_y * bin_h_;
    y_max = y_min + bin_h_;
}

// ============================================================================
// Statistics and Analysis
// ============================================================================

void DensityGrid::printDensityStats(double target_density) const {
    if (!initialized_) {
        std::cout << "Density grid not initialized" << std::endl;
        return;
    }

    double max_density = 0.0;
    double total_density = 0.0;
    int overcrowded_bins = 0;
    double total_overflow = 0.0;

    for (const auto& bin : bins_) {
        max_density = std::max(max_density, bin.density);
        total_density += bin.density;
        
        if (bin.isOvercrowded(target_density)) {
            overcrowded_bins++;
            total_overflow += bin.getOverflow(target_density);
        }
    }

    double avg_density = total_density / bins_.size();

    std::cout << "\n=== Density Grid Statistics ===" << std::endl;
    std::cout << "Grid size: " << num_bins_x_ << "x" << num_bins_y_ << " (" << bins_.size() << " bins)" << std::endl;
    std::cout << "Target density: " << target_density << std::endl;
    std::cout << "Average density: " << std::fixed << std::setprecision(3) << avg_density << std::endl;
    std::cout << "Maximum density: " << std::fixed << std::setprecision(3) << max_density << std::endl;
    std::cout << "Overcrowded bins: " << overcrowded_bins << " (" 
              << (100.0 * overcrowded_bins / bins_.size()) << "%)" << std::endl;
    std::cout << "Total overflow: " << std::fixed << std::setprecision(3) << total_overflow << std::endl;
    std::cout << "===============================" << std::endl;
}

double DensityGrid::getTotalOverflow(double target_density) const {
    if (!initialized_) return 0.0;

    double total_overflow = 0.0;
    for (const auto& bin : bins_) {
        total_overflow += bin.getOverflow(target_density);
    }
    return total_overflow;
}

double DensityGrid::getMaxDensity() const {
    if (!initialized_) return 0.0;

    double max_density = 0.0;
    for (const auto& bin : bins_) {
        max_density = std::max(max_density, bin.density);
    }
    return max_density;
}

double DensityGrid::getAverageDensity() const {
    if (!initialized_) return 0.0;

    double total_density = 0.0;
    for (const auto& bin : bins_) {
        total_density += bin.density;
    }
    return total_density / bins_.size();
}

// ============================================================================
// Export Functions
// ============================================================================

void DensityGrid::exportDensityMap(const std::string& filename) const {
    if (!initialized_) {
        std::cerr << "Error: Density grid not initialized" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        return;
    }

    // Write CSV header
    file << "x,y,density\n";

    // Write bin data
    for (const auto& bin : bins_) {
        file << bin.x << "," << bin.y << "," << bin.density << "\n";
    }

    file.close();
    debugLog("Density map exported to " + filename);
}

void DensityGrid::exportPotentialMap(const std::string& filename) const {
    if (!initialized_) {
        std::cerr << "Error: Density grid not initialized" << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        return;
    }

    // Write CSV header
    file << "x,y,potential\n";

    // Write bin data
    for (const auto& bin : bins_) {
        file << bin.x << "," << bin.y << "," << bin.electro_potential << "\n";
    }

    file.close();
    debugLog("Potential map exported to " + filename);
}

// ============================================================================
// Debug Helper
// ============================================================================

void DensityGrid::debugLog(const std::string& message) const {
    // In a real implementation, this could be controlled by a verbose flag
    // For now, just print the message
    std::cout << "[DensityGrid] " << message << std::endl;
}

bool DensityGrid::init(int grid_x, int grid_y, double core_w, double core_h, double core_x_min, double core_y_min) {
    if (grid_x <= 0 || grid_y <= 0 || core_w <= 0.0 || core_h <= 0.0) {
        std::cerr << "Error: Invalid dimensions for density grid initialization" << std::endl;
        return false;
    }

    // Store core area parameters
    core_w_ = core_w;
    core_h_ = core_h;
    core_x_min_ = core_x_min;
    core_y_min_ = core_y_min;

    // Set exact grid dimensions
    num_bins_x_ = grid_x;
    num_bins_y_ = grid_y;

    // Calculate bin dimensions to fit the core area
    bin_w_ = core_w / num_bins_x_;
    bin_h_ = core_h / num_bins_y_;

    // Initialize bins array
    bins_.resize(num_bins_x_ * num_bins_y_);

    // Initialize each bin
    for (int y = 0; y < num_bins_y_; ++y) {
        for (int x = 0; x < num_bins_x_; ++x) {
            int idx = y * num_bins_x_ + x;
            Bin& bin = bins_[idx];
            
            // Set bin center coordinates
            bin.x = core_x_min_ + (x + 0.5) * bin_w_;
            bin.y = core_y_min_ + (y + 0.5) * bin_h_;
            
            // Initialize other fields
            bin.density = 0.0;
            bin.electro_potential = 0.0;
        }
    }

    initialized_ = true;
    debugLog("Density grid initialized: " + std::to_string(num_bins_x_) + "x" + std::to_string(num_bins_y_) + " bins");
    debugLog("Bin dimensions: " + std::to_string(bin_w_) + "x" + std::to_string(bin_h_));
    debugLog("Core area: (" + std::to_string(core_x_min_) + "," + std::to_string(core_y_min_) + ") to (" + 
              std::to_string(core_x_min_ + core_w_) + "," + std::to_string(core_y_min_ + core_h_) + ")");
    
    return true;
}

} // namespace mini