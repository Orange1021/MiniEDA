/**
 * @file routing_grid.cpp
 * @brief MiniRouter v2.0 - Routing Grid Implementation
 */

#include "routing_grid.h"
#include <stdexcept>
#include <iostream>
#include <unordered_set>
#include "../../lib/include/debug_log.h"

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

RoutingGrid::RoutingGrid() 
    : core_area_(0.0, 0.0, 0.0, 0.0), pitch_x_(0.0), pitch_y_(0.0), grid_width_(0), grid_height_(0), num_layers_(0), 
      total_grid_size_(0), enable_neighbor_precomputation_(true), history_increment_(0.0) {
    // Grid will be initialized in init() method
}

// ============================================================================
// Core Methods
// ============================================================================

void RoutingGrid::init(const Rect& core_area, double pitch_x, double pitch_y, int num_layers) {
    // Set number of layers
    num_layers_ = num_layers;
    
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
    
    // Calculate total grid size for flattened arrays
    total_grid_size_ = grid_width_ * grid_height_ * num_layers_;
    
    // Resize flattened grid array
    grid_flat_.resize(total_grid_size_, GridNode(GridState::FREE, 0));
    
    ROUTING_LOG("RoutingGrid", "RoutingGrid initialized: " + std::to_string(grid_width_) + "x" + std::to_string(grid_height_) + 
               " x" + std::to_string(num_layers_) + " layers");
    ROUTING_LOG("RoutingGrid", "Core area: (" + std::to_string(core_area_.x_min) + "," + std::to_string(core_area_.y_min) + 
               ") to (" + std::to_string(core_area_.x_max) + "," + std::to_string(core_area_.y_max) + ")");
    ROUTING_LOG("RoutingGrid", "Pitch: X=" + std::to_string(pitch_x_) + "um, Y=" + std::to_string(pitch_y_) + "um");
    
    // Initialize congestion tracking
    initializeHistoryCosts();
    
    // Precompute neighbors for performance
    if (enable_neighbor_precomputation_) {
        precomputeNeighbors();
    }
}

void RoutingGrid::initializeHistoryCosts() {
    // Resize flattened history costs array
    history_costs_flat_.resize(total_grid_size_, 1.0);  // Base cost = 1.0
}

void RoutingGrid::addHistoryCost(int x, int y, int z, double increment) {
    GridPoint gp(x, y, z);
    if (isValid(gp)) {
        history_costs_flat_[toLinearIndex(gp)] += increment;
    }
}

double RoutingGrid::getHistoryCost(int x, int y, int z) const {
    GridPoint gp(x, y, z);
    if (isValid(gp)) {
        return history_costs_flat_[toLinearIndex(gp)];
    }
    return 1e9;  // Out of bounds protection - very expensive
}

void RoutingGrid::setHistoryCost(int x, int y, int z, double cost) {
    if (!isValid(GridPoint(x, y, z))) return;
    history_costs_flat_[toLinearIndex(GridPoint(x, y, z))] = cost;
}

void RoutingGrid::clearRoutes() {
    // Clear all ROUTED states but preserve history costs
    for (int i = 0; i < total_grid_size_; ++i) {
        if (grid_flat_[i].isRouted()) {
            grid_flat_[i].state = GridState::FREE;
            grid_flat_[i].net_id = 0;
        }
        // **CRITICAL FIX**: Don't reset PIN net_id - it should persist across iterations
        // PIN states represent physical pins and should keep their net association
    }
}

void RoutingGrid::ripUpNet(int net_id) {
    // **STABILIZER**: Remove routes for a specific net while preserving others as obstacles
    // This is the key to selective rerouting - good nets become "concrete pillars"

    for (int i = 0; i < total_grid_size_; ++i) {
        if (grid_flat_[i].isRouted() && grid_flat_[i].net_id == net_id) {
            // Clear this specific net's route
            grid_flat_[i].state = GridState::FREE;
            grid_flat_[i].net_id = 0;

            // Decrease usage count for this location
            if (grid_flat_[i].usage_count > 0) {
                grid_flat_[i].usage_count--;
            }
        }
    }
}

int RoutingGrid::countConflicts(bool verbose) const {
    int conflicts = 0;

    // Count actual conflicts: grid points used by multiple different nets
    // This is the real PathFinder conflict metric, not self-collision
    for (int i = 0; i < total_grid_size_; ++i) {
        if (grid_flat_[i].isConflicted()) {
            // This point was used by multiple different nets (true conflict)
            conflicts++;
        }
    }

    return conflicts;
}

std::unordered_set<int> RoutingGrid::getConflictedNetIDs() const {
    std::unordered_set<int> conflicted_nets;

    // Find all nets that are involved in conflicts
    for (int i = 0; i < total_grid_size_; ++i) {
        if (grid_flat_[i].isConflicted()) {
            // This point has conflicts, add the net ID to our set
            conflicted_nets.insert(grid_flat_[i].net_id);
        }
    }

    return conflicted_nets;
}

