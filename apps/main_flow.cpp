/**
 * @file main_flow.cpp
 * @brief MiniEDA Integrated Flow - Placement + Routing + STA
 * @details Complete EDA flow from netlist to post-routing timing analysis
 */

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>

// Core libraries
#include "../lib/include/netlist_db.h"
#include "../lib/include/verilog_parser.h"
#include "../lib/include/liberty_parser.h"
#include "../lib/include/lef_parser.h"

// NEW: Unified configuration management
#include "../lib/include/app_config.h"
#include "../lib/include/arg_parser.h"

// Placement modules
#include "../apps/mini_placement/placement_interface.h"
#include "../apps/mini_placement/placer_engine.h"
#include "../lib/include/placer_db.h"

// Routing modules
#include "../apps/mini_router/routing_interface.h"
#include "../apps/mini_router/maze_router.h"  // For RoutingResult definition

// STA modules
#include "../apps/mini_sta/sta_engine.h"
#include "../apps/mini_sta/timing_graph.h"
#include "../apps/mini_sta/delay_model.h"
#include "../apps/mini_sta/timing_constraints.h"
#include "../apps/mini_sta/cell_mapper.h"

using namespace mini;

/**
 * @brief Convert AppConfig to PlacementConfig
 */
PlacementConfig toPlacementConfig(const AppConfig& app_config) {
    PlacementConfig placement_config;
    placement_config.lef_file = app_config.lef_file;
    placement_config.liberty_file = app_config.liberty_file;
    placement_config.utilization = app_config.utilization;
    placement_config.row_height = app_config.row_height;
    placement_config.verbose = app_config.verbose;
    placement_config.run_id = app_config.run_id + "_placement";
    
    // Transfer placement algorithm parameters
    // Note: target_density should match utilization for consistent placement
    placement_config.target_density = app_config.utilization;
    placement_config.initial_lambda = app_config.placement_initial_lambda;
    placement_config.lambda_growth_rate = app_config.placement_lambda_growth_rate;
    placement_config.learning_rate = app_config.placement_learning_rate;
    placement_config.momentum = app_config.placement_momentum;
    placement_config.convergence_threshold = app_config.placement_convergence_threshold;
    
    // Transfer additional parameters
    placement_config.default_cell_area = app_config.default_cell_area;
    placement_config.site_width = app_config.site_width;
    placement_config.density_margin = app_config.placement_density_margin;
    placement_config.max_gradient_ratio = app_config.placement_max_gradient_ratio;
    placement_config.max_displacement_ratio = app_config.placement_max_displacement_ratio;
    placement_config.placement_hpwl_convergence_ratio = app_config.placement_hpwl_convergence_ratio;
    placement_config.placement_detailed_iterations = app_config.placement_detailed_iterations;
    placement_config.placement_warmup_stop_ratio = app_config.placement_warmup_stop_ratio;
    
    return placement_config;
}

/**
 * @brief Convert AppConfig to RoutingConfig
 */
RoutingConfig toRoutingConfig(const AppConfig& app_config) {
    RoutingConfig routing_config;
    routing_config.lef_file = app_config.lef_file;
    routing_config.liberty_file = app_config.liberty_file;
    routing_config.via_cost = app_config.via_cost;
    routing_config.wire_cost = app_config.wire_cost;
    routing_config.routing_pitch = app_config.routing_pitch;
    routing_config.routing_grid_fine_factor = app_config.routing_grid_fine_factor;
    routing_config.verbose = app_config.verbose;
    routing_config.run_id = app_config.run_id;
    
    // Routing algorithm parameters
    routing_config.initial_collision_penalty = app_config.initial_collision_penalty;
    routing_config.penalty_growth_rate = app_config.penalty_growth_rate;
    routing_config.max_penalty = app_config.max_penalty;
    routing_config.initial_history_increment = app_config.initial_history_increment;
    routing_config.max_history_increment = app_config.max_history_increment;
    routing_config.history_increment_growth_rate = app_config.history_increment_growth_rate;
    routing_config.decay_factor = app_config.decay_factor;
    routing_config.distance_weight = app_config.distance_weight;
    
    return routing_config;
}

/**
 * @brief Auto-detect row height from LEF library (with fallback to config)
 */
double detectRowHeight(const AppConfig& config) {
    double row_height = config.row_height;  // Start with config value
    
    if (!config.lef_file.empty()) {
        try {
            LefParser lef_parser(config.lef_file, config.verbose);
            auto lef_library = std::make_unique<LefLibrary>(lef_parser.parse());
            if (lef_library && !lef_library->macros.empty()) {
                // Use first macro's height as row height
                const LefMacro& macro = lef_library->macros.begin()->second;
                row_height = macro.height;
                if (config.verbose) {
                    std::cout << "  Auto-detected row height: " << row_height 
                              << " um from LEF macro: " << macro.name << std::endl;
                }
            }
        } catch (const std::exception& e) {
            if (config.verbose) {
                std::cout << "  Warning: Failed to parse LEF for row height: " 
                          << e.what() << std::endl;
                std::cout << "  Using configured row height: " << row_height << " um" << std::endl;
            }
        }
    }
    
    return row_height;
}

