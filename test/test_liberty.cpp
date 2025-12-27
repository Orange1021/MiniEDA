/**
 * @file test_liberty.cpp
 * @brief Liberty Library Test Program
 * @details Tests NLDM lookup table interpolation and data structures
 */

#include <iostream>
#include <cassert>
#include "../lib/include/liberty.h"

using namespace mini;

void testLookupTable() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Testing LookupTable Bilinear Interpolation" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Create a simple 3x3 lookup table
    // Example: Delay as function of (slew, cap)
    //          cap=0.1  cap=0.2  cap=0.3
    // slew=1   0.05     0.07     0.09
    // slew=2   0.08     0.10     0.12
    // slew=3   0.12     0.15     0.18

    LookupTable table;
    table.name = "cell_delay";

    // Setup index_1 (slew values)
    table.index_1 = {1.0, 2.0, 3.0};

    // Setup index_2 (cap values)
    table.index_2 = {0.1, 0.2, 0.3};

    // Setup values matrix
    table.values = {
        {0.05, 0.07, 0.09},  // slew=1
        {0.08, 0.10, 0.12},  // slew=2
        {0.12, 0.15, 0.18}   // slew=3
    };

    // Test 1: Exact lookup at grid points
    std::cout << "\nTest 1: Exact grid lookup" << std::endl;
    double val1 = table.lookup(1.0, 0.1);  // Should be 0.05
    std::cout << "  lookup(1.0, 0.1) = " << val1 << " (expected 0.05)" << std::endl;
    assert(std::abs(val1 - 0.05) < 0.001);

    double val2 = table.lookup(2.0, 0.2);  // Should be 0.10
    std::cout << "  lookup(2.0, 0.2) = " << val2 << " (expected 0.10)" << std::endl;
    assert(std::abs(val2 - 0.10) < 0.001);

    // Test 2: Bilinear interpolation (midpoint)
    std::cout << "\nTest 2: Bilinear interpolation" << std::endl;
    double val3 = table.lookup(1.5, 0.15);  // Midpoint between (1,0.1) and (2,0.2)
    // Expected: (0.05 + 0.08 + 0.07 + 0.10) / 4 = 0.075
    std::cout << "  lookup(1.5, 0.15) = " << val3 << " (expected ~0.075)" << std::endl;
    assert(std::abs(val3 - 0.075) < 0.01);

    double val4 = table.lookup(2.5, 0.25);  // Midpoint between (2,0.2) and (3,0.3)
    // Expected: (0.10 + 0.12 + 0.15 + 0.18) / 4 = 0.1375
    std::cout << "  lookup(2.5, 0.25) = " << val4 << " (expected ~0.1375)" << std::endl;
    assert(std::abs(val4 - 0.1375) < 0.01);

    // Test 3: Out of bounds (should clamp to nearest value)
    std::cout << "\nTest 3: Out of bounds clamping" << std::endl;
    double val5 = table.lookup(0.5, 0.05);  // Below minimum, should clamp to (1.0, 0.1)
    std::cout << "  lookup(0.5, 0.05) = " << val5 << " (expected 0.05)" << std::endl;
    assert(std::abs(val5 - 0.05) < 0.001);

    double val6 = table.lookup(5.0, 0.5);  // Above maximum, should clamp to (3.0, 0.3)
    std::cout << "  lookup(5.0, 0.5) = " << val6 << " (expected 0.18)" << std::endl;
    assert(std::abs(val6 - 0.18) < 0.001);

    std::cout << "\n[PASS] All LookupTable tests passed!" << std::endl;
}

void testLibrary() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Testing Library Data Structures" << std::endl;
    std::cout << "==========================================" << std::endl;

    // Create a library
    Library lib("TestLib");
    std::cout << "\nCreated library: " << lib.getName() << std::endl;

    // Create a cell (AND2)
    LibCell and2;
    and2.name = "AND2X1";
    and2.area = 12.0;

    // Create input pin A
    LibPin pinA;
    pinA.name = "A";
    pinA.direction = "input";
    pinA.capacitance = 0.01;
    and2.addPin("A", pinA);

    // Create input pin B
    LibPin pinB;
    pinB.name = "B";
    pinB.direction = "input";
    pinB.capacitance = 0.01;
    and2.addPin("B", pinB);

    // Create output pin Y
    LibPin pinY;
    pinY.name = "Y";
    pinY.direction = "output";

    // Add timing arc from A to Y
    LibTiming timingAY;
    timingAY.related_pin = "A";
    timingAY.timing_sense = "positive_unate";
    timingAY.cell_delay = {
        "cell_delay",
        {0.01, 0.02, 0.04},    // slew index
        {0.001, 0.01, 0.1},    // cap index
        {
            {0.05, 0.07, 0.09},
            {0.06, 0.08, 0.10},
            {0.08, 0.10, 0.12}
        }
    };
    pinY.timing_arcs.push_back(timingAY);

    and2.addPin("Y", pinY);

    // Add cell to library
    lib.addCell("AND2X1", and2);

    std::cout << "  Added cell: " << and2.name << " (area=" << and2.area << ")" << std::endl;
    std::cout << "  Total cells in library: " << lib.getCellCount() << std::endl;

    // Test cell retrieval
    std::cout << "\nTest: Retrieving cell AND2X1" << std::endl;
    LibCell* retrieved = lib.getCell("AND2X1");
    assert(retrieved != nullptr);
    std::cout << "  Cell name: " << retrieved->name << std::endl;
    std::cout << "  Cell area: " << retrieved->area << std::endl;
    std::cout << "  Pin count: " << retrieved->pins.size() << std::endl;

    // Test pin retrieval
    std::cout << "\nTest: Retrieving pin Y" << std::endl;
    LibPin* pinYret = retrieved->getPin("Y");
    assert(pinYret != nullptr);
    std::cout << "  Pin name: " << pinYret->name << std::endl;
    std::cout << "  Pin direction: " << pinYret->direction << std::endl;
    std::cout << "  Timing arc count: " << pinYret->timing_arcs.size() << std::endl;
    std::cout << "  Related pin: " << pinYret->timing_arcs[0].related_pin << std::endl;

    // Test timing lookup
    std::cout << "\nTest: Looking up cell delay" << std::endl;
    double delay = pinYret->timing_arcs[0].cell_delay.lookup(0.02, 0.01);
    std::cout << "  delay(0.02ns slew, 0.01pF cap) = " << delay << " ns" << std::endl;

    std::cout << "\n[PASS] All Library structure tests passed!" << std::endl;
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════╗" << std::endl;
    std::cout << "║  Liberty Library Test Suite        ║" << std::endl;
    std::cout << "╚════════════════════════════════════╝" << std::endl;

    try {
        testLookupTable();
        testLibrary();

        std::cout << "\n";
        std::cout << "╔════════════════════════════════════╗" << std::endl;
        std::cout << "║  [PASS] All Tests Passed!              ║" << std::endl;
        std::cout << "╚════════════════════════════════════╝" << std::endl;
        std::cout << "\n";

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test Failed: " << e.what() << std::endl;
        return 1;
    }
}
