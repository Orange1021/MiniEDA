/**
 * @file routing_grid.cpp
 * @brief MiniRouter v2.0 - Routing Grid Implementation
 */

#include "routing_grid.h"
#include <stdexcept>
#include <iostream>

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

RoutingGrid::RoutingGrid() 
    : core_area_(0.0, 0.0, 0.0, 0.0), pitch_x_(0.0), pitch_y_(0.0), grid_width_(0), grid_height_(0), num_layers_(3) {
    // Grid will be initialized in init() method
}

// ============================================================================
// Core Methods
// ============================================================================

void RoutingGrid::init(const Rect& core_area, double pitch_x, double pitch_y) {
    // Store physical parameters
    core_area_ = core_area;
    pitch_x_ = pitch_x;
    pitch_y_ = pitch_y;
    
    // Validate inputs
    if (pitch_x_ <= 0.0 || pitch_y_ <= 0.0) {
        throw std::invalid_argument("Pitch values must be positive");
    }
    
    // Calculate discrete dimensions using ceil to ensure full coverage
    double core_width = core_area_.x_max - core_area_.x_min;
    double core_height = core_area_.y_max - core_area_.y_min;
    
    grid_width_ = static_cast<int>(std::ceil(core_width / pitch_x_));
    grid_height_ = static_cast<int>(std::ceil(core_height / pitch_y_));
    
    // Resize 3D grid: [layer][y][x]
    grid_.resize(num_layers_);
    for (int layer = 0; layer < num_layers_; ++layer) {
        grid_[layer].resize(grid_height_);
        for (int y = 0; y < grid_height_; ++y) {
            grid_[layer][y].resize(grid_width_, GridNode(GridState::FREE, 0));
        }
    }
    
    std::cout << "RoutingGrid initialized: " << grid_width_ << "x" << grid_height_ 
              << " x" << num_layers_ << " layers" << std::endl;
    std::cout << "Core area: (" << core_area_.x_min << "," << core_area_.y_min 
              << ") to (" << core_area_.x_max << "," << core_area_.y_max << ")" << std::endl;
    std::cout << "Pitch: X=" << pitch_x_ << "um, Y=" << pitch_y_ << "um" << std::endl;
    
    // Initialize congestion tracking
    initializeHistoryCosts();
}

void RoutingGrid::initializeHistoryCosts() {
    // Resize history costs matrix to match grid dimensions
    history_costs_.resize(num_layers_);
    for (int layer = 0; layer < num_layers_; ++layer) {
        history_costs_[layer].resize(grid_height_);
        for (int y = 0; y < grid_height_; ++y) {
            history_costs_[layer][y].resize(grid_width_, 1.0);  // Base cost = 1.0
        }
    }
}

void RoutingGrid::addHistoryCost(int x, int y, int z, double increment) {
    GridPoint gp(x, y, z);
    if (isValid(gp)) {
        history_costs_[z][y][x] += increment;
    }
}

double RoutingGrid::getHistoryCost(int x, int y, int z) const {
    GridPoint gp(x, y, z);
    if (isValid(gp)) {
        return history_costs_[z][y][x];
    }
    return 1e9;  // Out of bounds protection - very expensive
}

void RoutingGrid::clearRoutes() {
    // Clear all ROUTED states but preserve history costs
    for (int z = 0; z < num_layers_; ++z) {
        for (int y = 0; y < grid_height_; ++y) {
            for (int x = 0; x < grid_width_; ++x) {
                if (grid_[z][y][x].state == GridState::ROUTED) {
                    grid_[z][y][x].state = GridState::FREE;
                    grid_[z][y][x].net_id = 0;
                }
            }
        }
    }
}

int RoutingGrid::countConflicts() const {
    int conflicts = 0;
    
    // Count grid points that are used by multiple nets
    for (int z = 0; z < num_layers_; ++z) {
        for (int y = 0; y < grid_height_; ++y) {
            for (int x = 0; x < grid_width_; ++x) {
                if (grid_[z][y][x].state == GridState::ROUTED) {
                    // Check if this point is also claimed by another net
                    // (This is a simplified check - a full implementation would track overlaps)
                    // For now, we count any ROUTED point as potentially conflicting
                    conflicts++;
                }
            }
        }
    }
    
    return conflicts;
}