void RoutingGrid::printConflictLocations(int current_net_id, bool verbose) const {
    int conflict_count = 0;

    for (int i = 0; i < total_grid_size_; ++i) {
        if (grid_flat_[i].isConflicted()) {
            // Convert linear index to 3D coordinates
            int layer = i / (grid_width_ * grid_height_);
            int remaining = i % (grid_width_ * grid_height_);
            int y = remaining / grid_width_;
            int x = remaining % grid_width_;

            // **CONFLICT DETECTED**: Print detailed information
            conflict_count++;

            // Always print conflict info (not just verbose mode)
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                          ">>> CONFLICT #" + std::to_string(conflict_count) + " <<<");
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                          "  Location: (" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(layer) + ")");
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                          "  Usage Count: " + std::to_string(grid_flat_[i].usage_count + 1));

            // **CRITICAL**: Show that this is a multi-net conflict
            if (grid_flat_[i].usage_count > 0) {
                ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                              "  >>> MULTI-NET CONFLICT: " + std::to_string(grid_flat_[i].usage_count + 1) +
                              " nets competing for this location!");
            }

            if (verbose) {
                ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                              "  (Additional verbose info would follow...)");
            }

            // Check if this is a Pin Access bottleneck
            bool below_is_pin = (layer > 0 && isValid(GridPoint(x, y, layer-1)) &&
                               grid_flat_[toLinearIndex(GridPoint(x, y, layer-1))].isPin());
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                          "  Is Pin Access? " + std::string(below_is_pin ? "YES" : "NO"));
            
            if (below_is_pin) {
                // Get the pin's net ID for analysis
                int pin_net_id = grid_flat_[toLinearIndex(GridPoint(x, y, layer-1))].net_id;
                ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                              "  Pin Net ID: " + std::to_string(pin_net_id));
                
                // Check if the pin net matches the routed net (should be true)
                if (pin_net_id != grid_flat_[i].net_id) {
                    ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                                  "  WARNING: Pin net ID mismatch!");
                }
            }
            
            // Check history cost at this location
            double history_cost = getHistoryCost(x, y, layer);
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                          "  History Cost: " + std::to_string(history_cost));
            
            // Check surrounding cells for congestion analysis
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                          "  Surrounding cell states:");
            for (int dz = -1; dz <= 1; ++dz) {
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0 && dz == 0) continue;
                        
                        int nx = x + dx, ny = y + dy, nz = layer + dz;
                        if (isValid(GridPoint(nx, ny, nz))) {
                            const GridNode& node = grid_flat_[toLinearIndex(GridPoint(nx, ny, nz))];
                            std::string state_str;
                            switch (node.state) {
                                case GridState::FREE: state_str = "FREE"; break;
                                case GridState::OBSTACLE: state_str = "OBSTACLE"; break;
                                case GridState::ROUTED: state_str = "ROUTED"; break;
                                case GridState::PIN: state_str = "PIN"; break;
                            }
                            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", 
                                          "    (" + std::to_string(nx) + "," + std::to_string(ny) + "," + std::to_string(nz) + "): " + state_str);
                        }
                    }
                }
            }
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "RoutingGrid", "");
        }
    }
    
    ROUTING_LOG("RoutingGrid", "Total conflicts found: " + std::to_string(conflict_count));
    std::cout << "===============================" << std::endl;
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
        const GridNode& node = grid_flat_[toLinearIndex(to)];
        if (node.isRouted() && node.net_id != current_net_id) {
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
    
    int idx = toLinearIndex(current);
    
    if (enable_neighbor_precomputation_) {
        // Use precomputed neighbors
        const std::vector<GridPoint>& precomputed = precomputed_neighbors_[idx];
        
        // Filter by isFree check (since precomputed neighbors don't consider current_net_id)
        for (const GridPoint& neighbor : precomputed) {
            if (isFree(neighbor, current_net_id)) {
                neighbors.emplace_back(neighbor);
            }
        }
    } else {
        // Legacy mode: compute neighbors on-the-fly
        int cx = current.x;
        int cy = current.y;
        int cz = current.layer;
        
        // Define directions: North, South, West, East
        const int dirs[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
        
        // Determine direction range based on layer
        // Even layers (0, 2, 4, ...): Horizontal (East/West)
        // Odd layers (1, 3, 5, ...): Vertical (North/South)
        int start_dir = 0;
        int end_dir = 4;
        
        if (cz % 2 == 0) {
            // Even layers: Horizontal (East/West)
            start_dir = 2;
            end_dir = 4;
        } else {
            // Odd layers: Vertical (North/South)
            start_dir = 0;
            end_dir = 2;
        }
        
        for (int i = start_dir; i < end_dir; ++i) {
            int nx = cx + dirs[i][0];
            int ny = cy + dirs[i][1];
            GridPoint neighbor(nx, ny, cz);
            
            if (isValid(neighbor) && isFree(neighbor, current_net_id)) {
                neighbors.emplace_back(neighbor);
            }
        }
        
        // Try to go up
        if (cz + 1 < num_layers_) {
            GridPoint up_neighbor(cx, cy, cz + 1);
            if (isValid(up_neighbor) && isFree(up_neighbor, current_net_id)) {
                neighbors.emplace_back(up_neighbor);
            }
        }
        
        // Try to go down
        if (cz - 1 >= 0) {
            GridPoint down_neighbor(cx, cy, cz - 1);
            if (isValid(down_neighbor) && isFree(down_neighbor, current_net_id)) {
                neighbors.emplace_back(down_neighbor);
            }
        }
    }
    
    return neighbors;
}

void RoutingGrid::precomputeNeighbors() {
    ROUTING_LOG("RoutingGrid", "Precomputing neighbors for all grid points...");
    
    // Resize precomputed neighbors array
    precomputed_neighbors_.resize(total_grid_size_);
    
    // Define directions: North, South, West, East
    const int dirs[4][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    
    // Precompute neighbors for each grid point
    for (int z = 0; z < num_layers_; ++z) {
        for (int y = 0; y < grid_height_; ++y) {
            for (int x = 0; x < grid_width_; ++x) {
                GridPoint current(x, y, z);
                int idx = toLinearIndex(current);
                
                // Determine direction range based on layer
                // Even layers (0, 2, 4, ...): Horizontal (East/West)
                // Odd layers (1, 3, 5, ...): Vertical (North/South)
                int start_dir = 0;
                int end_dir = 4;
                
                if (z % 2 == 0) {
                    // Even layers: Horizontal (East/West)
                    start_dir = 2;
                    end_dir = 4;
                } else {
                    // Odd layers: Vertical (North/South)
                    start_dir = 0;
                    end_dir = 2;
                }
                
                // Add planar neighbors
                for (int i = start_dir; i < end_dir; ++i) {
                    int nx = x + dirs[i][0];
                    int ny = y + dirs[i][1];
                    GridPoint neighbor(nx, ny, z);
                    
                    if (isValid(neighbor)) {
                        precomputed_neighbors_[idx].push_back(neighbor);
                    }
                }
                
                // Add vertical neighbors (vias)
                // Try to go up
                if (z + 1 < num_layers_) {
                    GridPoint up_neighbor(x, y, z + 1);
                    if (isValid(up_neighbor)) {
                        precomputed_neighbors_[idx].push_back(up_neighbor);
                    }
                }
                
                // Try to go down
                if (z - 1 >= 0) {
                    GridPoint down_neighbor(x, y, z - 1);
                    if (isValid(down_neighbor)) {
                        precomputed_neighbors_[idx].push_back(down_neighbor);
                    }
                }
            }
        }
    }
    
    ROUTING_LOG("RoutingGrid", "Precomputed " + std::to_string(total_grid_size_) + 
               " grid points with " + std::to_string(precomputed_neighbors_.size()) + " neighbor lists");
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
            int idx = toLinearIndex(GridPoint(x, y, layer));
            if (!grid_flat_[idx].isRouted()) {
                grid_flat_[idx].state = GridState::OBSTACLE;
                grid_flat_[idx].net_id = 0;
            }
        }
    }
}

