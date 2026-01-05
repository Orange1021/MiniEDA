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
    
    // Initialize congestion tracking
    initializeCongestionMap();
}

// ============================================================================
// Public Methods
// ============================================================================

RoutingResult MazeRouter::routeNet(Net* net, 
                                 const std::unordered_map<std::string, Point>& pin_locations) {
    RoutingResult result;
    
    // Set current net ID
    current_net_id_ = std::hash<std::string>{}(net->getName());
    
    // Calculate segment count
    current_net_segments_ = net->getAllPins().size() - 1;
    
    // 1. Safety check
    if (!net) {
        result.error_message = "Invalid net";
        return result;
    }
    
    if (!net->getDriver()) {
        if (net->getAllPins().empty()) {
            result.error_message = "Net has no connected pins";
        } else {
            result.error_message = "Net has no driver (undriven net - skipping)";
        }
        return result;
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
    GridPoint source_gp = grid_->physToGrid(driver_phys.x, driver_phys.y, 0);
    
    // Mark driver pin as PIN state
    grid_->setState(source_gp, GridState::PIN, current_net_id_);
    
    // 3. Build MST topology
    std::vector<Segment> mst_segments;
    
    if (placer_db_) {
        mst_segments = SteinerTreeBuilder::build(net, placer_db_);
    } else {
        // Fallback: simple MST
        std::vector<Point> pin_points;
        std::vector<Pin*> pin_ptrs;
        
        pin_points.push_back(driver_phys);
        pin_ptrs.push_back(driver);
        
        std::vector<Pin*> loads = net->getLoads();
        for (Pin* load : loads) {
            std::string load_key = pin_mapper_->getPinKey(load->getOwner(), load);
            auto load_it = pin_locations.find(load_key);
            if (load_it != pin_locations.end()) {
                pin_points.push_back(load_it->second);
                pin_ptrs.push_back(load);
            }
        }
        
        if (pin_points.size() >= 2) {
            // (Simple MST logic omitted for brevity, assuming it's correct in your file)
            // Re-using your existing simple MST logic here implies strict copy-paste
            // but to save space I will simplify the fallback logic block structure:
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
    }
    
    // Route each MST segment
    for (const auto& seg : mst_segments) {
        Point start_phys = seg.start;
        Point end_phys = seg.end;
        
        GridPoint start_gp_m1 = grid_->physToGrid(start_phys.x, start_phys.y, 0);
        GridPoint end_gp_m1 = grid_->physToGrid(end_phys.x, end_phys.y, 0);
        
        grid_->setState(start_gp_m1, GridState::PIN, current_net_id_);
        grid_->setState(end_gp_m1, GridState::PIN, current_net_id_);
        
        GridPoint start_gp = start_gp_m1;
        GridPoint end_gp = end_gp_m1;
        
        // Smart Access
        if (!findBestAccessPoint(start_gp.x, start_gp.y, current_net_id_, start_gp)) {
            if (grid_->isFree(GridPoint(start_gp.x, start_gp.y, 1), current_net_id_)) {
                start_gp = GridPoint(start_gp.x, start_gp.y, 1);
            }
        }
        
        if (!findBestAccessPoint(end_gp.x, end_gp.y, current_net_id_, end_gp)) {
            if (grid_->isFree(GridPoint(end_gp.x, end_gp.y, 1), current_net_id_)) {
                end_gp = GridPoint(end_gp.x, end_gp.y, 1);
            }
        }
        
        // 4. Call core A*
        std::vector<GridPoint> segment;
        
        grid_->setState(start_gp, GridState::FREE);
        grid_->setState(end_gp, GridState::FREE);
        
        if (findPath(start_gp, end_gp, segment)) {
            // SUCCESS
            int wirelength = calculateWirelength(segment);
            int vias = countVias(segment);

            result.segments.push_back(segment);
            markPath(segment, current_net_id_);

            result.total_wirelength += wirelength;
            result.total_vias += vias;
            
            grid_->setState(start_gp_m1, GridState::PIN, current_net_id_);
            grid_->setState(end_gp_m1, GridState::PIN, current_net_id_);
            
        } else {
            // FAILURE
            grid_->setState(start_gp_m1, GridState::PIN, current_net_id_);
            grid_->setState(end_gp_m1, GridState::PIN, current_net_id_);
            
            // Log simple failure message (Probes removed)
            // std::cout << "Failed to route segment for net " << net->getName() << std::endl;
            
            result.error_message = "Failed to route MST segment";
        }
    } // End of MST segment loop

    // 5. Determine success and update statistics
    result.success = !result.segments.empty();
    
    int expected_segments = static_cast<int>(mst_segments.size());
    int succeeded_segments = static_cast<int>(result.segments.size());
    int failed_segments = expected_segments - succeeded_segments;
    
    total_segments_attempted_ += expected_segments;
    total_segments_succeeded_ += succeeded_segments;
    total_segments_failed_ += failed_segments;
    
    if (result.success) {
        total_routed_nets_++;
        total_wirelength_ += result.total_wirelength;
        total_vias_ += result.total_vias;

        // Global Registry Update
        int net_id = std::hash<std::string>{}(net->getName());
        final_routes_[net_id] = result.segments;

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
    
    // **TACTICAL VICTORY**: Calculate bounding box with expansion for 2-pin networks
    // 2-pin networks often get trapped in small boxes - give them escape routes!
    int bbox_margin = 5;  // Default margin
    if (current_net_segments_ == 1) {  // This is a 2-pin network (single segment)
        bbox_margin = 15;  // Give 2-pin networks much larger search space
    }
    
    // Calculate bounding box and make variables accessible in neighbor check
    int min_x = std::max(0, std::min(start.x, end.x) - bbox_margin);
    int max_x = std::min(grid_->getGridWidth() - 1, std::max(start.x, end.x) + bbox_margin);
    int min_y = std::max(0, std::min(start.y, end.y) - bbox_margin);
    int max_y = std::min(grid_->getGridHeight() - 1, std::max(start.y, end.y) + bbox_margin);
    
    // Clamp to grid bounds
    min_x = std::max(0, min_x);
    max_x = std::min(grid_->getGridWidth() - 1, max_x);
    min_y = std::max(0, min_y);
    max_y = std::min(grid_->getGridHeight() - 1, max_y);
    
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
            // **TACTICAL VICTORY**: Check bounding box constraint
            // For 2-pin networks, enforce the expanded bounding box
            if (current_net_segments_ == 1) {
                // Bounding box already calculated at function start
                if (next.x < min_x || next.x > max_x || next.y < min_y || next.y > max_y) {
                    continue;  // Skip outside expanded bounding box
                }
            }
            
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
}

void MazeRouter::initializeCongestionMap() {
    int width = grid_->getGridWidth();
    int height = grid_->getGridHeight();
    int layers = grid_->getNumLayers();
    
    congestion_map_.resize(layers);
    for (int z = 0; z < layers; ++z) {
        congestion_map_[z].resize(height);
        for (int y = 0; y < height; ++y) {
            congestion_map_[z][y].resize(width, 0);
        }
    }
}

void MazeRouter::resetCongestionMap() {
    for (auto& layer : congestion_map_) {
        for (auto& row : layer) {
            std::fill(row.begin(), row.end(), 0);
        }
    }
}

void MazeRouter::updateHistoryCosts(double history_increment) {
    std::cout << "Updating History Costs with increment: " << history_increment << std::endl;
    
    int width = grid_->getGridWidth();
    int height = grid_->getGridHeight();
    int layers = grid_->getNumLayers();
    const int capacity = 1;  // Default capacity per grid point
    
    // **HISTORY COST DECAY**: First apply decay to existing costs
    const double decay_factor = 0.9;  // Decay factor for cooling down hotspots
    int decayed_count = 0;
    
    for (int z = 0; z < layers; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double current = grid_->getHistoryCost(x, y, z);
                if (current > 1.0) {
                    // Cool down hotspots gradually
                    double decayed = 1.0 + (current - 1.0) * decay_factor;
                    grid_->setHistoryCost(x, y, z, decayed);
                    decayed_count++;
                }
            }
        }
    }
    
    std::cout << "  Applied decay to " << decayed_count << " grid points" << std::endl;
    
    // Then apply new penalties based on current usage
    int punished_count = 0;
    for (int z = 0; z < layers; ++z) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int usage = congestion_map_[z][y][x];
                
                if (usage > capacity) {
                    // Found congestion! Apply penalty
                    double overflow = usage - capacity;
                    double penalty = overflow * history_increment;
                    
                    grid_->addHistoryCost(x, y, z, penalty);
                    punished_count++;
                    
                    // 补回 new_cost 定义
                    double new_cost = grid_->getHistoryCost(x, y, z);

                    std::cout << "  Grid point (" << x << "," << y << "," << z
                              << "): Usage=" << usage << ", Penalty=" << penalty 
                              << ", New HistCost=" << new_cost << std::endl;
                }
            }
        }
    }
    
    std::cout << "  Applied penalties to " << punished_count << " congested points" << std::endl;
}


