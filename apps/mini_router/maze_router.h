/**
 * @file maze_router.h
 * @brief Maze Router Implementation for MiniRouter
 * @details Lee's Algorithm and A* Search for net routing
 */

#ifndef MINI_MAZE_ROUTER_H
#define MINI_MAZE_ROUTER_H

#include "routing_grid.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <string>

// Forward declarations to avoid circular dependencies
namespace mini {
class PinMapper;
class Net;
}

namespace mini {

/**
 * @brief Routing result for a single net
 */
struct RoutingResult {
    bool success;                           ///< Routing success status
    std::vector<GridPoint> path;           ///< Complete routing path
    double wirelength;                     ///< Total wirelength in micrometers
    int num_vias;                          ///< Number of vias used
    std::string failure_reason;            ///< Reason for failure (if any)
    
    RoutingResult() : success(false), wirelength(0.0), num_vias(0) {}
};

/**
 * @brief A* search node
 */
struct AStarNode {
    GridPoint gp;                          ///< Grid point
    double g_cost;                         ///< Cost from start
    double h_cost;                         ///< Heuristic cost to goal
    double f_cost;                         ///< Total cost (g + h)
    std::shared_ptr<AStarNode> parent;     ///< Parent pointer
    
    AStarNode(const GridPoint& point) 
        : gp(point), g_cost(0.0), h_cost(0.0), f_cost(0.0) {}
    
    // Priority queue comparison (reverse for min-heap)
    bool operator>(const AStarNode& other) const {
        return f_cost > other.f_cost;
    }
};

/**
 * @brief Maze Router Class
 * @details Implements Lee's Algorithm and A* Search for net routing
 */
class MazeRouter {
public:
    /**
     * @brief Constructor
     * @param grid Pointer to routing grid
     * @param pin_mapper Pointer to pin mapper for unified key generation
     */
    explicit MazeRouter(RoutingGrid* grid, PinMapper* pin_mapper = nullptr);
    
    /**
     * @brief Destructor
     */
    ~MazeRouter();

    // ============ Main Routing Interface ============
    
    /**
     * @brief Route a single net
     * @param net Pointer to the net to route
     * @param pin_locations Map of pin locations in physical coordinates
     * @return Routing result containing path and statistics
     */
    RoutingResult routeNet(Net* net, 
                          const std::unordered_map<std::string, Point>& pin_locations);
    
    /**
     * @brief Route multiple nets (global routing)
     * @param nets Vector of nets to route
     * @param pin_locations Map of all pin locations
     * @return Vector of routing results
     */
    std::vector<RoutingResult> routeNets(
        const std::vector<Net*>& nets,
        const std::unordered_map<std::string, Point>& pin_locations);

    // ============ Algorithm Selection ============
    
    /**
     * @brief Set routing algorithm
     * @param use_astar True for A* search, False for Lee's Algorithm
     */
    void setAlgorithm(bool use_astar) { use_astar_ = use_astar; }
    
    /**
     * @brief Enable/disable rip-up and reroute
     * @param enable True to enable rip-up and reroute
     */
    void setRipUpAndReroute(bool enable) { ripup_reroute_enabled_ = enable; }
    
    /**
     * @brief Set maximum routing attempts per net
     * @param max_attempts Maximum number of attempts
     */
    void setMaxAttempts(int max_attempts) { max_attempts_ = max_attempts; }

    // ============ Cost Functions ============
    
    /**
     * @brief Set via cost multiplier
     * @param via_cost Cost multiplier for via usage
     */
    void setViaCost(double via_cost) { via_cost_ = via_cost; }
    
    /**
     * @brief Set congestion cost multiplier
     * @param congestion_cost Cost multiplier for congested areas
     */
    void setCongestionCost(double congestion_cost) { congestion_cost_ = congestion_cost; }

    // ============ Statistics ============
    
