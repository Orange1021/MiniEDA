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
    
    // Constructor
    GridNode() : state(GridState::FREE), net_id(0) {}
    GridNode(GridState s, int id = 0) : state(s), net_id(id) {}
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
    /// 3D array: [layer][y][x] -> GridNode
    /// Using nested vectors for dynamic resizing
    std::vector<std::vector<std::vector<GridNode>>> grid_;

public:
    // Constructor
    RoutingGrid();

    // Core Methods
    void init(const Rect& core_area, double pitch_x, double pitch_y);
    GridPoint physToGrid(double x, double y, int layer) const;
    std::vector<GridPoint> getNeighbors(const GridPoint& current, int current_net_id = 0) const;
    void addObstacle(const Rect& phys_rect, int layer);
    
    // State Management
    GridState getState(const GridPoint& gp) const;
    void setState(const GridPoint& gp, GridState state, int net_id = 0);
    bool isFree(const GridPoint& gp, int current_net_id = 0) const;
    int getNetId(const GridPoint& gp) const;
    
    // Utility Methods
    bool isValid(const GridPoint& gp) const;
    void clear();
    void markPath(const std::vector<GridPoint>& path, int net_id = 0);
    
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
};

} // namespace mini

#endif // MINI_ROUTING_GRID_H