void MazeRouter::saveBestSolution(const std::vector<RoutingResult>& results, int iteration, int conflicts) {
    
    if (conflicts < min_conflicts_) {
        std::cout << ">>> NEW RECORD! Conflicts: " << conflicts 
                  << " (Saving at iteration " << iteration << ") <<<" << std::endl;
        min_conflicts_ = conflicts;
        best_iteration_ = iteration;
        
        // Clear previous best solution
        best_solution_segments_.clear();
        best_solution_segments_with_ids_.clear();
        
        // **OPTIMAL SOLUTION**: Save ALL networks from global registry (final_routes_)
        for (const auto& [net_id, segments] : final_routes_) {
            // Add all segments for each net
            for (const auto& segment : segments) {
                if (!segment.empty()) {
                    best_solution_segments_with_ids_.push_back({segment, net_id});
                }
            }
        }
    }
}

void MazeRouter::restoreBestSolution() {
    if (best_iteration_ == -1) {
        std::cout << "No best solution to restore" << std::endl;
        return;
    }
    
    std::cout << "Restoring best solution from iteration " << best_iteration_ 
              << " with " << min_conflicts_ << " conflicts..." << std::endl;
    
    // Clear current routing (but preserve history costs)
    grid_->clearRoutes();
    
    // Restore best solution segments to grid with their correct net IDs
    // This will recreate the best conflict state found during iterations
    for (const auto& [segment, net_id] : best_solution_segments_with_ids_) {
        if (!segment.empty()) {
            grid_->markPath(segment, net_id);  // Use the correct net_id
        }
    }
    
    std::cout << "Best solution restored successfully! Grid now has " 
              << min_conflicts_ << " conflicts." << std::endl;
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
    // **PATHFINDER CHANGE**: Use RoutingGrid's cost calculation instead of local logic
    // This includes history costs and collision penalties
    double cost = grid_->calculateMovementCost(from, to, current_net_id_, collision_penalty_);
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
    
    // Update congestion map for history cost tracking
    for (const auto& gp : path) {
        if (grid_->isValid(gp)) {
            congestion_map_[gp.layer][gp.y][gp.x]++;
        }
    }
}

