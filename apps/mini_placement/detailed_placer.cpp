/**
 * @file detailed_placer.cpp
 * @brief Detailed Placement Algorithm Implementation
 */

#include "detailed_placer.h"
#include "../../lib/include/debug_log.h"
#include "../../lib/include/hpwl_calculator.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace mini {

DetailedPlacer::DetailedPlacer(PlacerDB* db, NetlistDB* netlist)
    : db_(db), netlist_(netlist), verbose_(false), iterations_(3), final_hpwl_(0.0) {
}

void DetailedPlacer::run() {
    DEBUG_LOG("DetailedPlacer", "Starting detailed placement optimization...");
    
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
    }
    
    DEBUG_LOG("DetailedPlacer", "Detailed placement optimization completed");
}



void DetailedPlacer::globalSwap() {
    DEBUG_LOG("DetailedPlacer", "Running global swap optimization...");
    
    auto all_cells = db_->getAllCells();
    int successful_swaps = 0;
    
    // Group cells by width for efficient equal-width swapping
    std::unordered_map<double, std::vector<Cell*>> width_groups;
    for (Cell* cell : all_cells) {
        double width = db_->getCellWidth(cell);
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
    // 1. Store original coordinates
    double x1 = db_->getCellX(c1);
    double y1 = db_->getCellY(c1);
    double x2 = db_->getCellX(c2);
    double y2 = db_->getCellY(c2);
    
    // 2. Calculate local wirelength before swap (Cost Before)
    // Only need to calculate nets connected to c1 and c2
    double cost_before = calculateLocalHPWL({c1, c2});
    
    // 3. Execute physical swap (Swap Coordinates)
    // c1 goes to (x2, y2), c2 goes to (x1, y1)
    db_->placeCell(c1, x2, y2);
    db_->placeCell(c2, x1, y1);
    
    // 4. Calculate local wirelength after swap (Cost After)
    double cost_after = calculateLocalHPWL({c1, c2});
    
    // 5. Decision
    // Only keep swap if wirelength actually decreased (Strict Improvement)
    if (cost_after < cost_before) {
        // Keep swap, no action needed as DB is already updated
        return true; 
    } else {
        // Got worse or no change, rollback
        db_->placeCell(c1, x1, y1);
        db_->placeCell(c2, x2, y2);
        return false;
    }
}

void DetailedPlacer::detailedReordering() {
    DEBUG_LOG("DetailedPlacer", "Running detailed reordering (Equal-Width Swap Strategy)...");
    
    // 1. Get cells grouped by row
    auto rows = getCellsByRow();
    int total_swaps = 0;
    
    for (auto& row_cells : rows) {
        if (row_cells.size() < 2) continue;
        
        // [Critical Step] Force sort by physical X coordinate before processing each row
        // Ensure vector order matches physical positions in DB
        std::sort(row_cells.begin(), row_cells.end(),
            [this](Cell* a, Cell* b) {
                return PlacerDB::compareByX(db_, a, b);
            });

        // 2. Sliding window (Window Size = 3)
        // Smaller window = faster, larger window = better solution space
        int window_size = 3; 
        
        // Skip rows with fewer cells than window size
        if (row_cells.size() < static_cast<size_t>(window_size)) {
            continue;
        }
        
        for (size_t i = 0; i <= row_cells.size() - window_size; ++i) {
            
            // Try pairwise swaps within window
            // Window range: [i, i + window_size - 1]
            
            // Try swapping cell A and B within window
            for (int offset_a = 0; offset_a < window_size; ++offset_a) {
                for (int offset_b = offset_a + 1; offset_b < window_size; ++offset_b) {
                    
                    Cell* cell_a = row_cells[i + offset_a];
                    Cell* cell_b = row_cells[i + offset_b];
                    
                    // [Core Constraint] Only swap equal-width cells!
                    // This is the "absolute safety" guarantee
                    if (!hasEqualWidth(cell_a, cell_b)) {
                        continue;
                    }

                    // Try swap
                    if (trySwap(cell_a, cell_b)) {
                        total_swaps++;
                        // [Important] After successful swap, coordinates in DB changed:
                        // cell_a moved to right, cell_b moved to left
                        // To maintain row_cells order, swap them in vector too
                        std::swap(row_cells[i + offset_a], row_cells[i + offset_b]);
                        
                        // After swap, cell_b (now on left) might swap with others
                        // Continue logic or break to next window
                    }
                }
            }
        }
    }
    
    if (verbose_) {
        std::cout << "  Swaps performed: " << total_swaps << std::endl;
    }
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

    // Calculate HPWL for connected nets using HPWLCalculator
    double total_hpwl = 0.0;
    for (Net* net : connected_nets) {
        total_hpwl += HPWLCalculator::calculateNetHPWL(net, db_);
    }

    return total_hpwl;
}

std::vector<std::vector<Cell*>> DetailedPlacer::getCellsByRow() {
    std::unordered_map<double, std::vector<Cell*>> row_map;
    
    // Group cells by Y coordinate (row)
    auto all_cells = db_->getAllCells();
    for (Cell* cell : all_cells) {
        // [FIX 3] Must exclude fixed cells!
        if (db_->isCellFixed(cell)) continue;
        
        double y = db_->getCellY(cell);
        row_map[y].push_back(cell);
    }
    
    // Convert to vector and sort each row by X coordinate
    std::vector<std::vector<Cell*>> rows;
    for (auto& [y, cells] : row_map) {
        std::sort(cells.begin(), cells.end(),
                 [this](Cell* a, Cell* b) {
                     return PlacerDB::compareByX(db_, a, b);
                 });
        rows.push_back(cells);
    }
    
    // Sort rows by Y coordinate
    std::sort(rows.begin(), rows.end(),
             [this](const std::vector<Cell*>& row1, const std::vector<Cell*>& row2) {
                 return db_->getCellY(row1[0]) < db_->getCellY(row2[0]);
             });
    
    return rows;
}

bool DetailedPlacer::hasEqualWidth(Cell* c1, Cell* c2) const {
    double w1 = db_->getCellWidth(c1);
    double w2 = db_->getCellWidth(c2);
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
        if (PlacerDB::isSiteAligned(info.x, site_width)) {
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
        
        double curr_end = db_->getCellX(current) + db_->getCellWidth(current);
        double next_start = db_->getCellX(next);
        
        // Allow tiny floating point error
        if (std::abs(next_start - curr_end) > 1e-4) {
            return false; // Gap or overlap detected
        }
    }
    return true;
}

} // namespace mini