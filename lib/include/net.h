/**
 * @file net.h
 * @brief Net (Wire/Signal) Data Model Definition
 * @details Defines the data structures for electrical connections between cells
 */

#ifndef MINI_NET_H
#define MINI_NET_H

#include <string>
#include <vector>
#include <algorithm>

namespace mini {

// Forward declarations
class Pin;
class Cell;

/**
 * @brief Net class - represents an electrical connection
 * @details A Net connects one driver pin (output) to multiple load pins (inputs)
 */
class Net {
public:
    /**
     * @brief Constructor
     * @param name Net name
     */
    explicit Net(const std::string& name);

    /**
     * @brief Destructor
     */
    ~Net();

    // Disable copy constructor and copy assignment
    Net(const Net&) = delete;
    Net& operator=(const Net&) = delete;

    // Enable move constructor and move assignment
    Net(Net&&) = default;
    Net& operator=(Net&&) = default;

    // ============ Basic Accessors ============
    const std::string& getName() const { return name_; }

    // ============ Pin Connection Management ============
    /**
     * @brief Set the driver pin (output pin)
     * @param pin Pointer to the driver pin
     * @note A net typically has only one driver
     */
    void setDriver(Pin* pin);

    /**
     * @brief Add a load pin (input pin)
     * @param pin Pointer to the load pin to add
     */
    void addLoad(Pin* pin);

    /**
     * @brief Remove a load pin
     * @param pin Pointer to the load pin to remove
     * @return true if pin was found and removed, false otherwise
     */
    bool removeLoad(Pin* pin);

    /**
     * @brief Disconnect all pins from this net
     */
    void disconnectAll();

    // ============ Connection Query ============
    /**
     * @brief Get the driver pin
     * @return Pointer to driver pin, nullptr if no driver
     */
    Pin* getDriver() const { return driver_; }

    /**
     * @brief Get all load pins
     * @return Vector of load pin pointers
     */
    const std::vector<Pin*>& getLoads() const { return loads_; }

    /**
     * @brief Get all connected pins (driver + loads)
     * @return Vector containing all connected pins
     */
    std::vector<Pin*> getAllPins() const;

    /**
     * @brief Get fanout count (number of load pins)
     * @return Number of load pins
     */
    size_t getFanout() const { return loads_.size(); }

    /**
     * @brief Check if net has a driver
     * @return true if driver is set, false otherwise
     */
    bool hasDriver() const { return driver_ != nullptr; }

    /**
     * @brief Check if net is connected to a specific pin
     * @param pin Pin to check
     * @return true if connected, false otherwise
     */
    bool isConnectedTo(const Pin* pin) const;

    /**
     * @brief Check if net is connected to any pin of a specific cell
     * @param cell Cell to check
     * @return true if connected to the cell, false otherwise
     */
    bool isConnectedTo(const Cell* cell) const;

    // ============ Timing Information (for STA) ============
    /**
     * @brief Set wire capacitance
     * @param cap Capacitance value (unit: pF)
     */
    void setWireCapacitance(double cap) { wire_capacitance_ = cap; }

    /**
     * @brief Get wire capacitance
     * @return Capacitance value (unit: pF)
     */
    double getWireCapacitance() const { return wire_capacitance_; }

    /**
     * @brief Set wire delay
     * @param delay Delay value (unit: ns)
     */
    void setWireDelay(double delay) { wire_delay_ = delay; }

    /**
     * @brief Get wire delay
     * @return Delay value (unit: ns)
     */
    double getWireDelay() const { return wire_delay_; }

    // ============ Special Attributes ============
    /**
     * @brief Set whether this net is a clock signal
     * @param is_clock true for clock net, false otherwise
     */
    void setClock(bool is_clock) { is_clock_ = is_clock; }

    /**
     * @brief Check if this is a clock net
     * @return true if clock net, false otherwise
     */
    bool isClock() const { return is_clock_; }

    // ============ Utility Functions ============
    /**
     * @brief Print net information (for debugging)
     */
    void print() const;

private:
    std::string name_;                  ///< Net name

    // Pin connectivity
    Pin* driver_;                       ///< Driver pin (output)
    std::vector<Pin*> loads_;           ///< Load pins (inputs)

    // Timing information
    double wire_capacitance_;           ///< Wire capacitance (pF)
    double wire_delay_;                 ///< Wire delay (ns)

    // Attributes
    bool is_clock_;                     ///< Whether this is a clock signal
};

} // namespace mini

#endif // MINI_NET_H
