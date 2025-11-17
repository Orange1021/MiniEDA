/**
 * @file test_repaired_features.cpp
 * @brief Test repaired VerilogParser features
 */

#include "netlist_db.h"
#include "verilog_parser.h"
#include <iostream>

using namespace mini;

int main() {
    std::cout << "=== Testing VerilogParser Repaired Features ===" << std::endl << std::endl;

    // Test 1: Strict mode - undefined signal detection
    std::cout << "Test 1: Strict mode - undefined signal detection" << std::endl;
    {
        NetlistDB db("test_strict");
        VerilogParser parser;
        parser.setStrictMode(true);
        parser.setVerbose(true);

        std::string test_code = R"(
            module test_strict(input in1, output out1);
                wire w1;
                AND2 and1(out1, in1, undefined_signal);
            endmodule
        )";

        bool success = parser.parseString(test_code, db);
        std::cout << "Parsing result: " << (success ? "SUCCESS" : "FAILED (expected)") << std::endl;

        if (!success) {
            std::cout << "Errors detected (as expected):" << std::endl;
            parser.printErrors();
        }
        std::cout << std::endl;
    }

    // Test 2: Duplicate wire declaration
    std::cout << "Test 2: Duplicate wire declaration detection" << std::endl;
    {
        NetlistDB db("test_dup_wire");
        VerilogParser parser;
        parser.setVerbose(false);

        std::string test_code = R"(
            module test_dup(input in1, output out1);
                wire w1;
                wire w1;  // Duplicate
                AND2 and1(out1, in1, w1);
            endmodule
        )";

        bool success = parser.parseString(test_code, db);
        std::cout << "Parsing result: " << (success ? "SUCCESS (with warnings)" : "FAILED") << std::endl;

        if (parser.hasErrors()) {
            std::cout << "Errors detected:" << std::endl;
            parser.printErrors();
        }
        std::cout << std::endl;
    }

    // Test 3: Identifier starting with digit
    std::cout << "Test 3: Identifier starting with digit detection" << std::endl;
    {
        NetlistDB db("test_digit");
        VerilogParser parser;

        std::string test_code = R"(
            module test_digit(input in1, output out1);
                wire 123abc;  // Invalid identifier
            endmodule
        )";

        bool success = parser.parseString(test_code, db);
        std::cout << "Parsing result: " << (success ? "SUCCESS" : "FAILED (expected)") << std::endl;

        if (!success) {
            std::cout << "Errors detected (as expected):" << std::endl;
            parser.printErrors();
        }
        std::cout << std::endl;
    }

    // Test 4: Port declaration not in module header
    std::cout << "Test 4: Port not in module header detection" << std::endl;
    {
        NetlistDB db("test_port_mismatch");
        VerilogParser parser;

        std::string test_code = R"(
            module test_port(input in1, output out1);
                input in2;  // Not in header
                output out2;  // Not in header
            endmodule
        )";

        bool success = parser.parseString(test_code, db);
        std::cout << "Parsing result: " << (success ? "SUCCESS (with warnings)" : "FAILED") << std::endl;

        if (parser.hasErrors()) {
            std::cout << "Errors detected:" << std::endl;
            parser.printErrors();
        }
        std::cout << std::endl;
    }

    // Test 5: Valid parsing should still work
    std::cout << "Test 5: Valid code should parse successfully" << std::endl;
    {
        NetlistDB db("test_valid");
        VerilogParser parser;
        parser.setVerbose(true);

        std::string test_code = R"(
            module test_valid(input in1, input in2, output out1);
                wire n1;
                AN2 and1(n1, in1, in2);
                OR2 or1(out1, n1, in1);
            endmodule
        )";

        bool success = parser.parseString(test_code, db);
        std::cout << "Parsing result: " << (success ? "SUCCESS" : "FAILED") << std::endl;

        if (success) {
            std::cout << "Database stats:" << std::endl;
            std::cout << "  Cells: " << db.getNumCells() << std::endl;
            std::cout << "  Nets: " << db.getNumNets() << std::endl;
        }
        std::cout << std::endl;
    }

    std::cout << "=== All tests completed ===" << std::endl;

    return 0;
}
