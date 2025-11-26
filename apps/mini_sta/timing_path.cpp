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
TimingArc::TimingArc(TimingArcType type, TimingNode* from, TimingNode* to, double delay)
    : type_(type),
      from_node_(from),
      to_node_(to),
      delay_(delay) {
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
      arrival_time_(-UNINITIALIZED),   // Initialize to negative infinity (for max calculation)
      required_time_(UNINITIALIZED),    // Initialize to positive infinity (for min calculation)
      slack_(0.0),
      slew_(0.0) {
}

/**
 * @brief Destructor
 */
TimingNode::~TimingNode() = default;

/**
 * @brief Reset timing data to uninitialized state
 * @details Clears arrival_time, required_time, slack, and slew for new analysis
 * @note arrival_time initialized to -INF (for max propagation)
 *       required_time initialized to +INF (for min propagation)
 */
void TimingNode::reset() {
    arrival_time_ = -UNINITIALIZED;   // Negative infinity: start very low for max calculation
    required_time_ = UNINITIALIZED;    // Positive infinity: start very high for min calculation
    slack_ = 0.0;
    slew_ = 0.0;
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
