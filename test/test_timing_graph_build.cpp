/**
 * @file test_timing_graph_build.cpp
 * @brief Test TimingGraph building from NetlistDB
 */

#include <iostream>
#include <memory>
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../apps/mini_sta/timing_graph.h"

using namespace mini;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <verilog_file>" << std::endl;
        return 1;
    }

    std::string verilog_file = argv[1];

    std::cout << "==========================================" << std::endl;
    std::cout << "TimingGraph Building Test" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Step 1: Parse Verilog netlist
    std::cout << "\nStep 1: Parsing Verilog file..." << std::endl;
    std::cout << "  File: " << verilog_file << std::endl;

    VerilogParser parser;
    std::unique_ptr<NetlistDB> db_ptr = std::make_unique<NetlistDB>();

    if (!parser.parseFile(verilog_file, *db_ptr) || db_ptr->isEmpty()) {
        std::cerr << "Error: Failed to parse netlist or netlist is empty!" << std::endl;
        parser.printErrors();
        return 1;
    }

    NetlistDB* db = db_ptr.get();

    std::cout << "  Success!" << std::endl;
    db->printStatistics();

    // Step 2: Build timing graph
    std::cout << "\nStep 2: Building timing graph..." << std::endl;

    TimingGraph timing_graph(db);

    if (!timing_graph.buildFromNetlist()) {
        std::cerr << "Error: Failed to build timing graph!" << std::endl;
        return 1;
    }

    std::cout << "  Success!" << std::endl;
    timing_graph.printStatistics();

    // Step 3: Topological sort
    std::cout << "\nStep 3: Performing topological sort..." << std::endl;

    try {
        std::vector<TimingNode*> topo_order = timing_graph.topologicalSort();
        std::cout << "  Success! Sorted nodes: " << topo_order.size() << std::endl;

        // Print first 5 nodes in topological order
        std::cout << "  First 5 nodes in order:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), topo_order.size()); ++i) {
            std::cout << "    " << (i+1) << ". " << topo_order[i]->getName() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "  Error during topological sort: " << e.what() << std::endl;
        return 1;
    }

    // Step 4: Get start and end points
    std::cout << "\nStep 4: Identifying timing path endpoints..." << std::endl;

    std::vector<TimingNode*> start_points = timing_graph.getStartPoints();
    std::vector<TimingNode*> end_points = timing_graph.getEndPoints();

    std::cout << "  Start points: " << start_points.size() << std::endl;
    for (size_t i = 0; i < std::min(size_t(3), start_points.size()); ++i) {
        std::cout << "    - " << start_points[i]->getName() << std::endl;
    }

    std::cout << "  End points: " << end_points.size() << std::endl;
    for (size_t i = 0; i < std::min(size_t(3), end_points.size()); ++i) {
        std::cout << "    - " << end_points[i]->getName() << std::endl;
    }

    // Step 5: Validate graph
    std::cout << "\nStep 5: Validating timing graph..." << std::endl;

    if (timing_graph.validate()) {
        std::cout << "  Validation PASSED!" << std::endl;
        std::cout << "  Graph is a valid DAG." << std::endl;
    } else {
        std::cerr << "  Validation FAILED!" << std::endl;
        return 1;
    }

    // Optional: Print full graph topology if verbose
    if (argc > 2 && std::string(argv[2]) == "-v") {
        std::cout << "\n==========================================" << std::endl;
        timing_graph.printGraph(true);
    }

    std::cout << "\n==========================================" << std::endl;
    std::cout << "All tests PASSED!" << std::endl;
    std::cout << "==========================================" << std::endl;

    return 0;
}