/**
 * @brief Back-annotate placement coordinates to NetlistDB
 * @param placer_db Placement database with coordinates
 * @param netlist_db Netlist database to update
 */


void backannotateCoordinates(PlacerDB& placer_db, NetlistDB& /* netlist_db */) {
    std::cout << "\n=== Back-annotating placement coordinates ===" << std::endl;
    
    size_t synced_cells = 0;
    auto all_cells = placer_db.getAllCells();
    
    for (Cell* cell : all_cells) {
        // Get cell position from PlacerDB
        Point center = placer_db.getCellCenter(cell);
        
        // Update NetlistDB cell coordinates
        cell->setX(center.x);
        cell->setY(center.y);
        cell->setPlaced(true);
        synced_cells++;
    }
    std::cout << "  Synced coordinates for " << synced_cells << " cells" << std::endl;
}

/**
 * @brief Run placement using unified configuration
 */
std::unique_ptr<PlacerDB> runPlacement(const AppConfig& config, std::shared_ptr<NetlistDB> netlist_db) {
    std::cout << "\n=== Running Placement ===" << std::endl;
    
    // Detect actual row height from LEF (override config if available)
    double actual_row_height = detectRowHeight(config);
    
    // Determine placement algorithm from environment variable
    std::string placement_algo = "hybrid";  // default to hybrid (best practice)
    const char* env_algo = std::getenv("MINIEDA_PLACEMENT_ALGO");
    if (env_algo) {
        placement_algo = std::string(env_algo);
    }
    
    std::cout << "  Using Placement Algorithm: " << placement_algo << std::endl;
    
    // Convert to placement configuration
    PlacementConfig placement_config = toPlacementConfig(config);
    placement_config.row_height = actual_row_height;  // Use detected height
    placement_config.placement_algo = placement_algo;  // Set algorithm
    
    // Extract base run_id without _routing suffix for placement
    std::string base_run_id = config.run_id;
    size_t routing_pos = base_run_id.find("_routing");
    if (routing_pos != std::string::npos) {
        base_run_id = base_run_id.substr(0, routing_pos);
    }
    placement_config.run_id = base_run_id;
    
    // Run placement using unified interface
    auto placer_db = PlacementInterface::runPlacementWithVisualization(
        placement_config, netlist_db, nullptr);
    
    if (placer_db && config.verbose) {
        std::cout << "  Placement completed successfully" << std::endl;
        std::cout << "  Final utilization: " << placement_config.utilization << std::endl;
        std::cout << "  Row height used: " << actual_row_height << " um" << std::endl;
        if (placement_config.placement_algo == "hybrid") {
            std::cout << "  Placement mode: Hybrid (Warm-up + Electrostatic)" << std::endl;
        } else if (placement_config.placement_algo == "nesterov") {
            std::cout << "  Placement mode: Electrostatic Field (Nesterov)" << std::endl;
        } else {
            std::cout << "  Placement mode: Basic Force-Directed" << std::endl;
        }
    }
    
    return placer_db;
}

/**
 * @brief Run routing using unified configuration
 */
bool runRouting(const AppConfig& config, std::shared_ptr<NetlistDB> netlist_db, 
                std::unique_ptr<PlacerDB>& placer_db) {
    if (config.skip_routing) {
        std::cout << "\n=== Skipping Routing (disabled) ===" << std::endl;
        return true;
    }
    
    std::cout << "\n=== Running Routing ===" << std::endl;
    
    // Check if LEF file is available
    if (config.lef_file.empty()) {
        std::cerr << "Error: LEF file is required for routing" << std::endl;
        return false;
    }
    
    // Convert to routing configuration
    RoutingConfig routing_config = toRoutingConfig(config);
    
    // Run routing
    auto routing_results = RoutingInterface::runRouting(
        routing_config, netlist_db, placer_db);
    
    // Get and report routing statistics
    double total_wirelength;
    int total_vias, routed_count, failed_count;
    RoutingInterface::getRoutingStatistics(routing_results, total_wirelength, 
                                          total_vias, routed_count, failed_count);
    
    std::cout << "\n  Routing completed:" << std::endl;
    std::cout << "    Successfully routed: " << routed_count << " nets" << std::endl;
    std::cout << "    Failed to route: " << failed_count << " nets" << std::endl;
    std::cout << "    Total wirelength: " << total_wirelength << std::endl;
    std::cout << "    Total vias: " << total_vias << std::endl;
    
    // Success if less than half failed (same as original logic)
    return (failed_count < static_cast<int>(netlist_db->getNumNets() / 2));
}

/**
 * @brief Run post-routing STA using unified configuration
 */
