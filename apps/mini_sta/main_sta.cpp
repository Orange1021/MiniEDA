/**
 * @file main_sta.cpp
 * @brief MiniSTA Main Entry Point
 * @details Command line interface for the Static Timing Analysis tool
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../../lib/include/liberty_parser.h"
#include "../../lib/include/app_config.h"
#include "../../lib/include/arg_parser.h"
#include "sta_engine.h"
#include "timing_graph.h"
#include "delay_model.h"
#include "timing_report.h"
#include "timing_constraints.h"

using namespace mini;

// No need for printUsage - using unified ArgParser

int main(int argc, char* argv[]) {
    std::cout << "MiniSTA - Static Timing Analysis Tool" << std::endl;
    std::cout << "=====================================" << std::endl;

    // ========================================================================
    // 1. Parse unified configuration
    // ========================================================================
    AppConfig config;
    if (!parseAndValidate(argc, argv, config)) {
        return 1;
    }
    
    // Force STA mode
    if (!config.sta_mode) {
        std::cout << "Note: Auto-switching to STA mode" << std::endl;
        config.sta_mode = true;
        config.flow_mode = false;
        config.placement_mode = false;
        config.routing_mode = false;
    }
    
    // Auto-generate run_id from circuit name
    config.generateRunId();

    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        // ========================================================================
    // 2. Parse Liberty Library
    // ========================================================================
    std::cout << "\n=== Parsing Liberty Library ===" << std::endl;
    std::cout << "Reading: " << config.liberty_file << std::endl;
    
    LibertyParser lib_parser;
    auto library = lib_parser.parseFile(config.liberty_file);
    if (!library) {
        std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
        return 1;
    }
    
    if (config.verbose) {
        std::cout << "  Loaded library: " << library->getName() << " with " 
                  << library->getCellCount() << " cells" << std::endl;
    }

    // ========================================================================
    // 3. Parse Verilog & Build Netlist
    // ========================================================================
    std::cout << "\n=== Parsing Verilog Netlist ===" << std::endl;
    std::cout << "Reading: " << config.verilog_file << std::endl;
    
    NetlistDB db("design");
    VerilogParser parser;
    if (!parser.parseFile(config.verilog_file, db)) {
        std::cerr << "Error: Failed to parse Verilog file" << std::endl;
        return 1;
    }

        // 4. Build Timing Graph
        std::cout << "Building Timing Graph..." << std::endl;
        TimingGraph timing_graph(&db);
        if (!timing_graph.buildFromNetlist()) {
            std::cerr << "Error: Failed to build timing graph" << std::endl;
            return 1;
        }

        // 5. Initialize NLDM-based Delay Model
        std::cout << "Initializing NLDM Delay Model..." << std::endl;
        auto delay_model = std::make_shared<TableDelayModel>(library.get());

        // ========================================================================
    // 4. Setup timing constraints
    // ========================================================================
    TimingConstraints constraints;
    constraints.createClock("main_clk", "CK", config.clock_period);

        // Optionally set I/O delays (these can be configured later)
        // For now, use default delays (0.0) for all ports
        // Example: constraints.setInputDelay("G0", 1.0);
        // Example: constraints.setOutputDelay("G17", 1.0);

        // 6. Run STA with NLDM
        std::cout << "\nInitializing STA Engine with NLDM..." << std::endl;
        STAEngine sta(&timing_graph, delay_model);
        sta.setConstraints(&constraints);

        sta.run();
        sta.reportSummary();

        // 7. Generate detailed timing report
        std::cout << "\nGenerating Timing Report..." << std::endl;
        TimingReporter reporter(&timing_graph);
        reporter.reportTiming(1);  // Report top 1 worst path

        // Debug dump (optional)
        // sta.dumpGraph();

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\nTotal runtime: " << duration.count() << " ms" << std::endl;
    return 0;
}
