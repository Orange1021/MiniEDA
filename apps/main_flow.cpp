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
#include "../lib/include/visualizer.h"

// NEW: Unified configuration management
#include "../lib/include/app_config.h"
#include "../lib/include/arg_parser.h"

// Placement modules
#include "../apps/mini_placement/placement_interface.h"
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
    routing_config.verbose = app_config.verbose;
    routing_config.run_id = app_config.run_id + "_routing";
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
    
    // Convert to placement configuration
    PlacementConfig placement_config = toPlacementConfig(config);
    placement_config.row_height = actual_row_height;  // Use detected height
    
    // Run placement
    auto placer_db = PlacementInterface::runPlacementWithVisualization(
        placement_config, netlist_db, nullptr);
    
    if (placer_db && config.verbose) {
        std::cout << "  Placement completed successfully" << std::endl;
        std::cout << "  Final utilization: " << placement_config.utilization << std::endl;
        std::cout << "  Row height used: " << actual_row_height << " um" << std::endl;
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
    
    // Run routing with visualization
    auto visualizer = config.enable_visualization ? 
        std::make_unique<Visualizer>(placer_db.get()) : nullptr;
    
    auto routing_results = RoutingInterface::runRoutingWithVisualization(
        routing_config, netlist_db, placer_db, visualizer.get());
    
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
    
    // Build timing graph
    auto timing_graph = std::make_unique<TimingGraph>(netlist_db.get());
    timing_graph->buildFromNetlist();
    
    // Initialize delay model
    auto delay_model = std::make_shared<TableDelayModel>(library.get());
    
    // Initialize STA engine
    STAEngine sta_engine(timing_graph.get(), delay_model);
    
    // Setup timing constraints
    TimingConstraints constraints;
    constraints.createClock("main_clk", "CK", config.clock_period);
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