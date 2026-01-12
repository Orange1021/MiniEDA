/**
 * @file routing_grid.h
 * @brief MiniRouter v2.0 - Routing Grid Definition
 * @details Robust and minimalistic routing grid design for A* pathfinding
 */

#ifndef MINI_ROUTING_GRID_H
#define MINI_ROUTING_GRID_H

#include <vector>
#include <functional>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <string>
#include <stdexcept>
#include "../../lib/include/geometry.h"

namespace mini {

// ============================================================================
// Core Definitions
// ============================================================================

/**
 * @enum GridState
 * @brief State machine for each grid point
 */
enum class GridState {
    FREE,      ///< Default state, can be routed
    OBSTACLE,  ///< Blocked by macro internal metal or blockage
    ROUTED,    ///< Occupied by previous net (dynamic obstacle)
    VIA,       ///< Via hole connecting upper and lower layers
    PIN        ///< Destination marker (target), accessible even in obstacles
};

/**
 * @struct GridPoint
 * @brief 3D coordinate point for A* algorithm operations
 */
struct GridPoint {
    int x, y, layer;  ///< Coordinates: layer (0=M1, 1=M2), x, y positions

    // Constructor
    GridPoint() : x(0), y(0), layer(0) {}
    GridPoint(int x_val, int y_val, int layer_val) : x(x_val), y(y_val), layer(layer_val) {}

    // Equality operator (required for comparisons)
    bool operator==(const GridPoint& other) const {
        return x == other.x && y == other.y && layer == other.layer;
    }

    // ============ Convenience Methods ============
    /**
     * @brief Create a new GridPoint with the same x,y but different layer
     * @param new_layer New layer value
     * @return New GridPoint with specified layer
     * @note This is a const method, does not modify the original object
     */
    GridPoint withLayer(int new_layer) const {
        return GridPoint(x, y, new_layer);
    }
};

// Hash function for GridPoint (required for std::unordered_map keys)
struct GridPointHash {
    std::size_t operator()(const GridPoint& gp) const {
        // Combine hash values using a simple but effective approach
        return std::hash<int>()(gp.x) ^ 
               (std::hash<int>()(gp.y) << 1) ^ 
               (std::hash<int>()(gp.layer) << 2);
    }
};

// ============================================================================
// Grid Node Structure
// ============================================================================

/**
 * @struct GridNode
 * @brief Individual grid cell with state and ownership information
 */
struct GridNode {
    GridState state;      ///< Current state of the cell
    int net_id;          ///< Owner net ID (0 = unowned, >0 = net ID)
    int usage_count;     ///< Number of different nets that used this cell (for conflict detection)

    // Constructor
    GridNode() : state(GridState::FREE), net_id(0), usage_count(0) {}
    GridNode(GridState s, int id = 0) : state(s), net_id(id), usage_count(0) {}

    // ============ State Check Methods ============
    /**
     * @brief Check if the cell is in FREE state
     */
    bool isFree() const { return state == GridState::FREE; }

    /**
     * @brief Check if the cell is an OBSTACLE
     */
    bool isObstacle() const { return state == GridState::OBSTACLE; }

    /**
     * @brief Check if the cell is ROUTED (occupied by a net)
     */
    bool isRouted() const { return state == GridState::ROUTED; }

    /**
     * @brief Check if the cell is a VIA
     */
    bool isVia() const { return state == GridState::VIA; }

    /**
     * @brief Check if the cell is a PIN
     */
    bool isPin() const { return state == GridState::PIN; }

    // ============ Combined State Check Methods ============
    /**
     * @brief Check if the cell is accessible (FREE or PIN)
     * @details These states allow routing even in PathFinder mode
     */
    bool isAccessible() const {
        return state == GridState::FREE || state == GridState::PIN;
    }

    /**
     * @brief Check if the cell is blocked (OBSTACLE or ROUTED)
     * @details These states prevent normal routing
     */
    bool isBlocked() const {
        return state == GridState::OBSTACLE || state == GridState::ROUTED;
    }

