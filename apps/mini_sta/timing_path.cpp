/**
 * @file timing_path.cpp
 * @brief TimingNode and TimingArc Implementation
 * @details Implementation of timing graph node and arc methods
 */

#include "timing_path.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"

namespace mini {

// ============================================================================
// TimingArc Implementation
// ============================================================================

/**
 * @brief Constructor
 */
TimingArc::TimingArc(TimingArcType type, TimingNode* from, TimingNode* to, double delay, const LibTiming* lib_timing)
    : type_(type),
      from_node_(from),
      to_node_(to),
      delay_(delay),
      output_slew_(0.0),
      output_slew_min_(0.0),
      lib_timing_(lib_timing) {
}

/**
 * @brief Calculate delay for this arc
 * @details Currently returns stored delay value. In future, this will use DelayModel
 * @return Delay value
 */
double TimingArc::calculateDelay() {
    // For now, just return the stored delay value
    // Future enhancement: integrate with DelayModel for dynamic calculation
    return delay_;
}

/**
 * @brief Get string representation of arc type
 */
std::string TimingArc::getTypeString() const {
    switch (type_) {
        case TimingArcType::CELL_ARC:
            return "CELL_ARC";
        case TimingArcType::NET_ARC:
            return "NET_ARC";
        default:
            return "UNKNOWN";
    }
}

// ============================================================================
// TimingNode Implementation
// ============================================================================

/**
 * @brief Constructor
 */
TimingNode::TimingNode(Pin* pin)
    : pin_(pin),
      at_max_(-UNINITIALIZED),       // Initialize to negative infinity (for max calculation)
      at_min_(UNINITIALIZED),         // Initialize to positive infinity (for min calculation)
      rat_max_(UNINITIALIZED),        // Initialize to positive infinity (for min calculation)
      rat_min_(-UNINITIALIZED),       // Initialize to negative infinity (for max calculation)
      slack_setup_(0.0),
      slack_hold_(0.0),
      slew_(0.0),
      pin_capacitance_(0.0) {
}

/**
 * @brief Destructor
 */
TimingNode::~TimingNode() = default;

/**
 * @brief Reset timing data to uninitialized state
 * @details Initializes:
 *       at_max initialized to -INF (for max propagation)
 *       at_min initialized to +INF (for min propagation)
 *       rat_max initialized to +INF (for min propagation - RAT)
 *       rat_min initialized to -INF (for max propagation - Hold RAT)
 *       slew_max/slew_min initialized to 0 (will be set during propagation)
 */
void TimingNode::reset() {
    at_max_ = -UNINITIALIZED;          // Negative infinity: start very low for max calculation
    at_min_ = UNINITIALIZED;            // Positive infinity: start very high for min calculation
    rat_max_ = UNINITIALIZED;           // Positive infinity: start very high for min calculation (Setup RAT)
    rat_min_ = -UNINITIALIZED;          // Negative infinity: start very low for max calculation (Hold RAT)
    slack_setup_ = 0.0;
    slack_hold_ = 0.0;
    slew_max_ = 0.0;                   // Setup path slew (will be set during propagation)
    slew_min_ = 0.0;                   // Hold path slew (will be set during propagation)
}

/**
 * @brief Check if this node is a clock source
 * @details Checks if the connected net is marked as clock
 * @return true if connected to clock net, false otherwise
 */
bool TimingNode::isClockSource() const {
    if (!pin_) {
        return false;
    }

    // Get the net connected to this pin
    Net* net = pin_->getNet();
    if (!net) {
        return false;
    }

    // Check if the net is marked as clock
    return net->isClock();
}

/**
 * @brief Get node name (full hierarchical name: CellName/PinName)
 * @return Node name string (e.g., "U1/A", "U2/Q")
 * @details This provides globally unique identification for debugging
 */
std::string TimingNode::getName() const {
    if (pin_) {
        if (Cell* owner = pin_->getOwner()) {
            return owner->getName() + "/" + pin_->getName();
        }
        return pin_->getName();  // For orphan pins (shouldn't happen normally)
    }
    return "UNKNOWN_NODE";
}

} // namespace mini
