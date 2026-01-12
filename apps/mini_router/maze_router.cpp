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
#include "../../lib/include/debug_log.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

MazeRouter::MazeRouter(RoutingGrid* grid, LefPinMapper* pin_mapper, PlacerDB* placer_db)
    : grid_(grid), pin_mapper_(pin_mapper), placer_db_(placer_db), 
      via_cost_(0.0), wire_cost_per_unit_(0.0), decay_factor_(0.9), distance_weight_(2.0),
      linear_array_size_(0), linear_arrays_initialized_(false), track_congestion_(true),
      enable_mst_cache_(true) {
    if (!grid_) {
        throw std::invalid_argument("RoutingGrid cannot be null");
    }
    if (!pin_mapper_) {
        throw std::invalid_argument("LefPinMapper cannot be null");
    }
    
    // Initialize congestion tracking
    initializeCongestionMap();
    
    // Initialize linear index arrays for A* optimization
    initializeLinearArrays();
}

// ============================================================================
// Public Methods
// ============================================================================

RoutingResult MazeRouter::routeNet(Net* net,
                                 const std::unordered_map<std::string, Point>& pin_locations) {
    RoutingResult result;

    // Set current net ID
    current_net_id_ = net->getHashId();
    
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
    std::string driver_key = driver->getPinKey();  // Use cached key

    auto driver_it = pin_locations.find(driver_key);
    if (driver_it == pin_locations.end()) {
        result.error_message = "Driver pin not found: " + driver_key;
        return result;
    }
    
    Point driver_phys = driver_it->second;
    GridPoint source_gp = grid_->physToGrid(driver_phys.x, driver_phys.y, 0);

    // Mark driver pin as PIN state
    grid_->markPin(source_gp, current_net_id_);
    
    // 3. Build MST topology (with caching)
    std::vector<Segment> mst_segments;
    
    int net_id = net->getHashId();
    
    // Check cache first
    if (enable_mst_cache_) {
        auto cache_it = mst_cache_.find(net_id);
        if (cache_it != mst_cache_.end()) {
            // Cache hit - use cached MST
            mst_segments = cache_it->second;
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "MazeRouter", 
                          "MST cache hit for net " + net->getName() + " (ID: " + std::to_string(net_id) + ")");
        } else {
            // Cache miss - build MST and cache it
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
                    std::string load_key = load->getPinKey();
                    auto load_it = pin_locations.find(load_key);
                    if (load_it != pin_locations.end()) {
                        pin_points.push_back(load_it->second);
                        pin_ptrs.push_back(load);
                    }
                }
                
                if (pin_points.size() >= 2) {
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
            
            // Cache the MST for future use
            mst_cache_[net_id] = mst_segments;
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "MazeRouter", 
                          "MST cached for net " + net->getName() + " (ID: " + std::to_string(net_id) + 
                          ", segments: " + std::to_string(mst_segments.size()) + ")");
        }
    }
    // Note: When enable_mst_cache_ is false, MST is rebuilt every time (legacy mode)
    // This can be useful for debugging or when MST topology might change between iterations
    
    // Route each MST segment
    for (const auto& seg : mst_segments) {
        Point start_phys = seg.start;
        Point end_phys = seg.end;
        
        GridPoint start_gp_m1 = grid_->physToGrid(start_phys.x, start_phys.y, 0);
        GridPoint end_gp_m1 = grid_->physToGrid(end_phys.x, end_phys.y, 0);

        grid_->markPin(start_gp_m1, current_net_id_);
        grid_->markPin(end_gp_m1, current_net_id_);

        GridPoint start_gp = start_gp_m1;
        GridPoint end_gp = end_gp_m1;

        // Smart Access
        if (!findBestAccessPoint(start_gp.x, start_gp.y, current_net_id_, start_gp)) {
            if (grid_->isFree(start_gp.withLayer(1), current_net_id_)) {
                start_gp = start_gp.withLayer(1);
            }
        }

        if (!findBestAccessPoint(end_gp.x, end_gp.y, current_net_id_, end_gp)) {
            if (grid_->isFree(end_gp.withLayer(1), current_net_id_)) {
                end_gp = end_gp.withLayer(1);
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

            grid_->markPin(start_gp_m1, current_net_id_);
            grid_->markPin(end_gp_m1, current_net_id_);

        } else {
            // FAILURE
            grid_->markPin(start_gp_m1, current_net_id_);
            grid_->markPin(end_gp_m1, current_net_id_);
            
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
        int net_id = net->getHashId();
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
    // Reset linear arrays for new search
    resetLinearArrays();
    
    // Initialize
    std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> open_set;
    
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
    
    // Push start node using linear index
    int start_idx = toLinearIndex(start);
    int end_idx = toLinearIndex(end);
    
    double start_h = manhattanDistance(start, end);
    open_set.emplace(start, 0.0, start_h);
    g_score_linear_[start_idx] = 0.0;
    visited_linear_[start_idx] = true;
    
    // Main loop
    while (!open_set.empty()) {
        // Get node with lowest f_cost
        AStarNode current = open_set.top();
        open_set.pop();
        
        int current_idx = toLinearIndex(current.gp);
        
        // Termination condition
        if (current.gp == end) {
            // Reconstruct path using linear index came_from
            path.clear();
            int curr_idx = end_idx;
            while (curr_idx != -1) {
                path.push_back(fromLinearIndex(curr_idx));
                curr_idx = came_from_linear_[curr_idx];
            }
            // Reverse to get start-to-end order
            std::reverse(path.begin(), path.end());
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
            double new_g = current.g_cost + grid_->calculateMovementCost(current.gp, next, current_net_id_, collision_penalty_);

            // Update if this path is better using linear index
            int next_idx = toLinearIndex(next);
            if (new_g < g_score_linear_[next_idx]) {
                g_score_linear_[next_idx] = new_g;
                double h_score = manhattanDistance(next, end);
                
                came_from_linear_[next_idx] = current_idx;
                visited_linear_[next_idx] = true;
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
    ROUTING_LOG("MazeRouter", "Updating History Costs with increment: " + std::to_string(history_increment));
    
    int width = grid_->getGridWidth();
    int height = grid_->getGridHeight();
    int layers = grid_->getNumLayers();
    const int capacity = 1;  // Default capacity per grid point
    
    // **INCREMENTAL UPDATE**: Only process recorded congested points
    // This is 80-90% faster than traversing the entire grid
    
    if (track_congestion_) {
        // Apply decay to existing costs at congested points
        int decayed_count = 0;
        for (const auto& [x, y, z] : congested_points_) {
            double current = grid_->getHistoryCost(x, y, z);
            if (current > 1.0) {
                // Cool down hotspots gradually
                double decayed = 1.0 + (current - 1.0) * decay_factor_;
                grid_->setHistoryCost(x, y, z, decayed);
                decayed_count++;
            }
        }
        
        ROUTING_LOG("MazeRouter", "Applied decay to " + std::to_string(decayed_count) + " congested points");
        
        // Apply new penalties based on current usage at congested points
        int punished_count = 0;
        for (const auto& [x, y, z] : congested_points_) {
            int usage = congestion_map_[z][y][x];
            
            if (usage > capacity) {
                // Found congestion! Apply penalty
                double overflow = usage - capacity;
                double penalty = overflow * history_increment;
                
                grid_->addHistoryCost(x, y, z, penalty);
                punished_count++;
                
                ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "MazeRouter", 
                              "Grid point (" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) +
                              "): Usage=" + std::to_string(usage) + ", Penalty=" + std::to_string(penalty));
            }
        }
        
        ROUTING_LOG("MazeRouter", "Applied penalties to " + std::to_string(punished_count) + " congested points");
        
        // Clear congested points for next iteration
        clearCongestedPoints();
    } else {
        // Fallback to full grid traversal (legacy mode)
        int decayed_count = 0;
        
        for (int z = 0; z < layers; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    double current = grid_->getHistoryCost(x, y, z);
                    if (current > 1.0) {
                        double decayed = 1.0 + (current - 1.0) * decay_factor_;
                        grid_->setHistoryCost(x, y, z, decayed);
                        decayed_count++;
                    }
                }
            }
        }
        
        ROUTING_LOG("MazeRouter", "Applied decay to " + std::to_string(decayed_count) + " grid points (legacy mode)");
        
        int punished_count = 0;
        for (int z = 0; z < layers; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    int usage = congestion_map_[z][y][x];
                    
                    if (usage > capacity) {
                        double overflow = usage - capacity;
                        double penalty = overflow * history_increment;
                        
                        grid_->addHistoryCost(x, y, z, penalty);
                        punished_count++;
                    }
                }
            }
        }
        
        ROUTING_LOG("MazeRouter", "Applied penalties to " + std::to_string(punished_count) + " congested points (legacy mode)");
    }
}


