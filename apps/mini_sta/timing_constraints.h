#ifndef MINI_TIMING_CONSTRAINTS_H
#define MINI_TIMING_CONSTRAINTS_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace mini {

// Forward declaration
class TimingNode;

/**
 * @struct Clock
 * @brief Describes a clock source in the design
 */
struct Clock {
    std::string name;            ///< Clock name
    std::string source_port;     ///< Clock source port name (e.g., "CK")
    double period;               ///< Clock period (ns)
    double waveform[2];          ///< Rising edge and falling edge times {rise, fall}

    /**
     * @brief Constructor
     * @param n Clock name
     * @param src Clock source port
     * @param p Clock period
     */
    Clock(const std::string& n, const std::string& src, double p)
        : name(n), source_port(src), period(p) {
        // Default: {0, period/2} (50% duty cycle)
        waveform[0] = 0.0;
        waveform[1] = p / 2.0;
    }
};

/**
 * @class TimingConstraints
 * @brief Manages timing constraints for the design (SDC-style)
 */
class TimingConstraints {
public:
    /**
     * @brief Constructor
     */
    TimingConstraints() = default;

    /**
     * @brief Destructor
     */
    ~TimingConstraints() = default;

    // Disable copy, enable move
    TimingConstraints(const TimingConstraints&) = delete;
    TimingConstraints& operator=(const TimingConstraints&) = delete;
    TimingConstraints(TimingConstraints&&) = default;
    TimingConstraints& operator=(TimingConstraints&&) = default;

    // ============ Setters (SDC command interfaces) ============

    /**
     * @brief Create clock constraint
     * @details SDC command: create_clock -name <name> -period <period> [get_ports <source>]
     * @param name Clock name
     * @param source_port Clock source port
     * @param period Clock period (ns)
     */
    void createClock(const std::string& name, const std::string& source_port, double period);

    /**
     * @brief Set input delay constraint
     * @details SDC command: set_input_delay <delay> [get_ports <port>]
     * @param port_name Port name
     * @param delay Input delay value (ns)
     */
    void setInputDelay(const std::string& port_name, double delay);

    /**
     * @brief Set output delay constraint
     * @details SDC command: set_output_delay <delay> [get_ports <port>]
     * @param port_name Port name
     * @param delay Output delay value (ns)
     */
    void setOutputDelay(const std::string& port_name, double delay);

    // ============ Getters (for STAEngine) ============

    /**
     * @brief Get clock by name
     * @param name Clock name
     * @return Pointer to Clock, or nullptr if not found
     */
    const Clock* getClock(const std::string& name) const;

    /**
     * @brief Get input delay for a port
     * @details Returns 0.0 if not set
     * @param port_name Port name
     * @return Input delay value (ns)
     */
    double getInputDelay(const std::string& port_name) const;

    /**
     * @brief Get output delay for a port
     * @details Returns 0.0 if not set
     * @param port_name Port name
     * @return Output delay value (ns)
     */
    double getOutputDelay(const std::string& port_name) const;

    /**
     * @brief Get main clock period
     * @details For v1.0 single-clock analysis, returns first clock's period
     * @return Main clock period (ns), or 10.0 if no clocks defined
     */
    double getMainClockPeriod() const;

    /**
     * @brief Check if constraints are empty
     * @return true if no constraints defined
     */
    bool empty() const { return clocks_.empty(); }

    /**
     * @brief Get number of clocks
     * @return Clock count
     */
    size_t getClockCount() const { return clocks_.size(); }

private:
    std::vector<std::unique_ptr<Clock>> clocks_;  ///< Clock storage
    std::unordered_map<std::string, double> input_delays_;   ///< Port name -> Input delay
    std::unordered_map<std::string, double> output_delays_;  ///< Port name -> Output delay
};

} // namespace mini

#endif // MINI_TIMING_CONSTRAINTS_H
