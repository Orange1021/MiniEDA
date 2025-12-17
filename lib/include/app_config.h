/**
 * @file app_config.h
 * @brief MiniEDA Unified Application Configuration
 * @details Single Source of Truth for all application parameters
 * @note This replaces scattered config structures across different apps
 */

#ifndef MINI_APP_CONFIG_H
#define MINI_APP_CONFIG_H

#include <string>
#include <vector>
#include <iostream>

namespace mini {

/**
 * @struct AppConfig
 * @brief Unified configuration structure for all MiniEDA applications
 * @details This is the superset of all parameters used across:
 * - MiniSTA (Static Timing Analysis)
 * - MiniPlacement (Chip Placement)
 * - MiniRouter (A* Maze Routing)
 * - MiniFlow (Integrated Flow)
 */
struct AppConfig {
    // ========================================================================
    // Input/Output Files
    // ========================================================================
    std::string verilog_file;                                 ///< Verilog netlist file (required)
    std::string liberty_file = "benchmarks/NangateOpenCellLibrary_typical.lib";  ///< Liberty timing library
    std::string lef_file;                                     ///< LEF physical library (optional for routing)
    std::string output_dir = "visualizations";                ///< Output directory for results
    std::string run_id = "default_run";                       ///< Run identifier for output files

    // ========================================================================
    // Physical Design Constraints
    // ========================================================================
    double utilization = 0.7;                                ///< Target core utilization (0.1-1.0)
    double row_height = 1.4;                                 ///< Standard cell row height (micrometers)
    double routing_pitch = 0.19;                             ///< Routing grid pitch (micrometers)
    
    // ========================================================================
    // Algorithm Parameters
    // ========================================================================
    double clock_period = 10.0;                              ///< Clock period in nanoseconds (STA)
    double via_cost = 10.0;                                  ///< Via penalty cost multiplier (Routing)
    double wire_cost = 1.0;                                  ///< Wire cost per unit length (Routing)
    double wire_cap_per_unit = 0.0002;                       ///< Wire capacitance per unit length (pF/μm, default: 0.2 fF/μm for 45nm)
    double wire_resistance_per_unit = 0.005;                 ///< Wire resistance per unit length (kΩ/μm, default: 5 Ω/μm for 45nm)
    
    // ========================================================================
    // [NEW] Industrial-Grade Timing Constraints
    // ========================================================================
    double clock_uncertainty = 0.05;                         ///< Clock uncertainty/jitter in nanoseconds
    double default_input_delay = 0.0;                        ///< Default input delay for all PI ports (ns)
    double default_output_delay = 0.0;                       ///< Default output delay for all PO ports (ns)
    double max_transition = 0.5;                             ///< Maximum transition time constraint (ns)
    std::vector<std::string> input_delay_ports;              ///< Specific ports with custom input delays
    std::vector<double> input_delay_values;                  ///< Corresponding input delay values
    std::vector<std::string> output_delay_ports;             ///< Specific ports with custom output delays  
    std::vector<double> output_delay_values;                 ///< Corresponding output delay values
    
    // ========================================================================
    // Flow Control
    // ========================================================================
    bool verbose = false;                                     ///< Enable verbose output
    bool skip_routing = false;                                ///< Skip routing stage (MiniFlow only)
    bool skip_placement = false;                              ///< Skip placement stage (future extension)
    bool enable_visualization = true;                         ///< Generate visualizations
    
    // ========================================================================
    // Advanced Parameters
    // ========================================================================
    int max_threads = 1;                                      ///< Maximum parallel threads (future)
    double convergence_threshold = 1e-6;                     ///< Placement convergence threshold
    int max_placement_iterations = 100;                       ///< Maximum placement iterations
    double routing_grid_expansion = 1.2;                     ///< Routing grid expansion factor
    
    // ========================================================================
    // Process-Based Default Values (Nangate 45nm)
    // ========================================================================
    double default_input_slew = 0.01 * 1e-9;                  ///< 10ps typical input slew
    double default_cell_delay = 0.02 * 1e-9;                  ///< 20ps typical cell delay
    double max_transition_time = 0.198535 * 1e-9;            ///< 198.5ps (from library)
    double min_transition_time = 0.001 * 1e-9;               ///< 1ps minimum transition
    
