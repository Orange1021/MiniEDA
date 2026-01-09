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
#include "../../lib/include/app_config.h"

namespace mini {

class PlacerDB;
class Visualizer;
struct RoutingResult;

/**
 * @class RoutingInterface
 * @brief High-level interface for running complete routing flow
 * @details Encapsulates all routing steps: I/O placement, grid initialization, and maze routing
 * @note Uses AppConfig as single source of truth for all routing parameters
 */
class RoutingInterface {
public:
    /**
     * @brief Run complete routing flow
     * @param config Application configuration (contains all routing parameters)
     * @param netlist_db Netlist database
     * @param placer_db Placement database with cell locations
     * @return Vector of routing results for visualization
     */
    static std::vector<RoutingResult> runRouting(
        const AppConfig& config,
        std::shared_ptr<NetlistDB> netlist_db,
        std::unique_ptr<PlacerDB>& placer_db
    );

    /**
     * @brief Run routing with visualization
     * @param config Application configuration (contains all routing parameters)
     * @param netlist_db Netlist database
     * @param placer_db Placement database with cell locations
     * @param visualizer Optional visualizer for debug output
     * @return Vector of routing results for visualization
     */
    static std::vector<RoutingResult> runRoutingWithVisualization(
        const AppConfig& config,
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