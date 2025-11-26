/**
 * @file timing_checks.cpp
 * @brief Timing Check Implementation
 * @details Implementation of Setup and Hold timing check formulas
 */

#include "timing_checks.h"
#include "timing_path.h"
#include <limits>

namespace mini {

/**
 * @brief Calculate Setup Slack
 * @details Setup Slack = RAT_max - AT_max
 * Setup check ensures signal is not too slow
 */
double TimingChecker::calculateSetupSlack(const TimingNode* node) {
    if (!node) {
        return 0.0;
    }

    double at_max = node->getArrivalTimeMax();
    double rat_max = node->getRequiredTimeMax();

    // Check for uninitialized values
    if (at_max <= -TimingNode::UNINITIALIZED / 2 ||
        rat_max >= TimingNode::UNINITIALIZED / 2) {
        return 0.0;
    }

    // Setup Slack Formula: RAT_max - AT_max
    // Positive slack: meets timing
    // Negative slack: too slow (violation)
    return rat_max - at_max;
}

/**
 * @brief Calculate Hold Slack
 * @details Hold Slack = AT_min - RAT_min
 * Hold check ensures signal is not too fast
 * @note For v1.0, assumes RAT_min is 0.0 for endpoints (ideal hold requirement)
 */
double TimingChecker::calculateHoldSlack(const TimingNode* node) {
    if (!node) {
        return 0.0;
    }

    double at_min = node->getArrivalTimeMin();
    double rat_min = node->getRequiredTimeMin();

    // Check for uninitialized values
    if (at_min >= TimingNode::UNINITIALIZED / 2 ||
        rat_min <= -TimingNode::UNINITIALIZED / 2) {
        return 0.0;
    }

    // Hold Slack Formula: AT_min - RAT_min
    // Positive slack: no race condition
    // Negative slack: too fast (hold violation)
    // For v1.0, rat_min is typically 0.0 (immediate hold requirement after clock edge)
    return at_min - rat_min;
}

/**
 * @brief Get check type name as string
 */
std::string TimingChecker::getCheckName(CheckType type) {
    switch (type) {
        case CheckType::SETUP:
            return "SETUP";
        case CheckType::HOLD:
            return "HOLD";
        default:
            return "UNKNOWN";
    }
}

} // namespace mini
