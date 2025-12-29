/**
 * @file detailed_placer.cpp
 * @brief Detailed Placement Algorithm Implementation
 */

#include "detailed_placer.h"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <map>

namespace mini {

DetailedPlacer::DetailedPlacer(PlacerDB* db, NetlistDB* netlist)
    : db_(db), netlist_(netlist), verbose_(false), iterations_(3), final_hpwl_(0.0) {
}

void DetailedPlacer::run() {
    debugLog("Starting detailed placement optimization...");
    
    // Calculate initial HPWL
// HPWLCalculator hpwl_calc;
    initial_hpwl_ = HPWLCalculator::calculateHPWL(netlist_, db_);
    
    if (verbose_) {
        std::cout << "  Initial HPWL: " << initial_hpwl_ << std::endl;
        std::cout << "  Running " << iterations_ << " iterations of detailed reordering..." << std::endl;
    }
    
    // Strategy 1: Global swap for long-distance optimization
    // globalSwap();  // Optional: uncomment for global optimization
    
    // Strategy 2: Detailed reordering using sliding window
    for (int iter = 0; iter < iterations_; ++iter) {
        if (verbose_) {
            std::cout << "  Iteration " << (iter + 1) << "/" << iterations_ << std::endl;
        }
        detailedReordering();
    }
    
    // Calculate final HPWL
    final_hpwl_ = HPWLCalculator::calculateHPWL(netlist_, db_);
    
    // Verify site alignment after optimization
    auto [aligned_count, total_count] = verifySiteAlignment();
    double alignment_rate = (total_count > 0) ? (double(aligned_count) / total_count * 100) : 100.0;
    
    if (verbose_) {
        std::cout << "  Final HPWL: " << final_hpwl_ << std::endl;
        double improvement = ((initial_hpwl_ - final_hpwl_) / initial_hpwl_) * 100;
        std::cout << "  HPWL improvement: " << std::fixed << std::setprecision(2) 
                  << improvement << "%" << std::endl;
        std::cout << "  Site alignment: " << aligned_count << "/" << total_count 
                  << " (" << std::fixed << std::setprecision(1) << alignment_rate << "%)" << std::endl;
        // Reset cout precision to avoid affecting subsequent output
        std::cout.unsetf(std::ios::fixed);
        std::cout.precision(6);
    }
    
    debugLog("Detailed placement optimization completed");
    
    // [DEBUG] Export detailed placement result for comparison
    exportResult("detailed_placement.csv");
}

void DetailedPlacer::exportResult(const std::string& filename) const {
    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        return;
    }
    
    // CSV header
    csv_file << "cell_name,x,y,width,height,fixed,algorithm\n";
    
    // Export all cells
    auto all_cells = db_->getAllCells();
    for (Cell* cell : all_cells) {
        const auto& info = db_->getCellInfo(cell);
        csv_file << cell->getName() << ","
                 << info.x << ","
                 << info.y << ","
                 << info.width << ","
                 << info.height << ","
                 << (info.fixed ? "1" : "0") << ","
                 << "Detailed" << "\n";
    }
    
    csv_file.close();
    debugLog("Detailed placement result exported to " + filename);
}

void DetailedPlacer::globalSwap() {
    debugLog("Running global swap optimization...");
    
    auto all_cells = db_->getAllCells();
    int successful_swaps = 0;
    
    // Group cells by width for efficient equal-width swapping
    std::unordered_map<double, std::vector<Cell*>> width_groups;
    for (Cell* cell : all_cells) {
        double width = db_->getCellInfo(cell).width;
        width_groups[width].push_back(cell);
    }
    
    // Try swapping within each width group
    for (auto& [width, cells] : width_groups) {
        if (cells.size() < 2) continue;
        
        for (size_t i = 0; i < cells.size(); ++i) {
            for (size_t j = i + 1; j < cells.size(); ++j) {
                if (trySwap(cells[i], cells[j])) {
                    successful_swaps++;
                }
            }
        }
    }
    
    if (verbose_) {
        std::cout << "  Global swap: " << successful_swaps << " successful swaps" << std::endl;
    }
}

