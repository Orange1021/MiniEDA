/**
 * @file delay_model.cpp
 * @brief Delay Model Implementation
 * @details Implementation of LinearDelayModel and delay calculation methods
 */

#include "delay_model.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"

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
double LinearDelayModel::calculateCellDelay(Cell* cell, double input_slew, double load_cap) {
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

} // namespace mini