    /**
     * @brief Check if this grid node has conflicts
     * @return true if routed and used by multiple different nets
     */
    bool isConflicted() const {
        return isRouted() && usage_count > 0;
    }

    // ============ State Conversion Methods ============
    /**
     * @brief Convert state to string representation
     * @return String representation of the state
     */
    std::string toString() const {
        switch (state) {
            case GridState::FREE: return "FREE";
            case GridState::OBSTACLE: return "OBSTACLE";
            case GridState::ROUTED: return "ROUTED";
            case GridState::VIA: return "VIA";
            case GridState::PIN: return "PIN";
            default: return "UNKNOWN";
        }
    }
};

// ============================================================================
// RoutingGrid Class
// ============================================================================

/**
 * @class RoutingGrid
 * @brief Physical to discrete world mapping and grid management
 */
class RoutingGrid {
private:
    // --- Physical Parameters (from init) ---
    Rect core_area_;      ///< Chip core area (origin at x_min, y_min)
    double pitch_x_;      ///< X-direction grid spacing (e.g., 0.19um)
    double pitch_y_;      ///< Y-direction grid spacing (e.g., 0.19um)

    // --- Discrete Dimensions (calculated) ---
    int grid_width_;      ///< Total number of X-direction cells
    int grid_height_;     ///< Total number of Y-direction cells
    int num_layers_;      ///< Number of layers (upgraded to 3: M1, M2, M3)

    // --- Core Data Storage ---
    /// Flattened 1D array for better cache performance
    /// Linear index: (layer * height + y) * width + x
    std::vector<GridNode> grid_flat_;
    
    // --- Congestion Tracking for PathFinder Algorithm ---
    /// Flattened history cost matrix for better cache performance
    std::vector<double> history_costs_flat_;
    
    // Total size of flattened arrays (width * height * layers)
    int total_grid_size_;
    
    // --- Precomputed Neighbors for Performance ---
    /// Precomputed neighbors for each grid point to avoid repeated calculations
    /// Linear index: (layer * height + y) * width + x
    std::vector<std::vector<GridPoint>> precomputed_neighbors_;
    
    /// Flag to enable/disable neighbor precomputation
    bool enable_neighbor_precomputation_;
    
    // Dynamic history cost increment for aggressive PathFinder
    double history_increment_;

public:
    // Constructor
    RoutingGrid();

    // Core Methods
    void init(const Rect& core_area, double pitch_x, double pitch_y, int num_layers = 3);
    GridPoint physToGrid(double x, double y, int layer) const;
    std::vector<GridPoint> getNeighbors(const GridPoint& current, int current_net_id = 0) const;
    void addObstacle(const Rect& phys_rect, int layer);
    
    /**
     * @brief Precompute neighbors for all grid points
     * @details Called during grid initialization to avoid repeated neighbor calculations
     */
    void precomputeNeighbors();
    
    // State Management
    GridState getState(const GridPoint& gp) const;
    void setState(const GridPoint& gp, GridState state, int net_id = 0);
    bool isFree(const GridPoint& gp, int current_net_id = 0) const;
    int getNetId(const GridPoint& gp) const;
    
    /**
     * @brief Mark a grid point as a PIN
     * @param gp Grid point to mark
     * @param net_id Net ID that owns this pin
     * @details Convenience method for setting PIN state with net ID
     */
    void markPin(const GridPoint& gp, int net_id) {
        setState(gp, GridState::PIN, net_id);
    }
    
    // Utility Methods
    bool isValid(const GridPoint& gp) const;
    void clear();
    void markPath(const std::vector<GridPoint>& path, int net_id = 0);
    
    // --- PathFinder Congestion Management ---
    /**
     * @brief Initialize history costs matrix
     * @details Called during grid initialization to set up congestion tracking
     */
    void initializeHistoryCosts();
    
    /**
     * @brief Add history cost penalty to a grid point
     * @param x Grid X coordinate
     * @param y Grid Y coordinate  
     * @param z Grid layer
     * @param increment Penalty increment (typically 1.0)
     */
    void addHistoryCost(int x, int y, int z, double increment);
    