// ============================================================================
// State Management
// ============================================================================

GridState RoutingGrid::getState(const GridPoint& gp) const {
    return getGridNode(gp).state;
}

int RoutingGrid::getNetId(const GridPoint& gp) const {
    return getGridNode(gp).net_id;
}

void RoutingGrid::setState(const GridPoint& gp, GridState state, int net_id) {
    GridNode& node = getGridNode(gp);
    node.state = state;
    node.net_id = net_id;
}

bool RoutingGrid::isFree(const GridPoint& gp, int current_net_id) const {
    if (!isValid(gp)) {
        return false;  // Out of bounds is not free
    }

    const GridNode& node = getGridNode(gp);

    // Always allow access to FREE and PIN
    if (node.isAccessible()) {
        return true;
    }

    // **PATHFINDER CHANGE**: Allow short circuits for congestion negotiation
    // Enemy nets can still use this point, but A* algorithm will add cost penalty
    if (node.isRouted()) {
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
    for (int i = 0; i < total_grid_size_; ++i) {
        grid_flat_[i].state = GridState::FREE;
        grid_flat_[i].net_id = 0;
        grid_flat_[i].usage_count = 0;
    }
}

void RoutingGrid::markPath(const std::vector<GridPoint>& path, int net_id) {
    for (const auto& gp : path) {
        if (isValid(gp)) {
            GridNode& node = grid_flat_[toLinearIndex(gp)];

            // Don't override PIN states, but mark everything else as ROUTED
            if (!node.isPin()) {
                // Check for conflicts and update usage count
                if (node.isRouted() && node.net_id != net_id) {
                    // This point is being used by a different net - increment usage count
                    node.usage_count++;
                }

                // Set state and net_id
                node.state = GridState::ROUTED;
                node.net_id = net_id;
            }
        }
    }
}

} // namespace mini