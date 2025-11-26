/**
 * @file delay_model.cpp
 * @brief Delay Model Implementation
 * @details Implementation of LinearDelayModel and TableDelayModel (NLDM)
 */

#include "delay_model.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"
#include <iostream>
#include <cmath>

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
    }
}

/**
 * @brief Calculate cell delay using NLDM lookup tables
 */
double TableDelayModel::calculateCellDelay(Cell* cell, double input_slew, double load_cap) {
    if (!library_ || !cell) {
        return 0.0;  // Default fallback
    }

    // 1. Find the corresponding Liberty cell
    const LibCell* lib_cell = library_->getCell(cell->getTypeString());
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

    // 1. Find the corresponding Liberty cell
    const LibCell* lib_cell = library_->getCell(cell->getTypeString());
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
 * @brief Calculate wire delay using simplified model
 */
double TableDelayModel::calculateWireDelay(Net* net) {
    // For now, keep the same simplified wire delay
    // TODO: Implement Elmore delay in a future version
    (void)net;
    return 0.0;
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
