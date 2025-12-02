/**
 * @file maze_router.cpp
 * @brief Maze Router Implementation for MiniRouter
 */

#include "maze_router.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <functional>
#include <iostream>
#include <queue>
#include "../../lib/include/pin_mapper.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"

namespace mini {

MazeRouter::MazeRouter(RoutingGrid* grid, PinMapper* pin_mapper)
    : grid_(grid), pin_mapper_(pin_mapper), use_astar_(true), ripup_reroute_enabled_(false),
      max_attempts_(3), via_cost_(2.0), congestion_cost_(1.5) {
    
    // Initialize statistics
    stats_["total_nets"] = 0.0;
    stats_["successful_nets"] = 0.0;
    stats_["failed_nets"] = 0.0;
    stats_["total_wirelength"] = 0.0;
    stats_["total_vias"] = 0.0;
    stats_["avg_attempts"] = 0.0;
}

MazeRouter::~MazeRouter() = default;

RoutingResult MazeRouter::routeNet(Net* net, 
                                  const std::unordered_map<std::string, Point>& pin_locations) {
    RoutingResult result;
    
    if (!net || !grid_) {
        result.failure_reason = "Invalid net or grid";
        return result;
    }
    
    stats_["total_nets"]++;
    
    // Get all pins for this net
    std::vector<GridPoint> pin_points;
    
    // Add driver pin
    if (net->getDriver()) {
        std::string driver_pin = net->getDriver()->getName();
        Cell* driver_cell = net->getDriver()->getOwner();
        
        GridPoint driver_gp(-1, -1, -1);
        
        if (driver_cell) {
            std::string driver_key;
            
            if (driver_cell->getType() == CellType::INPUT || driver_cell->getType() == CellType::OUTPUT) {
                // I/O Port: use cell name directly (e.g., "clk")
                driver_key = driver_cell->getName();
                std::cout << "   Debug: Driver I/O Port looking for key: " << driver_key << std::endl;
            } else if (pin_mapper_) {
                // Standard Cell: use PinMapper for consistent key generation
                driver_key = pin_mapper_->getKeyFromLogicalPin(
                    driver_cell->getName(),        // cell instance name
                    driver_cell->getTypeString(),  // cell type
                    driver_pin                   // logical pin name
                );
                std::cout << "   Debug: Driver Standard Cell looking for key: " << driver_key << std::endl;
            } else {
                // Fallback: use "cell_name:pin_name" format
                driver_key = driver_cell->getName() + ":" + driver_pin;
                std::cout << "   Debug: Driver Standard Cell (fallback) looking for key: " << driver_key << std::endl;
            }
            
            driver_gp = pinToGridPoint(driver_key, pin_locations, 0);
        }
        
        if (grid_->isValid(driver_gp)) {
            pin_points.push_back(driver_gp);
        } else {
            std::cout << "   Debug: Driver pin not found in pin_locations!" << std::endl;
        }
    }
    
    // Add load pins
    for (const auto& load : net->getLoads()) {
        std::string load_pin = load->getName();
        Cell* load_cell = load->getOwner();
        
        GridPoint load_gp(-1, -1, -1);
        
        if (load_cell) {
            std::string load_key;
            
            if (load_cell->getType() == CellType::INPUT || load_cell->getType() == CellType::OUTPUT) {
                // I/O Port: use cell name directly (e.g., "data_out")
                load_key = load_cell->getName();
                std::cout << "   Debug: Load I/O Port looking for key: " << load_key << std::endl;
            } else if (pin_mapper_) {
                // Standard Cell: use PinMapper for consistent key generation
                load_key = pin_mapper_->getKeyFromLogicalPin(
                    load_cell->getName(),        // cell instance name
                    load_cell->getTypeString(),  // cell type
                    load_pin                    // logical pin name
                );
                std::cout << "   Debug: Load Standard Cell looking for key: " << load_key << std::endl;
            } else {
                // Fallback: use "cell_name:pin_name" format
                load_key = load_cell->getName() + ":" + load_pin;
                std::cout << "   Debug: Load Standard Cell (fallback) looking for key: " << load_key << std::endl;
            }
            
            load_gp = pinToGridPoint(load_key, pin_locations, 1);
        }
        
        if (grid_->isValid(load_gp)) {
            pin_points.push_back(load_gp);
        } else {
            std::cout << "   Debug: Load pin not found in pin_locations!" << std::endl;
        }
        
        if (grid_->isValid(load_gp)) {
            pin_points.push_back(load_gp);
        }
    }
    
    if (pin_points.size() < 2) {
        result.failure_reason = "Net has less than 2 pins";
        stats_["failed_nets"]++;
        return result;
    }
    
    // Try routing with multiple attempts
    for (int attempt = 0; attempt < max_attempts_; ++attempt) {
        std::vector<std::vector<GridPoint>> paths;
        
        // Route multi-pin net
        if (routeMultiPin(pin_points, paths)) {
            // Combine all paths into single result path
            for (const auto& path : paths) {
                result.path.insert(result.path.end(), path.begin(), path.end());
            }
            
            // Calculate statistics
            result.wirelength = calculateWirelength(result.path);
            result.num_vias = countVias(result.path);
            result.success = true;
            
            // Mark paths as routed
            for (const auto& path : paths) {
                markPathRouted(path);
            }
            
            stats_["successful_nets"]++;
            stats_["total_wirelength"] += result.wirelength;
            stats_["total_vias"] += result.num_vias;
            stats_["avg_attempts"] += (attempt + 1);
            
            break;
        } else if (ripup_reroute_enabled_ && attempt < max_attempts_ - 1) {
            // Rip up existing paths and try again
            for (const auto& path : paths) {
                unmarkPath(path);
            }
        }
    }
    
    if (!result.success) {
        result.failure_reason = "Failed to find path after " + std::to_string(max_attempts_) + " attempts";
        stats_["failed_nets"]++;
    }
    
    return result;
}

std::vector<RoutingResult> MazeRouter::routeNets(
    const std::vector<Net*>& nets,
    const std::unordered_map<std::string, Point>& pin_locations) {
    
    std::vector<RoutingResult> results;
    
    // Sort nets by fanout (larger nets first for better resource allocation)
    std::vector<Net*> sorted_nets = nets;
    std::sort(sorted_nets.begin(), sorted_nets.end(), 
              [](const Net* a, const Net* b) {
                  return a->getLoads().size() > b->getLoads().size();
              });
    
    // Route each net
    for (Net* net : sorted_nets) {
        RoutingResult result = routeNet(net, pin_locations);
        results.push_back(result);
    }
    
    // Calculate average attempts
    if (stats_["total_nets"] > 0) {
        stats_["avg_attempts"] /= stats_["total_nets"];
    }
    
    return results;
}

bool MazeRouter::leeAlgorithm(const GridPoint& source, const GridPoint& target,
                             std::vector<GridPoint>& path) {
    if (!grid_->isValid(source) || !grid_->isValid(target) || 
        !grid_->isFree(source) || !grid_->isFree(target)) {
        return false;
    }
    
    // BFS queue
    std::queue<GridPoint> queue;
    std::unordered_map<GridPoint, GridPoint, GridPointHash> came_from;
    std::unordered_set<GridPoint, GridPointHash> visited;
    
    queue.push(source);
    visited.insert(source);
    
    while (!queue.empty()) {
        GridPoint current = queue.front();
        queue.pop();
        
        if (current == target) {
            reconstructPath(target, came_from, path);
            return true;
        }
        
        // Get neighbors
        std::vector<GridPoint> neighbors = grid_->getNeighbors(current);
        
        // Add via points
        std::vector<GridPoint> via_points = grid_->getViaPoints(current);
        neighbors.insert(neighbors.end(), via_points.begin(), via_points.end());
        
        for (const GridPoint& next : neighbors) {
            if (visited.find(next) == visited.end() && grid_->isFree(next)) {
                visited.insert(next);
                came_from[next] = current;
                queue.push(next);
            }
        }
    }
    
    return false;
}

bool MazeRouter::aStarSearch(const GridPoint& source, const GridPoint& target,
                           std::vector<GridPoint>& path) {
    if (!grid_->isValid(source) || !grid_->isValid(target) || 
        !grid_->isFree(source) || !grid_->isFree(target)) {
        return false;
    }
    
    // Priority queue for A* (min-heap)
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;
    std::unordered_map<GridPoint, GridPoint, GridPointHash> came_from;
    std::unordered_map<GridPoint, double, GridPointHash> g_score;
    std::unordered_set<GridPoint, GridPointHash> closed_set;
    
    // Initialize start node
    auto start_node = std::make_shared<AStarNode>(source);
    start_node->h_cost = manhattanDistance(source, target);
    start_node->f_cost = start_node->h_cost;
    
    open_set.push(*start_node);
    g_score[source] = 0.0;
    
    while (!open_set.empty()) {
        AStarNode current = open_set.top();
        open_set.pop();
        
        if (current.gp == target) {
            reconstructPath(target, came_from, path);
            return true;
        }
        
        if (closed_set.find(current.gp) != closed_set.end()) {
            continue;
        }
        closed_set.insert(current.gp);
        
        // Get neighbors
        std::vector<GridPoint> neighbors = grid_->getNeighbors(current.gp);
        
        // Add via points
        std::vector<GridPoint> via_points = grid_->getViaPoints(current.gp);
        neighbors.insert(neighbors.end(), via_points.begin(), via_points.end());
        
        for (const GridPoint& next : neighbors) {
            if (!grid_->isFree(next) || closed_set.find(next) != closed_set.end()) {
                continue;
            }
            
            // Calculate tentative g_score
            double tentative_g = g_score[current.gp] + getMovementCost(current.gp, next);
            
            if (g_score.find(next) == g_score.end() || tentative_g < g_score[next]) {
                came_from[next] = current.gp;
                g_score[next] = tentative_g;
                
                double h_cost = manhattanDistance(next, target);
                double f_cost = tentative_g + h_cost;
                
                AStarNode next_node(next);
                next_node.g_cost = tentative_g;
                next_node.h_cost = h_cost;
                next_node.f_cost = f_cost;
                
                open_set.push(next_node);
            }
        }
    }
    
    return false;
}

bool MazeRouter::routeMultiPin(const std::vector<GridPoint>& pins,
                              std::vector<std::vector<GridPoint>>& paths) {
    if (pins.size() < 2) return false;
    
    // Simple approach: connect all pins to the first pin (star topology)
    GridPoint source = pins[0];
    
    for (size_t i = 1; i < pins.size(); ++i) {
        GridPoint target = pins[i];
        std::vector<GridPoint> path;
        
        bool success = use_astar_ ? aStarSearch(source, target, path) 
                                 : leeAlgorithm(source, target, path);
        
        if (!success) {
            // Clean up already routed paths
            for (const auto& p : paths) {
                unmarkPath(p);
            }
            return false;
        }
        
        paths.push_back(path);
    }
    
    return true;
}

double MazeRouter::manhattanDistance(const GridPoint& from, const GridPoint& to) const {
    double dx = std::abs(from.x - to.x);
    double dy = std::abs(from.y - to.y);
    double dz = std::abs(from.layer - to.layer);
    
    // Get actual pitch values for realistic distance calculation
    auto pitch = grid_->getPitch();
    return dx * pitch.first + dy * pitch.second + dz * via_cost_;
}

double MazeRouter::getMovementCost(const GridPoint& from, const GridPoint& next) const {
    double base_cost = 1.0;
    
    // Add via cost
    if (from.layer != next.layer) {
        base_cost *= via_cost_;
    }
    
    // Add congestion cost (simplified - could be more sophisticated)
    // This is a placeholder for future congestion-aware routing
    // base_cost *= congestion_cost_;
    
    return base_cost;
}

void MazeRouter::reconstructPath(const GridPoint& target,
                                const std::unordered_map<GridPoint, GridPoint, GridPointHash>& came_from,
                                std::vector<GridPoint>& path) const {
    path.clear();
    GridPoint current = target;
    
    while (came_from.find(current) != came_from.end()) {
        path.push_back(current);
        current = came_from.at(current);
    }
    path.push_back(current);
    
    // Reverse to get path from source to target
    std::reverse(path.begin(), path.end());
}

void MazeRouter::markPathRouted(const std::vector<GridPoint>& path) {
    for (const GridPoint& gp : path) {
        if (gp.layer == 0 || gp.layer == 1) {
            grid_->setGridState(gp, GridState::ROUTED);
        }
    }
    
    // Mark vias
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i-1].layer != path[i].layer && path[i-1].x == path[i].x && path[i-1].y == path[i].y) {
            grid_->setGridState(path[i], GridState::VIA);
        }
    }
}

