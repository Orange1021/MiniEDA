/**
 * @file routing_interface.cpp
 * @brief High-level Routing Interface Implementation
 */

#include "routing_interface.h"
#include "maze_router.h"
#include "routing_grid.h"
#include "../../lib/include/lef_parser.h"
#include "../../lib/include/liberty_parser.h"
#include "../../lib/include/lef_pin_mapper.h"
#include "../../lib/include/visualizer.h"
#include "../../lib/include/placer_db.h"
#include "../../apps/mini_placement/macro_mapper.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <numeric>
#include <chrono>

namespace mini {

void placeIOPorts(PlacerDB& placer_db) {
    std::cout << "Placing I/O ports..." << std::endl;
    
    auto all_cells = placer_db.getAllCells();
    std::vector<Cell*> io_ports;
    
    // Separate I/O ports from standard cells
    for (Cell* cell : all_cells) {
        if (cell->isPort()) {
            io_ports.push_back(cell);
        }
    }
    
    // Place I/O ports around the periphery
    double core_width = placer_db.getCoreArea().width();
    double core_height = placer_db.getCoreArea().height();
    
    size_t io_count = io_ports.size();
    if (io_count == 0) return;
    
    // Distribute I/O ports evenly around the perimeter
    for (size_t i = 0; i < io_count; ++i) {
        Cell* port = io_ports[i];
        double position = static_cast<double>(i) / io_count * 2.0 * M_PI;
        
        double x, y;
        if (position < M_PI / 2) {
            // Top edge
            x = position / (M_PI / 2) * core_width;
            y = core_height;
        } else if (position < M_PI) {
            // Right edge
            x = core_width;
            y = core_height - (position - M_PI / 2) / (M_PI / 2) * core_height;
        } else if (position < 3 * M_PI / 2) {
            // Bottom edge
            x = core_width - (position - M_PI) / (M_PI / 2) * core_width;
            y = 0.0;
        } else {
            // Left edge
            x = 0.0;
            y = (position - 3 * M_PI / 2) / (M_PI / 2) * core_height;
        }
        
        // **EXPERIMENT**: Force some I/O ports to different rows to create vertical routing needs
        if (i % 3 == 0) {
            y = core_height * 0.8;  // Force to upper region
        } else if (i % 3 == 1) {
            y = core_height * 0.2;  // Force to lower region
        }
        
        port->setX(x);
        port->setY(y);
        port->setPlaced(true);
    }
    
    std::cout << "  Placed " << io_count << " I/O ports" << std::endl;
}

std::unordered_map<std::string, Point> buildPinLocations(
    PlacerDB& placer_db,
    const LefLibrary& lef_lib,
    const MacroMapper& macro_mapper) {
    
    std::cout << "Building pin locations map..." << std::endl;
    
    // Create PinMapper for unified key generation
    auto pin_mapper = std::make_unique<LefPinMapper>(lef_lib, macro_mapper);
    std::unordered_map<std::string, Point> pin_locations;
    
    auto all_cells = placer_db.getAllCells();
    size_t total_pins = 0;
    
    for (Cell* cell : all_cells) {
        if (cell->isPort()) {
            // I/O Port handling - use unified key generation
            std::string key = pin_mapper->getPinKey(cell, nullptr);  // Port has no pin
            Point center = placer_db.getCellCenter(cell);
            pin_locations[key] = center;
            total_pins++;
            
        } else {
            // Standard Cell handling
            std::string cell_type_name = cellTypeToString(cell->getType());
            const LefMacro* macro = macro_mapper.mapType(cell_type_name);
            
            if (!macro) {
                std::cout << "  Warning: No LEF macro found for cell type " << cell_type_name << std::endl;
                continue;
            }
            
            // Process each pin of the cell
            for (const auto& pin_ptr : cell->getPins()) {
                Pin* pin = pin_ptr.get();

                // Generate unified key using the unified method
                std::string key = pin_mapper->getPinKey(cell, pin);

                // Calculate pin center offset from macro origin using LEF data
                Point pin_offset = Point(0, 0);  // Default offset

                // Try to get pin geometry from LEF
                if (macro->getPin(pin->getName())) {
                    const LefPort* port = macro->getPin(pin->getName());
                    if (port && !port->rects.empty()) {
                        // Use the center of the first rectangle as pin offset
                        const Rect& rect = port->rects[0];
                        pin_offset.x = (rect.x_min + rect.x_max) / 2.0;
                        pin_offset.y = (rect.y_min + rect.y_max) / 2.0;
                    }
                }

                Point cell_origin = Point(cell->getX(), cell->getY());
                Point abs_position = cell_origin + pin_offset;

                pin_locations[key] = abs_position;
                total_pins++;
            }
        }
    }
    
    std::cout << "  Built pin locations map with " << total_pins << " pins" << std::endl;
    return pin_locations;
}

std::vector<RoutingResult> RoutingInterface::runRouting(
    const RoutingConfig& config,
    std::shared_ptr<NetlistDB> netlist_db,
    std::unique_ptr<PlacerDB>& placer_db) {
    
    std::vector<RoutingResult> results;
    
    try {
        // Phase 1: Parse LEF library
        if (config.verbose) {
            std::cout << "Parsing LEF library: " << config.lef_file << std::endl;
        }
        LefParser lef_parser(config.lef_file, config.verbose);
        auto lef_lib = std::make_unique<LefLibrary>(lef_parser.parse());
        if (!lef_lib) {
            std::cerr << "Error: Failed to parse LEF file" << std::endl;
            return results;
        }
        
        // Phase 2: Parse Liberty library
        if (config.verbose) {
            std::cout << "Parsing Liberty library: " << config.liberty_file << std::endl;
        }
        LibertyParser lib_parser;
        auto library = lib_parser.parseFile(config.liberty_file);
        if (!library) {
            std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
            return results;
        }
        
        // Phase 3: Place I/O ports
        placeIOPorts(*placer_db);
        
        // Phase 4: Initialize routing grid
        RoutingGrid routing_grid;
        Rect core_area = placer_db->getCoreArea();
        double pitch = 0.095;  // Standard pitch
        routing_grid.init(core_area, pitch, pitch);
        
        if (config.verbose) {
            std::cout << "  Routing grid initialized: " << routing_grid.getGridWidth()
                      << "x" << routing_grid.getGridHeight() << " x" << routing_grid.getNumLayers()
                      << " layers" << std::endl;
        }
        
        // Phase 5: Add cell obstacles (skip I/O ports)
        if (config.verbose) {
            std::cout << "  Adding cell obstacles to routing grid..." << std::endl;
        }
        int obstacle_count = 0;
        for (Cell* cell : placer_db->getAllCells()) {
            if (cell->isPort()) continue;  // Skip I/O ports

            const auto& info = placer_db->getCellInfo(cell);
            double cell_x = cell->getX();
            double cell_y = cell->getY();
            double cell_width = info.width;
            double cell_height = info.height;

            // Create obstacle rectangle on Layer 0 (M1)
            Rect obstacle(cell_x, cell_y, cell_x + cell_width, cell_y + cell_height);
            routing_grid.addObstacle(obstacle, 0);
            obstacle_count++;
        }
        if (config.verbose) {
            std::cout << "    Added " << obstacle_count << " cell obstacles on Layer 0" << std::endl;
        }
        
        // Phase 6: Build pin locations map
        MacroMapper macro_mapper(*lef_lib);
        auto pin_locations = buildPinLocations(*placer_db, *lef_lib, macro_mapper);
        
        // Phase 7: Run maze routing
        LefPinMapper pin_mapper(*lef_lib, macro_mapper);
        MazeRouter router(&routing_grid, &pin_mapper, placer_db.get());
        router.setViaCost(config.via_cost);
        router.setWireCostPerUnit(config.wire_cost);
        
        if (config.verbose) {
            std::cout << "Routing " << netlist_db->getNumNets() << " nets..." << std::endl;
        }
        
        // **CRITICAL FIX**: Net ordering optimization - small nets first
        std::vector<Net*> nets_to_route;
        for (const auto& net_ptr : netlist_db->getNets()) {
            Net* net = net_ptr.get();
            if (!net) continue;
            
            // Skip power/ground nets
            if (net->getName() == "VDD" || net->getName() == "VSS" || net->getName() == "GND") {
                continue;
            }
            
            nets_to_route.push_back(net);
        }
        
        // Sort nets by HPWL (Half-Perimeter Wire Length) - small nets first
        std::sort(nets_to_route.begin(), nets_to_route.end(), 
            [&pin_locations](Net* a, Net* b) {
                // Calculate HPWL for net A
                double hpwl_a = std::numeric_limits<double>::max();
                auto pins_a = a->getAllPins();
                if (!pins_a.empty()) {
                    double min_x = std::numeric_limits<double>::max();
                    double max_x = std::numeric_limits<double>::lowest();
                    double min_y = std::numeric_limits<double>::max();
                    double max_y = std::numeric_limits<double>::lowest();
                    
                    for (Pin* pin : pins_a) {
                        std::string pin_key = pin->getOwner()->getName() + ":" + pin->getName();
                        auto it = pin_locations.find(pin_key);
                        if (it != pin_locations.end()) {
                            const Point& p = it->second;
                            min_x = std::min(min_x, p.x);
                            max_x = std::max(max_x, p.x);
                            min_y = std::min(min_y, p.y);
                            max_y = std::max(max_y, p.y);
                        }
                    }
                    hpwl_a = (max_x - min_x) + (max_y - min_y);
                }
                
                // Calculate HPWL for net B
                double hpwl_b = std::numeric_limits<double>::max();
                auto pins_b = b->getAllPins();
                if (!pins_b.empty()) {
                    double min_x = std::numeric_limits<double>::max();
                    double max_x = std::numeric_limits<double>::lowest();
                    double min_y = std::numeric_limits<double>::max();
                    double max_y = std::numeric_limits<double>::lowest();
                    
                    for (Pin* pin : pins_b) {
                        std::string pin_key = pin->getOwner()->getName() + ":" + pin->getName();
                        auto it = pin_locations.find(pin_key);
                        if (it != pin_locations.end()) {
                            const Point& p = it->second;
                            min_x = std::min(min_x, p.x);
                            max_x = std::max(max_x, p.x);
                            min_y = std::min(min_y, p.y);
                            max_y = std::max(max_y, p.y);
                        }
                    }
                    hpwl_b = (max_x - min_x) + (max_y - min_y);
                }
                
                // Small HPWL first (ascending order)
                return hpwl_a < hpwl_b;
            });
        
// PathFinder parameters
        const int max_iterations = 30;  // Increased to allow more iterations for convergence
        const double initial_collision_penalty = 50.0;  // Start gentle, then grow aggressive
        const double penalty_growth_rate = 1.3;  // EXPONENTIAL: Now using 1.3x growth for strong pressure
        bool solution_found = false;
        
        // Track penalty for reporting
        double current_penalty = initial_collision_penalty;
        
        // Track conflict history for final analysis
        std::vector<int> conflict_history;
        
        // **STAGNATION DETECTION**: Track improvement to enable early termination
        int stagnation_count = 0;
        int last_conflicts = INT_MAX;
        
        // **NUCLEAR PATHFINDER**: Track history increment for escalating congestion pressure
        double current_history_increment = 1.0;
        
        for (int iter = 0; iter < max_iterations && !solution_found; ++iter) {
            std::cout << "\n>>> PathFinder Iteration " << iter << " <<<" << std::endl;
            std::cout << "Current collision penalty: " << current_penalty << std::endl;
            std::cout << "Current history increment: " << current_history_increment << std::endl;
            
            // Set collision penalty for this iteration (AGGRESSIVE exponential growth)
            // This creates massive pressure to find alternative routes
            router.setCollisionPenalty(current_penalty);
            
            // **NUCLEAR PATHFINDER**: Set escalating history increment
            // This makes congested cells increasingly "toxic" over time
            routing_grid.setHistoryIncrement(current_history_increment);
            
            // Clear previous iteration routes but preserve history costs
            if (iter > 0) {
                routing_grid.clearRoutes();
                
                // Update history costs based on previous conflicts
                int conflicts = routing_grid.countConflicts();
                if (config.verbose) {
                    std::cout << "  Updating history costs based on " << conflicts << " conflicts" << std::endl;
                }
                
                // **NUCLEAR PATHFINDER**: Add escalating history penalty to conflicted areas
                for (int z = 0; z < routing_grid.getNumLayers(); ++z) {
                    for (int y = 0; y < routing_grid.getGridHeight(); ++y) {
                        for (int x = 0; x < routing_grid.getGridWidth(); ++x) {
                            GridPoint gp(x, y, z);
                            if (routing_grid.getState(gp) == GridState::ROUTED) {
                                // Use dynamic history increment instead of fixed 1.0
                                routing_grid.addHistoryCost(x, y, z, current_history_increment);
                            }
                        }
                    }
                }
            }
            
            // **FORCED TURN-BY-TURN**: Randomize net order to break deadlocks
            results.clear();
            router.resetStatistics();  // Reset for clean iteration
            
            // Create indices for all nets
            std::vector<int> net_indices(nets_to_route.size());
            std::iota(net_indices.begin(), net_indices.end(), 0);  // 0, 1, 2, ...
            
            // **CRITICAL**: Use time-based seed for true randomness each iteration
            // This ensures different ordering each round, breaking persistent deadlocks
            auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            std::mt19937 rng(seed);
            
            // Shuffle order completely randomly each iteration
            std::shuffle(net_indices.begin(), net_indices.end(), rng);
            
            std::cout << "  Routing " << nets_to_route.size() << " nets in randomized order (seed=" << seed << ")" << std::endl;
            
            for (int net_idx : net_indices) {
                Net* net = nets_to_route[net_idx];
                RoutingResult result = router.routeNet(net, pin_locations);
                
                // **DIAGNOSTIC PROBE**: Track which networks fail
                if (!result.success) {
                    std::cout << "  FAILED NET: " << net->getName() 
                              << " - Reason: " << result.error_message << std::endl;
                }
                
                results.push_back(result);  // Push all results for accurate statistics
            }
            
            // Check for convergence
            int conflicts = routing_grid.countConflicts();
            double total_wirelength;
            int total_vias, routed_count, failed_count;
            getRoutingStatistics(results, total_wirelength, total_vias, routed_count, failed_count);
            
            // Record conflict history for final analysis
            conflict_history.push_back(conflicts);
            
            // **STAGNATION DETECTION**: Check for improvement or stagnation
            if (conflicts < last_conflicts) {
                // Found improvement - reset stagnation counter
                stagnation_count = 0;
                std::cout << "  Improvement: " << last_conflicts << " -> " << conflicts << " conflicts" << std::endl;
            } else {
                // No improvement - increment stagnation counter
                stagnation_count++;
                std::cout << "  Stagnation count: " << stagnation_count << " (conflicts stuck at " << conflicts << ")" << std::endl;
            }
            last_conflicts = conflicts;
            
            // **BEST SOLUTION TRACKING**: Save if this is the best solution found
            router.saveBestSolution(results, iter, conflicts);
            
            // **EARLY TERMINATION CONDITIONS**
            // 1. Perfect solution found
            if (conflicts == 0 && routed_count == static_cast<int>(nets_to_route.size())) {
                std::cout << "\n*** PERFECT SOLUTION FOUND! Early termination. ***" << std::endl;
                solution_found = true;
                break;
            }
            
            // 2. Stagnation detected - no improvement for too long
            if (stagnation_count >= 7) {
                std::cout << "\n>>> STAGNATION DETECTED: No improvement for " << stagnation_count 
                          << " iterations. Stopping early. <<<" << std::endl;
                break;
            }
            
            // 3. Divergence detected - conflicts getting much worse than best
            if (conflicts > router.getMinConflicts() + 10) {
                std::cout << "\n>>> DIVERGENCE DETECTED: Conflicts (" << conflicts 
                          << ") much worse than best (" << router.getMinConflicts() 
                          << "). Stopping early. <<<" << std::endl;
                break;
            }
            
            int segments_attempted = router.getTotalSegmentsAttempted();
            int segments_succeeded = router.getTotalSegmentsSucceeded();
            int segments_failed = router.getTotalSegmentsFailed();
            double segment_success_rate = router.getSegmentSuccessRate();
            
            std::cout << "  Success: " << routed_count << "/" << nets_to_route.size() 
                      << " nets, Conflicts: " << conflicts << std::endl;
            std::cout << "  Segments: " << segments_succeeded << "/" << segments_attempted 
                      << " (" << std::fixed << std::setprecision(1) << segment_success_rate << "%)" << std::endl;
            
            // Check if we have a valid solution (all nets routed, no conflicts)
            if (conflicts == 0 && routed_count == static_cast<int>(nets_to_route.size())) {
                std::cout << "\n*** VALID SOLUTION FOUND! ***" << std::endl;
                solution_found = true;
            }
            
            // [MISSING LINK] Congestion History Update Loop
            // This is the critical missing piece that connects conflict detection to history costs
            if (conflicts > 0) {
                std::cout << "  Updating history costs based on " << conflicts << " conflicts" << std::endl;
                router.updateHistoryCosts(current_history_increment);
            }
            
            // Early termination if no improvement in recent iterations
            // DISABLED: Let PathFinder run through all iterations as intended
            // PathFinder's philosophy is "chaos first, order later"
            // High conflict counts in early iterations are normal and necessary
            // if (iter > 2 && conflicts > 50) {
            //     std::cout << "\nEarly termination: too many conflicts remaining" << std::endl;
            //     break;
            // }
            
// **NUCLEAR PATHFINDER**: Exponential penalty explosion to force detours
            // This makes collision cost astronomically higher than detour cost
            
            // 1. Explosive exponential growth with very high ceiling (forces detours at all costs)
            double next_penalty = current_penalty * 1.5;  // AGGRESSIVE: 1.5x per iteration
            if (next_penalty > 100000.0) next_penalty = 100000.0;  // NO CAP: Allow up to 100000!
            
            router.setCollisionPenalty(next_penalty);
            
            // **CRITICAL FIX**: Update current_penalty for next iteration
            current_penalty = next_penalty;
            
            
            
            // 2. Strong history increment growth (maintains massive pressure)
            current_history_increment = 1.0 + (iter * 0.25);  // Strong growth
            if (current_history_increment > 20.0) current_history_increment = 20.0;  // Cap at 20.0
            
            // Cap penalties to prevent numerical overflow
            const double max_penalty = 100000.0;  // Increased cap for nuclear mode
            if (current_penalty > max_penalty) {
                current_penalty = max_penalty;
            }
            
            const double max_history_increment = 50.0;  // Cap history increment
            if (current_history_increment > max_history_increment) {
                current_history_increment = max_history_increment;
            }
        }
        
        // **RESTORE BEST SOLUTION**: Restore the best solution found during iterations
        std::cout << "\n=== RESTORING BEST SOLUTION ===" << std::endl;
        router.restoreBestSolution();
        
        // **CONFLICT EVOLUTION ANALYSIS**: Report conflict changes over all iterations
        std::cout << "\n=== CONFLICT EVOLUTION ANALYSIS ===" << std::endl;
        std::cout << "Total iterations: " << conflict_history.size() << std::endl;
        
        if (!conflict_history.empty()) {
            std::cout << "Initial conflicts: " << conflict_history[0] << std::endl;
            std::cout << "Final conflicts: " << conflict_history.back() << std::endl;
            
            // Find minimum conflicts and when it occurred
            auto min_it = std::min_element(conflict_history.begin(), conflict_history.end());
            int min_conflicts = *min_it;
            int min_iteration = std::distance(conflict_history.begin(), min_it);
            std::cout << "Minimum conflicts: " << min_conflicts << " (at iteration " << min_iteration << ")" << std::endl;
            
            // Calculate conflict reduction
            int total_reduction = conflict_history[0] - conflict_history.back();
            double reduction_percentage = (conflict_history[0] > 0) ? 
                (static_cast<double>(total_reduction) / conflict_history[0] * 100.0) : 0.0;
            std::cout << "Total conflict reduction: " << total_reduction 
                      << " (" << std::fixed << std::setprecision(1) << reduction_percentage << "%)" << std::endl;
            
            // Show conflict trend at key iterations
            std::cout << "Conflict trend at key iterations:" << std::endl;
            int report_interval = std::max(1, static_cast<int>(conflict_history.size() / 5));
            for (size_t i = 0; i < conflict_history.size(); i += report_interval) {
                std::cout << "  Iter " << i << ": " << conflict_history[i] << " conflicts" << std::endl;
            }
            // Always show the last iteration
            if (conflict_history.size() % report_interval != 0) {
                std::cout << "  Iter " << (conflict_history.size() - 1) 
                          << ": " << conflict_history.back() << " conflicts" << std::endl;
            }
        }
        std::cout << "=====================================" << std::endl;
        
        // **CONFLICT LOCATION ANALYSIS**: Print detailed conflict information
        if (!conflict_history.empty() && conflict_history.back() > 0) {
            routing_grid.printConflictLocations();
            
            // **FOUNDATION CHECK**: Inspect Layer 0 at the conflict hotspot (4,6)
            std::cout << "\n>>> FOUNDATION CHECK (Layer 0 at 4,6) <<<" << std::endl;
            int tx = 4, ty = 6;
            
            auto state_m0 = routing_grid.getState(GridPoint(tx, ty, 0));
            std::cout << "State at (4,6,0): ";
            if (state_m0 == GridState::PIN) {
                int pin_net_id = routing_grid.getNetId(GridPoint(tx, ty, 0));
                std::cout << "PIN (Net " << pin_net_id << ")";
            } else if (state_m0 == GridState::OBSTACLE) {
                std::cout << "OBSTACLE";
            } else {
                std::cout << "FREE/OTHER";
            }
            std::cout << std::endl;
            
            // Check surrounding area for other pins
            std::cout << "Neighbors at Layer 0:" << std::endl;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = tx + dx, ny = ty + dy;
                    if (routing_grid.isValid(GridPoint(nx, ny, 0))) {
                        auto neighbor_state = routing_grid.getState(GridPoint(nx, ny, 0));
                        if (neighbor_state == GridState::PIN) {
                            int neighbor_net_id = routing_grid.getNetId(GridPoint(nx, ny, 0));
                            std::cout << "  (" << nx << "," << ny << ") is PIN (Net " 
                                      << neighbor_net_id << ")" << std::endl;
                        }
                    }
                }
            }
            
            // **NET ID ANALYSIS**: Identify which nets are involved in conflicts
            std::cout << "\n=== NET ID CONFLICT ANALYSIS ===" << std::endl;
            
            // Map net_id_hash -> net_name for reporting
            std::unordered_map<int, std::string> net_id_to_name;
            for (Net* net : nets_to_route) {
                int net_id_hash = std::hash<std::string>{}(net->getName());
                net_id_to_name[net_id_hash] = net->getName();
            }
            
            // Report conflicting nets
            std::cout << "Conflicting Net IDs found:" << std::endl;
            for (const auto& [net_id, net_name] : net_id_to_name) {
                std::cout << "  Net ID " << net_id << " = " << net_name << std::endl;
            }
            std::cout << "================================" << std::endl;
            
            // **SURROUNDING CHECK**: Inspect Layer 1 escape routes around the hotspot
            std::cout << "\n>>> SURROUNDING CHECK (Layer 1 at 4,6) <<<" << std::endl;
            std::cout << "Checking escape routes in M2 layer around (4,6,1):" << std::endl;
            
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int nx = tx + dx, ny = ty + dy;
                    
                    if (routing_grid.isValid(GridPoint(nx, ny, 1))) {
                        double history = routing_grid.getHistoryCost(nx, ny, 1);
                        auto state = routing_grid.getState(GridPoint(nx, ny, 1));
                        int net_id = routing_grid.getNetId(GridPoint(nx, ny, 1));
                        
                        printf("(%d,%d) ", nx, ny);
                        if (state == GridState::OBSTACLE) {
                            printf("BLK ");
                        } else if (state == GridState::ROUTED) {
                            printf("Rtd(N%d) ", net_id); // Show which net occupies it
                        } else if (state == GridState::PIN) {
                            printf("PIN(N%d) ", net_id);
                        } else {
                            printf("Free ");
                        }
                        
                        printf("Hist=%.1f\n", history);
                    } else {
                        printf("(%d,%d) Invalid\n", nx, ny);
                    }
                }
            }
            std::cout << "=========================================" << std::endl;
        }
        
        // Final statistics
        {
            double total_wirelength;
            int total_vias, routed_count, failed_count;
            getRoutingStatistics(results, total_wirelength, total_vias, routed_count, failed_count);
            
            int segments_attempted = router.getTotalSegmentsAttempted();
            int segments_succeeded = router.getTotalSegmentsSucceeded();
            int segments_failed = router.getTotalSegmentsFailed();
            double segment_success_rate = router.getSegmentSuccessRate();
            
            std::cout << "\n  **PATHFINDER FINAL RESULTS:**" << std::endl;
            std::cout << "    Networks routed: " << routed_count << "/" << nets_to_route.size() << std::endl;
            std::cout << "    Segments attempted: " << segments_attempted << std::endl;
            std::cout << "    Segments succeeded: " << segments_succeeded << std::endl;
            std::cout << "    Real success rate: " << std::fixed << std::setprecision(1) 
                      << segment_success_rate << "%" << std::endl;
            std::cout << "  Traditional Statistics:" << std::endl;
            std::cout << "    Total wirelength: " << total_wirelength << std::endl;
            std::cout << "    Total vias: " << total_vias << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in routing: " << e.what() << std::endl;
    }
    
    return results;
}

