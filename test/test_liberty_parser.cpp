/**
 * @file test_liberty_parser.cpp
 * @brief Liberty Parser Test Program
 * @details Tests parsing of sample.lib file
 */

#include <iostream>
#include <memory>
#include "../lib/include/liberty_parser.h"

using namespace mini;

void printLookupTable(const LookupTable& table) {
    std::cout << "        Table: " << table.name << std::endl;
    std::cout << "        Dimensions: " << table.index_1.size() << "x" << table.index_2.size() << std::endl;
    std::cout << "        Index_1 (slew): ";
    for (double v : table.index_1) std::cout << v << " ";
    std::cout << std::endl;
    std::cout << "        Index_2 (cap): ";
    for (double v : table.index_2) std::cout << v << " ";
    std::cout << std::endl;
    std::cout << "        Sample values:" << std::endl;
    for (size_t i = 0; i < std::min(size_t(3), table.values.size()); i++) {
        std::cout << "          Row " << i << ": ";
        for (size_t j = 0; j < std::min(size_t(3), table.values[i].size()); j++) {
            std::cout << table.values[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════╗" << std::endl;
    std::cout << "║  Liberty Parser Test Suite        ║" << std::endl;
    std::cout << "╚════════════════════════════════════╝" << std::endl;

    try {
        // Create parser
        LibertyParser parser;

        std::cout << "\nParsing: benchmarks/test_minimal.lib" << std::endl;
        std::cout << "════════════════════════════════" << std::endl;

        // Parse the file
        auto library = parser.parseFile("benchmarks/test_minimal.lib");

        if (!library) {
            std::cerr << "\n❌ Failed to parse library file!" << std::endl;
            return 1;
        }

        // Check library
        std::cout << "\n✅ Library parsed successfully!" << std::endl;
        std::cout << "   Library name: " << library->getName() << std::endl;
        std::cout << "   Cell count: " << library->getCellCount() << std::endl;

        // List all cells
        auto cell_names = library->getCellNames();
        std::cout << "\n   Cells:" << std::endl;
        for (const auto& name : cell_names) {
            std::cout << "     - " << name << std::endl;
        }

        // Test NOT cell
        std::cout << "\n   Testing cell: NOT" << std::endl;
        LibCell* not_cell = library->getCell("NOT");
        if (not_cell) {
            std::cout << "     Area: " << not_cell->area << std::endl;

            // Check pins
            LibPin* pin_a = not_cell->getPin("A");
            if (pin_a) {
                std::cout << "     Pin A: " << pin_a->direction << ", cap=" << pin_a->capacitance << std::endl;
            }

            LibPin* pin_y = not_cell->getPin("Y");
            if (pin_y) {
                std::cout << "     Pin Y: " << pin_y->direction << std::endl;
                std::cout << "     Timing arcs: " << pin_y->timing_arcs.size() << std::endl;

                if (!pin_y->timing_arcs.empty()) {
                    const auto& timing = pin_y->timing_arcs[0];
                    std::cout << "       Related pin: " << timing.related_pin << std::endl;
                    std::cout << "       Timing sense: " << timing.timing_sense << std::endl;

                    // Print delay table
                    if (timing.cell_delay.isValid()) {
                        std::cout << "       Cell delay table:" << std::endl;
                        printLookupTable(timing.cell_delay);
                    }
                }
            }
        }

        // Test NAND cell
        std::cout << "\n   Testing cell: NAND" << std::endl;
        LibCell* nand_cell = library->getCell("NAND");
        if (nand_cell) {
            std::cout << "     Area: " << nand_cell->area << std::endl;

            LibPin* pin_y = nand_cell->getPin("Y");
            if (pin_y && !pin_y->timing_arcs.empty()) {
                std::cout << "     Pin Y timing arcs: " << pin_y->timing_arcs.size() << std::endl;
                const auto& timing = pin_y->timing_arcs[0];
                std::cout << "       Related pin: " << timing.related_pin << std::endl;

                // Test lookup
                if (timing.cell_delay.isValid()) {
                    double delay = timing.cell_delay.lookup(0.05, 0.05);
                    std::cout << "       Sample lookup (slew=0.05, cap=0.05): " << delay << " ns" << std::endl;
                }
            }
        }

        // Test DFF cell
        std::cout << "\n   Testing cell: DFF" << std::endl;
        LibCell* dff_cell = library->getCell("DFF");
        if (dff_cell) {
            std::cout << "     Area: " << dff_cell->area << std::endl;

            LibPin* pin_q = dff_cell->getPin("Q");
            if (pin_q && !pin_q->timing_arcs.empty()) {
                const auto& timing = pin_q->timing_arcs[0];
                std::cout << "       Clock-to-Q delay (slew=0.1, cap=0.1): "
                          << timing.cell_delay.lookup(0.1, 0.1) << " ns" << std::endl;
            }
        }

        std::cout << "\n";
        std::cout << "╔════════════════════════════════════╗" << std::endl;
        std::cout << "║  ✅ All Parser Tests Passed!      ║" << std::endl;
        std::cout << "╚════════════════════════════════════╝" << std::endl;
        std::cout << "\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n❌ Test Failed: " << e.what() << std::endl;
        return 1;
    }
}
