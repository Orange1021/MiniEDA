/**
 * @file timing_constraints.cpp
 * @brief Timing Constraints Implementation
 * @details Implementation of SDC-style timing constraints management
 */

#include "timing_constraints.h"
#include <iostream>
#include <algorithm>

namespace mini {

// ============================================================================
// Constructor
// ============================================================================

TimingConstraints::TimingConstraints() {
    // Initialize with industrial-grade defaults
    clock_uncertainty_ = 0.05;  // 50ps jitter/skew margin
    max_transition_ = 0.5;      // 500ps max slew rate
}

// ============================================================================
// Setters (SDC command interfaces)
// ============================================================================

void TimingConstraints::createClock(const std::string& name,
                                   const std::string& source_port,
                                   double period) {
    if (period <= 0.0) {
        std::cerr << "Warning: Attempt to create clock with non-positive period: "
                  << period << std::endl;
        return;
    }

    // Check if clock already exists
    for (const auto& clock : clocks_) {
        if (clock->name == name) {
            std::cerr << "Warning: Clock '" << name << "' already exists. "
                      << "Ignoring duplicate definition." << std::endl;
            return;
        }
    }

    // Create new clock
    auto new_clock = std::make_unique<Clock>(name, source_port, period);
    clocks_.push_back(std::move(new_clock));

    std::cout << "  Created clock: " << name
              << " (period: " << period << " ns, source: " << source_port << ")"
              << std::endl;
}

void TimingConstraints::setInputDelay(const std::string& port_name, double delay) {
    if (delay < 0.0) {
        std::cerr << "Warning: Attempt to set negative input delay for port '"
                  << port_name << "': " << delay << std::endl;
    }

    input_delays_[port_name] = delay;

    if (delay > 0.0) {
        std::cout << "  Set input delay: " << port_name
                  << " = " << delay << " ns" << std::endl;
    }
}

void TimingConstraints::setOutputDelay(const std::string& port_name, double delay) {
    if (delay < 0.0) {
        std::cerr << "Warning: Attempt to set negative output delay for port '"
                  << port_name << "': " << delay << std::endl;
    }

    output_delays_[port_name] = delay;

    if (delay > 0.0) {
        std::cout << "  Set output delay: " << port_name
                  << " = " << delay << " ns" << std::endl;
    }
}

// ============================================================================
// Getters (for STAEngine)
// ============================================================================

const Clock* TimingConstraints::getClock(const std::string& name) const {
    for (const auto& clock : clocks_) {
        if (clock->name == name) {
            return clock.get();
        }
    }
    return nullptr;
}

double TimingConstraints::getInputDelay(const std::string& port_name) const {
    auto it = input_delays_.find(port_name);
    if (it != input_delays_.end()) {
        return it->second;
    }
    return default_input_delay_; // Default: use default_input_delay_ if not set
}

double TimingConstraints::getOutputDelay(const std::string& port_name) const {
    auto it = output_delays_.find(port_name);
    if (it != output_delays_.end()) {
        return it->second;
    }
    return default_output_delay_; // Default: use default_output_delay_ if not set
}

double TimingConstraints::getMainClockPeriod() const {
    if (clocks_.empty()) {
        std::cerr << "Warning: No clocks defined. Using default period of 10.0 ns." << std::endl;
        return 10.0;
    }

    // For v1.0: Return the first clock's period (single-clock analysis)
    // In the future, support multiple clocks and analyze the "main" clock
    double period = clocks_[0]->period;

    // Warn if multiple clocks exist (not yet supported in analysis)
    if (clocks_.size() > 1) {
        std::cout << "Note: Multiple clocks defined (" << clocks_.size()
                  << "). Using first clock period: " << period << " ns."
                  << std::endl;
    }

    return period;
}

} // namespace mini
