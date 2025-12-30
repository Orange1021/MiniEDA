/**
 * @file maze_router.cpp
 * @brief MiniRouter v2.0 - Maze Router Implementation
 */

#include "maze_router.h"
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/lef_pin_mapper.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/steiner_tree.h"
#include "../../lib/include/placer_db.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

MazeRouter::MazeRouter(RoutingGrid* grid, LefPinMapper* pin_mapper, PlacerDB* placer_db)
    : grid_(grid), pin_mapper_(pin_mapper), placer_db_(placer_db) {
    if (!grid_) {
        throw std::invalid_argument("RoutingGrid cannot be null");
    }
    if (!pin_mapper_) {
        throw std::invalid_argument("LefPinMapper cannot be null");
    }
}

// ============================================================================
// Public Methods
// ============================================================================

RoutingResult MazeRouter::routeNet(Net* net, 
                                 const std::unordered_map<std::string, Point>& pin_locations) {
    RoutingResult result;
    
    // **CRITICAL FIX**: Set current net ID for enemy/friend identification
    // Use name hash as net ID since Net class doesn't have getId()
    current_net_id_ = std::hash<std::string>{}(net->getName());
    
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
    
    // 3. Build MST topology using SteinerTreeBuilder (tree topology decomposition)
    std::vector<Segment> mst_segments;
    
    // Use SteinerTreeBuilder if PlacerDB is available, otherwise fall back to simple MST
    if (placer_db_) {
        mst_segments = SteinerTreeBuilder::build(net, placer_db_);
    } else {
        // Fallback: simple MST construction for backward compatibility
        std::vector<Point> pin_points;
        std::vector<Pin*> pin_ptrs;
        
        // Add driver pin
        pin_points.push_back(driver_phys);
        pin_ptrs.push_back(driver);
        
        // Add all load pins
        std::vector<Pin*> loads = net->getLoads();
        if (loads.empty()) {
            result.error_message = "No load pins found";
            return result;
        }
        
        for (Pin* load : loads) {
            std::string load_key = pin_mapper_->getPinKey(load->getOwner(), load);
            auto load_it = pin_locations.find(load_key);
            if (load_it != pin_locations.end()) {
                pin_points.push_back(load_it->second);
                pin_ptrs.push_back(load);
            }
        }
        
        // Simple MST construction
        if (pin_points.size() < 2) {
            result.error_message = "Insufficient pins for topology generation";
            return result;
        }
        
        std::vector<bool> connected(pin_points.size(), false);
        std::vector<int> parent(pin_points.size(), -1);
        std::vector<double> min_dist(pin_points.size(), std::numeric_limits<double>::max());
        
        min_dist[0] = 0;
        
        for (size_t i = 0; i < pin_points.size(); ++i) {
            int u = -1;
            double shortest = std::numeric_limits<double>::max();
            
            for (size_t v = 0; v < pin_points.size(); ++v) {
                if (!connected[v] && min_dist[v] < shortest) {
                    shortest = min_dist[v];
                    u = static_cast<int>(v);
                }
            }
            
            if (u == -1) break;
            connected[u] = true;
            
            if (parent[u] != -1) {
                mst_segments.emplace_back(pin_points[parent[u]], pin_points[u], 
                                        pin_ptrs[parent[u]], pin_ptrs[u]);
            }
            
            for (size_t v = 0; v < pin_points.size(); ++v) {
                if (!connected[v]) {
                    double dist = std::abs(pin_points[u].x - pin_points[v].x) + 
                                 std::abs(pin_points[u].y - pin_points[v].y);
                    if (dist < min_dist[v]) {
                        min_dist[v] = dist;
                        parent[v] = u;
                    }
                }
            }
        }
    }
    
    // Route each MST segment
    for (const auto& seg : mst_segments) {
        Point start_phys = seg.start;
        Point end_phys = seg.end;
        
        // Convert to grid coordinates
        GridPoint start_gp = grid_->physToGrid(start_phys.x, start_phys.y, 0);
        GridPoint end_gp = grid_->physToGrid(end_phys.x, end_phys.y, 0);
        
        // Mark both endpoints as PIN state
        grid_->setState(start_gp, GridState::PIN);
        grid_->setState(end_gp, GridState::PIN);
        
        // 4. Call core A* algorithm for each segment
        std::vector<GridPoint> segment;
        
        // **CRITICAL FIX**: Temporarily mark start/end as FREE for A* to work
        // This is needed because A* algorithm's obstacle checking logic
        GridState original_start_state = grid_->getState(start_gp);
        GridState original_end_state = grid_->getState(end_gp);
        grid_->setState(start_gp, GridState::FREE);
        grid_->setState(end_gp, GridState::FREE);
        
        if (findPath(start_gp, end_gp, segment)) {
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
            markPath(segment, current_net_id_);

            // Update result object with actual segment statistics
            result.total_wirelength += wirelength;
            result.total_vias += vias;

            // Detect and log trivial routes (zero-length routing)
            if (segment.size() <= 1 || wirelength == 0) {
                static int trivial_count = 0;
                if (trivial_count < 3) {
                    std::cout << "Warning: Trivial route detected for net " << net->getName()
                              << " (Segment path size: " << segment.size() << ")" << std::endl;
                    trivial_count++;
                }
            }
            
            // **CRITICAL FIX**: Restore original PIN states after successful routing
            grid_->setState(start_gp, original_start_state);
            grid_->setState(end_gp, original_end_state);
        } else {
            // **CRITICAL FIX**: Restore original PIN states even if routing failed
            grid_->setState(start_gp, original_start_state);
            grid_->setState(end_gp, original_end_state);
            
            // **DEBUG**: Capture first few failed segments for analysis
            
                        static int debug_count = 0;
            
                        if (debug_count < 3) {
            
                            std::cout << "\n>>> DEBUG FAILED SEGMENT #" << (debug_count + 1) << " for net " << net->getName() << " <<<" << std::endl;
            
                            std::cout << "  Start (Phys): (" << start_phys.x << ", " << start_phys.y << ") (Layer " << start_gp.layer << ")" << std::endl;
            
                            std::cout << "  End   (Phys): (" << end_phys.x << ", " << end_phys.y << ") (Layer " << end_gp.layer << ")" << std::endl;
            
                            
            
                            // Check grid coordinates conversion
            
                            std::cout << "  Start (Grid): (" << start_gp.x << ", " << start_gp.y << ")" << std::endl;
            
                            std::cout << "  End   (Grid): (" << end_gp.x << ", " << end_gp.y << ")" << std::endl;
            
                            
            
                            // **SUSPECT B DEEP DIVE**: Check path blocking
            
                            std::cout << "  Checking path blocking..." << std::endl;
            
                            int path_length = std::abs(end_gp.x - start_gp.x) + std::abs(end_gp.y - start_gp.y);
            
                            std::cout << "  Manhattan distance: " << path_length << std::endl;
            
                            
            
                            // Check each point along the horizontal path
            
                            int step_dir = (end_gp.x > start_gp.x) ? 1 : -1;
            
                            for (int x = start_gp.x; x != end_gp.x; x += step_dir) {
            
                                GridPoint check_point(x, start_gp.y, start_gp.layer);
            
                                GridState state = grid_->getState(check_point);
            
                                std::cout << "  Point (" << x << "," << start_gp.y << "): " 
            
                                          << (state == GridState::FREE ? "FREE" : 
            
                                           state == GridState::OBSTACLE ? "OBSTACLE" :
            
                                           state == GridState::ROUTED ? "ROUTED" :
            
                                           state == GridState::PIN ? "PIN" : "OTHER") << std::endl;
            
                            }
            
                            
            
                            debug_count++;
            
                        }
            
            
            
            
            
            result.error_message = "Failed to route MST segment";
        }
    }

    // 5. Determine success and update statistics
    result.success = !result.segments.empty();
    
    // **NEW**: Update segments-level statistics
    int expected_segments = static_cast<int>(mst_segments.size());
    int succeeded_segments = static_cast<int>(result.segments.size());
    int failed_segments = expected_segments - succeeded_segments;
    
    total_segments_attempted_ += expected_segments;
    total_segments_succeeded_ += succeeded_segments;
    total_segments_failed_ += failed_segments;
    
    if (result.success) {
        total_routed_nets_++;
        // **FIXED**: Only add result.total_* once at the net level, not at segment level
        total_wirelength_ += result.total_wirelength;
        total_vias_ += result.total_vias;

        // 6. Back-annotate wire length to Net for timing analysis
        // Convert grid units to physical units (micrometers)
        double wire_length_um = result.total_wirelength * grid_->getPitchX();
        net->setWireLength(wire_length_um);
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
        std::vector<GridPoint> neighbors = grid_->getNeighbors(current.gp, current_net_id_);
        for (const GridPoint& next : neighbors) {
            // Legality check: allow walking into end even if it's an obstacle
            if (!(next == end) && !grid_->isFree(next, current_net_id_)) {
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
    
    // **NEW**: Reset segments statistics
    total_segments_attempted_ = 0;
    total_segments_succeeded_ = 0;
    total_segments_failed_ = 0;
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
    } else {
        // Planar movement cost with 3-layer directional strategy
        
        // Check movement direction
        bool is_horizontal_move = (from.y == to.y); // Y coordinate same, X changed
        bool is_vertical_move = (from.x == to.x);   // X coordinate same, Y changed
        
        // **3-LAYER ROUTING STRATEGY**:
        // M1 (Layer 0): Horizontal preferred (but mostly blocked by cells)
        // M2 (Layer 1): Vertical preferred (North/South)
        // M3 (Layer 2): Horizontal preferred (East/West) - NEW FREEWAY!
        
        if (from.layer == 0) {
            // M1: Horizontal movement is preferred
            if (is_horizontal_move) {
                cost += wire_cost_per_unit_; // Normal cost
            } else if (is_vertical_move) {
                cost += wire_cost_per_unit_ * 3.0; // Heavy penalty for vertical on M1
            }
        } else if (from.layer == 1) {
            // M2: Vertical movement is preferred
            if (is_vertical_move) {
                cost += wire_cost_per_unit_; // Normal cost
            } else if (is_horizontal_move) {
                cost += wire_cost_per_unit_ * 1.5; // Reduced penalty to encourage M3 usage
            }
        } else if (from.layer == 2) {
            // M3: Horizontal movement is preferred (this is the new highway!)
            if (is_horizontal_move) {
                cost += wire_cost_per_unit_; // Normal cost - M3 is designed for this!
            } else if (is_vertical_move) {
                cost += wire_cost_per_unit_ * 2.0; // Moderate penalty for vertical on M3
            }
        } else {
            // Fallback: normal cost
            cost += wire_cost_per_unit_;
        }
    }
    
    return cost;
}

double MazeRouter::manhattanDistance(const GridPoint& from, const GridPoint& to) const {
    // Calculate Manhattan distance in physical units
    double dx = std::abs(from.x - to.x) * grid_->getPitchX();
    double dy = std::abs(from.y - to.y) * grid_->getPitchY();
    double dz = std::abs(from.layer - to.layer) * via_cost_;  // Layer change penalty (now using correct via_cost_)
    
    return dx + dy + dz;
}

void MazeRouter::markPath(const std::vector<GridPoint>& path, int net_id) {
    grid_->markPath(path, net_id);
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