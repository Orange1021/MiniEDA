/**
 * @file maze_router.cpp
 * @brief MiniRouter v2.0 - Maze Router Implementation
 */

#include "maze_router.h"
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/pin_mapper.h"
#include "../../lib/include/cell.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

MazeRouter::MazeRouter(RoutingGrid* grid, PinMapper* pin_mapper)
    : grid_(grid), pin_mapper_(pin_mapper) {
    if (!grid_) {
        throw std::invalid_argument("RoutingGrid cannot be null");
    }
    if (!pin_mapper_) {
        throw std::invalid_argument("PinMapper cannot be null");
    }
}

// ============================================================================
// Public Methods
// ============================================================================

RoutingResult MazeRouter::routeNet(Net* net, 
                                 const std::unordered_map<std::string, Point>& pin_locations) {
    RoutingResult result;
    
    // 1. Safety check
    if (!net) {
        result.error_message = "Invalid net";
        return result;
    }
    
    if (!net->getDriver()) {
        // Check if this net has any pins connected (undriven nets are common in sequential circuits)
        if (net->getAllPins().empty()) {
            result.error_message = "Net has no connected pins";
        } else {
            result.error_message = "Net has no driver (undriven net - skipping)";
        }
        return result;
    }
    
    // Debug: Print net info for problematic nets
    if (net->getName() == "G5" || net->getName() == "G6" || net->getName() == "G7") {
        std::cout << "Debug Net " << net->getName() << ":" << std::endl;
        Pin* driver = net->getDriver();
        if (driver) {
            Cell* driver_cell = driver->getOwner();
            std::cout << "  Driver: " << driver_cell->getName() 
                      << ", Pin: " << driver->getName()
                      << ", CellType: " << cellTypeToString(driver_cell->getType()) << std::endl;
            std::string driver_key = pin_mapper_->getPinKey(driver_cell, driver);
            std::cout << "  Driver key: " << driver_key << std::endl;
        }
    }
    
    // 2. Get Driver coordinates
    Pin* driver = net->getDriver();
    Cell* driver_cell = driver->getOwner();
    std::string driver_key = pin_mapper_->getPinKey(driver_cell, driver);
    
    auto driver_it = pin_locations.find(driver_key);
    if (driver_it == pin_locations.end()) {
        result.error_message = "Driver pin not found: " + driver_key;
        return result;
    }
    
    Point driver_phys = driver_it->second;
    GridPoint source_gp = grid_->physToGrid(driver_phys.x, driver_phys.y, 0);  // Start on M1
    
    // Mark driver pin as PIN state
    grid_->setState(source_gp, GridState::PIN);
    
    // 3. Route to each Load Pin (star topology decomposition)
    std::vector<Pin*> loads = net->getLoads();
    if (loads.empty()) {
        result.error_message = "No load pins found";
        return result;
    }
    
    for (Pin* load : loads) {
        // Get load pin coordinates using PinMapper (critical: never manually construct keys)
        std::string load_key = pin_mapper_->getPinKey(load->getOwner(), load);

        auto load_it = pin_locations.find(load_key);
        if (load_it == pin_locations.end()) {
            std::cout << "Warning: Load pin not found: " << load_key << std::endl;
            continue;
        }

        Point load_phys = load_it->second;

        // **PHYSICALLY CORRECT**: All pins are on Layer 0 (M1), as per LEF file
        int target_layer = 0;
        GridPoint target_gp = grid_->physToGrid(load_phys.x, load_phys.y, target_layer);

        // Mark target pin as PIN state
        grid_->setState(target_gp, GridState::PIN);
        
        // 4. Call core algorithm
        std::vector<GridPoint> segment;
        if (findPath(source_gp, target_gp, segment)) {
            // Update statistics first
            int wirelength = calculateWirelength(segment);
            int vias = countVias(segment);

            // Debug: Trace path for suspicious nets - **FIXED**: Only print unique nets
            static std::unordered_set<std::string> debugged_nets;
            if (wirelength > 5 || segment.size() > 5) {
                if (debugged_nets.find(net->getName()) == debugged_nets.end()) {
                    debugged_nets.insert(net->getName());
                    std::cout << "=== DEBUG: Net " << net->getName() << " Path Analysis ===" << std::endl;
                    std::cout << "Path length: " << segment.size() << " points, wirelength=" << wirelength << ", vias=" << vias << std::endl;
                    std::cout << "Path: ";
                    for (size_t i = 0; i < std::min(segment.size(), size_t(10)); ++i) {
                        const auto& p = segment[i];
                        std::cout << "(" << p.x << "," << p.y << "," << p.layer << ")";
                        if (i < std::min(segment.size(), size_t(10)) - 1) std::cout << " -> ";
                    }
                    if (segment.size() > 10) std::cout << " ...";
                    std::cout << std::endl;
                    std::cout << "========================================" << std::endl;
                }
            }

            // Save path
            result.segments.push_back(segment);

            // Mark path as ROUTED (prevent future shorts)
            markPath(segment);

            // Update result object with actual segment statistics
            result.total_wirelength += wirelength;
            result.total_vias += vias;

            // Detect and log trivial routes (zero-length routing)
            if (segment.size() <= 1 || wirelength == 0) {
                static int trivial_count = 0;
                if (trivial_count < 3) {
                    std::cout << "Warning: Trivial route detected for net " << net->getName()
                              << " (Source == Target, path size: " << segment.size() << ")" << std::endl;
                    trivial_count++;
                }
                // Count as success but don't skew wirelength statistics
            }
        } else {
            result.error_message = "Failed to route segment to: " + load_key;
            std::cout << "Failed to route segment to: " << load_key << std::endl;
        }
    }

    // 5. Determine success
    result.success = !result.segments.empty();
    if (result.success) {
        total_routed_nets_++;
        // **FIXED**: Only add result.total_* once at the net level, not at segment level
        total_wirelength_ += result.total_wirelength;
        total_vias_ += result.total_vias;
    } else {
        failed_nets_++;
    }
    
    return result;
}

