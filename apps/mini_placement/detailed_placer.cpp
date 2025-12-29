/**
 * @file detailed_placer.cpp
 * @brief Detailed Placement Algorithm Implementation
 */

#include "detailed_placer.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

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
    
    if (verbose_) {
        std::cout << "  Final HPWL: " << final_hpwl_ << std::endl;
        double improvement = ((initial_hpwl_ - final_hpwl_) / initial_hpwl_) * 100;
        std::cout << "  HPWL improvement: " << std::fixed << std::setprecision(2) 
                  << improvement << "%" << std::endl;
    }
    
    debugLog("Detailed placement optimization completed");
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
            
            // Optimize this window
            if (optimizeWindow(window, start_x)) {
                total_improvements++;
                
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
    
    // Calculate original HPWL
    double best_hpwl = calculateLocalHPWL(window_cells);
    std::vector<Cell*> best_order = window_cells;
    
    // Store original coordinates for rollback
    std::vector<std::pair<double, double>> original_coords;
    for (Cell* cell : window_cells) {
        const auto& info = db_->getCellInfo(cell);
        original_coords.emplace_back(info.x, info.y);
    }
    
    // Generate all permutations of the window
    std::sort(window_cells.begin(), window_cells.end(), 
             [this](Cell* a, Cell* b) {
                 return db_->getCellInfo(a).x < db_->getCellInfo(b).x;
             });
    
    do {
        // === KEY: Site-aligned re-packing ===
        // Re-pack cells compactly from start_x
        rePackToSites(window_cells, start_x);
        
        // Calculate new HPWL
        double new_hpwl = calculateLocalHPWL(window_cells);
        
        // Keep if improvement found
        if (new_hpwl < best_hpwl) {
            best_hpwl = new_hpwl;
            best_order = window_cells;
        }
        
    } while (std::next_permutation(window_cells.begin(), window_cells.end()));
    
    // Apply best arrangement
    if (calculateLocalHPWL(best_order) < calculateLocalHPWL(window_cells)) {
        rePackToSites(best_order, start_x);
        window_cells = best_order;
        return true;
    }
    
    // Restore original arrangement
    for (size_t i = 0; i < window_cells.size(); ++i) {
        db_->placeCell(window_cells[i], original_coords[i].first, original_coords[i].second);
    }
    
    return false;
}

void DetailedPlacer::rePackToSites(const std::vector<Cell*>& cells, double start_x) {
    double current_x = start_x;
    
    for (Cell* cell : cells) {
        double cell_width = db_->getCellInfo(cell).width;
        double cell_y = db_->getCellInfo(cell).y;
        
        // Place cell at current position (should already be site-aligned)
        db_->placeCell(cell, current_x, cell_y);
        
        // Move to next position (width is guaranteed to be site-multiple)
        current_x += cell_width;
    }
}

bool DetailedPlacer::isSiteAligned(double x, double site_width) const {
    double quotient = x / site_width;
    return std::abs(quotient - std::round(quotient)) < 1e-9;
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
    std::unordered_map<double, std::vector<Cell*>> row_map;
    
    // Group cells by Y coordinate (row)
    auto all_cells = db_->getAllCells();
    for (Cell* cell : all_cells) {
        double y = db_->getCellInfo(cell).y;
        row_map[y].push_back(cell);
    }
    
    // Convert to vector and sort each row by X coordinate
    std::vector<std::vector<Cell*>> rows;
    for (auto& [y, cells] : row_map) {
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

} // namespace mini