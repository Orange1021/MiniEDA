/**
 * @file delay_model.cpp
 * @brief Delay Model Implementation
 * @details Implementation of LinearDelayModel and TableDelayModel (NLDM)
 */

#include "delay_model.h"
#include "cell_mapper.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"
#include <iostream>
#include <cmath>
#include <memory>
#include <utility>

namespace mini {

// ============================================================================
// LinearDelayModel Implementation
// ============================================================================

LinearDelayModel::LinearDelayModel(double intrinsic_delay,
                                   double load_coefficient,
                                   double slew_coefficient)
    : intrinsic_delay_(intrinsic_delay),
      load_coefficient_(load_coefficient),
      slew_coefficient_(slew_coefficient) {
}

/**
 * @brief Calculate cell delay using linear model: Delay = Intrinsic + K * Load_Cap
 */
double LinearDelayModel::calculateCellDelay(Cell* /* cell */, double input_slew, double load_cap) {
    // Simplified linear delay model
    // In real EDA tools, this would use NLDM (Non-Linear Delay Model) lookup tables
    // For educational purposes, we use: Delay = intrinsic + coefficient * load_cap

    // Note: input_slew is typically used to index a 2D table, but we ignore it in this simple model
    (void)input_slew;  // Suppress unused parameter warning

    return intrinsic_delay_ + load_coefficient_ * load_cap;
}

/**
 * @brief Calculate output slew using simplified linear model
 */
double LinearDelayModel::calculateOutputSlew(Cell* cell, double input_slew, double load_cap) {
    // Simplified slew model: similar to delay but with different coefficients
    // In practice, slew depends on both input slew and load capacitance
    (void)input_slew;  // Suppress unused parameter warning
    (void)cell;        // Suppress unused parameter warning

    // Use a fraction of intrinsic delay for slew calculation
    return intrinsic_delay_ * 0.1 + slew_coefficient_ * load_cap;
}

/**
 * @brief Calculate wire delay using simplified model
 */
double LinearDelayModel::calculateWireDelay(Net* net) {
    // Simplified wire delay calculation
    // Industrial EDA tools use Elmore delay model: delay = 0.5 * R * C
    // For this educational version, we return 0.0 to simplify the problem

    (void)net;  // Suppress unused parameter warning

    // TODO: Future enhancement - implement Elmore delay
    // double resistance = net->getResistance();
    // double capacitance = net->getTotalCapacitance();
    // return 0.5 * resistance * capacitance;

    return 0.0;  // Simplified: ignore wire delay for now
}

/**
 * @brief Calculate point-to-point interconnect delay (simplified)
 */
std::pair<double, double> LinearDelayModel::calculateInterconnectDelay(
    Pin* /* driver_pin */, 
    Pin* /* load_pin */,
    double input_slew,
    double /* wire_r_per_unit */,
    double /* wire_c_per_unit */) {
    // Simplified: return zero delay and pass through slew
    return {0.0, input_slew};
}

// ============================================================================
// TableDelayModel Implementation
// ============================================================================

TableDelayModel::TableDelayModel(Library* lib) : library_(lib) {
    if (!library_) {
        std::cerr << "Warning: TableDelayModel initialized with null library" << std::endl;
    } else {
        // Initialize cell mapper with the library
        cell_mapper_ = std::make_unique<CellMapper>(*library_);
        // Enable debug mode to see mapping details
        cell_mapper_->setDebugMode(false); // Set to true for debugging
    }
}

/**
 * @brief Calculate cell delay using NLDM lookup tables
 */
double TableDelayModel::calculateCellDelay(Cell* cell, double input_slew, double load_cap) {
    if (!library_ || !cell) {
        return 0.0;  // Default fallback
    }

    // 1. Find the corresponding Liberty cell using CellMapper
    const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->findWithWarning(cell->getTypeString()) : nullptr;
    if (!lib_cell) {
        return 0.01;  // Small default delay
    }

    // 2. Find the appropriate timing arc
    // For simplicity, we'll use the first timing arc from the first output pin
    // In a real implementation, we need to match input and output pins properly
    
    const LibTiming* timing_arc = getFirstTimingArc(lib_cell);
    
    if (!timing_arc) {
        std::cerr << "Warning: No timing arc found for cell '" << cell->getTypeString() << "'" << std::endl;
        return 0.01;  // Small default delay
    }

    // 3. Lookup delay from the NLDM table
    double delay = safeLookup(&timing_arc->cell_delay, input_slew, load_cap);
    // Convert from nanoseconds to seconds if needed (assuming lib is in ns)
    return delay * 1e-9;  // Convert ns to s
}

/**
 * @brief Calculate output slew using NLDM lookup tables
 */
double TableDelayModel::calculateOutputSlew(Cell* cell, double input_slew, double load_cap) {
    if (!library_ || !cell) {
        return 0.05 * 1e-9;  // Default fallback: 50ps
    }

    // 1. Find the corresponding Liberty cell using CellMapper
    const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->mapType(cell->getTypeString()) : nullptr;
    if (!lib_cell) {
        return 0.05 * 1e-9;  // Default fallback
    }

    // 2. Find the appropriate timing arc
    const LibTiming* timing_arc = getFirstTimingArc(lib_cell);
    
    if (!timing_arc) {
        return 0.05 * 1e-9;  // Default fallback
    }

    // 3. [NEW] Use proper NLDM slew table lookup
    // For now, we'll use rise_transition as default (will be enhanced with edge detection)
    const LookupTable* slew_table = nullptr;
    
    // Priority: rise_transition > fall_transition > delay approximation
    if (timing_arc->rise_transition.isValid()) {
        slew_table = &timing_arc->rise_transition;
    } else if (timing_arc->fall_transition.isValid()) {
        slew_table = &timing_arc->fall_transition;
    } else if (timing_arc->cell_delay.isValid()) {
        // Fallback: use delay table as rough approximation (better than magic number)
        double delay = safeLookup(&timing_arc->cell_delay, input_slew, load_cap);
        return delay * 0.7 * 1e-9;  // Slightly better approximation: 70% of delay
    }

    // 4. Perform NLDM lookup
    if (slew_table && slew_table->isValid()) {
        double output_slew = safeLookup(slew_table, input_slew, load_cap);
        return output_slew * 1e-9;  // Convert from ns to s
    }

    // Final fallback
    return 0.05 * 1e-9;
}

/**
 * @brief [NEW] Calculate output slew with explicit signal edge from NLDM tables
 * @param timing_arc Timing arc with Liberty data
 * @param input_slew Input signal transition slew rate
 * @param load_cap Output load capacitance
 * @param edge Signal edge direction (RISE or FALL)
 * @return Calculated output slew from appropriate NLDM table
 */
double TableDelayModel::calculateOutputSlewWithEdge(const LibTiming* timing_arc,
                                                     double input_slew, 
                                                     double load_cap, 
                                                     SignalEdge edge) const {
    // 1. Safety checks
    if (!timing_arc) {
        return 0.001 * 1e-9;  // Fallback: 1ps (minimum realistic value)
    }

    // 2. Select correct lookup table based on signal edge
    const LookupTable* slew_table = nullptr;
    
    if (edge == SignalEdge::RISE) {
        slew_table = &timing_arc->rise_transition;
    } else {  // FALL
        slew_table = &timing_arc->fall_transition;
    }

    // 3. Verify table is valid
    if (!slew_table || !slew_table->isValid()) {
        // Fallback: try the other table
        const LookupTable* fallback_table = (edge == SignalEdge::RISE) ? 
            &timing_arc->fall_transition : &timing_arc->rise_transition;
        
        if (fallback_table && fallback_table->isValid()) {
            slew_table = fallback_table;
        } else {
            // Final fallback: use delay approximation
            if (timing_arc->cell_delay.isValid()) {
                double delay = safeLookup(&timing_arc->cell_delay, input_slew, load_cap);
                return delay * 0.7 * 1e-9;  // 70% of delay as rough estimate
            }
            return 0.001 * 1e-9;  // 1ps fallback
        }
    }

    // 4. Perform NLDM lookup
    double output_slew = safeLookup(slew_table, input_slew, load_cap);

    // 5. Ensure reasonable bounds (prevent negative or extremely small slew)
    if (output_slew <= 0.001) {  // Less than 1ps
        output_slew = 0.001;  // Minimum 1ps
    }
    
    return output_slew * 1e-9;  // Convert from ns to s
}

/**
 * @brief [NEW] Determine signal edge based on timing sense
 * @param timing_sense Liberty timing sense ("positive_unate", "negative_unate", "non_unate")
 * @param input_edge Input signal edge (assumed to be the direction causing this calculation)
 * @return Output signal edge for NLDM table selection
 */
SignalEdge TableDelayModel::determineOutputEdge(const std::string& timing_sense, SignalEdge input_edge) const {
    // Handle unateness based on Liberty timing sense
    if (timing_sense == "positive_unate") {
        // Positive unate: output follows input direction
        // Input Rise → Output Rise, Input Fall → Output Fall
        return input_edge;
    } else if (timing_sense == "negative_unate") {
        // Negative unate: output inverts input direction
        // Input Rise → Output Fall, Input Fall → Output Rise
        return (input_edge == SignalEdge::RISE) ? SignalEdge::FALL : SignalEdge::RISE;
    } else {
        // Non_unate or unknown: default to positive unate
        return input_edge;
    }
}

/**
 * @brief Calculate wire delay using Elmore delay model with fanout-based estimation
 */
double TableDelayModel::calculateWireDelay(Net* net) {
    if (!net) {
        return 0.0;
    }

    // HPWL-based Elmore Delay Model for Post-Placement STA
    
    // 1. Calculate bounding box for all pins on this net
    double min_x = std::numeric_limits<double>::infinity();
    double max_x = -std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    double max_y = -std::numeric_limits<double>::infinity();
    
    bool has_placed_pins = false;
    
    // Include driver pin
    Pin* driver_pin = net->getDriver();
    if (driver_pin && driver_pin->getOwner() && driver_pin->getOwner()->isPlaced()) {
        double x = driver_pin->getOwner()->getX();
        double y = driver_pin->getOwner()->getY();
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);
        min_y = std::min(min_y, y);
        max_y = std::max(max_y, y);
        has_placed_pins = true;
    }
    
