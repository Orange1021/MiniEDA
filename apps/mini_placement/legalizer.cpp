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



} // namespace mini