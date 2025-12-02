/**
 * @file routing_grid.cpp
 * @brief Routing Grid Implementation for MiniRouter
 */

#include "routing_grid.h"
#include <algorithm>
#include <stdexcept>

namespace mini {

RoutingGrid::RoutingGrid() 
    : grid_width_(0), grid_height_(0), num_layers_(2), 
      pitch_x_(0.2), pitch_y_(0.2), core_area_(0.0, 0.0, 1.0, 1.0) {
}

RoutingGrid::~RoutingGrid() = default;

void RoutingGrid::init(const Rect& core_area, double pitch_x, double pitch_y) {
    core_area_ = core_area;
    pitch_x_ = pitch_x;
    pitch_y_ = pitch_y;
    
    // Calculate grid dimensions
    double core_width = core_area_.width();
    double core_height = core_area_.height();
    
    grid_width_ = static_cast<int>(std::ceil(core_width / pitch_x_));
    grid_height_ = static_cast<int>(std::ceil(core_height / pitch_y_));
    
    // Resize grid data structure
    resizeGrid();
    
    // Initialize all cells as FREE
    for (int layer = 0; layer < num_layers_; ++layer) {
        for (int y = 0; y < grid_height_; ++y) {
            for (int x = 0; x < grid_width_; ++x) {
                grid_[layer][y][x] = GridState::FREE;
            }
        }
    }
}

void RoutingGrid::resizeGrid() {
    grid_.resize(num_layers_);
    for (int layer = 0; layer < num_layers_; ++layer) {
        grid_[layer].resize(grid_height_);
        for (int y = 0; y < grid_height_; ++y) {
            grid_[layer][y].resize(grid_width_, GridState::FREE);
        }
    }
}

void RoutingGrid::addObstacle(const Rect& rect, int layer) {
    // Convert physical coordinates to grid coordinates
    GridPoint start = physToGrid(rect.x_min, rect.y_min, 0);
    GridPoint end = physToGrid(rect.x_max, rect.y_max, 0);
    
    // Clamp to grid boundaries
    start.x = std::max(0, std::min(grid_width_ - 1, start.x));
    start.y = std::max(0, std::min(grid_height_ - 1, start.y));
    end.x = std::max(0, std::min(grid_width_ - 1, end.x));
    end.y = std::max(0, std::min(grid_height_ - 1, end.y));
    
    if (layer == -1) {
        // Add obstacle to all layers
        for (int l = 0; l < num_layers_; ++l) {
            markObstacleCells(start.x, start.y, end.x, end.y, l);
        }
    } else {
        // Add obstacle to specific layer
        markObstacleCells(start.x, start.y, end.x, end.y, layer);
    }
}

void RoutingGrid::markObstacleCells(int x_start, int y_start, int x_end, int y_end, int layer) {
    for (int y = y_start; y <= y_end && y < grid_height_; ++y) {
        for (int x = x_start; x <= x_end && x < grid_width_; ++x) {
            grid_[layer][y][x] = GridState::OBSTACLE;
        }
    }
}

void RoutingGrid::setGridState(const GridPoint& gp, GridState state) {
    if (isValid(gp)) {
        grid_[gp.layer][gp.y][gp.x] = state;
    }
}

GridPoint RoutingGrid::physToGrid(double phys_x, double phys_y, int layer) const {
    // Relative position from core area origin
    double rel_x = phys_x - core_area_.x_min;
    double rel_y = phys_y - core_area_.y_min;
    
    // Convert to grid coordinates using round for nearest grid point
    int grid_x = static_cast<int>(std::round(rel_x / pitch_x_));
    int grid_y = static_cast<int>(std::round(rel_y / pitch_y_));
    
    return GridPoint(grid_x, grid_y, layer);
}

Point RoutingGrid::gridToPhys(const GridPoint& gp) const {
    // Convert to physical coordinates (center of grid cell)
    double phys_x = core_area_.x_min + (gp.x + 0.5) * pitch_x_;
    double phys_y = core_area_.y_min + (gp.y + 0.5) * pitch_y_;
    
    return Point(phys_x, phys_y);
}

Rect RoutingGrid::getGridCellRect(const GridPoint& gp) const {
    Point phys_center = gridToPhys(gp);
    return Rect(phys_center.x - pitch_x_/2, phys_center.y - pitch_y_/2, 
                phys_center.x + pitch_x_/2, phys_center.y + pitch_y_/2);
}

bool RoutingGrid::isValid(const GridPoint& gp) const {
    return gp.layer >= 0 && gp.layer < num_layers_ &&
           gp.x >= 0 && gp.x < grid_width_ &&
           gp.y >= 0 && gp.y < grid_height_;
}

bool RoutingGrid::isFree(const GridPoint& gp) const {
    return isValid(gp) && grid_[gp.layer][gp.y][gp.x] == GridState::FREE;
}

GridState RoutingGrid::getGridState(const GridPoint& gp) const {
    if (isValid(gp)) {
        return grid_[gp.layer][gp.y][gp.x];
    }
    return GridState::OBSTACLE; // Invalid points are treated as obstacles
}

std::vector<GridPoint> RoutingGrid::getNeighbors(const GridPoint& gp) const {
    std::vector<GridPoint> neighbors;
    
    if (!isValid(gp)) return neighbors;
    
    if (gp.layer == 0) {
        // M1: Horizontal neighbors only
        GridPoint left(gp.x - 1, gp.y, gp.layer);
        GridPoint right(gp.x + 1, gp.y, gp.layer);
        if (isValid(left)) neighbors.push_back(left);
        if (isValid(right)) neighbors.push_back(right);
    } else if (gp.layer == 1) {
        // M2: Vertical neighbors only
        GridPoint up(gp.x, gp.y - 1, gp.layer);
        GridPoint down(gp.x, gp.y + 1, gp.layer);
        if (isValid(up)) neighbors.push_back(up);
        if (isValid(down)) neighbors.push_back(down);
    }
    
    return neighbors;
}

std::vector<GridPoint> RoutingGrid::getViaPoints(const GridPoint& gp) const {
    std::vector<GridPoint> via_points;
    
    if (!isValid(gp)) return via_points;
    
    // Add via points on the other layer
    for (int layer = 0; layer < num_layers_; ++layer) {
        if (layer != gp.layer) {
            GridPoint via(gp.x, gp.y, layer);
            if (isValid(via)) {
                via_points.push_back(via);
            }
        }
    }
    
    return via_points;
}

std::unordered_map<GridState, size_t> RoutingGrid::getStatistics() const {
    std::unordered_map<GridState, size_t> stats;
    
    // Initialize counters
    stats[GridState::FREE] = 0;
    stats[GridState::OBSTACLE] = 0;
    stats[GridState::ROUTED] = 0;
    stats[GridState::VIA] = 0;
    
    // Count cells
    for (int layer = 0; layer < num_layers_; ++layer) {
        for (int y = 0; y < grid_height_; ++y) {
            for (int x = 0; x < grid_width_; ++x) {
                GridState state = grid_[layer][y][x];
                stats[state]++;
            }
        }
    }
    
    return stats;
}

double RoutingGrid::getUtilization() const {
    auto stats = getStatistics();
    size_t total_cells = grid_width_ * grid_height_ * num_layers_;
    size_t used_cells = stats[GridState::ROUTED] + stats[GridState::VIA];
    
    if (total_cells == 0) return 0.0;
    return static_cast<double>(used_cells) / static_cast<double>(total_cells);
}

} // namespace mini