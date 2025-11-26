/**
 * @file delay_model.h
 * @brief Delay Calculation Model Interface
 * @details Abstract interface for delay calculation, supporting polymorphic delay models
 */

#ifndef MINI_DELAY_MODEL_H
#define MINI_DELAY_MODEL_H

namespace mini {

// Forward declarations
class Cell;
class Net;

/**
 * @class DelayModel
 * @brief Abstract base class for delay calculation (Pure Calculator)
 * @details Defines interface for calculating cell delay, output slew, and wire delay
 */
class DelayModel {
public:
    /**
 * @brief Destructor
     */
    virtual ~DelayModel() = default;

    // ============ Pure Virtual Interfaces ============

    /**
     * @brief Calculate cell delay (input pin to output pin)
     * @param cell Cell instance pointer
     * @param input_slew Input signal transition slew rate
     * @param load_cap Output load capacitance
     * @return Calculated cell delay value
     */
    virtual double calculateCellDelay(Cell* cell, double input_slew, double load_cap) = 0;

    /**
     * @brief Calculate output slew (output signal transition time)
     * @param cell Cell instance pointer
     * @param input_slew Input signal transition slew rate
     * @param load_cap Output load capacitance
     * @return Calculated output slew value
     */
    virtual double calculateOutputSlew(Cell* cell, double input_slew, double load_cap) = 0;

    /**
     * @brief Calculate wire delay for a net
     * @details Simplified Elmore delay or lumped capacitance model
     * @param net Net instance pointer
     * @return Calculated wire delay value
     */
    virtual double calculateWireDelay(Net* net) = 0;
};

/**
 * @class LinearDelayModel
 * @brief Linear delay model implementation (Placeholder)
 * @details Simple linear formula: Delay = Intrinsic + K * Load_Cap
 */
class LinearDelayModel : public DelayModel {
public:
    /**
     * @brief Constructor
     * @param intrinsic_delay Intrinsic delay constant (unit: seconds)
     * @param load_coefficient Load coefficient (unit: seconds/farad)
     * @param slew_coefficient Slew coefficient (unit: seconds/farad)
     */
    LinearDelayModel(double intrinsic_delay = 1e-12,
                     double load_coefficient = 1e-12,
                     double slew_coefficient = 1e-12);

    /**
     * @brief Destructor
     */
    ~LinearDelayModel() override = default;

    // ============ Implementation of Pure Virtual Functions ============

    /**
     * @brief Calculate cell delay using linear model
     * @param cell Cell instance pointer
     * @param input_slew Input signal transition slew rate
     * @param load_cap Output load capacitance
     * @return Calculated cell delay: intrinsic + coefficient * load_cap
     */
    double calculateCellDelay(Cell* cell, double input_slew, double load_cap) override;

    /**
     * @brief Calculate output slew using linear model
     * @param cell Cell instance pointer
     * @param input_slew Input signal transition slew rate
     * @param load_cap Output load capacitance
     * @return Calculated output slew
     */
    double calculateOutputSlew(Cell* cell, double input_slew, double load_cap) override;

    /**
     * @brief Calculate wire delay using simplified model
     * @details Lumped capacitance model for simplicity
     * @param net Net instance pointer
     * @return Calculated wire delay
     */
    double calculateWireDelay(Net* net) override;

private:
    double intrinsic_delay_;        ///< Intrinsic delay constant (seconds)
    double load_coefficient_;       ///< Load coefficient (seconds/farad)
    double slew_coefficient_;       ///< Slew coefficient (seconds/farad)
};

} // namespace mini

#endif // MINI_DELAY_MODEL_H
