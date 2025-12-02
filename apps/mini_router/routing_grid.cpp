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
    : core_area_(0.0, 0.0, 0.0, 0.0), pitch_x_(0.0), pitch_y_(0.0), grid_width_(0), grid_height_(0), num_layers_(2) {
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
            grid_[layer][y].resize(grid_width_, GridState::FREE);
        }
    }
    
    std::cout << "RoutingGrid initialized: " << grid_width_ << "x" << grid_height_ 
              << " x" << num_layers_ << " layers" << std::endl;
    std::cout << "Core area: (" << core_area_.x_min << "," << core_area_.y_min 
              << ") to (" << core_area_.x_max << "," << core_area_.y_max << ")" << std::endl;
    std::cout << "Pitch: X=" << pitch_x_ << "um, Y=" << pitch_y_ << "um" << std::endl;
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

std::vector<GridPoint> RoutingGrid::getNeighbors(const GridPoint& current) const {
    std::vector<GridPoint> neighbors;
    
    if (!isValid(current)) {
        return neighbors;  // Empty list for invalid points
    }
    
    // Same-layer movement (Routing Constraints)
    if (current.layer == 0) {  // M1: Horizontal preferred
        // Left (x-1)
        if (current.x > 0) {
            neighbors.emplace_back(current.x - 1, current.y, current.layer);
        }
        // Right (x+1)
        if (current.x < grid_width_ - 1) {
            neighbors.emplace_back(current.x + 1, current.y, current.layer);
        }
    } else if (current.layer == 1) {  // M2: Vertical preferred
        // Up (y+1)
        if (current.y < grid_height_ - 1) {
            neighbors.emplace_back(current.x, current.y + 1, current.layer);
        }
        // Down (y-1)
        if (current.y > 0) {
            neighbors.emplace_back(current.x, current.y - 1, current.layer);
        }
    }
    
    // Inter-layer movement (Via)
    // Always allow via to the other layer at same coordinates
    int other_layer = 1 - current.layer;  // 0 -> 1, 1 -> 0
    neighbors.emplace_back(current.x, current.y, other_layer);
    
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
            if (grid_[layer][y][x] != GridState::ROUTED) {
                grid_[layer][y][x] = GridState::OBSTACLE;
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
    return grid_[gp.layer][gp.y][gp.x];
}

void RoutingGrid::setState(const GridPoint& gp, GridState state) {
    if (!isValid(gp)) {
        throw std::out_of_range("Grid point out of bounds");
    }
    grid_[gp.layer][gp.y][gp.x] = state;
}

bool RoutingGrid::isFree(const GridPoint& gp) const {
    if (!isValid(gp)) {
        return false;  // Out of bounds is not free
    }
    GridState state = grid_[gp.layer][gp.y][gp.x];
    return state == GridState::FREE || state == GridState::PIN;
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
                grid_[layer][y][x] = GridState::FREE;
            }
        }
    }
}

void RoutingGrid::markPath(const std::vector<GridPoint>& path) {
    for (const auto& gp : path) {
        if (isValid(gp)) {
            // Don't override PIN states, but mark everything else as ROUTED
            if (grid_[gp.layer][gp.y][gp.x] != GridState::PIN) {
                grid_[gp.layer][gp.y][gp.x] = GridState::ROUTED;
            }
        }
    }
}

} // namespace mini