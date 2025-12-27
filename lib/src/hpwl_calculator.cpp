/**
 * @file hpwl_calculator.cpp
 * @brief Unified HPWL (Half-Perimeter Wire Length) Calculator Implementation
 */

#include "hpwl_calculator.h"
#include <algorithm>

namespace mini {

double HPWLCalculator::calculateHPWL(NetlistDB* netlist_db, PlacerDB* placer_db) {
    if (!netlist_db || !placer_db) return 0.0;

    double total_hpwl = 0.0;
    
    // Calculate HPWL for each net
    for (const auto& net_ptr : netlist_db->getNets()) {
        const Net* net = net_ptr.get();
        if (!net) continue;
        
        total_hpwl += calculateNetHPWL(net, placer_db);
    }
    
    return total_hpwl;
}

double HPWLCalculator::calculateNetHPWL(const Net* net, PlacerDB* placer_db) {
    if (!net || !placer_db) return 0.0;
    
    double min_x, max_x, min_y, max_y;
    if (!calculateNetBoundingBox(net, placer_db, min_x, max_x, min_y, max_y)) {
        return 0.0;  // Net has no valid pins
    }
    
    return (max_x - min_x) + (max_y - min_y);
}

Point HPWLCalculator::getCellCenter(const Cell* cell, PlacerDB* placer_db) {
    if (!cell || !placer_db) {
        return Point{0.0, 0.0};
    }
    
    // Try to get cell info from PlacerDB first
    const auto& cell_info = placer_db->getCellInfo(const_cast<Cell*>(cell));
    return Point{
        cell_info.x + cell_info.width / 2.0,
        cell_info.y + cell_info.height / 2.0
    };
}

bool HPWLCalculator::calculateNetBoundingBox(const Net* net, PlacerDB* placer_db,
                                             double& min_x, double& max_x,
                                             double& min_y, double& max_y) {
    // Initialize to opposite extremes
    min_x = std::numeric_limits<double>::max();
    max_x = std::numeric_limits<double>::lowest();
    min_y = std::numeric_limits<double>::max();
    max_y = std::numeric_limits<double>::lowest();
    
    bool has_pins = false;
    
    // Check driver pin
    Pin* driver = net->getDriver();
    if (driver) {
        Cell* driver_cell = driver->getOwner();
        if (driver_cell) {
            Point center = getCellCenter(driver_cell, placer_db);
            min_x = max_x = center.x;
            min_y = max_y = center.y;
            has_pins = true;
        }
    }
    
    // Check load pins
    for (Pin* load : net->getLoads()) {
        if (!load) continue;
        Cell* load_cell = load->getOwner();
        if (!load_cell) continue;
        
        Point center = getCellCenter(load_cell, placer_db);
        min_x = std::min(min_x, center.x);
        max_x = std::max(max_x, center.x);
        min_y = std::min(min_y, center.y);
        max_y = std::max(max_y, center.y);
        has_pins = true;
    }
    
    return has_pins;
}

} // namespace mini