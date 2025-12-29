/**
 * @file abacus_legalizer.cpp
 * @brief Abacus Legalization Algorithm Implementation
 */

#include "abacus_legalizer.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <iostream>

namespace mini {

// ============================================================================
// AbacusCluster Implementation
// ============================================================================

void AbacusCluster::initialize(Cell* cell, const PlacerDB::CellInfo& cell_info) {
    cells.clear();
    cells.push_back(cell);
    
    // Initialize cluster parameters for single cell
    x = cell_info.x;                    // Cluster position = cell's original position
    width = cell_info.width;             // Cluster width = cell width
    weight = 1.0;                        // Each cell has weight 1
    q_score = cell_info.x;               // For single cell: q = original position
}

void AbacusCluster::addCluster(const AbacusCluster& other) {
    // 1. Merge cell lists
    cells.insert(cells.end(), other.cells.begin(), other.cells.end());
    
    // 2. Update mathematical parameters (CORE!)
    // New weight is sum of both weights
    weight += other.weight;
    
    // New q value = q_self + q_other - weight_other * width_self
    // Physical meaning: subtract the offset of other cluster relative to new combined cluster
    q_score += other.q_score - other.weight * width;
    
    // 3. Update width
    width += other.width;
    
    // 4. Recalculate optimal position (Quadratic Optimization Result)
    updateToOptimalPosition();
}

// ============================================================================
// AbacusLegalizer Implementation
// ============================================================================



void AbacusLegalizer::run() {
    debugLog("Starting Abacus legalization...");
    
    // Phase 1: Distribute cells to rows
    distributeCellsToRows();
    
    // Phase 2: Solve row conflicts with cluster merging
    debugLog("Phase 2: Row-based conflict resolution with cluster merging");
    for (auto& row : rows_) {
        legalizeRow(row);
    }
    
    // Phase 3: Snap to site grid (final alignment)
    debugLog("Phase 3: Site alignment (snap to grid)");
    snapToSiteGrid();
    
    // Calculate and report final statistics
    double final_hpwl = calculateHPWL();
    std::cout << "  Final HPWL: " << final_hpwl << std::endl;
    
    // Check for overlaps
    bool has_overlaps = hasOverlaps();
    std::cout << "  Overlap check: " << (has_overlaps ? "FOUND OVERLAPS!" : "No overlaps") << std::endl;
    
    // Export result for visualization
    Legalizer::exportResult("abacus_complete.csv");
    
    debugLog("Abacus legalization completed (Phase 1 + Phase 2 + Phase 3)");
}

void AbacusLegalizer::distributeCellsToRows() {
    // Initialize rows based on core area
    initializeRows();
    
    // Get all movable cells
    auto all_cells = db_->getAllCells();
    
    // Distribute each cell to its nearest row
    for (Cell* cell : all_cells) {
        const auto& cell_info = db_->getCellInfo(cell);
        
        // Skip fixed cells (like I/O pads)
        if (cell_info.fixed) {
            debugLog("Skipping fixed cell: " + cell->getName());
            continue;
        }
        
        // Find nearest row based on Y coordinate
        double current_y = cell_info.y;
        const Rect& core_area = db_->getCoreArea();
        double row_height = db_->getRowHeight();
        
        // Calculate row index: round to nearest row
        int row_idx = static_cast<int>(
            std::round((current_y - core_area.y_min) / row_height)
        );
        
        // Boundary protection
        if (row_idx < 0) row_idx = 0;
        if (row_idx >= static_cast<int>(rows_.size())) row_idx = rows_.size() - 1;
        
        // Assign cell to row
        rows_[row_idx].cells.push_back(cell);
        
        // Snap Y coordinate to row (vertical alignment)
        snapCellToRow(cell, row_idx);
    }
    
    // Sort cells in each row by X coordinate (critical for Abacus)
    for (auto& row : rows_) {
        std::sort(row.cells.begin(), row.cells.end(), 
            [this](Cell* a, Cell* b) {
                return db_->getCellInfo(a).x < db_->getCellInfo(b).x;
            });
    }
}

void AbacusLegalizer::initializeRows() {
    const Rect& core_area = db_->getCoreArea();
    double row_height = db_->getRowHeight();
    
    // Calculate number of rows
    int num_rows = static_cast<int>(
        std::ceil(core_area.height() / row_height)
    );
    
    rows_.resize(num_rows);
    
    // Initialize each row
    for (int i = 0; i < num_rows; ++i) {
        rows_[i].index = i;
        rows_[i].y_coordinate = core_area.y_min + i * row_height;
        rows_[i].min_x = core_area.x_min;
        rows_[i].max_x = core_area.x_min + core_area.width();
        
        debugLog("Row " + std::to_string(i) + 
                 ": Y=" + std::to_string(rows_[i].y_coordinate) +
                 ", Range=[" + std::to_string(rows_[i].min_x) + 
                 "," + std::to_string(rows_[i].max_x) + "]");
    }
}

void AbacusLegalizer::snapCellToRow(Cell* cell, int row_idx) {
    double row_y = rows_[row_idx].y_coordinate;
    
    // Get current X position, keep it unchanged for now
    const auto& cell_info = db_->getCellInfo(cell);
    double current_x = cell_info.x;
    
    // Update cell position (snap Y to row, keep X)
    cell->setPosition(current_x, row_y);
    
    // Also update PlacerDB to maintain consistency
    db_->placeCell(cell, current_x, row_y);
    
    debugLog("Snapped cell " + cell->getName() + 
             " to row " + std::to_string(row_idx) + 
             " at Y=" + std::to_string(row_y));
}

void AbacusLegalizer::printRowStatistics() const {
    std::cout << "\n=== Row Distribution Statistics ===" << std::endl;
    
    int total_cells = 0;
    int max_cells_in_row = 0;
    int row_with_max_cells = 0;
    
    for (size_t i = 0; i < rows_.size(); ++i) {
        int cell_count = static_cast<int>(rows_[i].cells.size());
        total_cells += cell_count;
        
        if (cell_count > max_cells_in_row) {
            max_cells_in_row = cell_count;
            row_with_max_cells = static_cast<int>(i);
        }
        
        if (verbose_ || cell_count > 0) {
            std::cout << "Row " << std::setw(3) << i 
                      << ": " << std::setw(3) << cell_count << " cells" << std::endl;
        }
    }
    
    std::cout << "=====================================" << std::endl;
    std::cout << "Total distributed cells: " << total_cells << std::endl;
    std::cout << "Average cells per row: " 
              << std::fixed << std::setprecision(2) 
              << static_cast<double>(total_cells) / rows_.size() << std::endl;
    std::cout << "Max cells in a row: " << max_cells_in_row 
              << " (Row " << row_with_max_cells << ")" << std::endl;
    std::cout << "=====================================" << std::endl;
}

void AbacusLegalizer::exportResult(const std::string& filename) const {
    // Create visualization directory
    std::string mkdir_cmd = "mkdir -p visualizations/abacus_projection";
    std::system(mkdir_cmd.c_str());
    
    // Full path for CSV file
    std::string full_path = "visualizations/abacus_projection/" + filename;
    std::ofstream file(full_path);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << full_path << " for writing" << std::endl;
        return;
    }
    
