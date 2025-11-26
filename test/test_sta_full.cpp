/**
 * @file test_sta_full.cpp
 * @brief Full STA Test - Verilog to Timing Report
 * @details Complete flow: Parse Verilog → Build TimingGraph → Run STA → Report
 */

#include <iostream>
#include <memory>
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../../apps/mini_sta/timing_graph.h"
#include "../../apps/mini_sta/delay_model.h"
#include "../../apps/mini_sta/sta_engine.h"

using namespace mini;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <verilog_file> [clock_period_ns]" << std::endl;
        return 1;
    }

    std::string verilog_file = argv[1];
    double clock_period = (argc >= 3) ? std::stod(argv[2]) : 10.0;

    // Step 1: Parse Verilog netlist
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Step 1: Parse Verilog Netlist" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "File: " << verilog_file << std::endl;

    VerilogParser parser;
    std::unique_ptr<NetlistDB> db_ptr = std::make_unique<NetlistDB>();

    if (!parser.parseFile(verilog_file, *db_ptr) || db_ptr->isEmpty()) {
        std::cerr << "Error: Failed to parse netlist!" << std::endl;
        parser.printErrors();
        return 1;
    }

    std::cout << "Success! Netlist loaded." << std::endl;
    db_ptr->printStatistics();

    // Step 2: Build timing graph
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Step 2: Build Timing Graph" << std::endl;
    std::cout << "==========================================" << std::endl;

    TimingGraph timing_graph(db_ptr.get());

    if (!timing_graph.buildFromNetlist()) {
        std::cerr << "Error: Failed to build timing graph!" << std::endl;
        return 1;
    }

    timing_graph.printStatistics();

    // Step 3: Create delay model
    std::cout << "\nStep 3: Create Delay Model (Linear)" << std::endl;
    std::shared_ptr<DelayModel> delay_model =
        std::make_shared<LinearDelayModel>(2.5e-12, 0.5e-12, 0.3e-12);
    std::cout << "Delay model created with clock period = " << clock_period << " ns" << std::endl;

    // Step 4: Create and configure STA engine
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Step 4: Configure STA Engine" << std::endl;
    std::cout << "==========================================" << std::endl;

    STAEngine sta_engine(&timing_graph, delay_model);
    sta_engine.setClockPeriod(clock_period);

    // Step 5: Run complete STA analysis
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Step 5: Run STA Analysis" << std::endl;
    std::cout << "==========================================" << std::endl;

    sta_engine.run();

    // Step 6: Generate timing report
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Step 6: Timing Report" << std::endl;
    std::cout << "==========================================" << std::endl;

    sta_engine.reportSummary();

    // Optional: Dump detailed timing data if requested
    if (argc > 3 && std::string(argv[3]) == "-v") {
        std::cout << "\n[Verbose] Dumping detailed graph data..." << std::endl;
        sta_engine.dumpGraph();
    }

    std::cout << "\n==========================================" << std::endl;
    std::cout << "STA Analysis Complete!" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Return number of timing violations as exit code (if negative slack exists)
    double wns = 0.0;
    for (const auto& node : timing_graph.getNodes()) {
        double slack = node->getSlack();
        if (slack < wns) {
            wns = slack;
        }
    }

    std::cout << "\nExit code: " << ((wns < 0) ? "FAIL (timing violations)" : "PASS") << std::endl;

    return (wns < 0) ? 1 : 0;
}
