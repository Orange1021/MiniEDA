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
#include "../../lib/include/app_config.h"
#include "legalizer.h"

namespace mini {

class PlacerDB;
class Visualizer;

/**
 * @class PlacementInterface
 * @brief High-level interface for running complete placement flow
 * @details Encapsulates all placement steps: parsing, initialization, and optimization
 * @note Uses AppConfig as single source of truth for all placement parameters
 */
class PlacementInterface {
public:
    /**
     * @brief Run complete placement flow
     * @param config Application configuration (contains all placement parameters)
     * @param netlist_db Netlist database
     * @return Unique pointer to placement database with results
     */
    static std::unique_ptr<PlacerDB> runPlacement(
        const AppConfig& config,
        std::shared_ptr<NetlistDB> netlist_db
    );

    /**
     * @brief Run placement with visualization
     * @param config Application configuration (contains all placement parameters)
     * @param netlist_db Netlist database
     * @param visualizer Optional visualizer for debug output
     * @return Unique pointer to placement database with results
     */
    static std::unique_ptr<PlacerDB> runPlacementWithVisualization(
        const AppConfig& config,
        std::shared_ptr<NetlistDB> netlist_db,
        Visualizer* visualizer = nullptr
    );
};

} // namespace mini

#endif // MINI_PLACEMENT_INTERFACE_H