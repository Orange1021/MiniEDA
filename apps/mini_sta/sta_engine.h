/**
 * @file sta_engine.h
 * @brief Static Timing Analysis Engine
 * @details Core engine to drive timing propagation and reporting
 */

#ifndef MINI_STA_ENGINE_H
#define MINI_STA_ENGINE_H

#include <vector>
#include <memory>
#include <string>

namespace mini {

// Forward declarations
class TimingGraph;
class DelayModel;
class TimingNode;
class NetlistDB;
class TimingConstraints;

/**
 * @class STAEngine
 * @brief Static Timing Analysis Engine
 * @details High-level controller responsible for driving complete STA flow:
 * Delay Calculation → AT Propagation → RAT Propagation → Slack Calculation
 */
class STAEngine {
public:
    /**
     * @brief Constructor
     * @param graph Pointer to the timing graph (must be valid)
     * @param delay_model Pointer to the delay model (must be valid)
     */
    STAEngine(TimingGraph* graph, std::shared_ptr<DelayModel> delay_model);

    // ============ Core Analysis Flow ============

    /**
     * @brief Run complete STA flow
     * @details Execution sequence:
     * 1. Update Arc Delays (update delay for all arcs)
     * 2. Update Arrival Times (forward AT propagation)
     * 3. Update Required Times (backward RAT propagation)
     * 4. Update Slacks (calculate timing slack)
     */
    void run();

    /**
     * @brief Set timing constraints for analysis
     * @param constraints Pointer to timing constraints (must be valid)
     */
    void setConstraints(TimingConstraints* constraints);

    // ============ Reporting Methods ============

    /**
     * @brief Report timing summary
     * @details Prints WNS (Worst Negative Slack) and TNS (Total Negative Slack)
     * @note WNS: Worst Negative Slack (worst timing violation)
     * @note TNS: Total Negative Slack (sum of all violations)
     */
    void reportSummary() const;

    /**
     * @brief Dump complete graph delay information (for debug)
     */
    void dumpGraph() const;

private:
    TimingGraph* graph_;                      ///< Pointer to timing graph (non-owning)
    std::shared_ptr<DelayModel> delay_model_; ///< Shared ownership of delay model
    TimingConstraints* constraints_ = nullptr; ///< Pointer to timing constraints (non-owning)

    // ============ Internal Algorithm Steps ============

    /**
     * @brief Update delay values for all arcs
     * @details Iterate all TimingArcs, call DelayModel to calculate and fill in delays
     */
    void updateArcDelays();

    /**
     * @brief Forward propagation to calculate Arrival Time (AT)
     * @details Traverse in topological order:
     * - Set start point AT (typically 0)
     * - For each node, apply formula: AT(sink) = max(AT(source) + Delay(arc))
     */
    void updateArrivalTimes();

    /**
     * @brief Backward propagation to calculate Required Time (RAT)
     * @details Traverse in reverse topological order:
     * - Set end point RAT (typically clock_period)
     * - For each node, apply formula: RAT(source) = min(RAT(sink) - Delay(arc))
     */
    void updateRequiredTimes();

    /**
     * @brief Calculate timing slack for all nodes
     * @details Slack = RAT - AT (Required Time - Arrival Time)
     * - Slack > 0: Satisfies timing requirement
     * - Slack = 0: On critical path
     * - Slack < 0: Timing violation
     */
    void updateSlacks();
};

} // namespace mini

#endif // MINI_STA_ENGINE_H