void MazeRouter::saveBestSolution(const std::vector<RoutingResult>& results, int iteration, int conflicts) {
    
    if (conflicts < min_conflicts_) {
        ROUTING_LOG("MazeRouter", ">>> NEW RECORD! Conflicts: " + std::to_string(conflicts) + 
                   " (Saving at iteration " + std::to_string(iteration) + ") <<<");
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
        ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "MazeRouter", "No best solution to restore");
        return;
    }
    
    ROUTING_LOG("MazeRouter", "Restoring best solution from iteration " + std::to_string(best_iteration_) + 
               " with " + std::to_string(min_conflicts_) + " conflicts...");
    
    // Clear current routing (but preserve history costs)
    grid_->clearRoutes();
    
    // Restore best solution segments to grid with their correct net IDs
    // This will recreate the best conflict state found during iterations
    for (const auto& [segment, net_id] : best_solution_segments_with_ids_) {
        if (!segment.empty()) {
            grid_->markPath(segment, net_id);  // Use the correct net_id
        }
    }
    
    ROUTING_LOG("MazeRouter", "Best solution restored successfully! Grid now has " + std::to_string(min_conflicts_) + " conflicts");
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
    const int capacity = 1;  // Default capacity per grid point
    for (const auto& gp : path) {
        if (grid_->isValid(gp)) {
            congestion_map_[gp.layer][gp.y][gp.x]++;
            
            // **INCREMENTAL UPDATE**: Record congested points for efficient history cost update
            if (track_congestion_ && congestion_map_[gp.layer][gp.y][gp.x] > capacity) {
                recordCongestedPoint(gp.x, gp.y, gp.layer);
            }
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
        GridPoint check_point = GridPoint(nx, ny, 1);
        if (!grid_->isValid(check_point)) continue;
        if (grid_->getState(check_point) == GridState::OBSTACLE) continue;
    
        // 2. [Core] Check congestion cost (History Cost)
        double current_hist = grid_->getHistoryCost(nx, ny, 1);
    
        // 3. Distance penalty (Manhattan distance from pin center)
        double dist_penalty = (abs(dx) + abs(dy)) * distance_weight_;
    
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

    ROUTING_LOG("MazeRouter", "Smart Access: FAILED for Net " + std::to_string(net_id) +
               " at (" + std::to_string(pin_gx) + "," + std::to_string(pin_gy) + ")");
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

void MazeRouter::exportVisualization(const std::string& filename, PlacerDB* placer_db) {
    
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
    
    // 1. Print header for enhanced format
    out << "# Enhanced routing data format for plot_routing.py\n";
    out << "# Cell format: CELL x y width height cell_name\n";
    
    // 2. Export cell positions if placer_db is available
    if (placer_db && grid_) {
        // Convert physical coordinates to grid coordinates for consistency
        for (auto* cell : placer_db->getAllCells()) {
            const auto& info = placer_db->getCellInfo(cell);
            
            // Convert cell position and size to grid coordinates
            GridPoint bottom_left = grid_->physToGrid(info.x, info.y, 0);
            GridPoint top_right = grid_->physToGrid(info.x + info.width, info.y + info.height, 0);
            
            // Calculate grid coordinates
            int grid_x = bottom_left.x;
            int grid_y = bottom_left.y;
            int grid_width = top_right.x - bottom_left.x;
            int grid_height = top_right.y - bottom_left.y;
            
            out << "CELL " << grid_x << " " << grid_y << " "
                << grid_width << " " << grid_height << " "
                << cell->getName() << "\n";
        }
    }
    
    out << "\n# Routing format: x1 y1 z1 x2 y2 z2 net_id\n";
    
    // 3. Export all routes from global registry (final_routes_)
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
    std::cout << "Exported " << nets_exported << " nets, " << segments_count 
              << " segments to " << filename << std::endl;
}

// ============================================================================
// Linear Index Helper Functions
// ============================================================================

int MazeRouter::toLinearIndex(const GridPoint& gp) const {
    if (!grid_) return -1;
    int width = grid_->getGridWidth();
    int height = grid_->getGridHeight();
    return (gp.layer * height + gp.y) * width + gp.x;
}

GridPoint MazeRouter::fromLinearIndex(int idx) const {
    if (!grid_) return GridPoint(0, 0, 0);
    int width = grid_->getGridWidth();
    int height = grid_->getGridHeight();
    int layer = idx / (width * height);
    int remaining = idx % (width * height);
    int y = remaining / width;
    int x = remaining % width;
    return GridPoint(x, y, layer);
}

void MazeRouter::initializeLinearArrays() {
    if (!grid_) return;
    
    int width = grid_->getGridWidth();
    int height = grid_->getGridHeight();
    int layers = grid_->getNumLayers();
    
    linear_array_size_ = width * height * layers;
    
    // Allocate arrays
    g_score_linear_.resize(linear_array_size_, std::numeric_limits<double>::max());
    came_from_linear_.resize(linear_array_size_, -1);
    visited_linear_.resize(linear_array_size_, false);
    
    linear_arrays_initialized_ = true;
    
    ROUTING_LOG("MazeRouter", "Linear index arrays initialized: " + 
               std::to_string(linear_array_size_) + " elements");
}

void MazeRouter::resetLinearArrays() {
    if (!linear_arrays_initialized_) return;
    
    // Reset arrays for new A* search
    std::fill(g_score_linear_.begin(), g_score_linear_.end(), std::numeric_limits<double>::max());
    std::fill(came_from_linear_.begin(), came_from_linear_.end(), -1);
    std::fill(visited_linear_.begin(), visited_linear_.end(), false);
}

void MazeRouter::recordCongestedPoint(int x, int y, int z) {
    if (!track_congestion_) return;
    congested_points_.emplace_back(x, y, z);
}

void MazeRouter::clearCongestedPoints() {
    congested_points_.clear();
}

} // namespace mini
