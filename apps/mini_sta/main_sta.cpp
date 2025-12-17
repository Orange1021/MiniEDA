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

        // 4. Build Timing Graph with Liberty library for accurate arc extraction
        std::cout << "Building Timing Graph..." << std::endl;
        TimingGraph timing_graph(&db, library.get());
        if (!timing_graph.buildFromNetlist()) {
            std::cerr << "Error: Failed to build timing graph" << std::endl;
            return 1;
        }

        // 5. Initialize NLDM-based Delay Model
        std::cout << "Initializing NLDM Delay Model..." << std::endl;
        auto delay_model = std::make_shared<TableDelayModel>(library.get());

        // ========================================================================
    // 4. Setup timing constraints with industrial-grade features
    // ========================================================================
    TimingConstraints constraints;
    constraints.createClock("main_clk", "CK", config.clock_period);
    
    // [NEW] Set industrial-grade timing constraints
    constraints.setClockUncertainty(config.clock_uncertainty);
    constraints.setMaxTransition(config.max_transition);
    
    // Set default input/output delays for all ports
    constraints.setDefaultInputDelay(config.default_input_delay);
    constraints.setDefaultOutputDelay(config.default_output_delay);
    
    if (config.default_input_delay > 0.0) {
        std::cout << "  Setting default input delay: " << config.default_input_delay << " ns" << std::endl;
    }
    
    if (config.default_output_delay > 0.0) {
        std::cout << "  Setting default output delay: " << config.default_output_delay << " ns" << std::endl;
    }
    
    // Set port-specific input delays
    for (size_t i = 0; i < config.input_delay_ports.size(); ++i) {
        constraints.setInputDelay(config.input_delay_ports[i], config.input_delay_values[i]);
    }
    
    // Set port-specific output delays
    for (size_t i = 0; i < config.output_delay_ports.size(); ++i) {
        constraints.setOutputDelay(config.output_delay_ports[i], config.output_delay_values[i]);
    }

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