double RoutingGrid::calculateMovementCost(const GridPoint& from, const GridPoint& to, 
                                           int current_net_id, double collision_penalty) const {
    // Base movement cost
    double base_cost = 1.0;
    
    // Get history cost for destination
    double history_cost = getHistoryCost(to.x, to.y, to.layer);
    
    // Check for collision penalty (short circuit cost)
    double collision_cost = 0.0;
    if (isValid(to)) {
        const GridNode& node = grid_[to.layer][to.y][to.x];
        if (node.state == GridState::ROUTED && node.net_id != current_net_id) {
            // This is a short circuit - add collision penalty
            collision_cost = collision_penalty;
        }
    }
    
    // Total cost = base * history + collision
    return base_cost * history_cost + collision_cost;
}

GridPoint RoutingGrid::physToGrid(double x, double y, int layer) const {
    // 1. Calculate relative coordinates
    double rel_x = x - core_area_.x_min;
    double rel_y = y - core_area_.y_min;
    
    // 2. Discretize using round (critical fix: not floor!)
    // round ensures 3.19um (pitch 0.2) maps to nearest grid point 16, not floored to 15
    int gx = static_cast<int>(std::round(rel_x / pitch_x_));
    int gy = static_cast<int>(std::round(rel_y / pitch_y_));
    
    // 3. Boundary clamping to prevent array out-of-bounds crashes
    // This handles pins that are slightly out of bounds
    gx = clamp(gx, 0, grid_width_ - 1);
    gy = clamp(gy, 0, grid_height_ - 1);
    
    // 4. Layer validation
    if (layer < 0 || layer >= num_layers_) {
        throw std::out_of_range("Layer index out of range");
    }
    
    return GridPoint(gx, gy, layer);
}

std::vector<GridPoint> RoutingGrid::getNeighbors(const GridPoint& current, int current_net_id) const {
    std::vector<GridPoint> neighbors;
    
    if (!isValid(current)) {
        return neighbors;  // Empty list for invalid points
    }
    
    int cx = current.x;
    int cy = current.y;
    int cz = current.layer;
    
    // Define directions: North, South, West, East
    // dirs[0] = North (0, +1), dirs[1] = South (0, -1), dirs[2] = West (-1, 0), dirs[3] = East (+1, 0)
    const int dirs[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    
    // === 3-LAYER ROUTING STRATEGY ===
    // M1 (Layer 0): Access layer - restricted to horizontal (East/West) due to cell obstacles
    // M2 (Layer 1): Vertical layer - preferred direction North/South for Manhattan routing
    // M3 (Layer 2): Horizontal layer - preferred direction East/West for long-distance routing
    
    // Determine direction range based on layer
    int start_dir = 0;
    int end_dir = 4; // Default: try all 4 directions
    
    if (cz == 0) {
        // M1: Only allow horizontal (East/West -> dirs[2], dirs[3])
        // M1 is heavily blocked by cells, vertical movement is meaningless
        start_dir = 2;
        end_dir = 4;
    } else if (cz == 1) {
        // M2: Preferred vertical direction (North/South -> dirs[0], dirs[1])
        // Allow horizontal but with penalty (handled in calculateMovementCost)
        start_dir = 0;
        end_dir = 2; // Only North/South for preferred routing
    } else if (cz == 2) {
        // M3: Preferred horizontal direction (East/West -> dirs[2], dirs[3])
        // This is the new horizontal freeway!
        start_dir = 2;
        end_dir = 4;
    }
    
    for (int i = start_dir; i < end_dir; ++i) {
        int nx = cx + dirs[i][0];
        int ny = cy + dirs[i][1];
        GridPoint neighbor(nx, ny, cz);
        
        // Check if planar neighbor is valid with enemy/friend identification
        if (isValid(neighbor) && isFree(neighbor, current_net_id)) {
            neighbors.emplace_back(neighbor);
        }
    }
    
    // === 3. Vertical movement (Via) - 3-LAYER ESCAPE ROUTES ===
    // Support M1<->M2 and M2<->M3 vias
    
    // Try to go up (Up)
    if (cz + 1 < num_layers_) {
        GridPoint up_neighbor(cx, cy, cz + 1);
        // Via requires both current and target to be valid
        if (isValid(up_neighbor) && isFree(up_neighbor, current_net_id)) {
            neighbors.emplace_back(up_neighbor);
        }
    }
    
    // Try to go down (Down)
    if (cz - 1 >= 0) {
        GridPoint down_neighbor(cx, cy, cz - 1);
        if (isValid(down_neighbor) && isFree(down_neighbor, current_net_id)) {
            neighbors.emplace_back(down_neighbor);
        }
    }
    
    return neighbors;
}

void RoutingGrid::addObstacle(const Rect& phys_rect, int layer) {
    if (layer < 0 || layer >= num_layers_) {
        throw std::out_of_range("Layer index out of range");
    }
    
    // Convert physical rectangle to grid coordinates
    GridPoint start_gp = physToGrid(phys_rect.x_min, phys_rect.y_min, layer);
    GridPoint end_gp = physToGrid(phys_rect.x_max, phys_rect.y_max, layer);
    
    // Ensure start <= end
    int min_x = std::min(start_gp.x, end_gp.x);
    int max_x = std::max(start_gp.x, end_gp.x);
    int min_y = std::min(start_gp.y, end_gp.y);
    int max_y = std::max(start_gp.y, end_gp.y);
    
    // Mark all cells in range as OBSTACLE
    // Don't override ROUTED cells with OBSTACLE (improvement)
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {
            if (grid_[layer][y][x].state != GridState::ROUTED) {
                grid_[layer][y][x].state = GridState::OBSTACLE;
                grid_[layer][y][x].net_id = 0;
            }
        }
    }
}