std::vector<RoutingResult> RoutingInterface::runRoutingWithVisualization(
    const RoutingConfig& config,
    std::shared_ptr<NetlistDB> netlist_db,
    std::unique_ptr<PlacerDB>& placer_db,
    Visualizer* visualizer) {
    
    // Run routing first
    auto results = runRouting(config, netlist_db, placer_db);
    
    // Generate visualization if requested
    if (visualizer && !results.empty()) {
        // Create routing grid for visualization
        LefParser lef_parser(config.lef_file, config.verbose);
        auto lef_lib = std::make_unique<LefLibrary>(lef_parser.parse());
        
        if (lef_lib) {
            RoutingGrid routing_grid;
            Rect core_area = placer_db->getCoreArea();
            double pitch = 0.095;
            routing_grid.init(core_area, pitch, pitch);
            
            std::string output_name = "visualizations/" + config.run_id + "/post_routing";
            visualizer->drawRoutedPlacement(output_name, results, &routing_grid);
        }
    }
    
    return results;
}

void RoutingInterface::getRoutingStatistics(
    const std::vector<RoutingResult>& results,
    double& total_wirelength,
    int& total_vias,
    int& routed_count,
    int& failed_count) {
    
    total_wirelength = 0.0;
    total_vias = 0;
    routed_count = 0;
    failed_count = 0;
    
    for (const auto& result : results) {
        total_wirelength += result.total_wirelength;
        total_vias += result.total_vias;
        
        if (result.success) {
            routed_count++;
        } else {
            failed_count++;
        }
    }
}

} // namespace mini