void runPostRoutingSTA(const AppConfig& config, std::shared_ptr<NetlistDB> netlist_db) {
    std::cout << "\n=== Running Post-Routing STA ===" << std::endl;
    
    // Parse Liberty library
    LibertyParser lib_parser;
    auto library = lib_parser.parseFile(config.liberty_file);
    if (!library) {
        std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
        return;
    }
    
    if (config.verbose) {
        std::cout << "  Loaded library: " << library->getName() 
                  << " with " << library->getCellCount() << " cells" << std::endl;
    }
    
    // Build timing graph with Liberty library for accurate arc extraction
    auto timing_graph = std::make_unique<TimingGraph>(netlist_db.get(), library.get());
    timing_graph->buildFromNetlist();
    
    // Initialize delay model
    auto delay_model = std::make_shared<TableDelayModel>(library.get());
    
    // Initialize STA engine
    STAEngine sta_engine(timing_graph.get(), delay_model);
    
    // Set physical parameters from config
    sta_engine.setPhysicalParameters(config.wire_resistance_per_unit, config.wire_cap_per_unit,
                                     config.default_input_slew_max * 1e-9, config.default_input_slew_min * 1e-9);
    
    // Setup timing constraints
    TimingConstraints constraints;
    constraints.createClock("main_clk", "CK", config.clock_period);
    
    // Set timing constraints from config
    constraints.setClockUncertainty(config.clock_uncertainty);
    constraints.setMaxTransition(config.max_transition);
    constraints.setDefaultInputDelay(config.default_input_delay);
    constraints.setDefaultOutputDelay(config.default_output_delay);
    
    // Set port-specific delays if configured
    for (size_t i = 0; i < config.input_delay_ports.size(); ++i) {
        constraints.setInputDelay(config.input_delay_ports[i], config.input_delay_values[i]);
    }
    for (size_t i = 0; i < config.output_delay_ports.size(); ++i) {
        constraints.setOutputDelay(config.output_delay_ports[i], config.output_delay_values[i]);
    }
    
    sta_engine.setConstraints(&constraints);
    
    // Run STA analysis
    sta_engine.run();
    
    // Report results
    sta_engine.reportSummary();
}

int main(int argc, char* argv[]) {
    std::cout << "MiniEDA Integrated Flow - Unified Configuration" << std::endl;
    std::cout << "================================================" << std::endl;
    
    // ========================================================================
    // 1. Parse unified configuration
    // ========================================================================
    AppConfig config;
    if (!parseAndValidate(argc, argv, config)) {
        return 1;
    }
    
    // Ensure we're in flow mode
    if (!config.flow_mode) {
        std::cout << "Note: Auto-switching to integrated flow mode" << std::endl;
        config.flow_mode = true;
        config.sta_mode = false;
        config.placement_mode = false;
        config.routing_mode = false;
    }
    
    // Auto-generate run_id from circuit name
    config.generateRunId();
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // ========================================================================
        // 2. Parse Verilog netlist
        // ========================================================================
        std::cout << "\n=== Parsing Verilog Netlist ===" << std::endl;
        std::cout << "Reading: " << config.verilog_file << std::endl;
        
        VerilogParser v_parser;
        auto netlist_db = std::make_shared<NetlistDB>();
        if (!v_parser.parseFile(config.verilog_file, *netlist_db)) {
            std::cerr << "Error: Failed to parse Verilog file" << std::endl;
            return 1;
        }
        
        if (config.verbose) {
            std::cout << "  Parsed " << netlist_db->getNumCells() << " cells, " 
                      << netlist_db->getNumNets() << " nets" << std::endl;
        }
        
        // ========================================================================
        // 3. Run Placement
        // ========================================================================
        auto placer_db = runPlacement(config, netlist_db);
        if (!placer_db) {
            std::cerr << "Error: Placement failed" << std::endl;
            return 1;
        }
        
        // ========================================================================
        // 4. Run Routing
        // ========================================================================
        bool routing_success = runRouting(config, netlist_db, placer_db);
        if (!routing_success) {
            std::cout << "Warning: Routing had issues, but continuing with STA..." << std::endl;
        }
        
        // ========================================================================
        // 5. Back-annotate coordinates
        // ========================================================================
        backannotateCoordinates(*placer_db, *netlist_db);
        
        // ========================================================================
        // 6. Run Post-Routing STA
        // ========================================================================
        runPostRoutingSTA(config, netlist_db);
        
        // ========================================================================
        // 7. Report completion
        // ========================================================================
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n=========================================\n" << std::endl;
        std::cout << "Integrated Flow Completed!" << std::endl;
        std::cout << "Total runtime: " << duration.count() << " ms" << std::endl;
        std::cout << "Output directory: " << config.output_dir << std::endl;
        std::cout << "Run ID: " << config.run_id << std::endl;
        std::cout << "=========================================\n" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}