bool MazeRouter::findBestAccessPoint(int pin_gx, int pin_gy, int net_id, GridPoint& out_gp) const {
    
    int best_x = -1, best_y = -1;
    double min_cost = 1e9;
    bool found = false;
    
    // Search around pin location for best access point
    //扩大搜索范围：5x5区域，可以跳过整个拥塞团块
    int search_radius = 2;
    // Priority: direct access (0,0), then adjacent cells
    std::vector<std::pair<int, int>> search_offsets = {
        {0, 0},   // Direct access - highest priority
        {-1, 0}, {1, 0}, {0, -1}, {0, 1},  // Adjacent cells
        {-1, -1}, {-1, 1}, {1, -1}, {1, 1}  // Diagonal cells - lower priority
    };
    
    for (const auto& [dx, dy] : search_offsets) {
        int nx = pin_gx + dx;
        int ny = pin_gy + dy;
        
        // 1. Basic validity checks
        if (!grid_->isValid(GridPoint(nx, ny, 1))) continue;
        if (grid_->getState(GridPoint(nx, ny, 1)) == GridState::OBSTACLE) continue;
        
        // 2. [Core] Check congestion cost (History Cost)
        double current_hist = grid_->getHistoryCost(nx, ny, 1);
        
        // 3. Distance penalty (Manhattan distance from pin center)
        double dist_penalty = (abs(dx) + abs(dy)) * 2.0;
        
        // 4. Combined score - lower is better
        double total_score = current_hist + dist_penalty;
        
        if (total_score < min_cost) {
            min_cost = total_score;
            best_x = nx;
            best_y = ny;
            found = true;
        }
    }
    
    if (found) {
        out_gp = GridPoint(best_x, best_y, 1);
        
        
        

        return true;
    }
    
    std::cout << "Smart Access: FAILED for Net " << net_id 
              << " at (" << pin_gx << "," << pin_gy << ")" << std::endl;
    return false;
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

void MazeRouter::exportVisualization(const std::string& filename) {
    
    // Create directory if needed
    size_t last_slash = filename.find_last_of('/');
    if (last_slash != std::string::npos) {
        std::string directory = filename.substr(0, last_slash);
        std::string mkdir_cmd = "mkdir -p " + directory;
        std::system(mkdir_cmd.c_str());
    }
    
    std::ofstream out(filename);
    if (!out.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    // 1. Print header
    out << "# Net_ID Start_X Start_Y Start_Layer End_X End_Y End_Layer\n";
    
    // 2. Export all routes from global registry (final_routes_)
    int segments_count = 0;
    int nets_exported = 0;
    
    for (auto const& [net_id, paths] : final_routes_) {
        if (paths.empty()) continue;
        nets_exported++;
        
        // Each net may have multiple paths (segments)
        for (const auto& path : paths) {
            if (path.size() < 2) continue;
            
            // Convert node list to line segments
            for (size_t i = 0; i < path.size() - 1; ++i) {
                // Format: x1 y1 layer1 x2 y2 layer2 net_id
                out << path[i].x << " " << path[i].y << " " << path[i].layer << " "
                    << path[i+1].x << " " << path[i+1].y << " " << path[i+1].layer << " "
                    << net_id << "\n";
                segments_count++;
            }
        }
    }
    
    out.close();
}
} // namespace mini
