/**
 * @file placer_db.cpp
 * @brief Physical Database Implementation for MiniPlacement
 */

#include "placer_db.h"
#include <algorithm>
#include <iostream>

namespace mini {

PlacerDB::PlacerDB(NetlistDB* netlist_db) 
    : netlist_db_(netlist_db)
    , core_area_(0.0, 0.0, 1000.0, 1000.0)  // Default large core area
    , row_height_(3.0)                       // Default 3.0um row height
    , site_width_(0.19)                      // Default 0.19um site width (Nangate 45nm)
    , random_engine_(42)                     // Fixed seed for reproducibility
{
}

void PlacerDB::setCoreArea(const Rect& core_area) {
    core_area_ = core_area;
}

void PlacerDB::addCell(Cell* cell, double area) {
    if (!cell) return;

    // Calculate width and height from area using row height
    double width = area / row_height_;
    double height = row_height_;

    // Initialize at origin (0, 0)
    cell_infos_[cell] = CellInfo(0.0, 0.0, width, height);
}

void PlacerDB::addCell(Cell* cell, double width, double height) {
    if (!cell) return;

    // Initialize at origin (0, 0) with precise dimensions
    cell_infos_[cell] = CellInfo(0.0, 0.0, width, height);
}

void PlacerDB::setCellLefMacro(Cell* cell, const LefMacro* lef_macro) {
    if (!cell) return;
    cell_lef_macros_[cell] = lef_macro;
}

const LefMacro* PlacerDB::getCellLefMacro(Cell* cell) const {
    auto it = cell_lef_macros_.find(cell);
    return (it != cell_lef_macros_.end()) ? it->second : nullptr;
}

void PlacerDB::placeCell(Cell* cell, double x, double y) {
    auto it = cell_infos_.find(cell);
    if (it != cell_infos_.end()) {
        it->second.x = x;
        it->second.y = y;
        
        // Also update the Cell object's position to maintain consistency
        cell->setPosition(x, y);
    }
}

Point PlacerDB::getCellCenter(Cell* cell) const {
    auto it = cell_infos_.find(cell);
    if (it != cell_infos_.end()) {
        const CellInfo& info = it->second;
        return Point(info.x + info.width / 2.0, info.y + info.height / 2.0);
    }
    return Point(0.0, 0.0);  // Default if not found
}

void PlacerDB::commitPlacement() {
    size_t synced_count = 0;
    
    for (auto& entry : cell_infos_) {
        Cell* cell = entry.first;
        const CellInfo& info = entry.second;
        
        // Synchronize CellInfo (DB) coordinates to Cell (Netlist)
        // Note: GlobalPlacer reads from cell->getX(), cell->getY()
        cell->setPosition(info.x, info.y);
        synced_count++;
    }
    
    std::cout << "  [PlacerDB] Synced " << synced_count << " cell positions to Netlist." << std::endl;
}

const PlacerDB::CellInfo& PlacerDB::getCellInfo(Cell* cell) const {
    static CellInfo default_info;  // Return default if not found
    auto it = cell_infos_.find(cell);
    return (it != cell_infos_.end()) ? it->second : default_info;
}

PlacerDB::CellInfo& PlacerDB::getCellInfo(Cell* cell) {
    static CellInfo default_info;  // Return default if not found
    auto it = cell_infos_.find(cell);
    return (it != cell_infos_.end()) ? it->second : default_info;
}

void PlacerDB::initializeRandom() {
    std::uniform_real_distribution<double> x_dist(core_area_.x_min, core_area_.x_max);
    std::uniform_real_distribution<double> y_dist(core_area_.y_min, core_area_.y_max);

    for (auto& [cell, info] : cell_infos_) {
        if (!info.fixed) {
            // Generate random position within core area
            double x = x_dist(random_engine_);
            double y = y_dist(random_engine_);
            
            // Ensure cell fits within core area
            x = std::max(core_area_.x_min, std::min(x, core_area_.x_max - info.width));
            y = std::max(core_area_.y_min, std::min(y, core_area_.y_max - info.height));
            
            placeCell(cell, x, y);
        }
    }
}

std::vector<Cell*> PlacerDB::getAllCells() const {
    std::vector<Cell*> cells;
    cells.reserve(cell_infos_.size());
    
    for (const auto& [cell, info] : cell_infos_) {
        cells.push_back(cell);
    }
    
    return cells;
}

double PlacerDB::getTotalCellArea() const {
    double total_area = 0.0;
    for (const auto& [cell, info] : cell_infos_) {
        total_area += info.width * info.height;
    }
    return total_area;
}

bool PlacerDB::isValidPlacement(Cell* cell) const {
    auto it = cell_infos_.find(cell);
    if (it == cell_infos_.end()) return false;
    
    const CellInfo& info = it->second;
    
    // Check if cell is within core area bounds
    return (info.x >= core_area_.x_min && 
            info.y >= core_area_.y_min &&
            info.x + info.width <= core_area_.x_max &&
            info.y + info.height <= core_area_.y_max);
}

std::vector<std::vector<Cell*>> PlacerDB::getCellsByRow() {
    std::unordered_map<double, std::vector<Cell*>> row_map;
    
    // Group cells by Y coordinate (row)
    auto all_cells = getAllCells();
    for (Cell* cell : all_cells) {
        double y = getCellInfo(cell).y;
        row_map[y].push_back(cell);
    }
    
    // Convert to vector and sort each row by X coordinate
    std::vector<std::vector<Cell*>> rows;
    for (auto& [y, cells] : row_map) {
        std::sort(cells.begin(), cells.end(), 
                 [this](Cell* a, Cell* b) {
                     return getCellInfo(a).x < getCellInfo(b).x;
                 });
        rows.push_back(cells);
    }
    
    // Sort rows by Y coordinate
    std::sort(rows.begin(), rows.end(),
             [this](const std::vector<Cell*>& row1, const std::vector<Cell*>& row2) {
                 return getCellInfo(row1[0]).y < getCellInfo(row2[0]).y;
             });
    
    return rows;
}

} // namespace mini

