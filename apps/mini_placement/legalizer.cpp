/**
 * @file legalizer.cpp
 * @brief Legalizer Base Class Implementation
 */

#include "legalizer.h"
#include <iostream>
#include <fstream>
#include "../../lib/include/hpwl_calculator.h"

namespace mini {

double Legalizer::calculateHPWL() const {
    if (!db_) return 0.0;

    NetlistDB* netlist_db = db_->getNetlistDB();
    if (!netlist_db) return 0.0;

    return HPWLCalculator::calculateHPWL(netlist_db, db_);
}

bool Legalizer::hasOverlaps() const {
    if (!db_) return false;

    auto cells = db_->getAllCells();
    
    for (size_t i = 0; i < cells.size(); ++i) {
        const auto& info_i = db_->getCellInfo(cells[i]);
        if (info_i.fixed) continue;

        for (size_t j = i + 1; j < cells.size(); ++j) {
            const auto& info_j = db_->getCellInfo(cells[j]);
            if (info_j.fixed) continue;

            // Check for rectangle overlap
            bool x_overlap = (info_i.x < info_j.x + info_j.width) && 
                            (info_i.x + info_i.width > info_j.x);
            bool y_overlap = (info_i.y < info_j.y + info_j.height) && 
                            (info_i.y + info_i.height > info_j.y);

            if (x_overlap && y_overlap) {
                return true;
            }
        }
    }

    return false;
}

void Legalizer::exportResult(const std::string& filename) const {
    // Create visualization directory
    std::string mkdir_cmd = "mkdir -p visualizations/legalizer";
    std::system(mkdir_cmd.c_str());
    
    // Full path for CSV file
    std::string full_path = "visualizations/legalizer/" + filename;
    std::ofstream file(full_path);
    
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << full_path << " for writing" << std::endl;
        return;
    }
    
    // CSV header
    file << "cell_name,x,y,width,height,fixed,algorithm" << std::endl;
    
    // Export all cells
    auto all_cells = db_->getAllCells();
    for (Cell* cell : all_cells) {
        const auto& cell_info = db_->getCellInfo(cell);
        
        file << cell->getName() << ","
             << cell->getX() << ","
             << cell->getY() << ","
             << cell_info.width << ","
             << cell_info.height << ","
             << (cell_info.fixed ? "1" : "0") << ","
             << getAlgorithmName() << std::endl;
    }
    
    file.close();
    
    debugLog("Result exported to " + filename);
}

} // namespace mini