    // Include all load pins
    for (Pin* load_pin : net->getLoads()) {
        if (load_pin && load_pin->getOwner() && load_pin->getOwner()->isPlaced()) {
            double x = load_pin->getOwner()->getX();
            double y = load_pin->getOwner()->getY();
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
            has_placed_pins = true;
        }
    }
    
    // If no placed pins, fall back to fanout-based estimation
    if (!has_placed_pins) {
        size_t fanout = net->getFanout();
        double estimated_r_per_fanout = 0.01;  // 0.01 kΩ = 10 Ω per fanout
        double estimated_c_per_fanout = 0.002; // 0.002 pF = 2 fF per fanout
        double wire_r = fanout * estimated_r_per_fanout;  // kΩ
        double wire_c = fanout * estimated_c_per_fanout;  // pF
        
        double total_pin_cap = 0.0;
        for (Pin* load_pin : net->getLoads()) {
            if (load_pin && load_pin->getOwner()) {
                std::string cell_type = load_pin->getOwner()->getTypeString();
                std::string pin_name = load_pin->getName();
                
                const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->findWithLibraryFallback(cell_type, library_) : nullptr;
                if (lib_cell) {
                    const LibPin* lib_pin = lib_cell->getPin(pin_name);
                    if (lib_pin) {
                        total_pin_cap += lib_pin->capacitance;  // pF
                    }
                }
            }
        }
        
        return wire_r * (wire_c/2.0 + total_pin_cap);  // kΩ * pF = ns
    }
    
