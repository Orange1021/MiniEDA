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
    std::string lef_file = "benchmarks/NangateOpenCellLibrary.macro.lef";  ///< LEF physical library
    std::string output_dir = "visualizations";                ///< Output directory for results
    std::string run_id = "default_run";                       ///< Run identifier for output files

    // ========================================================================
    // Physical Design Constraints
    // ========================================================================
    double utilization = 0.7;                                ///< Target core utilization (0.1-1.0)
    double row_height = 1.4;                                 ///< Standard cell row height (micrometers)
    double routing_pitch = 0.19;                             ///< Routing grid pitch (micrometers)
    double routing_grid_fine_factor = 0.5;                  ///< Fine grid factor for routing (0.5 = half pitch)
    double site_width = 0.19;                               ///< Standard cell site width (micrometers)
    double default_cell_area = 10.0;                        ///< Default cell area when not found in library (square micrometers)
    
    // ========================================================================
    // Algorithm Parameters
    // ========================================================================
    double clock_period = 10.0;                              ///< Clock period in nanoseconds (STA)
    double via_cost = 5.0;                                   ///< Via penalty cost multiplier (Routing)
    double wire_cost = 1.0;                                  ///< Wire cost per unit length (Routing)
    double wire_cap_per_unit = 0.0002;                       ///< Wire capacitance per unit length (pF/μm, default: 0.2 fF/μm for 45nm)
    double wire_resistance_per_unit = 0.005;                 ///< Wire resistance per unit length (kΩ/μm, default: 5 Ω/μm for 45nm)
    
    // ========================================================================
    // [NEW] Placement Algorithm Parameters
    // ========================================================================
    std::string placement_algo = "hybrid";                  ///< Placement algorithm: "basic", "nesterov", or "hybrid"
    double placement_target_density = 0.8;                   ///< Target utilization for global placement (same as utilization)
    double placement_initial_lambda = 0.0001;                ///< Initial density penalty factor
    double placement_lambda_growth_rate = 1.05;              ///< Lambda growth rate per iteration
    double placement_learning_rate = 0.1;                    ///< Learning rate (step size)
    double placement_momentum = 0.9;                         ///< Momentum factor
    double placement_convergence_threshold = 0.001;          ///< Convergence threshold
    double placement_hpwl_convergence_ratio = 0.0001;       ///< HPWL convergence ratio (0.01% improvement threshold)
    double placement_density_margin = 0.1;                  ///< Density margin for target density calculation
    double placement_max_gradient_ratio = 0.01;            ///< Maximum gradient as ratio of core width
    double placement_max_displacement_ratio = 0.02;       ///< Maximum displacement as ratio of core width
    int placement_detailed_iterations = 3;                   ///< Number of iterations for detailed placement
    double placement_warmup_stop_ratio = 0.3;              ///< Stop warmup when HPWL drops to this ratio of initial value
    
    // ========================================================================
    // [NEW] Routing Algorithm Parameters
    // ========================================================================
    double initial_collision_penalty = 50.0;                 ///< Initial collision penalty for PathFinder
    double penalty_growth_rate = 1.5; ///< Penalty growth rate per iteration
    double max_penalty = 1000.0;                             ///< Maximum collision penalty cap
    double initial_history_increment = 1.0;                  ///< Initial history increment
    double max_history_increment = 50.0;                     ///< Maximum history increment cap
    double decay_factor = 0.9;                               ///< History decay factor for cooling hotspots
    double distance_weight = 2.0;                             ///< Distance weight for pin access scoring
    double history_increment_growth_rate = 0.05;             ///< History increment growth rate per iteration
    
    // ========================================================================
    // [NEW] Industrial-Grade Timing Constraints
    // ========================================================================
    double clock_uncertainty = 0.05;                         ///< Clock uncertainty/jitter in nanoseconds
    double default_input_delay = 0.0;                        ///< Default input delay for all PI ports (ns)
    double default_output_delay = 0.0;                       ///< Default output delay for all PO ports (ns)
    double max_transition = 0.5;                             ///< Maximum transition time constraint (ns)
    double default_input_slew_max = 0.05;                    ///< Default maximum input slew (ns)
    double default_input_slew_min = 0.01;                    ///< Default minimum input slew (ns)
    double default_pin_capacitance = 0.002;                  ///< Default pin capacitance (pF)
    double min_wire_length = 0.01;                           ///< Minimum wire length for estimation (μm)
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