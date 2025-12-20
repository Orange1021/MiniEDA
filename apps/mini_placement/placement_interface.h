/**
 * @file placement_interface.h
 * @brief High-level Placement Interface for MiniEDA
 * @details Provides a simple interface to run complete placement flow
 */

#ifndef MINI_PLACEMENT_INTERFACE_H
#define MINI_PLACEMENT_INTERFACE_H

#include <memory>
#include <string>
#include "../../lib/include/netlist_db.h"
#include "legalizer.h"

namespace mini {

class PlacerDB;
class Visualizer;

/**
 * @struct PlacementConfig
 * @brief Configuration parameters for placement
 */
struct PlacementConfig {
    std::string lef_file;           // LEF physical library file
    std::string liberty_file;       // Liberty timing library file
    double utilization = 0.7;       // Target core utilization
    double row_height = 3.0;        // Standard cell row height (micrometers)
    bool verbose = false;           // Enable verbose output
    std::string run_id = "placement"; // Run identifier for output files
    std::string placement_algo = "basic"; // Algorithm selection: "basic" or "nesterov"
    LegalizationAlgorithm leg_algo = LegalizationAlgorithm::ABACUS; // Legalization algorithm selection
};

/**
 * @class PlacementInterface
 * @brief High-level interface for running complete placement flow
 * @details Encapsulates all placement steps: parsing, initialization, and optimization
 */
class PlacementInterface {
public:
    /**
     * @brief Run complete placement flow
     * @param config Placement configuration
     * @param netlist_db Netlist database
     * @return Unique pointer to placement database with results
     */
    static std::unique_ptr<PlacerDB> runPlacement(
        const PlacementConfig& config,
        std::shared_ptr<NetlistDB> netlist_db
    );

    /**
     * @brief Run placement with visualization
     * @param config Placement configuration
     * @param netlist_db Netlist database
     * @param visualizer Optional visualizer for debug output
     * @return Unique pointer to placement database with results
     */
    static std::unique_ptr<PlacerDB> runPlacementWithVisualization(
        const PlacementConfig& config,
        std::shared_ptr<NetlistDB> netlist_db,
        Visualizer* visualizer = nullptr
    );
};

} // namespace mini

#endif // MINI_PLACEMENT_INTERFACE_H