    // 2. Calculate HPWL (Half-Perimeter Wire Length)
    double hpwl = (max_x - min_x) + (max_y - min_y);  // in micrometers
    
    // 3. Nangate 45nm technology parameters (approximate)
    double unit_r = 0.005;   // kΩ/μm (5 Ω/μm)
    double unit_c = 0.0002;  // pF/μm (0.2 fF/μm)
    
    double wire_r = hpwl * unit_r;  // kΩ
    double wire_c = hpwl * unit_c;  // pF
    
    // 4. Calculate total pin capacitance
    double total_pin_cap = 0.0;
    for (Pin* load_pin : net->getLoads()) {
        if (load_pin && load_pin->getOwner()) {
            std::string cell_type = load_pin->getOwner()->getTypeString();
            std::string pin_name = load_pin->getName();
            
            const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->findWithLibraryFallback(cell_type, library_) : nullptr;
            if (lib_cell) {
                const LibPin* lib_pin = lib_cell->getPin(pin_name);
                if (lib_pin) {
                    total_pin_cap += lib_pin->capacitance;  // pF
                }
            }
        }
    }
    
    // 5. Elmore delay model: Delay = R_wire * (C_wire/2 + C_load)
    double delay = wire_r * (wire_c/2.0 + total_pin_cap);  // kΩ * pF = ns
    
