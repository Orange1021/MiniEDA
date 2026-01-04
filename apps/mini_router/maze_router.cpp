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
        
        // Convert to grid coordinates (initially on M1 layer)
        GridPoint start_gp_m1 = grid_->physToGrid(start_phys.x, start_phys.y, 0);
        GridPoint end_gp_m1 = grid_->physToGrid(end_phys.x, end_phys.y, 0);
        
        // Mark both endpoints as PIN state on M1
        grid_->setState(start_gp_m1, GridState::PIN);
        grid_->setState(end_gp_m1, GridState::PIN);
        
        // **SMART ACCESS STRATEGY**: Use congestion-aware pin access selection
        GridPoint start_gp = start_gp_m1;  // Default to M1 coordinates
        GridPoint end_gp = end_gp_m1;      // Default to M1 coordinates
        
        // Try to find best access point for start pin
        if (!findBestAccessPoint(start_gp.x, start_gp.y, current_net_id_, start_gp)) {
            // Fallback: try direct elevation if smart access fails
            if (grid_->isFree(GridPoint(start_gp.x, start_gp.y, 1), current_net_id_)) {
                start_gp = GridPoint(start_gp.x, start_gp.y, 1);
            }
        }
        
        // Try to find best access point for end pin
        if (!findBestAccessPoint(end_gp.x, end_gp.y, current_net_id_, end_gp)) {
            // Fallback: try direct elevation if smart access fails
            if (grid_->isFree(GridPoint(end_gp.x, end_gp.y, 1), current_net_id_)) {
                end_gp = GridPoint(end_gp.x, end_gp.y, 1);
            }
        }
        
        // Debug: Check if we're dealing with the conflict hotspot
        if ((start_gp.x == 4 && start_gp.y == 6) || (end_gp.x == 4 && end_gp.y == 6)) {
            std::cout << "DEBUG: Net " << current_net_id_ 
                      << " routing through hotspot area. Start: (" 
                      << start_gp.x << "," << start_gp.y << ") End: (" 
                      << end_gp.x << "," << end_gp.y << ")" << std::endl;
        }
        
        // 4. Call core A* algorithm for each segment
        std::vector<GridPoint> segment;
        
        // **CRITICAL FIX**: Temporarily mark start/end as FREE for A* to work
        // Note: We work with the elevated points (M2) but restore original M1 PIN states later
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
            
            // **TARGET ELEVATION FIX**: Always restore M1 PIN states (not elevated M2 states)
            // The actual pins are on M1 and must remain marked as PIN
            grid_->setState(start_gp_m1, GridState::PIN);
            grid_->setState(end_gp_m1, GridState::PIN);
        } else {
            // **TARGET ELEVATION FIX**: Restore M1 PIN states even if routing failed
            grid_->setState(start_gp_m1, GridState::PIN);
            grid_->setState(end_gp_m1, GridState::PIN);
            
            // **FAILURE CLASSIFICATION PROBE**: Diagnose why routing failed
            static int fail_count = 0;
            if (++fail_count <= 5) {  // Only show first 5 failures to avoid spam
                std::cout << "\n>>> FAILURE DIAGNOSIS #" << fail_count << " <<<" << std::endl;
                std::cout << "Net: " << net->getName() << std::endl;
                
                // Check M1 PIN status
                bool start_m1_ok = (grid_->getState(GridPoint(start_gp_m1.x, start_gp_m1.y, 0)) == GridState::PIN);
                bool end_m1_ok = (grid_->getState(GridPoint(end_gp_m1.x, end_gp_m1.y, 0)) == GridState::PIN);
                
                // Check M2 access availability  
                bool start_m2_ok = grid_->isFree(GridPoint(start_gp_m1.x, start_gp_m1.y, 1), current_net_id_);
                bool end_m2_ok = grid_->isFree(GridPoint(end_gp_m1.x, end_gp_m1.y, 1), current_net_id_);
                
                // Check M3 overlay
                bool m3_blocked = grid_->getState(GridPoint(start_gp_m1.x, start_gp_m1.y, 2)) == GridState::OBSTACLE;
                
                std::cout << "START Point: M1=" << (start_m1_ok ? "PIN" : "BLOCK/FREE") 
                          << ", M2 Access=" << (start_m2_ok ? "OK" : "BLOCKED!") << std::endl;
                std::cout << "END   Point: M1=" << (end_m1_ok ? "PIN" : "BLOCK/FREE") 
                          << ", M2 Access=" << (end_m2_ok ? "OK" : "BLOCKED!") << std::endl;
                std::cout << "M3 Overlay: " << (m3_blocked ? "BLOCKED (Error)" : "FREE") << std::endl;
                
                // Classification
                if (!start_m2_ok && !end_m2_ok) {
                    std::cout << "CLASSIFICATION: DOUBLE LAUNCH FAIL (Both ends blocked on M2)" << std::endl;
                } else if (!start_m2_ok) {
                    std::cout << "CLASSIFICATION: LAUNCH FAIL (Start blocked on M2)" << std::endl;
                } else if (!end_m2_ok) {
                    std::cout << "CLASSIFICATION: LANDING FAIL (End blocked on M2)" << std::endl;
                } else {
                    std::cout << "CLASSIFICATION: NO PATH (M2 access OK but no path found)" << std::endl;
                }
                std::cout << "========================================" << std::endl;
            }
            
            // **GRID MICROSCOPE**: Analyze failed segment to detect coordinate mismatch
            static int microscope_count = 0;
            if (microscope_count < 3) {  // Only show first 3 failures to avoid spam
                microscope_count++;
                
                auto& target = end_gp;  // Check the target endpoint
                int cx = target.x;
                int cy = target.y;
                int cz = target.layer;
                
                std::cout << "\n=== GRID MICROSCOPE #" << microscope_count 
                          << " (Failed Segment Target) ===" << std::endl;
                std::cout << "Target Physical: (" << end_phys.x << ", " 
                          << end_phys.y << ")" << std::endl;
                std::cout << "Target Grid: (" << cx << ", " << cy << ", " << cz << ")" << std::endl;
                
                // Print 5x5 grid state around target
                std::cout << "Grid State (5x5 area):" << std::endl;
                for (int y = cy + 2; y >= cy - 2; --y) {
                    printf("y=%-3d | ", y);
                    for (int x = cx - 2; x <= cx + 2; ++x) {
                        if (!grid_->isValid(GridPoint(x, y, cz))) {
                            std::cout << " X ";
                            continue;
                        }
                        
                        auto state = grid_->getState(GridPoint(x, y, cz));
                        if (state == GridState::PIN) std::cout << " P ";
                        else if (state == GridState::OBSTACLE) std::cout << " # ";
                        else if (state == GridState::FREE) std::cout << " . ";
                        else if (state == GridState::ROUTED) std::cout << " = ";
                        else std::cout << " ? ";
                    }
                    std::cout << "\n";
                }
                std::cout << "Legend: P=Pin, #=Obstacle, .=Free, ==Routed, X=Out of bounds" << std::endl;
                
                // Check if start point is properly marked as PIN
                auto start_state = grid_->getState(start_gp);
                auto end_state = grid_->getState(end_gp);
                std::cout << "Start point state: " << (start_state == GridState::PIN ? "PIN" : "NOT PIN") << std::endl;
                std::cout << "End point state: " << (end_state == GridState::PIN ? "PIN" : "NOT PIN") << std::endl;
                
                // Show grid conversion details
                double pitch_x = grid_->getPitchX();
                double pitch_y = grid_->getPitchY();
                std::cout << "Grid pitch: X=" << pitch_x << ", Y=" << pitch_y << std::endl;
                std::cout << "Expected grid X: (round(" << end_phys.x << " / " << pitch_x << ")) = " 
                          << static_cast<int>(std::round(end_phys.x / pitch_x)) << std::endl;
                std::cout << "Expected grid Y: (round(" << end_phys.y << " / " << pitch_y << ")) = " 
                          << static_cast<int>(std::round(end_phys.y / pitch_y)) << std::endl;
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
                    
                    // Debug probe for our stubborn conflict points
                    if ((x == 4 && y == 6 && z == 1) || (x == 16 && y == 6 && z == 1)) {
                        double new_cost = grid_->getHistoryCost(x, y, z);
                        std::cout << "  >>> PUNISHING (" << x << "," << y << "," << z 
                                  << "): Usage=" << usage << ", Penalty=" << penalty 
                                  << ", New HistCost=" << new_cost << std::endl;
                    }
                }
            }
        }
    }
    
    std::cout << "  Applied penalties to " << punished_count << " congested points" << std::endl;
}

