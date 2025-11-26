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
#include "sta_engine.h"
#include "timing_graph.h"
#include "delay_model.h"
#include "timing_report.h"
#include "timing_constraints.h"

using namespace mini;

void printUsage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <verilog_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -clk <period>   Set target clock period in ns (default: 10.0)" << std::endl;
    std::cout << "  -lib <file>     Liberty library file (default: benchmarks/sample.lib)" << std::endl;
    std::cout << "  -help           Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << prog_name << " design.v -clk 10.0 -lib benchmarks/sample.lib" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "MiniSTA - Static Timing Analysis Tool" << std::endl;
    std::cout << "=====================================" << std::endl;

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    // 1. Parse Command Line Arguments
    std::string verilog_file;
    std::string liberty_file = "benchmarks/sample.lib";
    double clock_period = 10.0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-clk") {
            if (i + 1 < argc) {
                clock_period = std::stod(argv[++i]);
            } else {
                std::cerr << "Error: -clk requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-lib") {
            if (i + 1 < argc) {
                liberty_file = argv[++i];
            } else {
                std::cerr << "Error: -lib requires an argument" << std::endl;
                return 1;
            }
        } else if (arg == "-help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg[0] != '-') {
            verilog_file = arg;
        }
    }

    if (verilog_file.empty()) {
        std::cerr << "Error: No verilog file specified" << std::endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        // 2. Parse Liberty Library
        std::cout << "Reading Liberty library: " << liberty_file << std::endl;
        LibertyParser lib_parser;
        auto library = lib_parser.parseFile(liberty_file);
        if (!library) {
            std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
            return 1;
        }
        std::cout << "  Loaded library: " << library->getName() << " with " 
                  << library->getCellCount() << " cells" << std::endl;

        // 3. Parse Verilog & Build Netlist
        std::cout << "\nReading Verilog file: " << verilog_file << std::endl;
        NetlistDB db("design");
        VerilogParser parser;
        if (!parser.parseFile(verilog_file, db)) {
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

        // 5. Setup Timing Constraints
        std::cout << "\nSetting up Timing Constraints..." << std::endl;
        TimingConstraints constraints;

        // Create main clock (use the clock port name from the design)
        constraints.createClock("main_clk", "CK", clock_period);

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