bool DetailedPlacer::trySwap(Cell* c1, Cell* c2) {
    if (!hasEqualWidth(c1, c2)) return false;
    
    // Store original positions
    double c1_x = db_->getCellInfo(c1).x;
    double c1_y = db_->getCellInfo(c1).y;
    double c2_x = db_->getCellInfo(c2).x;
    double c2_y = db_->getCellInfo(c2).y;
    
    // Calculate HPWL before swap
    double hpwl_before = calculateLocalHPWL({c1, c2});
    
    // Perform swap
    db_->placeCell(c1, c2_x, c2_y);
    db_->placeCell(c2, c1_x, c1_y);
    
    // Calculate HPWL after swap
    double hpwl_after = calculateLocalHPWL({c1, c2});
    
    // Keep swap if HPWL improved
    if (hpwl_after < hpwl_before) {
        return true;
    } else {
        // Rollback swap
        db_->placeCell(c1, c1_x, c1_y);
        db_->placeCell(c2, c2_x, c2_y);
        return false;
    }
}

void DetailedPlacer::detailedReordering() {
    debugLog("Running detailed reordering with sliding windows...");
    
    auto rows = getCellsByRow();
    int total_improvements = 0;
    
    // Print row analysis for debugging
    if (verbose_) {
        std::cout << "  [DEBUG] Row analysis:" << std::endl;
        for (size_t i = 0; i < rows.size(); ++i) {
            std::cout << "    Row " << i << ": " << rows[i].size() << " cells (y=" 
                      << (rows[i].empty() ? 0.0 : db_->getCellInfo(rows[i][0]).y) << ")" << std::endl;
        }
    }
    
    for (auto& row_cells : rows) {
        if (row_cells.size() < 3) continue;  // Need at least 3 cells for sliding window
        
        double site_width = db_->getSiteWidth();
        
        // Sliding window: size 3
        for (size_t i = 0; i <= row_cells.size() - 3; ++i) {
            // Extract window cells
            std::vector<Cell*> window;
            window.push_back(row_cells[i]);
            window.push_back(row_cells[i + 1]);
            window.push_back(row_cells[i + 2]);
            
            // Get starting X coordinate (should be site-aligned)
            double start_x = db_->getCellInfo(window[0]).x;
            
            // Debug: Print window before optimization
            if (verbose_ && i < 3) {  // Only print first few windows
                std::cout << "  [DEBUG] Window " << i << ": ";
                for (size_t j = 0; j < window.size(); ++j) {
                    auto& info = db_->getCellInfo(window[j]);
                    std::cout << window[j]->getName() << "(" << info.x << ") ";
                }
                std::cout << "start_x=" << start_x << std::endl;
            }
            
            // Optimize this window
            if (optimizeWindow(window, start_x)) {
                total_improvements++;
                
                // Debug: Print window after optimization
                if (verbose_ && i < 3) {
                    std::cout << "  [DEBUG] After opt: ";
                    for (size_t j = 0; j < window.size(); ++j) {
                        auto& info = db_->getCellInfo(window[j]);
                        std::cout << window[j]->getName() << "(" << info.x << ") ";
                    }
                    std::cout << std::endl;
                }
                
                // Update row_cells with new order
                row_cells[i] = window[0];
                row_cells[i + 1] = window[1];
                row_cells[i + 2] = window[2];
            }
        }
    }
    
    if (verbose_) {
        std::cout << "  Detailed reordering: " << total_improvements << " improvements found" << std::endl;
    }
}

