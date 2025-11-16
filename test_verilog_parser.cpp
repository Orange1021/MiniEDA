/**
 * @file test_verilog_parser.cpp
 * @brief Test program for VerilogParser
 */

#include "netlist_db.h"
#include "verilog_parser.h"
#include <iostream>

using namespace mini;

int main() {
    std::cout << "=== Verilog Parser Test ===" << std::endl << std::endl;

    // Create database
    NetlistDB db("s27");

    // Create parser
    VerilogParser parser;
    parser.setVerbose(true);

    // Parse s27.v
    std::string verilog_file = "benchmarks/ISCAS/Verilog/s27.v";
    std::cout << "Parsing file: " << verilog_file << std::endl << std::endl;

    bool success = parser.parseFile(verilog_file, db);

    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Parsing result: " << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    if (success) {
        // Print statistics
        db.printStatistics();
        std::cout << std::endl;

        // Validate netlist
        std::cout << "Validating netlist..." << std::endl;
        bool valid = db.validate();
        std::cout << "Validation: " << (valid ? "PASS" : "FAIL") << std::endl << std::endl;

        // Print detailed netlist
        std::cout << "Detailed netlist:" << std::endl;
        db.printNetlist();
    } else {
        std::cout << "Parsing failed!" << std::endl;
        parser.printErrors();
    }

    return success ? 0 : 1;
}
