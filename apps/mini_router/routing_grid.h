/**
 * @file routing_grid.h
 * @brief Routing Grid for MiniRouter
 * @details Core data structure for 2-layer HV routing grid representation
 */

#ifndef MINI_ROUTING_GRID_H
#define MINI_ROUTING_GRID_H

#include "../../lib/include/geometry.h"
#include <vector>
#include <unordered_set>
#include <memory>

namespace mini {

/**
 * @brief Grid point in 3D routing space (x, y, layer)
 */
struct GridPoint {
    int x, y;        ///< Grid coordinates
    int layer;       ///< Layer: 0=M1 (horizontal), 1=M2 (vertical)
    
    GridPoint() : x(0), y(0), layer(0) {}
    GridPoint(int x_, int y_, int layer_) : x(x_), y(y_), layer(layer_) {}
    
    // Equality operator for unordered_map and unordered_set
    bool operator==(const GridPoint& other) const {
        return x == other.x && y == other.y && layer == other.layer;
    }
};

/**
 * @brief Hash function for GridPoint
 */
struct GridPointHash {
    size_t operator()(const GridPoint& gp) const {
        // Combine hashes using bit manipulation for better distribution
        size_t h1 = std::hash<int>{}(gp.x);
        size_t h2 = std::hash<int>{}(gp.y);
        size_t h3 = std::hash<int>{}(gp.layer);
        
        // Use a common hash combination technique
        return h1 ^ (h2 << 1) ^ (h2 >> 1) ^ (h3 << 2) ^ (h3 >> 2);
    }
};

/**
 * @brief Grid cell state enumeration
 */
enum class GridState {
    FREE,        ///< Available for routing
    OBSTACLE,    ///< Occupied by cell or blocked
    ROUTED,      ///< Already used by a net
    VIA          ///< Via location connecting M1-M2
};

/**
 * @brief Routing Grid Class
 * @details Represents the chip area as a 3D grid for maze routing
 */
class RoutingGrid {
public:
    /**
     * @brief Constructor
     */
    RoutingGrid();
    
    /**
     * @brief Destructor
     */
    ~RoutingGrid();

    // ============ Grid Initialization ============
    
    /**
     * @brief Initialize grid based on core area and routing pitch
     * @param core_area Core area rectangle for routing
     * @param pitch_x Horizontal routing pitch (in micrometers)
     * @param pitch_y Vertical routing pitch (in micrometers)
     */
    void init(const Rect& core_area, double pitch_x = 0.2, double pitch_y = 0.2);
    
    /**
     * @brief Get grid dimensions
     * @return Pair of (width, height) in grid cells
     */
    std::pair<int, int> getGridSize() const { return {grid_width_, grid_height_}; }
    
    /**
     * @brief Get routing pitch
     * @return Pair of (pitch_x, pitch_y) in micrometers
     */
    std::pair<double, double> getPitch() const { return {pitch_x_, pitch_y_}; }

    // ============ Obstacle Management ============
    
    /**
     * @brief Add obstacle (occupied by cell)
     * @param rect Obstacle rectangle in physical coordinates
     * @param layer Layer where obstacle exists (0=M1, 1=M2, -1=all layers)
     */
    void addObstacle(const Rect& rect, int layer = -1);
    
    /**
     * @brief Mark grid point as occupied
     * @param gp Grid point to mark
     * @param state New state for the grid point
     */
    void setGridState(const GridPoint& gp, GridState state);

    // ============ Coordinate Conversion ============
    
    /**
     * @brief Convert physical coordinates to grid coordinates
     * @param phys_x Physical x coordinate
     * @param phys_y Physical y coordinate
     * @param layer Layer number
     * @return Grid point
     */
    GridPoint physToGrid(double phys_x, double phys_y, int layer) const;
    
    /**
     * @brief Convert grid coordinates to physical coordinates
     * @param gp Grid point
     * @return Physical point (center of grid cell)
     */
    Point gridToPhys(const GridPoint& gp) const;
    
    /**
     * @brief Get physical bounds of a grid cell
     * @param gp Grid point
     * @return Rectangle representing the grid cell
     */
    Rect getGridCellRect(const GridPoint& gp) const;

    // ============ Grid State Queries ============
    
    /**
     * @brief Check if grid point is valid (within bounds)
     * @param gp Grid point to check
     * @return True if valid, false otherwise
     */
    bool isValid(const GridPoint& gp) const;
    
    /**
     * @brief Check if grid point is available for routing
     * @param gp Grid point to check
     * @return True if free for routing, false otherwise
     */
    bool isFree(const GridPoint& gp) const;
    
    /**
     * @brief Get grid state at point
     * @param gp Grid point
     * @return Grid state
     */
    GridState getGridState(const GridPoint& gp) const;

    // ============ Neighbor Queries ============
    
    /**
     * @brief Get valid neighboring grid points
     * @param gp Current grid point
     * @return Vector of valid neighboring points
     * @details For M1: horizontal neighbors only; For M2: vertical neighbors only
     */
    std::vector<GridPoint> getNeighbors(const GridPoint& gp) const;
    
    /**
     * @brief Get via points (same x,y on different layers)
     * @param gp Current grid point
     * @return Vector of via points
     */
    std::vector<GridPoint> getViaPoints(const GridPoint& gp) const;

    // ============ Statistics ============
    
    /**
     * @brief Get routing statistics
     * @return Map of state counts
     */
    std::unordered_map<GridState, size_t> getStatistics() const;
    
    /**
     * @brief Calculate routing resource utilization
     * @return Utilization percentage (0.0 to 1.0)
     */
    double getUtilization() const;

private:
    // Grid dimensions
    int grid_width_;       ///< Number of grid cells in X direction
    int grid_height_;      ///< Number of grid cells in Y direction
    int num_layers_;       ///< Number of routing layers (fixed at 2)
    
    // Physical parameters
    double pitch_x_;       ///< Horizontal routing pitch (micrometers)
    double pitch_y_;       ///< Vertical routing pitch (micrometers)
    Rect core_area_;       ///< Core area for routing
    
    // Grid data structure
    std::vector<std::vector<std::vector<GridState>>> grid_;  ///< 3D grid [layer][y][x]
    
    // Helper methods
    void resizeGrid();
    void markObstacleCells(int x_start, int y_start, int x_end, int y_end, int layer);
};

} // namespace mini

#endif // MINI_ROUTING_GRID_H