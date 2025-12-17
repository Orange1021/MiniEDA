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
 * @brief Manages timing constraints for the design (SDC-style with industrial features)
 */
class TimingConstraints {
public:
    /**
     * @brief Constructor
     */
    TimingConstraints();

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
     * @brief Set default input delay for all primary inputs
     * @param delay Default input delay value (ns)
     */
    void setDefaultInputDelay(double delay) { default_input_delay_ = delay; }

    /**
     * @brief Set default output delay for all primary outputs
     * @param delay Default output delay value (ns)
     */
    void setDefaultOutputDelay(double delay) { default_output_delay_ = delay; }

    /**
     * @brief Get input delay for a port
     * @details Returns default_input_delay_ if not set for specific port
     * @param port_name Port name
     * @return Input delay value (ns)
     */
    double getInputDelay(const std::string& port_name) const;

    /**
     * @brief Get output delay for a port
     * @details Returns default_output_delay_ if not set for specific port
     * @param port_name Port name
     * @return Output delay value (ns)
     */
    double getOutputDelay(const std::string& port_name) const;

    /**
     * @brief Get default input delay
     * @return Default input delay value (ns)
     */
    double getDefaultInputDelay() const { return default_input_delay_; }

    /**
     * @brief Get default output delay
     * @return Default output delay value (ns)
     */
    double getDefaultOutputDelay() const { return default_output_delay_; }

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

    // ============ Industrial-grade Constraints ============

    /**
     * @brief Set clock uncertainty (jitter + skew margin)
     * @details Setup check subtracts this, Hold check adds this
     * @param uncertainty Clock uncertainty value (ns)
     */
    void setClockUncertainty(double uncertainty) { clock_uncertainty_ = uncertainty; }

    /**
     * @brief Get clock uncertainty
     * @return Clock uncertainty value (ns)
     */
    double getClockUncertainty() const { return clock_uncertainty_; }

    /**
     * @brief Set maximum transition time (DRC constraint)
     * @details SDC command: set_max_transition <value> [get_ports <port>]
     * @param max_trans Maximum transition time (ns)
     */
    void setMaxTransition(double max_trans) { max_transition_ = max_trans; }

    /**
     * @brief Get maximum transition time
     * @return Maximum transition time (ns)
     */
    double getMaxTransition() const { return max_transition_; }

private:
    std::vector<std::unique_ptr<Clock>> clocks_;  ///< Clock storage
    std::unordered_map<std::string, double> input_delays_;   ///< Port name -> Input delay
    std::unordered_map<std::string, double> output_delays_;  ///< Port name -> Output delay
    
    // Industrial-grade timing constraints
    double clock_uncertainty_ = 0.05;     ///< Clock uncertainty (jitter + skew margin) in ns
    double max_transition_ = 0.5;         ///< Maximum transition time constraint in ns
    double default_input_delay_ = 0.0;    ///< Default input delay for all PI ports in ns
    double default_output_delay_ = 0.0;   ///< Default output delay for all PO ports in ns
};

} // namespace mini

#endif // MINI_TIMING_CONSTRAINTS_H
