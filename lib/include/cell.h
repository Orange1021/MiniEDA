/**
 * @file cell.h
 * @brief Cell (Logic Gate) Data Model Definition
 * @details Defines the data structures and interfaces for basic logic units in digital circuits
 */

#ifndef MINI_CELL_H
#define MINI_CELL_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace mini {

// Forward declarations
class Net;
class Pin;

/**
 * @brief Cell type enumeration
 * @details Defines the supported standard logic gate types
 */
enum class CellType {
    UNKNOWN,    ///< Unknown type
    // Basic logic gates
    AND,        ///< AND gate
    OR,         ///< OR gate
    NOT,        ///< NOT gate (inverter)
    NAND,       ///< NAND gate
    NOR,        ///< NOR gate
    XOR,        ///< XOR gate
    XNOR,       ///< XNOR gate
    BUF,        ///< Buffer
    // Sequential elements
    DFF,        ///< D flip-flop
    // Combinational logic
    MUX,        ///< Multiplexer
    // I/O ports
    INPUT,      ///< Input port
    OUTPUT      ///< Output port
};

/**
 * @brief Pin direction enumeration
 */
enum class PinDirection {
    INPUT,      ///< Input pin
    OUTPUT,     ///< Output pin
    INOUT       ///< Bidirectional pin
};

/**
 * @brief Pin class - represents a pin of a Cell
 * @details Pins are the interface between Cells and Nets
 */
class Pin {
public:
    /**
     * @brief Constructor
     * @param name Pin name
     * @param direction Pin direction
     * @param owner Pointer to the owning Cell
     */
    Pin(const std::string& name, PinDirection direction, class Cell* owner);

    // Basic accessors
    const std::string& getName() const { return name_; }
    PinDirection getDirection() const { return direction_; }
    Cell* getOwner() const { return owner_; }
    Net* getNet() const { return net_; }

    // Modifiers
    void setNet(Net* net) { net_ = net; }

    // Utility functions
    bool isInput() const { return direction_ == PinDirection::INPUT; }
    bool isOutput() const { return direction_ == PinDirection::OUTPUT; }
    bool isConnected() const { return net_ != nullptr; }

private:
    std::string name_;              ///< Pin name
    PinDirection direction_;        ///< Pin direction
    Cell* owner_;                   ///< Owning Cell
    Net* net_;                      ///< Connected Net (may be null)
};

/**
 * @brief Cell class - represents a logic unit instance
 * @details Contains information about gate type, pins, position, etc.
 */
class Cell {
public:
    /**
     * @brief Constructor
     * @param name Cell instance name
     * @param type Cell type
     */
    Cell(const std::string& name, CellType type);

    /**
     * @brief Destructor
     */
    ~Cell();

    // Disable copy constructor and copy assignment
    Cell(const Cell&) = delete;
    Cell& operator=(const Cell&) = delete;

    // Enable move constructor and move assignment
    Cell(Cell&&) = default;
    Cell& operator=(Cell&&) = default;

    // ============ Basic Accessors ============
    const std::string& getName() const { return name_; }
    CellType getType() const { return type_; }

    // ============ Pin Management ============
    /**
     * @brief Add a pin
     * @param pin_name Pin name
     * @param direction Pin direction
     * @return Pointer to the created Pin
     */
    Pin* addPin(const std::string& pin_name, PinDirection direction);

    /**
     * @brief Find pin by name
     * @param pin_name Pin name
     * @return Pin pointer if found, nullptr otherwise
     */
    Pin* getPin(const std::string& pin_name) const;

    /**
     * @brief Get all pins
     */
    const std::vector<std::unique_ptr<Pin>>& getPins() const { return pins_; }

    /**
     * @brief Get all input pins
     */
    std::vector<Pin*> getInputPins() const;

    /**
     * @brief Get all output pins
     */
    std::vector<Pin*> getOutputPins() const;

    // ============ Position Information (for placement) ============
    void setPosition(double x, double y) { x_ = x; y_ = y; placed_ = true; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    bool isPlaced() const { return placed_; }

    // ============ Timing Information (for STA) ============
    /**
     * @brief Set cell delay
     * @param delay Delay value (unit: ns)
     */
    void setDelay(double delay) { delay_ = delay; }
    double getDelay() const { return delay_; }

    // ============ Utility Functions ============
    /**
     * @brief Check if this is a combinational logic
     */
    bool isCombinational() const;

    /**
     * @brief Check if this is a sequential logic
     */
    bool isSequential() const;

    /**
     * @brief Check if this is an I/O port
     */
    bool isPort() const;

    /**
     * @brief Get string representation of cell type
     */
    std::string getTypeString() const;

    /**
     * @brief Print cell information (for debugging)
     */
    void print() const;

private:
    std::string name_;                              ///< Cell instance name
    CellType type_;                                 ///< Cell type

    // Pin management
    std::vector<std::unique_ptr<Pin>> pins_;        ///< All pins owned by this cell
    std::unordered_map<std::string, Pin*> pin_map_; ///< Pin name to pointer mapping

    // Placement information
    double x_;                                      ///< X coordinate
    double y_;                                      ///< Y coordinate
    bool placed_;                                   ///< Whether the cell has been placed

    // Timing information
    double delay_;                                  ///< Cell delay (ns)
};

/**
 * @brief Convert CellType to string
 * @param type Cell type
 * @return String representation of the type
 */
std::string cellTypeToString(CellType type);

/**
 * @brief Convert string to CellType
 * @param type_str Type string
 * @return Corresponding CellType, UNKNOWN if not recognized
 */
CellType stringToCellType(const std::string& type_str);

} // namespace mini

#endif // MINI_CELL_H
