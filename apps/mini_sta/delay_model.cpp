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
    const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->mapType(cell->getTypeString()) : nullptr;
    if (!lib_cell) {
        std::cerr << "Warning: Cell type '" << cell->getTypeString() << "' not found in library" << std::endl;
        return 0.01;  // Small default delay
    }

    // 2. Find the appropriate timing arc
    // For simplicity, we'll use the first timing arc from the first output pin
    // In a real implementation, we need to match input and output pins properly
    
    const LibTiming* timing_arc = nullptr;
    
    // Iterate through pins to find an output pin with timing arcs
    for (const auto& pin_pair : lib_cell->pins) {
        const LibPin& pin = pin_pair.second;
        if (pin.direction == "output" && !pin.timing_arcs.empty()) {
            // Use the first timing arc from this output pin
            timing_arc = &pin.timing_arcs[0];
            break;
        }
    }
    
    if (!timing_arc) {
        std::cerr << "Warning: No timing arc found for cell '" << cell->getTypeString() << "'" << std::endl;
        return 0.01;  // Small default delay
    }

    // 3. Lookup delay from the NLDM table
    if (timing_arc->cell_delay.isValid()) {
        double delay = timing_arc->cell_delay.lookup(input_slew, load_cap);
        // Convert from nanoseconds to seconds if needed (assuming lib is in ns)
        return delay * 1e-9;  // Convert ns to s
    }

    // Fallback: use intrinsic delay if available
    return lib_cell->area * 0.01 * 1e-9;  // Rough estimate
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
    const LibTiming* timing_arc = nullptr;
    
    for (const auto& pin_pair : lib_cell->pins) {
        const LibPin& pin = pin_pair.second;
        if (pin.direction == "output" && !pin.timing_arcs.empty()) {
            timing_arc = &pin.timing_arcs[0];
            break;
        }
    }
    
    if (!timing_arc) {
        return 0.05 * 1e-9;  // Default fallback
    }

    // 3. For simplicity, use a fraction of the delay as slew
    // In a real library, there would be separate slew tables
    if (timing_arc->cell_delay.isValid()) {
        double delay = timing_arc->cell_delay.lookup(input_slew, load_cap);
        // Approximate slew as 50% of delay (this is library-dependent)
        return delay * 0.5 * 1e-9;  // Convert ns to s
    }

    // Fallback
    return 0.05 * 1e-9;
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
                
                const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->mapType(cell_type) : library_->getCell(cell_type);
                if (lib_cell) {
                    const LibPin* lib_pin = lib_cell->getPin(pin_name);
                    if (lib_pin) {
                        total_pin_cap += lib_pin->capacitance;  // pF
                    }
                }
            }
        }
        
        double total_cap = wire_c + total_pin_cap;
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
            
            const LibCell* lib_cell = cell_mapper_ ? cell_mapper_->mapType(cell_type) : library_->getCell(cell_type);
            if (lib_cell) {
                const LibPin* lib_pin = lib_cell->getPin(pin_name);
                if (lib_pin) {
                    total_pin_cap += lib_pin->capacitance;  // pF
                }
            }
        }
    }
    
    // 5. Elmore delay model: Delay = R_wire * (C_wire/2 + C_load)
    double total_cap = wire_c + total_pin_cap;  // pF
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

} // namespace mini
