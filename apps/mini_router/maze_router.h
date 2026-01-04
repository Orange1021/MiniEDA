/**
 * @file maze_router.h
 * @brief MiniRouter v2.0 - Maze Router with A* Algorithm
 * @details Textbook-level A* pathfinding implementation for net routing
 */

#ifndef MINI_MAZE_ROUTER_H
#define MINI_MAZE_ROUTER_H

#include "routing_grid.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <memory>
#include <string>

// Forward declarations
namespace mini {
    class Net;
    class Pin;
    class LefPinMapper;
    class PlacerDB;
}

namespace mini {

// ============================================================================
// Core Definitions
// ============================================================================

/**
 * @struct AStarNode
 * @brief Node for A* algorithm with cost information
 */
struct AStarNode {
    GridPoint gp;           ///< Current grid point coordinate
    double g_cost;          ///< Actual cost from start to this node (steps + bends + vias)
    double h_cost;          ///< Estimated remaining cost to goal (Manhattan distance)
    double f_cost;          ///< Total cost (F = G + H)
    
    AStarNode(const GridPoint& point, double g, double h)
        : gp(point), g_cost(g), h_cost(h), f_cost(g + h) {}
    
    // Comparator for priority queue (min-heap based on f_cost)
    bool operator>(const AStarNode& other) const {
        return f_cost > other.f_cost;
    }
};

/**
 * @struct RoutingResult
 * @brief Result of routing operation
 */
struct RoutingResult {
    bool success;                           ///< Whether routing succeeded
    std::vector<std::vector<GridPoint>> segments;  ///< Path segments for multi-pin nets
    int total_wirelength;                   ///< Total wire length in grid units
    int total_vias;                         ///< Total number of vias used
    std::string error_message;              ///< Error message if failed
    
    RoutingResult() : success(false), total_wirelength(0), total_vias(0) {}
};

// ============================================================================
// MazeRouter Class
// ============================================================================

/**
 * @class MazeRouter
 * @brief A* pathfinding router for net routing
 */
class MazeRouter {
private:
    RoutingGrid* grid_;
    LefPinMapper* pin_mapper_;  ///< [Key] Unified Key generator
    PlacerDB* placer_db_;       ///< Placement database for Steiner tree building
    
    // --- Cost Parameters ---
    double via_cost_ = 10.0;              ///< Via cost (expensive, minimize usage)
    double wire_cost_per_unit_ = 1.0;     ///< Wire cost per unit length
    
    // --- PathFinder Parameters ---
    double collision_penalty_ = 50.0;      ///< Penalty for short circuits (increases with iterations)
    
    // --- Statistics ---
    int total_routed_nets_ = 0;
    int failed_nets_ = 0;
    double total_wirelength_ = 0.0;
    int total_vias_ = 0;
    
    // **NEW**: Segments-level statistics for accurate reporting
    int total_segments_attempted_ = 0;
    int total_segments_succeeded_ = 0;
    int total_segments_failed_ = 0;
    
    // **CRITICAL FIX**: Current net ID for enemy/friend identification
    int current_net_id_ = 0;

public:
    /**
     * @brief Constructor
     * @param grid Pointer to routing grid
     * @param pin_mapper Pointer to pin mapper for key generation
     * @param placer_db Pointer to placement database (for Steiner tree building)
     */
    MazeRouter(RoutingGrid* grid, LefPinMapper* pin_mapper, PlacerDB* placer_db = nullptr);
    
    /**
     * @brief Unified entry point for routing a net
     * @param net Pointer to the net to route
     * @param pin_locations Map of pin keys to physical coordinates
     * @return Routing result
     */
    RoutingResult routeNet(Net* net, 
                          const std::unordered_map<std::string, Point>& pin_locations);
    
    /**
     * @brief Core A* pathfinding algorithm
     * @param start Starting grid point
     * @param end Ending grid point
     * @param path Output path if found
     * @return True if path found, false otherwise
     */
    bool findPath(const GridPoint& start, const GridPoint& end, 
                  std::vector<GridPoint>& path);
    
    // --- Configuration ---
    void setViaCost(double cost) { via_cost_ = cost; }
    void setWireCostPerUnit(double cost) { wire_cost_per_unit_ = cost; }
    void setCollisionPenalty(double penalty) { collision_penalty_ = penalty; }
    
    // --- Statistics ---
    int getTotalRoutedNets() const { return total_routed_nets_; }
    int getFailedNets() const { return failed_nets_; }
    double getTotalWirelength() const { return total_wirelength_; }
    int getTotalVias() const { return total_vias_; }
    
    // **NEW**: Segments-level statistics
    int getTotalSegmentsAttempted() const { return total_segments_attempted_; }
    int getTotalSegmentsSucceeded() const { return total_segments_succeeded_; }
    int getTotalSegmentsFailed() const { return total_segments_failed_; }
    double getSegmentSuccessRate() const { 
        return (total_segments_attempted_ > 0) ? 
            (double)total_segments_succeeded_ / total_segments_attempted_ * 100.0 : 0.0; 
    }
    
    /**
     * @brief Reset statistics
     */
    void resetStatistics();

private:
    /**
     * @brief Backtrack to reconstruct path from came_from map
     * @param came_from Map of node -> parent
     * @param current Current node (end point)
     * @return Reconstructed path from start to end
     */
    std::vector<GridPoint> backtrack(
        const std::unordered_map<GridPoint, GridPoint, GridPointHash>& came_from,
        const GridPoint& current) const;
    
    /**
     * @brief Calculate movement cost between two adjacent grid points
     * @param from Source grid point
     * @param to Destination grid point
     * @return Movement cost
     */
    double calculateMovementCost(const GridPoint& from, const GridPoint& to) const;
    
    /**
     * @brief Calculate Manhattan distance heuristic
     * @param from Source grid point
     * @param to Destination grid point
     * @return Manhattan distance in physical units
     */
    double manhattanDistance(const GridPoint& from, const GridPoint& to) const;
    
    /**
     * @brief Mark path on routing grid as ROUTED
     * @param path Path to mark
     */
    void markPath(const std::vector<GridPoint>& path, int net_id = 0);
    
    /**
     * @brief Calculate wire length of a path
     * @param path Path to calculate
     * @return Wire length in grid units
     */
    int calculateWirelength(const std::vector<GridPoint>& path) const;
    
    /**
     * @brief Count vias in a path
     * @param path Path to analyze
     * @return Number of vias
     */
    int countVias(const std::vector<GridPoint>& path) const;
};

} // namespace mini

#endif // MINI_MAZE_ROUTER_H