// ============================================================================
// State Management
// ============================================================================

GridState RoutingGrid::getState(const GridPoint& gp) const {
    if (!isValid(gp)) {
        throw std::out_of_range("Grid point out of bounds");
    }
    return grid_[gp.layer][gp.y][gp.x].state;
}

int RoutingGrid::getNetId(const GridPoint& gp) const {
    if (!isValid(gp)) {
        throw std::out_of_range("Grid point out of bounds");
    }
    return grid_[gp.layer][gp.y][gp.x].net_id;
}

void RoutingGrid::setState(const GridPoint& gp, GridState state, int net_id) {
    if (!isValid(gp)) {
        throw std::out_of_range("Grid point out of bounds");
    }
    grid_[gp.layer][gp.y][gp.x].state = state;
    grid_[gp.layer][gp.y][gp.x].net_id = net_id;
}

bool RoutingGrid::isFree(const GridPoint& gp, int current_net_id) const {
    if (!isValid(gp)) {
        return false;  // Out of bounds is not free
    }
    
    const GridNode& node = grid_[gp.layer][gp.y][gp.x];
    
    // Always allow access to FREE and PIN
    if (node.state == GridState::FREE || node.state == GridState::PIN) {
        return true;
    }
    
    // **PATHFINDER CHANGE**: Allow short circuits for congestion negotiation
    // Enemy nets can still use this point, but A* algorithm will add cost penalty
    if (node.state == GridState::ROUTED) {
        // Friend: same net_id -> allow access
        // Enemy: different net_id -> also allow access (but with penalty in A*)
        return true;
    }
    
    // OBSTACLE cells are never free
    return false;
}

// ============================================================================
// Utility Methods
// ============================================================================

bool RoutingGrid::isValid(const GridPoint& gp) const {
    return (gp.layer >= 0 && gp.layer < num_layers_ &&
            gp.x >= 0 && gp.x < grid_width_ &&
            gp.y >= 0 && gp.y < grid_height_);
}

void RoutingGrid::clear() {
    // Reset all cells to FREE
    for (int layer = 0; layer < num_layers_; ++layer) {
        for (int y = 0; y < grid_height_; ++y) {
            for (int x = 0; x < grid_width_; ++x) {
                grid_[layer][y][x].state = GridState::FREE;
                grid_[layer][y][x].net_id = 0;
            }
        }
    }
}

void RoutingGrid::markPath(const std::vector<GridPoint>& path, int net_id) {
    for (const auto& gp : path) {
        if (isValid(gp)) {
            // Don't override PIN states, but mark everything else as ROUTED
            if (grid_[gp.layer][gp.y][gp.x].state != GridState::PIN) {
                grid_[gp.layer][gp.y][gp.x].state = GridState::ROUTED;
                grid_[gp.layer][gp.y][gp.x].net_id = net_id;
            }
        }
    }
}

} // namespace mini