bool DetailedPlacer::optimizeWindow(std::vector<Cell*>& window_cells, double start_x) {
    if (window_cells.size() != 3) return false;
    
    // [FIX 1] Pre-sort to ensure physical order
    std::sort(window_cells.begin(), window_cells.end(), 
             [this](Cell* a, Cell* b) {
                 return db_->getCellInfo(a).x < db_->getCellInfo(b).x;
             });
    
    // [FIX 2] CRITICAL: Check contiguity before re-packing!
    // If there are gaps, re-packing will swallow gaps and cause overlaps
    if (!isContiguous(window_cells)) {
        debugLog("Skipping non-contiguous window to avoid gap collapse");
        return false;
    }
    
    // Calculate original HPWL
    double best_hpwl = calculateLocalHPWL(window_cells);
    std::vector<Cell*> best_order = window_cells;
    
    // Record original physical right boundary
    double original_end_x = db_->getCellInfo(window_cells.back()).x + 
                           db_->getCellInfo(window_cells.back()).width;
    
    // Store original coordinates for rollback
    std::vector<std::pair<double, double>> original_coords;
    for (Cell* cell : window_cells) {
        const auto& info = db_->getCellInfo(cell);
        original_coords.emplace_back(info.x, info.y);
    }
    
    // Generate all permutations of the window
    do {
        // Width conservation check
        double current_perm_width = 0;
        for(Cell* c : window_cells) current_perm_width += db_->getCellInfo(c).width;
        
        // Prevent floating point drift
        if (start_x + current_perm_width > original_end_x + 1e-9) {
            continue; // Skip if width changed
        }
        
// Safe to place - re-pack cells compactly from start_x
        rePackToSites(window_cells, start_x);
        
        // [CRITICAL] Post-placement boundary verification
        // Check actual positions after placement to prevent floating point drift
        double actual_end_x = db_->getCellInfo(window_cells.back()).x + 
                           db_->getCellInfo(window_cells.back()).width;
        
        // If actual placement exceeds boundary, skip this permutation
        if (actual_end_x > original_end_x + 1e-9) {
            continue; // Skip due to boundary violation
        }
        
        // Calculate new HPWL
        double new_hpwl = calculateLocalHPWL(window_cells);
        
        // Keep if improvement found
        if (new_hpwl < best_hpwl) {
            best_hpwl = new_hpwl;
            best_order = window_cells;
        }
        
    } while (std::next_permutation(window_cells.begin(), window_cells.end()));
    
    // Apply best arrangement with final boundary verification
    if (calculateLocalHPWL(best_order) < calculateLocalHPWL(window_cells)) {
        rePackToSites(best_order, start_x);
        
        // Final boundary check
        double final_end_x = db_->getCellInfo(best_order.back()).x + 
                           db_->getCellInfo(best_order.back()).width;
        
        if (final_end_x <= original_end_x + 1e-9) {
            window_cells = best_order;
            return true;
        }
    }
    
    // Restore original arrangement
    for (size_t i = 0; i < window_cells.size(); ++i) {
        db_->placeCell(window_cells[i], original_coords[i].first, original_coords[i].second);
    }
    
    return false;
}

void DetailedPlacer::rePackToSites(const std::vector<Cell*>& cells, double start_x) {
    // NEVER modify start_x - it's the window left boundary determined by previous cell
    // If we snap start_x, the entire window shifts right and overlaps with neighbors
    
    double current_x = start_x;
    
    for (Cell* cell : cells) {
        double cell_width = db_->getCellInfo(cell).width;
        double cell_y = db_->getCellInfo(cell).y;
        
        // Place cell directly without any snapping
        db_->placeCell(cell, current_x, cell_y);
        current_x += cell_width;
    }
}

bool DetailedPlacer::isSiteAligned(double x, double site_width) const {
    if (site_width <= 0.0) return false;
    
    // Use relative epsilon for robustness
    const double EPSILON = 1e-10;
    double quotient = x / site_width;
    double rounded_quotient = std::round(quotient);
    return std::abs(quotient - rounded_quotient) < EPSILON;
}

double DetailedPlacer::calculateLocalHPWL(const std::vector<Cell*>& cells) {
    if (cells.empty()) return 0.0;
    
    // For local optimization, we consider nets connected to these cells
    std::unordered_set<Net*> connected_nets;
    
    for (Cell* cell : cells) {
        for (const auto& pin_ptr : cell->getPins()) {
            Pin* pin = pin_ptr.get();
            if (pin->getNet()) {
                connected_nets.insert(pin->getNet());
            }
        }
    }
    
    // Calculate HPWL for connected nets
    double total_hpwl = 0.0;
    for (Net* net : connected_nets) {
        total_hpwl += calculateNetHPWL(net);
    }
    
    return total_hpwl;
}

