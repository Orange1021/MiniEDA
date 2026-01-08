/**
 * @file greedy_legalizer.cpp
 * @brief Greedy (Tetris-like) Legalization Implementation
 */

#include "greedy_legalizer.h"
#include "../../lib/include/hpwl_calculator.h"
#include <algorithm>
#include <iostream>

namespace mini {

void GreedyLegalizer::run() {
    debugLog("Starting Greedy (Tetris) legalization...");
    
    // 1. Collect all movable cells
    auto movable_cells = Legalizer::collectMovableCells();
    debugLog("Collected " + std::to_string(movable_cells.size()) + " movable cells");
    
    // 2. Sort cells by Y then X coordinates
    sortCells(movable_cells);
    debugLog("Cells sorted by Y then X coordinates");
    
    // 3. Perform linear packing placement
    int cells_placed = performLinearPacking(movable_cells);
    
    // 4. Calculate final statistics
    const Rect& core_area = db_->getCoreArea();
    double row_height = db_->getRowHeight();
    int max_rows = static_cast<int>(std::floor(core_area.height() / row_height));
    
    // Estimate rows used (simplified calculation)
    double total_width = 0.0;
    for (Cell* cell : movable_cells) {
        const auto& info = db_->getCellInfo(cell);
        total_width += info.width;
    }
    int rows_used = static_cast<int>(std::ceil(total_width / core_area.width()));
    rows_used = std::min(rows_used, max_rows);
    
    printStatistics(cells_placed, rows_used, max_rows);

    // Report final statistics
    reportFinalStatistics();

    debugLog("Greedy legalization completed");
}

void GreedyLegalizer::sortCells(std::vector<Cell*>& cells) const {
    std::sort(cells.begin(), cells.end(),
             [this](Cell* a, Cell* b) {
                 const auto& info_a = db_->getCellInfo(a);
                 const auto& info_b = db_->getCellInfo(b);

                 // Primary sort: Y coordinate (center of cell)
                 double y_a = info_a.y + info_a.height / 2.0;
                 double y_b = info_b.y + info_b.height / 2.0;
                 if (std::abs(y_a - y_b) > 1e-9) {
                     return y_a < y_b;
                 }

                 // Secondary sort: X coordinate
                 return info_a.x < info_b.x;
             });
}

int GreedyLegalizer::performLinearPacking(const std::vector<Cell*>& cells) {
    const Rect& core_area = db_->getCoreArea();
    double row_height = db_->getRowHeight();
    
    // Calculate maximum number of rows that fit in core
    int max_rows = static_cast<int>(std::floor(core_area.height() / row_height));
    debugLog("Core can accommodate " + std::to_string(max_rows) + " rows");
    
    // Linear Packing: Fill rows from bottom-left to top-right
    int current_row = 0;
    double current_x = core_area.x_min;
    int total_cells_placed = 0;
    
    for (Cell* cell : cells) {
        const auto& info = db_->getCellInfo(cell);
        double cell_width = info.width;
        
        // Check if current cell fits in current row
        if (current_x + cell_width > core_area.x_max) {
            // Need to move to next row
            current_row++;
            current_x = core_area.x_min;
            
            // Safety check: ensure we don't exceed core boundary
            if (current_row >= max_rows) {
                std::cerr << "  ERROR: Core area too small! Cannot place all cells." << std::endl;
                std::cerr << "  Maximum rows: " << max_rows << ", trying to place in row " << current_row << std::endl;
                break;
            }
        }
        
        // Calculate Y coordinate for current row
        double new_y = core_area.y_min + current_row * row_height;
        
        // Place the cell
        db_->placeCell(cell, current_x, new_y);
        
        // Update cursor for next cell
        current_x += cell_width;
        total_cells_placed++;
    }
    
    return total_cells_placed;
}

void GreedyLegalizer::printStatistics(int cells_placed, int rows_used, int max_rows) const {
    std::cout << "  Successfully legalized " << cells_placed << " cells" << std::endl;
    std::cout << "  Used " << rows_used << " rows out of " << max_rows << " available" << std::endl;
}

} // namespace mini