    // ========================================================================
    // Application-Specific Flags
    // ========================================================================
    bool sta_mode = false;                                    ///< Run STA only
    bool placement_mode = false;                              ///< Run placement only
    bool routing_mode = false;                                ///< Run routing only
    bool flow_mode = false;                                   ///< Run integrated flow (default if no specific mode)
    
    // ========================================================================
    // Validation Methods
    // ========================================================================
    
    /**
     * @brief Validate configuration parameters
     * @return true if configuration is valid, false otherwise
     */
    bool validate() const {
        // Required files
        if (verilog_file.empty()) {
            return false;
        }
        
        // Parameter ranges
        if (utilization <= 0.0 || utilization > 1.0) {
            return false;
        }
        
        if (row_height <= 0.0) {
            return false;
        }
        
        if (routing_pitch <= 0.0) {
            return false;
        }
        
        if (clock_period <= 0.0) {
            return false;
        }
        
        if (via_cost < 0.0) {
            return false;
        }
        
        if (wire_cost < 0.0) {
            return false;
        }
        
        if (max_threads <= 0) {
            return false;
        }
        
        return true;
    }
    
    /**
     * @brief Auto-generate run_id from verilog file path
     */
    void generateRunId() {
        if (run_id == "default_run" && !verilog_file.empty()) {
            // Extract circuit name from verilog file path
            size_t last_slash = verilog_file.find_last_of('/');
            size_t last_dot = verilog_file.find_last_of('.');
            if (last_slash != std::string::npos && last_dot != std::string::npos && last_dot > last_slash) {
                run_id = verilog_file.substr(last_slash + 1, last_dot - last_slash - 1);
            } else if (last_dot != std::string::npos) {
                run_id = verilog_file.substr(0, last_dot);
            }
        }
    }
    
    /**
     * @brief Get application mode based on flags
     * @return String describing the active mode
     */
    std::string getMode() const {
        if (sta_mode) return "STA";
        if (placement_mode) return "Placement";
        if (routing_mode) return "Routing";
        if (flow_mode) return "Integrated Flow";
        return "Auto-detect";
    }
    
    /**
     * @brief Print configuration summary
     */
    void printSummary() const {
        std::cout << "=== Configuration Summary ===" << std::endl;
        std::cout << "Mode: " << getMode() << std::endl;
        std::cout << "Verilog: " << verilog_file << std::endl;
        std::cout << "Liberty: " << liberty_file << std::endl;
        if (!lef_file.empty()) {
            std::cout << "LEF: " << lef_file << std::endl;
        }
        std::cout << "Utilization: " << utilization << std::endl;
        std::cout << "Row Height: " << row_height << " um" << std::endl;
        std::cout << "Routing Pitch: " << routing_pitch << " um" << std::endl;
        std::cout << "Clock Period: " << clock_period << " ns" << std::endl;
        std::cout << "Via Cost: " << via_cost << std::endl;
        std::cout << "Wire Cost: " << wire_cost << std::endl;
        std::cout << "Verbose: " << (verbose ? "Yes" : "No") << std::endl;
        std::cout << "=============================" << std::endl;
    }
};

/**
 * @brief Create configuration for STA mode
 */
inline AppConfig createSTAConfig() {
    AppConfig config;
    config.sta_mode = true;
    config.flow_mode = false;
    return config;
}

/**
 * @brief Create configuration for Placement mode
 */
inline AppConfig createPlacementConfig() {
    AppConfig config;
    config.placement_mode = true;
    config.flow_mode = false;
    return config;
}

/**
 * @brief Create configuration for Routing mode
 */
inline AppConfig createRoutingConfig() {
    AppConfig config;
    config.routing_mode = true;
    config.flow_mode = false;
    return config;
}

/**
 * @brief Create configuration for Integrated Flow mode
 */
inline AppConfig createFlowConfig() {
    AppConfig config;
    config.flow_mode = true;
    return config;
}

} // namespace mini

#endif // MINI_APP_CONFIG_H