double DetailedPlacer::calculateNetHPWL(Net* net) {
    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();
    
    bool has_pins = false;
    
    // Consider driver pin
    if (net->getDriver()) {
        Cell* driver_cell = net->getDriver()->getOwner();
        if (driver_cell && driver_cell->isPlaced()) {
            double x = driver_cell->getX();
            double y = driver_cell->getY();
            min_x = max_x = x;
            min_y = max_y = y;
            has_pins = true;
        }
    }
    
    // Consider load pins
    for (Pin* load_pin : net->getLoads()) {
        Cell* load_cell = load_pin->getOwner();
        if (load_cell && load_cell->isPlaced()) {
            double x = load_cell->getX();
            double y = load_cell->getY();
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
            has_pins = true;
        }
    }
    
    if (!has_pins) return 0.0;
    
    return (max_x - min_x) + (max_y - min_y);
}

void DetailedPlacer::debugLog(const std::string& message) const {
    if (verbose_) {
        std::cout << "[DetailedPlacer] " << message << std::endl;
    }
}

std::vector<std::vector<Cell*>> DetailedPlacer::getCellsByRow() {
    // Use integer row index instead of floating point Y coordinate
    std::map<int, std::vector<Cell*>> row_map;
    double row_height = db_->getRowHeight();
    const auto& core_area = db_->getCoreArea();
    double core_y = core_area.y_min;
    
    // Group cells by Y coordinate (row)
    auto all_cells = db_->getAllCells();
    for (Cell* cell : all_cells) {
        // Must exclude fixed cells
        if (db_->getCellInfo(cell).fixed) continue;
        
        double y = db_->getCellInfo(cell).y;
        // Quantize Y coordinate to row index
        int row_idx = std::round((y - core_y) / row_height);
        row_map[row_idx].push_back(cell);
    }
    
    // Convert to vector and sort each row by X coordinate
    std::vector<std::vector<Cell*>> rows;
    for (auto& [idx, cells] : row_map) {
        std::sort(cells.begin(), cells.end(), 
                 [this](Cell* a, Cell* b) {
                     return db_->getCellInfo(a).x < db_->getCellInfo(b).x;
                 });
        rows.push_back(cells);
    }
    
    // Sort rows by Y coordinate
    std::sort(rows.begin(), rows.end(),
             [this](const std::vector<Cell*>& row1, const std::vector<Cell*>& row2) {
                 return db_->getCellInfo(row1[0]).y < db_->getCellInfo(row2[0]).y;
             });
    
    return rows;
}

bool DetailedPlacer::hasEqualWidth(Cell* c1, Cell* c2) const {
    double w1 = db_->getCellInfo(c1).width;
    double w2 = db_->getCellInfo(c2).width;
    return std::abs(w1 - w2) < 1e-9;  // Use epsilon for floating point comparison
}

std::pair<int, int> DetailedPlacer::verifySiteAlignment() const {
    double site_width = db_->getSiteWidth();
    auto all_cells = db_->getAllCells();
    
    int aligned_count = 0;
    int total_count = 0;
    
    for (Cell* cell : all_cells) {
        const auto& info = db_->getCellInfo(cell);
        if (info.fixed) continue;  // Skip fixed cells
        
        total_count++;
        if (isSiteAligned(info.x, site_width)) {
            aligned_count++;
        }
    }
    
    return {aligned_count, total_count};
}

bool DetailedPlacer::isContiguous(const std::vector<Cell*>& cells) const {
    if (cells.size() < 2) return true;
    
    // Check if cells are contiguous (no gaps between them)
    for (size_t i = 0; i < cells.size() - 1; ++i) {
        Cell* current = cells[i];
        Cell* next = cells[i+1];
        
        double curr_end = db_->getCellInfo(current).x + db_->getCellInfo(current).width;
        double next_start = db_->getCellInfo(next).x;
        
        // Allow tiny floating point error
        if (std::abs(next_start - curr_end) > 1e-4) {
            return false; // Gap or overlap detected
        }
    }
    return true;
}

} // namespace mini