bool MazeRouter::findPath(const GridPoint& start, const GridPoint& end, 
                         std::vector<GridPoint>& path) {
    // Initialize
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;
    std::unordered_map<GridPoint, GridPoint, GridPointHash> came_from;
    std::unordered_map<GridPoint, double, GridPointHash> g_score;
    
    // Push start node
    double start_h = manhattanDistance(start, end);
    open_set.emplace(start, 0.0, start_h);
    g_score[start] = 0.0;
    
    // Main loop
    while (!open_set.empty()) {
        // Get node with lowest f_cost
        AStarNode current = open_set.top();
        open_set.pop();
        
        // Termination condition
        if (current.gp == end) {
            path = backtrack(came_from, end);
            return true;
        }
        
        // Expand neighbors
        std::vector<GridPoint> neighbors = grid_->getNeighbors(current.gp);
        for (const GridPoint& next : neighbors) {
            // Legality check: allow walking into end even if it's an obstacle
            if (!(next == end) && !grid_->isFree(next)) {
                continue;  // Skip obstacles (except the target point)
            }
            
            // Calculate G value
            double new_g = current.g_cost + calculateMovementCost(current.gp, next);
            
            // Update if this path is better
            auto it = g_score.find(next);
            if (it == g_score.end() || new_g < it->second) {
                g_score[next] = new_g;
                double h_score = manhattanDistance(next, end);
                
                came_from[next] = current.gp;
                open_set.emplace(next, new_g, h_score);
            }
        }
    }
    
    // No path found
    return false;
}

void MazeRouter::resetStatistics() {
    total_routed_nets_ = 0;
    failed_nets_ = 0;
    total_wirelength_ = 0.0;
    total_vias_ = 0;
}

// ============================================================================
// Private Methods
// ============================================================================

std::vector<GridPoint> MazeRouter::backtrack(
    const std::unordered_map<GridPoint, GridPoint, GridPointHash>& came_from,
    const GridPoint& current) const {
    
    std::vector<GridPoint> path;
    GridPoint curr = current;
    
    // Trace back from end to start
    while (came_from.find(curr) != came_from.end()) {
        path.push_back(curr);
        curr = came_from.at(curr);
    }
    path.push_back(curr);  // Add start point
    
    // Reverse to get start-to-end order
    std::reverse(path.begin(), path.end());
    return path;
}

double MazeRouter::calculateMovementCost(const GridPoint& from, const GridPoint& to) const {
    double cost = 0.0;
    
    // Add via cost if changing layers
    if (from.layer != to.layer) {
        cost += via_cost_;
    }
    
    // Add wire cost (always 1 unit for adjacent grid points)
    cost += wire_cost_per_unit_;
    
    return cost;
}

double MazeRouter::manhattanDistance(const GridPoint& from, const GridPoint& to) const {
    // Calculate Manhattan distance in physical units
    double dx = std::abs(from.x - to.x) * grid_->getPitchX();
    double dy = std::abs(from.y - to.y) * grid_->getPitchY();
    double dz = std::abs(from.layer - to.layer) * via_cost_;  // Layer change penalty
    
    return dx + dy + dz;
}

void MazeRouter::markPath(const std::vector<GridPoint>& path) {
    grid_->markPath(path);
}

int MazeRouter::calculateWirelength(const std::vector<GridPoint>& path) const {
    if (path.size() < 2) return 0;
    
    int wirelength = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        // Only count horizontal/vertical movement, not vias
        if (path[i].layer == path[i-1].layer) {
            wirelength++;
        }
    }
    return wirelength;
}

int MazeRouter::countVias(const std::vector<GridPoint>& path) const {
    if (path.size() < 2) return 0;
    
    int vias = 0;
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i].layer != path[i-1].layer) {
            vias++;
        }
    }
    return vias;
}

} // namespace mini