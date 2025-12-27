/**
 * @file hpwl_calculator.h
 * @brief Unified HPWL (Half-Perimeter Wire Length) Calculator
 * @details Provides a single, consistent implementation for HPWL calculation
 *          to eliminate code duplication across placement components
 */

#ifndef MINI_HPWL_CALCULATOR_H
#define MINI_HPWL_CALCULATOR_H

#include <limits>
#include "netlist_db.h"
#include "placer_db.h"

namespace mini {

/**
 * @class HPWLCalculator
 * @brief Unified HPWL calculation utility
 * @details Eliminates code duplication by providing a single, well-tested
 *          implementation for HPWL calculation across all placement modules
 */
class HPWLCalculator {
public:
    /**
     * @brief Calculate HPWL for a netlist using PlacerDB
     * @param netlist_db Netlist database (must be valid)
     * @param placer_db Placement database with cell positions (must be valid)
     * @return Total HPWL value, 0.0 if inputs are invalid
     * @details HPWL = Sum((max_x - min_x) + (max_y - min_y)) for all nets
     *          Uses cell center coordinates for pin positions
     */
    static double calculateHPWL(NetlistDB* netlist_db, PlacerDB* placer_db);

    /**
     * @brief Calculate HPWL for a single net
     * @param net Single net to calculate HPWL for
     * @param placer_db Placement database with cell positions
     * @return HPWL for the net, 0.0 if net has no pins
     */
    static double calculateNetHPWL(const Net* net, PlacerDB* placer_db);

    /**
     * @brief Get cell center coordinates
     * @param cell Cell pointer
     * @param placer_db Placement database
     * @return Point representing cell center
     */
    static Point getCellCenter(const Cell* cell, PlacerDB* placer_db);

private:
    /**
     * @brief Calculate bounding box for a net
     * @param net Net to calculate bounding box for
     * @param placer_db Placement database
     * @param min_x Output: minimum X coordinate
     * @param max_x Output: maximum X coordinate
     * @param min_y Output: minimum Y coordinate
     * @param max_y Output: maximum Y coordinate
     * @return true if net has valid pins, false otherwise
     */
    static bool calculateNetBoundingBox(const Net* net, PlacerDB* placer_db,
                                       double& min_x, double& max_x,
                                       double& min_y, double& max_y);
};

} // namespace mini

#endif // MINI_HPWL_CALCULATOR_H