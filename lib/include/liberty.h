/**
 * @file liberty.h
 * @brief Liberty Library Format Support
 * @details Data structures for NLDM (Non-Linear Delay Model) timing characterization
 * Supports industrial standard .lib format for accurate delay calculation
 */

#ifndef MINI_LIBERTY_H
#define MINI_LIBERTY_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace mini {

/**
 * @struct LookupTable
 * @brief 2D lookup table for NLDM characterization data
 * @details Stores timing data as function of input slew and output load capacitance
 * Supports bilinear interpolation for accurate delay calculation
 */
struct LookupTable {
    std::string name;                                   ///< Table name (e.g., "cell_delay")
    std::vector<double> index_1;                        ///< First dimension values (typically input slew)
    std::vector<double> index_2;                        ///< Second dimension values (typically output cap)
    std::vector<std::vector<double>> values;            ///< 2D matrix of timing values

    /**
     * @brief Perform bilinear interpolation lookup
     * @param x First dimension coordinate (e.g., input slew)
     * @param y Second dimension coordinate (e.g., output capacitance)
     * @return Interpolated value
     * @details Algorithm:
     * 1. Find bounding indices for x and y
     * 2. Clamp out-of-range values to table boundaries
     * 3. Perform weighted average using bilinear interpolation formula:
     *    f(x,y) = f00*(x2-x)*(y2-y) + f10*(x-x1)*(y2-y) +
     *             f01*(x2-x)*(y-y1) + f11*(x-x1)*(y-y1)
     *    where f00, f10, f01, f11 are the four corner values
     */
    double lookup(double x, double y) const;

    /**
     * @brief Check if table is valid (has data)
     */
    bool isValid() const {
        return !index_1.empty() && !index_2.empty() && !values.empty();
    }

    /**
     * @brief Get table dimensions
     */
    size_t getDim1Size() const { return index_1.size(); }
    size_t getDim2Size() const { return index_2.size(); }
};

/**
 * @struct LibTiming
 * @brief Timing arc characterization data for a cell pin
 * @details Contains NLDM tables for delay and output slew calculation
 */
struct LibTiming {
    std::string related_pin;                            ///< Related input pin name
    std::string timing_sense;                           ///< "positive_unate", "negative_unate", "non_unate"
    std::string timing_type;                            ///< "combinational", "preset", "clear", etc.

    LookupTable cell_delay;                             ///< Cell delay vs (slew, cap)
    LookupTable rise_transition;                        ///< Rise slew vs (slew, cap)
    LookupTable fall_transition;                        ///< Fall slew vs (slew, cap)

    /**
     * @brief Check if timing data is valid
     */
    bool isValid() const {
        return !related_pin.empty() && cell_delay.isValid();
    }
};

/**
 * @struct LibPin
 * @brief Cell pin characterization data
 * @details Contains pin capacitance and timing arcs
 */
struct LibPin {
    std::string name;                                   ///< Pin name
    std::string direction;                              ///< "input", "output", "inout"
    double capacitance;                                 ///< Pin capacitance (farads)
    double rise_capacitance;                            ///< Rise capacitance
    double fall_capacitance;                            ///< Fall capacitance
    double max_transition;                              ///< Maximum allowed transition time

    std::vector<LibTiming> timing_arcs;                 ///< Timing arcs from this pin (for output pins)

    /**
     * @brief Constructor
     */
    LibPin() : capacitance(0.0), rise_capacitance(0.0),
               fall_capacitance(0.0), max_transition(0.0) {}

    /**
     * @brief Check if pin is input
     */
    bool isInput() const { return direction == "input"; }

    /**
     * @brief Check if pin is output
     */
    bool isOutput() const { return direction == "output"; }
};

/**
 * @struct LibCell
 * @brief Standard cell characterization data
 * @details Complete timing and power data for a standard cell
 */
struct LibCell {
    std::string name;                                   ///< Cell name (e.g., "AND2X1")
    double area;                                        ///< Cell area
    std::string cell_footprint;                         ///< Cell footprint for legalization

    std::unordered_map<std::string, LibPin> pins;       ///< Pin name -> Pin data

    /**
     * @brief Get pin by name
     * @param pin_name Pin name
     * @return Pointer to pin or nullptr if not found
     */
    LibPin* getPin(const std::string& pin_name) {
        auto it = pins.find(pin_name);
        return (it != pins.end()) ? &it->second : nullptr;
    }

    /**
     * @brief Get pin by name (const version)
     */
    const LibPin* getPin(const std::string& pin_name) const {
        auto it = pins.find(pin_name);
        return (it != pins.end()) ? &it->second : nullptr;
    }

    /**
     * @brief Add a new pin
     */
    void addPin(const std::string& pin_name, const LibPin& pin) {
        pins[pin_name] = pin;
    }
};

/**
 * @class Library
 * @brief Top-level Liberty library container
 * @details Manages an entire process library (e.g., "45nm.lib")
 */
class Library {
public:
    /**
     * @brief Constructor
     * @param name Library name
     */
    explicit Library(const std::string& name) : name_(name) {}

    /**
     * @brief Destructor
     */
    ~Library() = default;

    // Disable copy, enable move
    Library(const Library&) = delete;
    Library& operator=(const Library&) = delete;
    Library(Library&&) = default;
    Library& operator=(Library&&) = default;

    /**
     * @brief Get library name
     */
    const std::string& getName() const { return name_; }

    /**
     * @brief Get cell by name
     * @param cell_name Cell name
     * @return Pointer to cell or nullptr if not found
     */
    LibCell* getCell(const std::string& cell_name) {
        auto it = cells_.find(cell_name);
        return (it != cells_.end()) ? &it->second : nullptr;
    }

    /**
     * @brief Get cell by name (const version)
     */
    const LibCell* getCell(const std::string& cell_name) const {
        auto it = cells_.find(cell_name);
        return (it != cells_.end()) ? &it->second : nullptr;
    }

    /**
     * @brief Add a new cell
     */
    void addCell(const std::string& cell_name, const LibCell& cell) {
        cells_[cell_name] = cell;
    }

    /**
     * @brief Get all cell names
     */
    std::vector<std::string> getCellNames() const;

    /**
     * @brief Get number of cells
     */
    size_t getCellCount() const { return cells_.size(); }

    /**
     * @brief Check if library is empty
     */
    bool empty() const { return cells_.empty(); }

    /**
     * @brief Clear all cells
     */
    void clear() { cells_.clear(); }

private:
    std::string name_;                                          ///< Library name
    std::unordered_map<std::string, LibCell> cells_;            ///< Cell name -> Cell data
};

} // namespace mini

#endif // MINI_LIBERTY_H