    return delay;
}

/**
 * @brief Find the appropriate timing arc for a cell
 */
const LibTiming* TableDelayModel::findTimingArc(const LibCell* lib_cell, 
                                                const std::string& from_pin, 
                                                const std::string& to_pin) const {
    if (!lib_cell) return nullptr;

    // Look through all pins to find the output pin
    for (const auto& pin_pair : lib_cell->pins) {
        const LibPin& pin = pin_pair.second;
        if (pin.direction == "output" && pin.name == to_pin) {
            // Look through timing arcs for this output pin
            for (const auto& timing_arc : pin.timing_arcs) {
                if (timing_arc.related_pin == from_pin) {
                    return &timing_arc;
                }
            }
        }
    }

    return nullptr;  // No matching timing arc found
}

/**
 * @brief Get the first timing arc from the first output pin
 */
const LibTiming* TableDelayModel::getFirstTimingArc(const LibCell* lib_cell) const {
    if (!lib_cell) return nullptr;

    // Iterate through pins to find the first output pin with timing arcs
    for (const auto& pin_pair : lib_cell->pins) {
        const LibPin& pin = pin_pair.second;
        if (pin.direction == "output" && !pin.timing_arcs.empty()) {
            // Return the first timing arc from this output pin
            return &pin.timing_arcs[0];
        }
    }

    return nullptr;  // No timing arc found
}

/**
 * @brief Safe lookup with boundary checking
 */
double TableDelayModel::safeLookup(const LookupTable* table,
                                   double x, double y,
                                   double x_min, double x_max,
                                   double y_min, double y_max,
                                   double default_value) const {
    if (!table || !table->isValid()) {
        return default_value;
    }

    // Clamp input values to reasonable ranges
    double clamped_x = std::max(x_min, std::min(x_max, x));
    double clamped_y = std::max(y_min, std::min(y_max, y));

    // Perform lookup
    return table->lookup(clamped_x, clamped_y);
}

/**
 * @brief Calculate point-to-point interconnect delay using Elmore model
 * @details Implements:
 * 1. Calculate Manhattan distance between driver and load pins
 * 2. Elmore Delay: delay = R_wire * (C_load + 0.5 * C_wire)
 * 3. Slew Degradation: out_slew = sqrt(in_slew^2 + (k * delay)^2)
 */