void MazeRouter::saveBestSolution(const std::vector<RoutingResult>& results, int iteration, int conflicts) {
    if (conflicts < min_conflicts_) {
        std::cout << ">>> NEW RECORD! Conflicts: " << conflicts << " (Saving at iteration " << iteration << ") <<<" << std::endl;
        min_conflicts_ = conflicts;
        best_iteration_ = iteration;
        
        // Clear previous best solution
        best_solution_segments_.clear();
        
        // Save all successful routing segments
        for (const auto& result : results) {
            if (result.success) {
                for (const auto& segment : result.segments) {
                    best_solution_segments_.push_back(segment);
                }
            }
        }
        
        std::cout << "  Saved " << best_solution_segments_.size() << " segments from best solution" << std::endl;
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
    
    // Restore best solution segments to grid
    // This will recreate the best conflict state found during iterations
    for (const auto& segment : best_solution_segments_) {
        grid_->markPath(segment, 0);  // Use 0 as default net_id for visualization
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
        
        
        
        // Original smart access logging
        if (best_x != pin_gx || best_y != pin_gy) {
            std::cout << "Smart Access: Net " << net_id 
                      << " shifted from (" << pin_gx << "," << pin_gy << ") to (" 
                      << best_x << "," << best_y << ") [Cost: " << min_cost << "]" << std::endl;
        }
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

} // namespace mini