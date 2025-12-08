/**
 * @file routing_interface.h
 * @brief High-level Routing Interface for MiniEDA
 * @details Provides a simple interface to run complete routing flow
 */

#ifndef MINI_ROUTING_INTERFACE_H
#define MINI_ROUTING_INTERFACE_H

#include <memory>
#include <string>
#include <vector>
#include "../../lib/include/netlist_db.h"

namespace mini {

class PlacerDB;
class Visualizer;
struct RoutingResult;

/**
 * @struct RoutingConfig
 * @brief Configuration parameters for routing
 */
struct RoutingConfig {
    std::string lef_file;           // LEF physical library file
    std::string liberty_file;       // Liberty timing library file
    double via_cost = 10.0;         // Via penalty cost multiplier
    double wire_cost = 1.0;         // Wire cost per unit length
    bool verbose = false;           // Enable verbose output
    std::string run_id = "routing";  // Run identifier for output files
};

/**
 * @class RoutingInterface
 * @brief High-level interface for running complete routing flow
 * @details Encapsulates all routing steps: I/O placement, grid initialization, and maze routing
 */
class RoutingInterface {
public:
    /**
     * @brief Run complete routing flow
     * @param config Routing configuration
     * @param netlist_db Netlist database
     * @param placer_db Placement database with cell locations
     * @return Vector of routing results for visualization
     */
    static std::vector<RoutingResult> runRouting(
        const RoutingConfig& config,
        std::shared_ptr<NetlistDB> netlist_db,
        std::unique_ptr<PlacerDB>& placer_db
    );

    /**
     * @brief Run routing with visualization
     * @param config Routing configuration
     * @param netlist_db Netlist database
     * @param placer_db Placement database with cell locations
     * @param visualizer Optional visualizer for debug output
     * @return Vector of routing results for visualization
     */
    static std::vector<RoutingResult> runRoutingWithVisualization(
        const RoutingConfig& config,
        std::shared_ptr<NetlistDB> netlist_db,
        std::unique_ptr<PlacerDB>& placer_db,
        Visualizer* visualizer = nullptr
    );

    /**
     * @brief Get routing statistics
     * @param results Vector of routing results
     * @param total_wirelength Output total wirelength
     * @param total_vias Output total vias
     * @param routed_count Output number of successfully routed nets
     * @param failed_count Output number of failed nets
     */
    static void getRoutingStatistics(
        const std::vector<RoutingResult>& results,
        double& total_wirelength,
        int& total_vias,
        int& routed_count,
        int& failed_count
    );
};

} // namespace mini

#endif // MINI_ROUTING_INTERFACE_H