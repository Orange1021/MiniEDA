/**
 * @file delay_model.h
 * @brief Delay Calculation Model Interface
 * @details Abstract interface for delay calculation, supporting polymorphic delay models
 */

#ifndef MINI_DELAY_MODEL_H
#define MINI_DELAY_MODEL_H

#include "../../lib/include/liberty.h"

namespace mini {

// Forward declarations
class Cell;
class Net;
class Library;
class CellMapper;

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

/**
 * @class TableDelayModel
 * @brief NLDM-based delay model using Liberty library lookup tables
 * @details Implements realistic delay calculation using NLDM lookup tables from Liberty files
 */
class TableDelayModel : public DelayModel {
public:
    /**
     * @brief Constructor
     * @param lib Pointer to parsed Liberty library
     */
    explicit TableDelayModel(Library* lib);

    /**
     * @brief Destructor
     */
    ~TableDelayModel() override = default;

    // ============ Implementation of Pure Virtual Functions ============

    /**
     * @brief Calculate cell delay using NLDM lookup tables
     * @param cell Cell instance pointer
     * @param input_slew Input signal transition slew rate
     * @param load_cap Output load capacitance
     * @return Calculated cell delay from lookup table
     */
    double calculateCellDelay(Cell* cell, double input_slew, double load_cap) override;

    /**
     * @brief Calculate output slew using NLDM lookup tables
     * @param cell Cell instance pointer
     * @param input_slew Input signal transition slew rate
     * @param load_cap Output load capacitance
     * @return Calculated output slew from lookup table
     */
    double calculateOutputSlew(Cell* cell, double input_slew, double load_cap) override;

    /**
     * @brief Calculate wire delay using simplified model
     * @param net Net instance pointer
     * @return Calculated wire delay
     */
    double calculateWireDelay(Net* net) override;

private:
    Library* library_;              ///< Pointer to Liberty library
    std::unique_ptr<CellMapper> cell_mapper_;  ///< Cell type mapper

    /**
     * @brief Find the appropriate timing arc for a cell
     * @param lib_cell Liberty cell pointer
     * @param from_pin Input pin name
     * @param to_pin Output pin name
     * @return Pointer to matching timing arc, or nullptr if not found
     */
    const LibTiming* findTimingArc(const LibCell* lib_cell, 
                                   const std::string& from_pin, 
                                   const std::string& to_pin) const;
};

} // namespace mini

#endif // MINI_DELAY_MODEL_H
