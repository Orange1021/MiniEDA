/**
 * @file timing_path.h
 * @brief Timing Graph Infrastructure - Node and Arc Definitions
 * @details Defines TimingNode and TimingArc classes for Static Timing Analysis (STA)
 */

#ifndef MINI_TIMING_PATH_H
#define MINI_TIMING_PATH_H

#include <string>
#include <vector>

namespace mini {

// Forward declarations
class Pin;
class TimingNode;

/**
 * @brief Timing Arc Type Enumeration
 * @details Defines the type of timing relationship between two nodes
 */
enum class TimingArcType {
    CELL_ARC,   ///< Cell delay arc (Input Pin -> Output Pin)
    NET_ARC     ///< Net delay arc (Driver Pin -> Load Pin)
};

/**
 * @class TimingArc
 * @brief Represents a directed edge in the timing graph (Delay path)
 * @details TimingArc connects two TimingNodes and represents delay propagation
 */
class TimingArc {
public:
    /**
     * @brief Constructor
     * @param type Type of timing arc (CELL_ARC or NET_ARC)
     * @param from Source TimingNode
     * @param to Destination TimingNode
     * @param delay Initial delay value
     */
    TimingArc(TimingArcType type, TimingNode* from, TimingNode* to, double delay = 0.0);

    // ============ Basic Accessors ============
    TimingArcType getType() const { return type_; }
    TimingNode* getFromNode() const { return from_node_; }
    TimingNode* getToNode() const { return to_node_; }
    double getDelay() const { return delay_; }
    double getOutputSlew() const { return output_slew_; }

    // ============ Modifiers ============
    void setDelay(double delay) { delay_ = delay; }
    void setOutputSlew(double slew) { output_slew_ = slew; }
    void setFromNode(TimingNode* node) { from_node_ = node; }
    void setToNode(TimingNode* node) { to_node_ = node; }

    // ============ Utility Methods ============
    /**
     * @brief Calculate delay for this arc
     * @details This will be extended later to use DelayModel
     * @return Calculated delay value
     */
    double calculateDelay();

    /**
     * @brief Get string representation of arc type
     * @return Type name string
     */
    std::string getTypeString() const;

private:
    TimingArcType type_;        ///< Arc type (CELL_ARC or NET_ARC)
    TimingNode* from_node_;     ///< Source node
    TimingNode* to_node_;       ///< Destination node
    double delay_;              ///< Calculated delay value
    double output_slew_;        ///< Output slew from this arc (for CELL_ARC)
};

/**
 * @class TimingNode
 * @brief Represents a node in the timing graph (STA evaluation point)
 * @details TimingNode corresponds to a Pin in physical netlist, stores transient timing data
 * @note Supports Min/Max dual-rail analysis for Setup (Max) and Hold (Min) checks
 */
class TimingNode {
public:
    static constexpr double UNINITIALIZED = 1e30;   ///< Sentinel value

    /**
     * @brief Constructor
     * @param pin Pointer to corresponding physical pin in NetlistDB (anchor point)
     */
    explicit TimingNode(Pin* pin);

    /**
     * @brief Destructor
     */
    ~TimingNode();

    // ============ Basic Accessors ============
    Pin* getPin() const { return pin_; }
    double getSlew() const { return slew_; }

    // ============ Min/Max Timing Accessors (Setup/Hold) ============
    // Arrival Time (AT): when signal arrives
    double getArrivalTimeMax() const { return at_max_; }
    double getArrivalTimeMin() const { return at_min_; }

    // Required Time (RAT): when signal must arrive
    double getRequiredTimeMax() const { return rat_max_; }
    double getRequiredTimeMin() const { return rat_min_; }

    // Slack timing checks
    double getSlackSetup() const { return slack_setup_; }
    double getSlackHold() const { return slack_hold_; }

    // ============ Legacy getters (backward compatibility) ============
    // These map to Max values (Setup analysis)
    double getArrivalTime() const { return at_max_; }
    double getRequiredTime() const { return rat_max_; }
    double getSlack() const { return slack_setup_; }

    // ============ Connection Accessors ============
    const std::vector<TimingArc*>& getIncomingArcs() const { return incoming_arcs_; }
    const std::vector<TimingArc*>& getOutgoingArcs() const { return outgoing_arcs_; }

    // ============ Modifiers ============
    void setArrivalTimeMax(double at) { at_max_ = at; }
    void setArrivalTimeMin(double at) { at_min_ = at; }
    void setRequiredTimeMax(double rat) { rat_max_ = rat; }
    void setRequiredTimeMin(double rat) { rat_min_ = rat; }
    void setSlackSetup(double slack) { slack_setup_ = slack; }
    void setSlackHold(double slack) { slack_hold_ = slack; }
    void setSlew(double slew) { slew_ = slew; }
    void addIncomingArc(TimingArc* arc) { incoming_arcs_.push_back(arc); }
    void addOutgoingArc(TimingArc* arc) { outgoing_arcs_.push_back(arc); }

    // ============ Legacy setters (backward compatibility) ============
    // These map to Max values (Setup analysis)
    void setArrivalTime(double at) { at_max_ = at; }
    void setRequiredTime(double rat) { rat_max_ = rat; }
    void setSlack(double slack) { slack_setup_ = slack; }

    // ============ Core Methods ============
    /**
     * @brief Reset timing data to uninitialized state
     * @details Resets AT/RAT/Slack/Slew for new analysis round
     */
    void reset();

    /**
     * @brief Check if this node is a clock source
     * @return true if connected to clock net, false otherwise
     */
    bool isClockSource() const;

    /**
     * @brief Get node name derived from pin name
     * @return Node name string
     */
    std::string getName() const;

    /**
     * @brief Get fanout count (number of outgoing arcs)
     */
    size_t getFanout() const { return outgoing_arcs_.size(); }

    /**
     * @brief Get fanin count (number of incoming arcs)
     */
    size_t getFanin() const { return incoming_arcs_.size(); }

private:
    Pin* pin_;                              ///< Anchor pointer to physical pin in NetlistDB

    // ============ Min/Max Timing Data (Setup/Hold) ============
    double at_max_;                         ///< Arrival Time (Max): for Setup analysis
    double at_min_;                         ///< Arrival Time (Min): for Hold analysis
    double rat_max_;                        ///< Required Time (Max): for Setup analysis
    double rat_min_;                        ///< Required Time (Min): for Hold analysis
    double slack_setup_;                    ///< Setup slack: rat_max - at_max
    double slack_hold_;                     ///< Hold slack: at_min - rat_min (hold check)
    double slew_;                           ///< Signal transition slew rate

    std::vector<TimingArc*> incoming_arcs_; ///< Incoming edges (predecessors)
    std::vector<TimingArc*> outgoing_arcs_; ///< Outgoing edges (successors)
};

} // namespace mini

#endif // MINI_TIMING_PATH_H
