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
    double utilization;             // Target core utilization
    double row_height;              // Standard cell row height (micrometers)
    bool verbose = false;           // Enable verbose output
    std::string run_id = "placement"; // Run identifier for output files
    std::string placement_algo = "basic"; // Algorithm selection: "basic" or "nesterov"
    LegalizationAlgorithm leg_algo = LegalizationAlgorithm::ABACUS; // Legalization algorithm selection
    
    // Global placement algorithm parameters
    double target_density;                          // Target utilization for global placement
    double initial_lambda;                          // Initial density penalty factor
    double lambda_growth_rate;                      // Lambda growth rate per iteration
    double learning_rate;                           // Learning rate (step size)
    double momentum;                                // Momentum factor
    double convergence_threshold;                   // Convergence threshold
    
    // Additional parameters
    double default_cell_area;                       // Default cell area when not found in library
    double site_width;                              // Standard cell site width
    double density_margin;                          // Density margin for overflow handling
    double max_gradient_ratio;                      // Maximum gradient as ratio of core width
    double max_displacement_ratio;                  // Maximum displacement as ratio of core width
    double placement_hpwl_convergence_ratio;        // HPWL convergence threshold
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