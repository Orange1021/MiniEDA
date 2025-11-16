/**
 * @file test_netlist_db.cpp
 * @brief Simple test program for NetlistDB functionality
 */

#include "netlist_db.h"
#include <iostream>

using namespace mini;

int main() {
    std::cout << "=== NetlistDB Test Program ===" << std::endl << std::endl;

    // Create a database
    NetlistDB db("test_design");
    std::cout << "Created database: " << db.getDesignName() << std::endl << std::endl;

    // Test 1: Create cells
    std::cout << "Test 1: Creating cells..." << std::endl;
    Cell* in1 = db.createCell("IN1", CellType::INPUT);
    Cell* in2 = db.createCell("IN2", CellType::INPUT);
    Cell* and1 = db.createCell("AND1", CellType::AND);
    Cell* or1 = db.createCell("OR1", CellType::OR);
    Cell* out1 = db.createCell("OUT1", CellType::OUTPUT);

    std::cout << "Created " << db.getNumCells() << " cells" << std::endl << std::endl;

    // Test 2: Create pins
    std::cout << "Test 2: Creating pins..." << std::endl;
    in1->addPin("Y", PinDirection::OUTPUT);
    in2->addPin("Y", PinDirection::OUTPUT);
    and1->addPin("A", PinDirection::INPUT);
    and1->addPin("B", PinDirection::INPUT);
    and1->addPin("Y", PinDirection::OUTPUT);
    or1->addPin("A", PinDirection::INPUT);
    or1->addPin("B", PinDirection::INPUT);
    or1->addPin("Y", PinDirection::OUTPUT);
    out1->addPin("A", PinDirection::INPUT);
    std::cout << "Pins created successfully" << std::endl << std::endl;

    // Test 3: Create nets
    std::cout << "Test 3: Creating nets..." << std::endl;
    Net* n1 = db.createNet("n1");
    Net* n2 = db.createNet("n2");
    Net* n3 = db.createNet("n3");
    Net* n4 = db.createNet("n4");

    std::cout << "Created " << db.getNumNets() << " nets" << std::endl << std::endl;

    // Test 4: Connect pins to nets
    std::cout << "Test 4: Connecting pins to nets..." << std::endl;
    // IN1.Y -> n1 -> AND1.A
    db.connect("IN1", "Y", "n1");
    db.connect("AND1", "A", "n1");

    // IN2.Y -> n2 -> AND1.B, OR1.A
    db.connect("IN2", "Y", "n2");
    db.connect("AND1", "B", "n2");
    db.connect("OR1", "A", "n2");

    // AND1.Y -> n3 -> OR1.B
    db.connect("AND1", "Y", "n3");
    db.connect("OR1", "B", "n3");

    // OR1.Y -> n4 -> OUT1.A
    db.connect("OR1", "Y", "n4");
    db.connect("OUT1", "A", "n4");

    std::cout << "Connections established" << std::endl << std::endl;

    // Test 5: Query topology
    std::cout << "Test 5: Topology queries..." << std::endl;

    // Fanout of IN2
    auto fanout_in2 = db.getFanoutCells(in2);
    std::cout << "Fanout of IN2: ";
    for (Cell* c : fanout_in2) {
        std::cout << c->getName() << " ";
    }
    std::cout << "(" << fanout_in2.size() << " cells)" << std::endl;

    // Fanin of OR1
    auto fanin_or1 = db.getFaninCells(or1);
    std::cout << "Fanin of OR1: ";
    for (Cell* c : fanin_or1) {
        std::cout << c->getName() << " ";
    }
    std::cout << "(" << fanin_or1.size() << " cells)" << std::endl;

    // Check direct connection
    bool connected = db.isDirectlyConnected(and1, or1);
    std::cout << "AND1 -> OR1: " << (connected ? "Connected" : "Not connected") << std::endl;
    std::cout << std::endl;

    // Test 6: Get input/output ports
    std::cout << "Test 6: Port queries..." << std::endl;
    auto inputs = db.getInputPorts();
    auto outputs = db.getOutputPorts();
    std::cout << "Input ports: " << inputs.size() << std::endl;
    std::cout << "Output ports: " << outputs.size() << std::endl << std::endl;

    // Test 7: Statistics
    std::cout << "Test 7: Database statistics" << std::endl;
    db.printStatistics();
    std::cout << std::endl;

    // Test 8: Validation
    std::cout << "Test 8: Netlist validation" << std::endl;
    bool valid = db.validate();
    std::cout << "Validation result: " << (valid ? "PASS" : "FAIL") << std::endl << std::endl;

    // Test 9: Print detailed netlist
    std::cout << "Test 9: Detailed netlist dump" << std::endl;
    db.printNetlist();

    // Test 10: Clear database
    std::cout << "\nTest 10: Clear database" << std::endl;
    db.clear();
    std::cout << "Database cleared. Is empty? " << (db.isEmpty() ? "Yes" : "No") << std::endl;
    std::cout << "Cells: " << db.getNumCells() << ", Nets: " << db.getNumNets() << std::endl;

    std::cout << "\n=== All tests completed ===" << std::endl;

    return 0;
}
