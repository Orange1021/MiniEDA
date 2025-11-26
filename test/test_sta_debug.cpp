/**
 * @file test_sta_debug.cpp
 * @brief Debug version with detailed arc/node reporting
 */

#include <iostream>
#include <memory>
#include <algorithm>
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../../apps/mini_sta/timing_graph.h"
#include "../../apps/mini_sta/delay_model.h"
#include "../../apps/mini_sta/sta_engine.h"

using namespace mini;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <verilog_file>" << std::endl;
        return 1;
    }

    std::string verilog_file = argv[1];

    // Parse netlist
    VerilogParser parser;
    std::unique_ptr<NetlistDB> db_ptr = std::make_unique<NetlistDB>();

    if (!parser.parseFile(verilog_file, *db_ptr)) {
        std::cerr << "Parse failed!" << std::endl;
        return 1;
    }

    // Build timing graph
    TimingGraph graph(db_ptr.get());
    if (!graph.buildFromNetlist()) {
        std::cerr << "Build failed!" << std::endl;
        return 1;
    }

    // Print debug info
    std::cout << "\n========== Debug Information ==========" << std::endl;

    // Check start points
    std::cout << "\n--- Start Points ---" << std::endl;
    auto start_points = graph.getStartPoints();
    std::cout << "Count: " << start_points.size() << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), start_points.size()); ++i) {
        std::cout << "  " << start_points[i]->getName() << std::endl;
    }

    // Check end points
    std::cout << "\n--- End Points ---" << std::endl;
    auto end_points = graph.getEndPoints();
    std::cout << "Count: " << end_points.size() << std::endl;
    for (size_t i = 0; i < std::min(size_t(5), end_points.size()); ++i) {
        std::cout << "  " << end_points[i]->getName() << std::endl;
    }

    // Debug: Check arcs immediately after graph building
    std::cout << "\n--- Immediate Arc Verification ---" << std::endl;
    std::cout << "Total arcs created: " << graph.getArcs().size() << std::endl;
    if (!graph.getArcs().empty()) {
        TimingArc* first_arc = graph.getArcs()[0].get();
        if (first_arc) {
            TimingNode* from = first_arc->getFromNode();
            TimingNode* to = first_arc->getToNode();
            std::cout << "First arc: " << (from ? from->getName() : "NULL")
                      << " -> " << (to ? to->getName() : "NULL") << std::endl;
            if (from) {
                std::cout << "  From node outgoing arcs: " << from->getOutgoingArcs().size() << std::endl;
            }
            if (to) {
                std::cout << "  To node incoming arcs: " << to->getIncomingArcs().size() << std::endl;
            }
        }
    }

    // Get a sample node from arc building to compare addresses
    std::cout << "\n--- Sample Node Address Comparison ---" << std::endl;
    const auto& nodes = graph.getNodes();

    // Find NOT_0/A and NOT_0/Y (from the first arc that was created)
    TimingNode* not0_a = graph.getNode("NOT_0/A");
    TimingNode* not0_y = graph.getNode("NOT_0/Y");

    if (not0_a && not0_y) {
        std::cout << "NOT_0/A: addr=" << not0_a << std::endl;
        std::cout << "  Outgoing arcs: " << not0_a->getOutgoingArcs().size() << std::endl;
        std::cout << "  Incoming arcs: " << not0_a->getIncomingArcs().size() << std::endl;

        std::cout << "NOT_0/Y: addr=" << not0_y << std::endl;
        std::cout << "  Outgoing arcs: " << not0_y->getOutgoingArcs().size() << std::endl;
        std::cout << "  Incoming arcs: " << not0_y->getIncomingArcs().size() << std::endl;

        std::cout << "\n✓ ARCS ARE CORRECTLY CONNECTED!" << std::endl;
    } else {
        std::cout << "  NOT_0 nodes not found!" << std::endl;
    }

    // Also check that pin lookup works correctly
    std::cout << "\n--- Pin Lookup Verification ---" << std::endl;
    if (not0_a) {
        Pin* pin = not0_a->getPin();
        TimingNode* looked_up = graph.getNode(pin);
        if (looked_up == not0_a) {
            std::cout << "✓ Pin lookup returns same node address!" << std::endl;
        } else {
            std::cout << "✗ Pin lookup returns different address!" << std::endl;
            std::cout << "  Original: " << not0_a << std::endl;
            std::cout << "  Looked up: " << looked_up << std::endl;
        }
    }

    // Check sample nodes
    std::cout << "\n--- Sample Nodes (first 10) ---" << std::endl;
    size_t count = 0;
    for (const auto& node : nodes) {
        if (count++ >= 10) break;

        Pin* pin = node->getPin();
        if (!pin || !pin->getOwner()) continue;

        std::cout << "\n  Node: " << node->getName()
                  << " (Type: " << pin->getOwner()->getTypeString() << ")"
                  << " addr=" << node.get()
                  << std::endl;

        auto incoming = node->getIncomingArcs();
        auto outgoing = node->getOutgoingArcs();

        std::cout << "    Incoming arcs: " << incoming.size() << std::endl;
        if (!incoming.empty()) {
            for (size_t i = 0; i < std::min(size_t(2), incoming.size()); ++i) {
                auto* from = incoming[i]->getFromNode();
                if (from) {
                    std::cout << "      From: " << from->getName()
                              << " (delay=" << incoming[i]->getDelay() << ")" << std::endl;
                }
            }
        }

        std::cout << "    Outgoing arcs: " << outgoing.size() << std::endl;
        if (!outgoing.empty()) {
            for (size_t i = 0; i < std::min(size_t(2), outgoing.size()); ++i) {
                auto* to = outgoing[i]->getToNode();
                if (to) {
                    std::cout << "      To: " << to->getName()
                              << " (delay=" << outgoing[i]->getDelay() << ")" << std::endl;
                }
            }
        }
    }

    // Run STA with verbose
    std::cout << "\n========== Running STA ==========" << std::endl;
    auto delay_model = std::make_shared<LinearDelayModel>(2.5e-12, 0.5e-12, 0.3e-12);
    STAEngine engine(&graph, delay_model);
    engine.setClockPeriod(10.0);
    engine.run();

    return 0;
}