std::pair<double, double> TableDelayModel::calculateInterconnectDelay(
    Pin* driver_pin, 
    Pin* load_pin,
    double input_slew,
    double wire_r_per_unit,
    double wire_c_per_unit) {
    
    // Default fallback values
    const double MIN_DELAY = 1e-15;  // 1fs minimum
    const double SLEW_FACTOR = 2.2;  // Bakoglu metric coefficient
    
    // Safety checks
    if (!driver_pin || !load_pin) {
        return {MIN_DELAY, input_slew};
    }
    
    Cell* driver_cell = driver_pin->getOwner();
    Cell* load_cell = load_pin->getOwner();
    if (!driver_cell || !load_cell) {
        return {MIN_DELAY, input_slew};
    }
    
    // ========================================================================
    // 1. Calculate wire length (prioritize actual routing length)
    // ========================================================================
    double wire_length = 0.0;
    
    // Priority 1: Use actual routed wire length from Router (if available)
    Net* net = driver_pin->getNet();
    if (net && net->hasWireLength()) {
        wire_length = net->getWireLength();  // [INFO] Actual routing length
    } else {
        // Priority 2: Fallback to Manhattan distance estimate (pre-routing)
        double driver_x = driver_cell->getX();
        double driver_y = driver_cell->getY();
        double load_x = load_cell->getX();
        double load_y = load_cell->getY();
        
        wire_length = std::abs(load_x - driver_x) + std::abs(load_y - driver_y);  // μm
    }
    
    // If cells are not placed or at same location, use minimal delay
    if (wire_length < 0.01) {  // Less than 0.01 μm
        return {MIN_DELAY, input_slew};
    }
    
    // ========================================================================
    // 2. Calculate wire R and C
    // ========================================================================
    double r_wire = wire_length * wire_r_per_unit;  // kΩ
    double c_wire = wire_length * wire_c_per_unit;  // pF
    
    // ========================================================================
    // 3. Get load pin capacitance from Liberty
    // ========================================================================
    double c_load = 0.002;  // Default 2fF
    
    if (library_ && cell_mapper_) {
        std::string load_cell_type = load_cell->getTypeString();
        const LibCell* lib_cell = cell_mapper_->mapType(load_cell_type);
        
        if (lib_cell) {
            std::string load_pin_name = load_pin->getName();
            const LibPin* lib_pin = lib_cell->getPin(load_pin_name);
            
            if (lib_pin && lib_pin->capacitance > 0.0) {
                c_load = lib_pin->capacitance;  // pF
            }
        }
    }
    
    // ========================================================================
    // 4. Elmore Delay Model: delay = R_wire * (C_load + 0.5 * C_wire)
    // ========================================================================
    double elmore_delay = r_wire * (c_load + 0.5 * c_wire);  // kΩ * pF = ns
    
    // Ensure minimum delay
    if (elmore_delay < MIN_DELAY * 1e9) {  // Convert fs to ns for comparison
        elmore_delay = MIN_DELAY * 1e9;
    }
    
    // ========================================================================
    // 5. Slew Degradation (Bakoglu approximation)
    // ========================================================================
    // Wire contributes to slew degradation: slew_wire ≈ k * delay
    // Total output slew uses RSS (Root Sum Square) combination:
    // out_slew = sqrt(in_slew^2 + slew_wire^2)
    
    double slew_wire_contribution = SLEW_FACTOR * elmore_delay * 1e-9;  // Convert ns to s
    double output_slew = std::sqrt(
        std::pow(input_slew, 2) + 
        std::pow(slew_wire_contribution, 2)
    );
    
    // Ensure reasonable bounds
    if (output_slew < input_slew) {
        output_slew = input_slew;  // Slew can only degrade, not improve
    }
    
    // ========================================================================
    // 6. Return results
    // ========================================================================
    return {elmore_delay * 1e-9, output_slew};  // Convert delay from ns to s
}

} // namespace mini