    // CSV header
    file << "cell_name,x,y,width,height,fixed,row_index" << std::endl;
    
    // Export all cells (including fixed ones for complete visualization)
    auto all_cells = db_->getAllCells();
    for (Cell* cell : all_cells) {
        const auto& cell_info = db_->getCellInfo(cell);
        
        // Find which row this cell belongs to (if any)
        int row_index = -1;
        for (size_t i = 0; i < rows_.size(); ++i) {
            auto it = std::find(rows_[i].cells.begin(), rows_[i].cells.end(), cell);
            if (it != rows_[i].cells.end()) {
                row_index = static_cast<int>(i);
                break;
            }
        }
        
        file << cell->getName() << ","
             << cell->getX() << ","
             << cell->getY() << ","
             << cell_info.width << ","
             << cell_info.height << ","
             << (cell_info.fixed ? "1" : "0") << ","
             << row_index << std::endl;
    }
    
    file.close();
    
    if (verbose_) {
        std::cout << "[AbacusLegalizer] Projection result exported to " << filename << std::endl;
    }
    
    std::cout << "Abacus projection exported to: " << full_path << std::endl;
}



// ============================================================================
// TODO: Core Abacus Algorithm (Phase 2) - To be implemented next
// ============================================================================

void AbacusLegalizer::legalizeRow(AbacusRow& row) {
    debugLog("Legalizing row " + std::to_string(row.index) + " with " + 
             std::to_string(row.cells.size()) + " cells");
    
    // Clear existing clusters
    row.clusters.clear();
    
    // Process each cell in this row (already sorted by X coordinate)
    for (Cell* cell : row.cells) {
        // 1. Create a new cluster for current cell
        AbacusCluster new_cluster;
        const auto& cell_info = db_->getCellInfo(cell);
        new_cluster.initialize(cell, cell_info);
        
        // 2. Push to cluster stack
        row.clusters.push_back(new_cluster);
        
        // 3. Check for conflicts and merge (ABACUS CLASH!)
        while (row.clusters.size() > 1) {
            AbacusCluster& curr = row.clusters.back();           // Current (rightmost) cluster
            AbacusCluster& prev = row.clusters[row.clusters.size() - 2];  // Previous cluster
            
            // Check for overlap: curr.x < prev.x + prev.width
            double prev_right_boundary = prev.x + prev.width;
            if (curr.x < prev_right_boundary - 1e-9) {  // Small epsilon for floating point
                // === COLLISION DETECTED! MERGE! ===
                debugLog("  Collision: merging clusters (curr.x=" + 
                         std::to_string(curr.x) + " < prev.right=" + 
                         std::to_string(prev_right_boundary) + ")");
                
                // Merge curr into prev
                prev.addCluster(curr);
                
                // Remove curr (it has been absorbed)
                row.clusters.pop_back();
                
                debugLog("  After merge: new cluster pos=" + 
                         std::to_string(prev.x) + ", width=" + 
                         std::to_string(prev.width) + ", weight=" + 
                         std::to_string(prev.weight));
                
                // Continue checking - the enlarged prev might now overlap with its previous neighbor
            } else {
                // No collision, stop checking
                break;
            }
        }
    }
    
    debugLog("Row " + std::to_string(row.index) + " reduced to " + 
             std::to_string(row.clusters.size()) + " clusters");
    
    // 4. Write back final positions
    for (auto& cluster : row.clusters) {
        double current_x = cluster.x;
        
        // Boundary checking (Clamp to row boundaries)
        if (current_x < row.min_x) {
            current_x = row.min_x;
            debugLog("  Clamped cluster to left boundary: " + std::to_string(current_x));
        }
        if (current_x + cluster.width > row.max_x) {
            current_x = row.max_x - cluster.width;
            debugLog("  Clamped cluster to right boundary: " + std::to_string(current_x));
        }
        
        // Place each cell in the cluster
        for (Cell* cell : cluster.cells) {
            const auto& cell_info = db_->getCellInfo(cell);
            
            // Set final coordinates
            cell->setPosition(current_x, row.y_coordinate);
            db_->placeCell(cell, current_x, row.y_coordinate);  // Sync with DB
            
            // Next cell goes right after this one
            current_x += cell_info.width;
        }
    }
    
    debugLog("Row " + std::to_string(row.index) + " legalization completed");
}

