/**
 * @file timing_checks.h
 * @brief Timing Check Calculator
 * @details Encapsulates Setup and Hold timing check formulas for STA
 * This class provides static methods to calculate timing slacks.
 */

#ifndef MINI_TIMING_CHECKS_H
#define MINI_TIMING_CHECKS_H

#include <string>

namespace mini {

// Forward declaration
class TimingNode;

/**
 * @enum CheckType
 * @brief Type of timing check
 */
enum class CheckType {
    SETUP,  ///< Setup check (Max path analysis)
    HOLD    ///< Hold check (Min path analysis)
};

/**
 * @class TimingChecker
 * @brief Static utility class for timing check calculations
 * @details Calculates Setup and Hold slacks for timing nodes
 */
class TimingChecker {
public:
    /**
     * @brief Calculate Setup Slack
     * @details Formula: Slack = Data Required Time (Max) - Data Arrival Time (Max)
     *          Positive slack means signal arrives early enough
     *          Negative slack means signal is too slow
     * @param node Pointer to timing node
     * @return Setup slack value (ns)
     */
    static double calculateSetupSlack(const TimingNode* node);

    /**
     * @brief Calculate Hold Slack
     * @details Formula: Slack = Data Arrival Time (Min) - Data Required Time (Min)
     *          Positive slack means signal doesn't arrive too early
     *          Negative slack means signal is too fast (race condition)
     * @note For v1.0, assumes Hold Required Time (Min) is 0.0 for endpoints
     * @param node Pointer to timing node
     * @return Hold slack value (ns)
     */
    static double calculateHoldSlack(const TimingNode* node);

    /**
     * @brief Get check type name as string
     * @param type Check type
     * @return Name string
     */
    static std::string getCheckName(CheckType type);
};

} // namespace mini

#endif // MINI_TIMING_CHECKS_H