void MazeRouter::unmarkPath(const std::vector<GridPoint>& path) {
    for (const GridPoint& gp : path) {
        if (grid_->getGridState(gp) == GridState::ROUTED) {
            grid_->setGridState(gp, GridState::FREE);
        }
    }
}

GridPoint MazeRouter::pinToGridPoint(const std::string& pin_name,
                                    const std::unordered_map<std::string, Point>& pin_locations,
                                    int preferred_layer) const {
    auto it = pin_locations.find(pin_name);
    if (it != pin_locations.end()) {
        return grid_->physToGrid(it->second.x, it->second.y, preferred_layer);
    }
    
    // Debug: Show available keys that might match
    std::cout << "   Debug: Pin '" << pin_name << "' not found. Total available keys: " << pin_locations.size() << std::endl;
    int count = 0;
    for (const auto& pair : pin_locations) {
        if (count < 5) {  // Show first 5 keys as sample
            std::cout << "      - " << pair.first << std::endl;
            count++;
        }
    }
    if (pin_locations.size() > 5) {
        std::cout << "      ... and " << (pin_locations.size() - 5) << " more keys" << std::endl;
    }
    
    // Return invalid point if pin not found
    return GridPoint(-1, -1, -1);
}

double MazeRouter::calculateWirelength(const std::vector<GridPoint>& path) const {
    if (path.size() < 2) return 0.0;
    
    double wirelength = 0.0;
    auto pitch = grid_->getPitch();
    
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i-1].layer == path[i].layer) {
            if (path[i-1].layer == 0) {
                // Horizontal movement on M1
                wirelength += std::abs(path[i].x - path[i-1].x) * pitch.first;
            } else {
                // Vertical movement on M2
                wirelength += std::abs(path[i].y - path[i-1].y) * pitch.second;
            }
        }
        // Via length is not counted in wirelength (only via count)
    }
    
    return wirelength;
}

int MazeRouter::countVias(const std::vector<GridPoint>& path) const {
    int via_count = 0;
    
    for (size_t i = 1; i < path.size(); ++i) {
        if (path[i-1].layer != path[i].layer) {
            via_count++;
        }
    }
    
    return via_count;
}

void MazeRouter::updateStatistics(const RoutingResult& result) {
    // Statistics are updated in routeNet method
}

std::unordered_map<std::string, double> MazeRouter::getStatistics() const {
    return stats_;
}

} // namespace mini