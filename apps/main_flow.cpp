/**
 * @file main_flow.cpp
 * @brief MiniEDA Integrated Flow - Placement + STA
 * @details Complete EDA flow from netlist to post-placement timing analysis
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

// Placement modules
#include "../apps/mini_placement/placement_interface.h"
#include "../apps/mini_placement/placer_db.h"

// STA modules
#include "../apps/mini_sta/sta_engine.h"
#include "../apps/mini_sta/timing_graph.h"
#include "../apps/mini_sta/delay_model.h"
#include "../apps/mini_sta/timing_constraints.h"
#include "../apps/mini_sta/cell_mapper.h"

using namespace mini;

void printUsage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <verilog_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v <file>        Verilog netlist file (required)" << std::endl;
    std::cout << "  -lib <file>      Liberty library file (required)" << std::endl;
    std::cout << "  -lef <file>      LEF physical library file (optional)" << std::endl;
    std::cout << "  -clk <period>    Clock period in ns (default: 10.0)" << std::endl;
    std::cout << "  -util <value>    Target utilization (default: 0.7)" << std::endl;
    std::cout << "  -rowheight <val> Row height in micrometers (default: 3.0)" << std::endl;
    std::cout << "  -help            Show this help message" << std::endl;
}

struct FlowConfig {
    std::string verilog_file;
    std::string liberty_file = "benchmarks/NangateOpenCellLibrary_typical.lib";
    std::string lef_file;
    double clock_period = 10.0;
    double utilization = 0.5;
    double row_height = 3.0;
};

bool parseArguments(int argc, char* argv[], FlowConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-v") {
            if (i + 1 < argc) {
                config.verilog_file = argv[++i];
            } else {
                std::cerr << "Error: -v requires a filename" << std::endl;
                return false;
            }
        } else if (arg == "-lib") {
            if (i + 1 < argc) {
                config.liberty_file = argv[++i];
            } else {
                std::cerr << "Error: -lib requires a filename" << std::endl;
                return false;
            }
        } else if (arg == "-lef") {
            if (i + 1 < argc) {
                config.lef_file = argv[++i];
            } else {
                std::cerr << "Error: -lef requires a filename" << std::endl;
                return false;
            }
        } else if (arg == "-clk") {
            if (i + 1 < argc) {
                config.clock_period = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: -clk requires a period value" << std::endl;
                return false;
            }
        } else if (arg == "-util") {
            if (i + 1 < argc) {
                config.utilization = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: -util requires a utilization value" << std::endl;
                return false;
            }
        } else if (arg == "-rowheight") {
            if (i + 1 < argc) {
                config.row_height = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: -rowheight requires a height value" << std::endl;
                return false;
            }
        } else if (arg == "-help") {
            return false;
        } else if (arg[0] != '-') {
            config.verilog_file = arg;
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return false;
        }
    }
    
    return !config.verilog_file.empty();
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
 * @brief Run complete placement flow using placement interface
 */
std::unique_ptr<PlacerDB> runPlacement(const FlowConfig& config, std::shared_ptr<NetlistDB> netlist_db) {
    // Extract circuit name from verilog file path for unique run_id
    size_t last_slash = config.verilog_file.find_last_of('/');
    size_t last_dot = config.verilog_file.find_last_of('.');
    std::string circuit_name = (last_slash != std::string::npos && last_dot != std::string::npos) 
                              ? config.verilog_file.substr(last_slash + 1, last_dot - last_slash - 1)
                              : "unknown";
    std::string run_id = "mini_flow_" + circuit_name;
    
    // Convert FlowConfig to PlacementConfig
    PlacementConfig placement_config;
    placement_config.lef_file = config.lef_file;
    placement_config.liberty_file = config.liberty_file;
    placement_config.utilization = config.utilization;
    placement_config.row_height = config.row_height;
    placement_config.verbose = true;
    placement_config.run_id = run_id;
    
    // Run placement using the interface (visualizer will be created internally)
    auto placer_db = PlacementInterface::runPlacementWithVisualization(
        placement_config, netlist_db, nullptr);
    
    return placer_db;
}

/**
 * @brief Run post-placement STA
 */
void runPostPlacementSTA(const FlowConfig& config, std::shared_ptr<NetlistDB> netlist_db) {
    std::cout << "\n=== Running Post-Placement STA ===" << std::endl;
    
    // Parse Liberty library
    LibertyParser lib_parser;
    auto library = lib_parser.parseFile(config.liberty_file);
    if (!library) {
        std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
        return;
    }
    
    std::cout << "  Loaded library: " << library->getName() 
              << " with " << library->getCellCount() << " cells" << std::endl;
    
    // Build timing graph
    auto timing_graph = std::make_unique<TimingGraph>(netlist_db.get());
    timing_graph->buildFromNetlist();
    
    // Initialize HPWL-based delay model
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
    std::cout << "MiniEDA Integrated Flow - Placement + Post-Placement STA" << std::endl;
    std::cout << "=========================================================" << std::endl;
    
    FlowConfig config;
    if (!parseArguments(argc, argv, config)) {
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Step 1: Parse Verilog netlist
        std::cout << "Reading Verilog netlist: " << config.verilog_file << std::endl;
        VerilogParser v_parser;
        auto netlist_db = std::make_shared<NetlistDB>();
        if (!v_parser.parseFile(config.verilog_file, *netlist_db)) {
            std::cerr << "Error: Failed to parse Verilog file" << std::endl;
            return 1;
        }
        std::cout << "  Parsed " << netlist_db->getNumCells() << " cells, " 
                  << netlist_db->getNumNets() << " nets" << std::endl;
        
        // Step 2: Run placement
        auto placer_db = runPlacement(config, netlist_db);
        if (!placer_db) {
            std::cerr << "Error: Placement failed" << std::endl;
            return 1;
        }
        
        // Step 3: Back-annotate coordinates
        backannotateCoordinates(*placer_db, *netlist_db);
        
        // Step 4: Run post-placement STA
        runPostPlacementSTA(config, netlist_db);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n=========================================" << std::endl;
        std::cout << "Integrated Flow Completed!" << std::endl;
        std::cout << "Total runtime: " << duration.count() << " ms" << std::endl;
        std::cout << "=========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