    /**
     * @brief Get history cost for a grid point
     * @param x Grid X coordinate
     * @param y Grid Y coordinate
     * @param z Grid layer
     * @return History cost multiplier (1.0 = no penalty, higher = more expensive)
     */
    double getHistoryCost(int x, int y, int z) const;
    
    /**
     * @brief Set history cost for a grid point (for decay mechanism)
     * @param x, y, z Grid coordinates
     * @param cost New history cost value
     */
    void setHistoryCost(int x, int y, int z, double cost);
    
    /**
     * @brief Set the history cost increment for congested cells
     * @param increment History cost increment value
     */
    void setHistoryIncrement(double increment) { history_increment_ = increment; }
    
    /**
     * @brief Get the current history cost increment
     * @return Current history increment value
     */
    double getHistoryIncrement() const { return history_increment_; }
    
    /**
     * @brief Clear all routing traces but preserve history costs
     * @details Critical for PathFinder iterations - removes ROUTED states
     *          but keeps accumulated congestion penalties
     */
    void clearRoutes();
    
    /**
     * @brief Rip up (remove) routes for a specific net only
     * @details Used for selective rerouting - removes ROUTED states for specified net
     *          while preserving routes of other nets as obstacles
     * @param net_id ID of the net to rip up
     */
    void ripUpNet(int net_id);
    
    /**
     * @brief Count total conflicts (overlaps) in current routing
     * @return Number of grid points with multiple net assignments
     */
    int countConflicts(bool verbose = false) const;
    
    /**
     * @brief Get set of net IDs involved in conflicts
     * @return Set of net IDs that have conflicts
     */
    std::unordered_set<int> getConflictedNetIDs() const;
    
    /**
     * @brief Print detailed conflict locations for debugging
     * @param current_net_id Current net ID being routed (for enemy/friend identification)
     */
    void printConflictLocations(int current_net_id = 0, bool verbose = false) const;
    
    /**
     * @brief Calculate movement cost for PathFinder algorithm
     * @param from Source grid point
     * @param to Destination grid point
     * @param current_net_id Current net ID
     * @param collision_penalty Current collision penalty factor
     * @return Total movement cost (base_cost * history_cost + collision_penalty)
     */
    double calculateMovementCost(const GridPoint& from, const GridPoint& to, 
                                 int current_net_id, double collision_penalty) const;
    
    // Getters
    int getGridWidth() const { return grid_width_; }
    int getGridHeight() const { return grid_height_; }
    int getNumLayers() const { return num_layers_; }
    double getPitchX() const { return pitch_x_; }
    double getPitchY() const { return pitch_y_; }
    const Rect& getCoreArea() const { return core_area_; }

private:
    // Helper Methods
    int clamp(int value, int min_val, int max_val) const {
        return std::max(min_val, std::min(value, max_val));
    }

    /**
     * @brief Convert 3D grid point to 1D linear index
     * @param gp Grid point to convert
     * @return Linear index
     * @details Linear index formula: (layer * height + y) * width + x
     */
    int toLinearIndex(const GridPoint& gp) const {
        return (gp.layer * grid_height_ + gp.y) * grid_width_ + gp.x;
    }

    /**
     * @brief Get grid node with bounds checking (const version)
     * @param gp Grid point
     * @return Const reference to grid node
     * @throws std::out_of_range if point is out of bounds
     */
    const GridNode& getGridNode(const GridPoint& gp) const {
        if (!isValid(gp)) {
            throw std::out_of_range("Grid point out of bounds");
        }
        return grid_flat_[toLinearIndex(gp)];
    }

    /**
     * @brief Get grid node with bounds checking (non-const version)
     * @param gp Grid point
     * @return Reference to grid node
     * @throws std::out_of_range if point is out of bounds
     */
    GridNode& getGridNode(const GridPoint& gp) {
        if (!isValid(gp)) {
            throw std::out_of_range("Grid point out of bounds");
        }
        return grid_flat_[toLinearIndex(gp)];
    }
};

} // namespace mini

#endif // MINI_ROUTING_GRID_H