void AbacusLegalizer::collapseClusters(AbacusRow& row, int cluster_idx) {
    // TODO: Implement cluster merging with optimal position calculation
    debugLog(" collapseClusters() - to be implemented in next phase");
}

bool AbacusLegalizer::clusterFitsAt(const AbacusCluster& cluster, double x, const AbacusRow& row) const {
    // TODO: Check if cluster fits within row boundaries
    return (x >= row.min_x) && (x + cluster.width <= row.max_x);
}

// ============================================================================
// Phase 3: Site Alignment Implementation
// ============================================================================

void AbacusLegalizer::snapToSiteGrid() {
    double site_width = db_->getSiteWidth();
    
    if (site_width <= 0.0) {
        std::cerr << "Warning: Invalid site width (" << site_width 
                  << "), skipping site alignment" << std::endl;
        return;
    }
    
    std::cout << "  Snapping cells to site grid (width = " << site_width << " um)" << std::endl;
    
    for (auto& row : rows_) {
        double current_x_limit = row.min_x;
        
        // Sort cells in each row by their current X coordinate
        std::sort(row.cells.begin(), row.cells.end(), 
                 [this](Cell* a, Cell* b) {
                     return db_->getCellInfo(a).x < db_->getCellInfo(b).x;
                 });
        
        // Left-to-right scan with site alignment
        for (Cell* cell : row.cells) {
            const auto& cell_info = db_->getCellInfo(cell);
            double cell_width = cell_info.width;
            
            // Calculate the nearest site position that doesn't violate the boundary
            // We want: snapped_x >= current_x_limit
            double raw_x = cell_info.x;
            double snapped_x = std::round(raw_x / site_width) * site_width;
            
            // If snapped position would cause overlap, move to next available site
            if (snapped_x < current_x_limit) {
                snapped_x = std::ceil(current_x_limit / site_width) * site_width;
            }
            
            // Ensure we're still within row boundaries
            if (snapped_x + cell_width > row.max_x) {
                std::cerr << "Warning: Cell " << cell->getName() 
                         << " would exceed row boundary after site alignment" << std::endl;
                snapped_x = std::max(row.min_x, row.max_x - cell_width);
            }
            
            // Place the cell at the snapped position
            db_->placeCell(cell, snapped_x, row.y_coordinate);
            
            // Update boundary pointer for next cell
            current_x_limit = snapped_x + cell_width;
        }
    }
    
    std::cout << "  Site alignment completed" << std::endl;
}

} // namespace mini