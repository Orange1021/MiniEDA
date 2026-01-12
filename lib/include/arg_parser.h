/**
 * @file arg_parser.h
 * @brief MiniEDA Unified Command Line Argument Parser
 * @details Single parser for all MiniEDA applications
 * @note This replaces scattered parseArguments functions across different apps
 */

#ifndef MINI_ARG_PARSER_H
#define MINI_ARG_PARSER_H

#include "app_config.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

namespace mini {

/**
 * @class ArgParser
 * @brief Unified command line argument parser for all MiniEDA applications
 * @details Handles parsing, validation, and help generation
 */
class ArgParser {
public:
    /**
     * @brief Parse command line arguments into AppConfig
     * @param argc Argument count from main()
     * @param argv Argument values from main()
     * @param config Output configuration structure
     * @return true if parsing successful, false if help requested or error
     */
    static bool parse(int argc, char* argv[], AppConfig& config) {
        try {
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                
                // Help flags
                if (arg == "-h" || arg == "-help" || arg == "--help") {
                    printUsage();
                    return false;
                }
                
                // Required files
                else if (arg == "-v" && i + 1 < argc) {
                    config.verilog_file = argv[++i];
                }
                else if (arg == "-lib" && i + 1 < argc) {
                    config.liberty_file = argv[++i];
                }
                else if (arg == "-lef" && i + 1 < argc) {
                    config.lef_file = argv[++i];
                }
                
                // Output configuration
                else if (arg == "-o" && i + 1 < argc) {
                    config.output_dir = argv[++i];
                }
                else if (arg == "-run_id" && i + 1 < argc) {
                    config.run_id = argv[++i];
                }
                
                // Physical constraints
                else if (arg == "-util" && i + 1 < argc) {
                    config.utilization = std::stod(argv[++i]);
                }
                else if (arg == "-rowheight" && i + 1 < argc) {
                    config.row_height = std::stod(argv[++i]);
                }
                else if (arg == "-pitch" && i + 1 < argc) {
                    config.routing_pitch = std::stod(argv[++i]);
                }
                
                // Algorithm parameters
                else if (arg == "-clk" && i + 1 < argc) {
                    config.clock_period = std::stod(argv[++i]);
                }
                else if (arg == "-via_cost" && i + 1 < argc) {
                    config.via_cost = std::stod(argv[++i]);
                }
                else if (arg == "-wire_cost" && i + 1 < argc) {
                    config.wire_cost = std::stod(argv[++i]);
                }
                else if (arg == "-num_layers" && i + 1 < argc) {
                    config.num_routing_layers = std::stoi(argv[++i]);
                }
                
                // [NEW] Industrial-grade timing constraints
                else if (arg == "-uncertainty" && i + 1 < argc) {
                    config.clock_uncertainty = std::stod(argv[++i]);
                }
                else if (arg == "-input_delay" && i + 1 < argc) {
                    config.default_input_delay = std::stod(argv[++i]);
                }
                else if (arg == "-output_delay" && i + 1 < argc) {
                    config.default_output_delay = std::stod(argv[++i]);
                }
                else if (arg == "-max_transition" && i + 1 < argc) {
                    config.max_transition = std::stod(argv[++i]);
                }
                else if (arg == "-input_delay_port" && i + 2 < argc) {
                    config.input_delay_ports.push_back(argv[++i]);
                    config.input_delay_values.push_back(std::stod(argv[++i]));
                }
                else if (arg == "-output_delay_port" && i + 2 < argc) {
                    config.output_delay_ports.push_back(argv[++i]);
                    config.output_delay_values.push_back(std::stod(argv[++i]));
                }
                
                // Flow control
                else if (arg == "-verbose") {
                    config.verbose = true;
                }
                else if (arg == "-skip_routing") {
                    config.skip_routing = true;
                }
                else if (arg == "-skip_placement") {
                    config.skip_placement = true;
                }
                else if (arg == "-no_viz") {
                    config.enable_visualization = false;
                }
                
                // Advanced parameters
                else if (arg == "-threads" && i + 1 < argc) {
                    config.max_threads = std::stoi(argv[++i]);
                }
                else if (arg == "-conv_thresh" && i + 1 < argc) {
                    config.convergence_threshold = std::stod(argv[++i]);
                }
                else if (arg == "-max_iter" && i + 1 < argc) {
                    config.max_placement_iterations = std::stoi(argv[++i]);
                }
                else if (arg == "-grid_expand" && i + 1 < argc) {
                    config.routing_grid_expansion = std::stod(argv[++i]);
                }
                
                // Application mode selection
                else if (arg == "-sta") {
                    config.sta_mode = true;
                    config.flow_mode = false;
                }
                else if (arg == "-placement") {
                    config.placement_mode = true;
                    config.flow_mode = false;
                }
                else if (arg == "-routing") {
                    config.routing_mode = true;
                    config.flow_mode = false;
                }
                else if (arg == "-flow") {
                    config.flow_mode = true;
                }
                
                // Positional argument (verilog file without -v flag)
                else if (arg[0] != '-') {
                    if (config.verilog_file.empty()) {
                        config.verilog_file = arg;
                    } else {
                        std::cerr << "Error: Multiple verilog files specified" << std::endl;
                        return false;
                    }
                }
                
                // Unknown option
                else {
                    std::cerr << "Error: Unknown option " << arg << std::endl;
                    printUsage();
                    return false;
                }
            }
            
            // Auto-detect mode if not explicitly set
            if (!config.sta_mode && !config.placement_mode && 
                !config.routing_mode && !config.flow_mode) {
                config.flow_mode = true;  // Default to integrated flow
            }
            
            // Validate configuration
            if (!config.validate()) {
                std::cerr << "Error: Invalid configuration parameters" << std::endl;
                printValidationErrors(config);
                return false;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing arguments: " << e.what() << std::endl;
            return false;
        }
    }
    
