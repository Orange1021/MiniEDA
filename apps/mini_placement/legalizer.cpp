/**
 * @file legalizer.cpp
 * @brief Legalizer Base Class Implementation
 */

#include "legalizer.h"
#include "overlap_detector.h"
#include <iostream>
#include <fstream>
#include "../../lib/include/hpwl_calculator.h"

namespace mini {

bool Legalizer::hasOverlaps() const {
    if (!db_) return false;
    OverlapDetector detector(db_);
    return detector.hasOverlaps();
}

void Legalizer::reportOverlapCheck() const {
    bool has_overlaps = hasOverlaps();
    std::cout << "  Overlap check: " << (has_overlaps ? "FOUND OVERLAPS!" : "No overlaps") << std::endl;
}

void Legalizer::reportFinalStatistics() const {
    // Calculate and report final HPWL
    NetlistDB* netlist_db = db_->getNetlistDB();
    double final_hpwl = netlist_db ? HPWLCalculator::calculateHPWL(netlist_db, db_) : 0.0;
    std::cout << "  Final HPWL: " << final_hpwl << std::endl;

    // Check for overlaps
    reportOverlapCheck();
}

std::vector<Cell*> Legalizer::collectMovableCells() const {
    std::vector<Cell*> movable_cells;

    for (Cell* cell : db_->getAllCells()) {
        if (!db_->isCellFixed(cell)) {
            movable_cells.push_back(cell);
        }
    }

    return movable_cells;
}

} // namespace mini