/**
 * @file abacus_legalizer.cpp
 * @brief Abacus Legalization Algorithm Implementation
 */

#include "abacus_legalizer.h"
#include "../../lib/include/debug_log.h"
#include <algorithm>
#include <cmath>
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
    DEBUG_LOG("AbacusLegalizer", "Starting Abacus legalization...");
    
    // Phase 1: Distribute cells to rows
    distributeCellsToRows();
    
    // Phase 2: Solve row conflicts with cluster merging
    DEBUG_LOG("AbacusLegalizer", "Phase 2: Row-based conflict resolution with cluster merging");
    for (auto& row : rows_) {
        legalizeRow(row);
    }
    
    // Phase 3: Snap to site grid (final alignment)
    DEBUG_LOG("AbacusLegalizer", "Phase 3: Site alignment (snap to grid)");
    snapToSiteGrid();

    // Report final statistics
    reportFinalStatistics();

    DEBUG_LOG("AbacusLegalizer", "Abacus legalization completed (Phase 1 + Phase 2 + Phase 3)");
}

void AbacusLegalizer::distributeCellsToRows() {
    // Initialize rows
    initializeRows();
    
    // Row capacity tracker
    std::vector<double> row_usage(rows_.size(), 0.0);
    double core_width = db_->getCoreArea().width();
    
    // Get movable cells
    auto movable_cells = collectMovableCells();

    // Sort by X for better capacity distribution
    std::sort(movable_cells.begin(), movable_cells.end(),
        [this](Cell* a, Cell* b) {
            return PlacerDB::compareByX(db_, a, b);
        });
    
    // Smart allocation with capacity awareness
    for (Cell* cell : movable_cells) {
        const auto& cell_info = db_->getCellInfo(cell);
        double cell_w = cell_info.width;
        
        // Find ideal row
        double current_y = cell_info.y;
        double row_height = db_->getRowHeight();
        int best_row_idx = static_cast<int>(std::round((current_y - db_->getCoreArea().y_min) / row_height));
        
        // Spiral search for available row
        int final_row_idx = -1;
        int max_search_range = static_cast<int>(rows_.size());
        
        for (int offset = 0; offset < max_search_range; ++offset) {
            int search_dir = (offset % 2 == 0) ? (offset / 2) : -(offset + 1) / 2;
            int try_row = best_row_idx + search_dir;
            
            if (try_row < 0 || try_row >= static_cast<int>(rows_.size())) continue;
            
            // Capacity check
            if (row_usage[try_row] + cell_w <= core_width + 1e-5) {
                final_row_idx = try_row;
                break;
            }
        }
        
        // Emergency: chip full
        if (final_row_idx == -1) {
            std::cerr << "CRITICAL WARNING: Chip is 100% full! Cannot place cell " << cell->getName() << std::endl;
            final_row_idx = std::max(0, std::min(best_row_idx, (int)rows_.size()-1));
        }
        
        // Place cell
        rows_[final_row_idx].cells.push_back(cell);
        row_usage[final_row_idx] += cell_w;
        snapCellToRow(cell, final_row_idx);
    }
    
    // Re-sort by X for Abacus
    for (auto& row : rows_) {
        std::sort(row.cells.begin(), row.cells.end(),
            [this](Cell* a, Cell* b) {
                return PlacerDB::compareByX(db_, a, b);
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
        
        DEBUG_LOG("AbacusLegalizer", "Row " + std::to_string(i) + 
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
    // Note: placeCell() internally calls cell->setPosition()
    db_->placeCell(cell, current_x, row_y);
    
    DEBUG_LOG("AbacusLegalizer", "Snapped cell " + cell->getName() + 
             " to row " + std::to_string(row_idx) + 
             " at Y=" + std::to_string(row_y));
}

void AbacusLegalizer::legalizeRow(AbacusRow& row) {
    DEBUG_LOG("AbacusLegalizer", "Legalizing row " + std::to_string(row.index) + " with " + 
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
                DEBUG_LOG("AbacusLegalizer", "  Collision: merging clusters (curr.x=" + 
                         std::to_string(curr.x) + " < prev.right=" + 
                         std::to_string(prev_right_boundary) + ")");
                
                // Merge curr into prev
                prev.addCluster(curr);
                
                // Remove curr (it has been absorbed)
                row.clusters.pop_back();
                
                DEBUG_LOG("AbacusLegalizer", "  After merge: new cluster pos=" + 
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
    
    DEBUG_LOG("AbacusLegalizer", "Row " + std::to_string(row.index) + " reduced to " + 
             std::to_string(row.clusters.size()) + " clusters");
    
// Right-to-Left compaction to fix boundary pile-up
    double current_right_limit = row.max_x;
    
    // Process from right to left
    for (int i = row.clusters.size() - 1; i >= 0; --i) {
        AbacusCluster& cluster = row.clusters[i];
        
        // Push back if exceeds boundary
        if (cluster.x + cluster.width > current_right_limit) {
            cluster.x = current_right_limit - cluster.width;
        }
        
        // Update limit for next cluster
        current_right_limit = cluster.x;
    }
    
    // Handle left boundary overflow
    if (!row.clusters.empty() && row.clusters[0].x < row.min_x) {
        std::cerr << "Warning: Row " << row.index << " is OVER-UTILIZED! Cells cannot fit." << std::endl;
        // Force left alignment
        row.clusters[0].x = row.min_x;
        for(size_t i=1; i<row.clusters.size(); ++i) {
            row.clusters[i].x = row.clusters[i-1].x + row.clusters[i-1].width;
        }
    }

    // Step 4. Write back final positions
    for (auto& cluster : row.clusters) {
        double current_x = cluster.x;
        
        // Place sequentially (boundary handled above)
        for (Cell* cell : cluster.cells) {
            const auto& cell_info = db_->getCellInfo(cell);

            // Note: placeCell() internally calls cell->setPosition()
            db_->placeCell(cell, current_x, row.y_coordinate);

            current_x += cell_info.width;
        }
    }

    DEBUG_LOG("AbacusLegalizer", "Row " + std::to_string(row.index) + " legalization completed");
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
                     return PlacerDB::compareByX(db_, a, b);
                 });
        
        // Left-to-right scan with site alignment
        for (Cell* cell : row.cells) {
            const auto& cell_info = db_->getCellInfo(cell);
            double cell_width = cell_info.width;
            
            // Calculate the nearest site position that doesn't violate the boundary
            // We want: snapped_x >= current_x_limit
            double raw_x = cell_info.x;
            
            // [FIX] Apply epsilon tolerance for floating-point precision issues
            double snapped_x;
            
            // Check if the cell is already effectively aligned (considering floating-point errors)
            if (PlacerDB::isSiteAligned(raw_x, site_width, 1e-9)) {
                // Cell is already effectively aligned, keep original position to avoid unnecessary movement
                snapped_x = raw_x;
            } else {
                // Calculate the nearest site position
                double ratio = raw_x / site_width;
                double rounded_ratio = std::round(ratio);
                snapped_x = rounded_ratio * site_width;
            }
            
            // If snapped position would cause overlap, move to next available site
            if (std::abs(snapped_x - current_x_limit) > 1e-9 && snapped_x < current_x_limit) {
                double limit_ratio = current_x_limit / site_width;
                snapped_x = std::ceil(limit_ratio) * site_width;
            }
            
// Allow slight overflow over creating overlap
            if (snapped_x + cell_width > row.max_x + 1e-9) {
                std::cerr << "Warning: Cell " << cell->getName() 
                         << " slightly exceeds boundary after site alignment (overflow preferred over overlap)" << std::endl;
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