    /**
     * @brief Print usage information
     */
    static void printUsage() {
        std::cout << R"(MiniEDA Unified Command Line Interface
========================================

Usage: MiniEDA [options] <verilog_file>

Required Files:
  -v <file>              Verilog netlist file
  -lib <file>            Liberty timing library (default: NangateOpenCellLibrary_typical.lib)
  -lef <file>            LEF physical library (default: NangateOpenCellLibrary.macro.lef)

Output Configuration:
  -o <dir>               Output directory (default: visualizations)
  -run_id <name>         Run identifier for output files (default: default_run)

Physical Design Constraints:
  -util <value>          Target core utilization (0.1-1.0, default: 0.7)
  -rowheight <value>     Standard cell row height in micrometers (default: 1.4)
  -pitch <value>         Routing grid pitch in micrometers (default: 0.19)

Algorithm Parameters:
  -clk <period>          Clock period in nanoseconds (default: 10.0)
  -via_cost <value>      Via penalty cost multiplier (default: 5.0)
  -wire_cost <value>     Wire cost per unit length (default: 1.0)
  -num_layers <count>     Number of routing layers (default: 3)

Industrial-Grade Timing Constraints:
  -uncertainty <value>   Clock uncertainty/jitter in nanoseconds (default: 0.05)
  -input_delay <value>   Default input delay for all PI ports in nanoseconds (default: 0.0)
  -output_delay <value>  Default output delay for all PO ports in nanoseconds (default: 0.0)
  -max_transition <val>  Maximum transition time constraint in nanoseconds (default: 0.5)
  -input_delay_port <p> <v>  Set input delay for specific port p to value v
  -output_delay_port <p> <v> Set output delay for specific port p to value v

Flow Control:
  -verbose               Enable verbose output
  -skip_routing          Skip routing stage (integrated flow only)
  -skip_placement        Skip placement stage (future extension)
  -no_viz                Disable visualization generation

Advanced Parameters:
  -threads <count>       Maximum parallel threads (default: 1)
  -conv_thresh <value>   Placement convergence threshold (default: 1e-6)
  -max_iter <count>      Maximum placement iterations (default: 100)
  -grid_expand <factor>  Routing grid expansion factor (default: 1.2)

Application Modes:
  -sta                   Run Static Timing Analysis only
  -placement             Run Placement only
  -routing               Run Routing only
  -flow                  Run Integrated Flow (default)

Help:
  -h, -help, --help      Show this help message

Examples:
  # Run integrated flow with default settings
  MiniEDA design.v -lib nangate.lib -lef nangate.lef

  # Run placement only with custom utilization
  MiniEDA -placement design.v -lib nangate.lib -lef nangate.lef -util 0.8

  # Run STA with custom clock period
  MiniEDA -sta design.v -lib nangate.lib -clk 8.0

  # Run integrated flow with custom parameters
  MiniEDA design.v -lib nangate.lib -lef nangate.lef -util 0.7 -clk 10.0 -via_cost 5.0 -verbose
)";
    }
    
private:
    /**
     * @brief Print specific validation errors
     */
    static void printValidationErrors(const AppConfig& config) {
        if (config.verilog_file.empty()) {
            std::cerr << "  - Verilog file is required" << std::endl;
        }
        if (config.utilization <= 0.0 || config.utilization > 1.0) {
            std::cerr << "  - Utilization must be between 0.0 and 1.0" << std::endl;
        }
        if (config.row_height <= 0.0) {
            std::cerr << "  - Row height must be positive" << std::endl;
        }
        if (config.routing_pitch <= 0.0) {
            std::cerr << "  - Routing pitch must be positive" << std::endl;
        }
        if (config.clock_period <= 0.0) {
            std::cerr << "  - Clock period must be positive" << std::endl;
        }
        if (config.via_cost < 0.0) {
            std::cerr << "  - Via cost must be non-negative" << std::endl;
        }
        if (config.wire_cost < 0.0) {
            std::cerr << "  - Wire cost must be non-negative" << std::endl;
        }
        if (config.max_threads <= 0) {
            std::cerr << "  - Thread count must be positive" << std::endl;
        }
    }
};

/**
 * @brief Convenience function to parse and validate in one call
 */
inline bool parseAndValidate(int argc, char* argv[], AppConfig& config) {
    if (!ArgParser::parse(argc, argv, config)) {
        return false;
    }
    
    if (config.verbose) {
        config.printSummary();
    }
    
    return true;
}

} // namespace mini

#endif // MINI_ARG_PARSER_H