    /**
     * @brief Get routing statistics
     * @return Map of statistic names to values
     */
    std::unordered_map<std::string, double> getStatistics() const;

private:
    // Core components
    RoutingGrid* grid_;                    ///< Routing grid
    PinMapper* pin_mapper_;                 ///< Pin mapper for unified key generation
    bool use_astar_;                       ///< Algorithm selection flag
    bool ripup_reroute_enabled_;           ///< Rip-up and reroute flag
    int max_attempts_;                     ///< Maximum routing attempts
    
    // Cost parameters
    double via_cost_;                      ///< Via cost multiplier
    double congestion_cost_;               ///< Congestion cost multiplier
    
    // Statistics
    mutable std::unordered_map<std::string, double> stats_;

    // ============ Core Routing Algorithms ============
    
    /**
     * @brief Lee's Algorithm (BFS) for shortest path
     * @param source Starting grid point
     * @param target Target grid point
     * @param path Output path from source to target
     * @return True if path found, false otherwise
     */
    bool leeAlgorithm(const GridPoint& source, const GridPoint& target, 
                     std::vector<GridPoint>& path);
    
    /**
     * @brief A* Search for optimal path
     * @param source Starting grid point
     * @param target Target grid point
     * @param path Output path from source to target
     * @return True if path found, false otherwise
     */
    bool aStarSearch(const GridPoint& source, const GridPoint& target,
                    std::vector<GridPoint>& path);
    
    /**
     * @brief Multi-pin routing (Steiner tree approach)
     * @param pins Vector of pin grid points
     * @param paths Output vector of paths connecting all pins
     * @return True if all pins connected, false otherwise
     */
    bool routeMultiPin(const std::vector<GridPoint>& pins, 
                      std::vector<std::vector<GridPoint>>& paths);

    // ============ Helper Functions ============
    
    /**
     * @brief Calculate Manhattan distance heuristic
     * @param from Starting point
     * @param to Target point
     * @return Manhattan distance
     */
    double manhattanDistance(const GridPoint& from, const GridPoint& to) const;
    
    /**
     * @brief Calculate movement cost between adjacent points
     * @param from Current point
     * @param next Next point
     * @return Movement cost
     */
    double getMovementCost(const GridPoint& from, const GridPoint& next) const;
    
    /**
     * @brief Reconstruct path from parent pointers
     * @param target Target point
     * @param came_from Map of parent pointers
     * @param path Output path (reversed order)
     */
    void reconstructPath(const GridPoint& target,
                        const std::unordered_map<GridPoint, GridPoint, GridPointHash>& came_from,
                        std::vector<GridPoint>& path) const;
    
    /**
     * @brief Mark path as routed in the grid
     * @param path Path to mark
     */
    void markPathRouted(const std::vector<GridPoint>& path);
    
    /**
     * @brief Unmark path from the grid (for rip-up)
     * @param path Path to unmark
     */
    void unmarkPath(const std::vector<GridPoint>& path);
    
    /**
     * @brief Convert pin physical location to grid point
     * @param pin_name Pin name
     * @param pin_locations Map of pin locations
     * @param preferred_layer Preferred routing layer
     * @return Grid point for the pin
     */
    GridPoint pinToGridPoint(const std::string& pin_name,
                            const std::unordered_map<std::string, Point>& pin_locations,
                            int preferred_layer = 0) const;
    
    /**
     * @brief Calculate wirelength of a path
     * @param path Routing path
     * @return Total wirelength in micrometers
     */
    double calculateWirelength(const std::vector<GridPoint>& path) const;
    
    /**
     * @brief Count number of vias in a path
     * @param path Routing path
     * @return Number of vias
     */
    int countVias(const std::vector<GridPoint>& path) const;
    
    /**
     * @brief Update routing statistics
     * @param result Routing result to include in statistics
     */
    void updateStatistics(const RoutingResult& result);
};

} // namespace mini

#endif // MINI_MAZE_ROUTER_H