/**
 * @file test_verilog_parser.cpp
 * @brief Comprehensive test for VerilogParser with ISCAS benchmark suite
 * @details Tests parsing of real ISCAS circuits (s27, s344, s349, s382)
 */

#include "netlist_db.h"
#include "verilog_parser.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

using namespace mini;

/**
 * @brief Test result structure
 */
struct TestResult {
    std::string filename;
    bool success;
    double elapsed_ms;
    size_t num_cells;
    size_t num_nets;
    size_t num_errors;
    size_t num_warnings;
};

/**
 * @brief Run a single test case
 */
TestResult runTest(const std::string& filename, bool verbose = false) {
    TestResult result;
    result.filename = filename;

    auto start = std::chrono::high_resolution_clock::now();

    // Create database and parser
    NetlistDB db;
    VerilogParser parser;
    parser.setVerbose(verbose);

    // Parse file
    result.success = parser.parseFile(filename, db);

    auto end = std::chrono::high_resolution_clock::now();
    result.elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();

    // Collect statistics
    result.num_cells = db.getNumCells();
    result.num_nets = db.getNumNets();
    result.num_errors = parser.hasErrors() ? parser.getErrors().size() : 0;
    result.num_warnings = 0;  // TODO: Track warnings separately

    return result;
}

/**
 * @brief Print test result
 */
void printResult(const TestResult& result) {
    std::cout << std::setw(25) << std::left << result.filename;
    std::cout << std::setw(10) << std::left << (result.success ? "✓ PASS" : "✗ FAIL");
    std::cout << std::setw(8) << std::right << result.num_cells << " cells";
    std::cout << std::setw(8) << std::right << result.num_nets << " nets";
    std::cout << std::setw(10) << std::right << std::fixed << std::setprecision(2)
              << result.elapsed_ms << " ms";

    if (!result.success) {
        std::cout << " (" << result.num_errors << " errors)";
    }

    std::cout << std::endl;
}

/**
 * @brief Run all tests
 */
int main(int argc, char* argv[]) {
    std::cout << "========================================" << std::endl;
    std::cout << "Verilog Parser Test Suite" << std::endl;
    std::cout << "ISCAS Benchmark Circuits" << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    // Test cases
    std::vector<std::string> test_files = {
        "benchmarks/ISCAS/Verilog/s27.v",
        "benchmarks/ISCAS/Verilog/s344.v",
        "benchmarks/ISCAS/Verilog/s349.v",
        "benchmarks/ISCAS/Verilog/s382.v"
    };

    std::vector<TestResult> results;
    size_t total_passed = 0;
    size_t total_failed = 0;
    double total_time = 0.0;

    // Run tests
    std::cout << "Running tests..." << std::endl << std::endl;

    std::cout << std::setw(25) << std::left << "Circuit";
    std::cout << std::setw(10) << std::left << "Status";
    std::cout << std::setw(16) << std::right << "Statistics";
    std::cout << std::setw(20) << std::right << "Time";
    std::cout << std::endl;

    std::cout << std::string(71, '-') << std::endl;

    for (const auto& filename : test_files) {
        TestResult result = runTest(filename, false);
        printResult(result);
        results.push_back(result);

        if (result.success) {
            total_passed++;
        } else {
            total_failed++;
        }
        total_time += result.elapsed_ms;
    }

    // Summary
    std::cout << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Summary" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Total: " << results.size() << " circuits" << std::endl;
    std::cout << "Passed: " << total_passed << std::endl;
    std::cout << "Failed: " << total_failed << std::endl;
    std::cout << "Total time: " << std::fixed << std::setprecision(2)
              << total_time << " ms" << std::endl;
    std::cout << "Average time: " << std::fixed << std::setprecision(2)
              << (results.empty() ? 0.0 : total_time / results.size()) << " ms"
              << std::endl;
    std::cout << "========================================" << std::endl << std::endl;

    // Print errors for failed tests
    if (total_failed > 0) {
        std::cout << "Failed tests:" << std::endl;
        std::cout << std::string(71, '-') << std::endl;

        for (const auto& result : results) {
            if (!result.success) {
                std::cout << "\nErrors for " << result.filename << ":\n";

                // Re-parse with error output
                NetlistDB db;
                VerilogParser parser;
                parser.parseFile(result.filename, db);
                parser.printErrors();
            }
        }
    }

    // Detailed analysis of first successful test
    if (total_passed > 0) {
        std::cout << "\n\nDetailed analysis of s27 (smallest circuit):\n";
        std::cout << std::string(71, '-') << std::endl;

        NetlistDB db("s27");
        VerilogParser parser;
        parser.setVerbose(true);

        if (parser.parseFile("benchmarks/ISCAS/Verilog/s27.v", db)) {
            std::cout << "\nNetlist validation: "
                      << (db.validate() ? "✓ PASS" : "✗ FAIL") << std::endl;

            std::cout << "\nCircuit statistics:\n";
            db.printStatistics();

            std::cout << "\nPress Enter to see detailed netlist...";
            std::cin.get();

            std::cout << "\nDetailed netlist:\n";
            db.printNetlist();
        }
    }

    return (total_failed == 0) ? 0 : 1;
}
