/**
 * @file sta_engine.cpp
 * @brief STA Engine Implementation
 * @details Core algorithms for Static Timing Analysis
 */

#include "sta_engine.h"
#include "timing_graph.h"
#include "timing_path.h"
#include "delay_model.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"
#include <iostream>
#include <limits>
#include <algorithm>

namespace mini {

// ============================================================================
// Constructor & Destructor
// ============================================================================

/**
 * @brief Constructor
 */
STAEngine::STAEngine(TimingGraph* graph, std::shared_ptr<DelayModel> delay_model)
    : graph_(graph),
      delay_model_(std::move(delay_model)),
      clock_period_(10.0) {
}

// ============================================================================
// Core Analysis Flow
// ============================================================================

/**
 * @brief Run complete STA flow
 * @details Execution sequence:
 * 1. Reset timing data
 * 2. Update Arc Delays
 * 3. Update Arrival Times (forward propagation)
 * 4. Update Required Times (backward propagation)
 * 5. Update Slacks
 */
void STAEngine::run() {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "STA Engine: Running Analysis" << std::endl;
    std::cout << "==========================================" << std::endl;

    if (!graph_ || !delay_model_) {
        std::cerr << "Error: Null pointer to graph or delay model!" << std::endl;
        return;
    }

    // Step 0: Reset all timing data
    std::cout << "\n[Step 0] Resetting timing data..." << std::endl;
    graph_->resetTimingData();
    std::cout << "  Reset " << graph_->getNumNodes() << " nodes." << std::endl;

    // Step 1: Update arc delays
    std::cout << "\n[Step 1] Updating arc delays..." << std::endl;
    updateArcDelays();
    std::cout << "  Updated " << graph_->getNumArcs() << " arcs." << std::endl;

    // Step 2: Forward propagation - Calculate Arrival Times
    std::cout << "\n[Step 2] Propagating Arrival Times (Forward)..." << std::endl;
    updateArrivalTimes();

    // Step 3: Backward propagation - Calculate Required Times
    std::cout << "\n[Step 3] Propagating Required Times (Backward)..." << std::endl;
    updateRequiredTimes();

    // Step 4: Calculate Slacks
    std::cout << "\n[Step 4] Calculating Slacks..." << std::endl;
    updateSlacks();

    std::cout << "\n==========================================" << std::endl;
    std::cout << "STA Analysis Complete!" << std::endl;
    std::cout << "==========================================" << std::endl;
}

/**
 * @brief Set target clock period
 */
void STAEngine::setClockPeriod(double period_ns) {
    clock_period_ = period_ns;
    std::cout << "  Set clock period to " << period_ns << " ns" << std::endl;
}

// ============================================================================
// Delay Calculation
// ============================================================================

/**
 * @brief Update delay values for all arcs
 * @details Iterate all TimingArcs, call DelayModel to calculate and fill in delays
 */
void STAEngine::updateArcDelays() {
    size_t net_arc_count = 0;
    size_t cell_arc_count = 0;

    for (const auto& arc : graph_->getArcs()) {
        double delay = 0.0;

        if (arc->getType() == TimingArcType::NET_ARC) {
            // NET_ARC: Calculate wire delay
            TimingNode* to_node = arc->getToNode();
            if (to_node) {
                Pin* to_pin = to_node->getPin();
                if (to_pin) {
                    Net* net = to_pin->getNet();
                    if (net) {
                        delay = delay_model_->calculateWireDelay(net);
                        net_arc_count++;
                    }
                }
            }
        } else if (arc->getType() == TimingArcType::CELL_ARC) {
            // CELL_ARC: Calculate cell delay
            TimingNode* from_node = arc->getFromNode();
            TimingNode* to_node = arc->getToNode();

            if (from_node && to_node) {
                Pin* to_pin = to_node->getPin();
                if (to_pin) {
                    Cell* cell = to_pin->getOwner();
                    if (cell) {
                        // For first version, use simplified parameters
                        double input_slew = 0.0;  // Simplified: assume ideal slew
                        double load_cap = 0.0;    // Simplified: ignore load for now

                        delay = delay_model_->calculateCellDelay(cell, input_slew, load_cap);
                        cell_arc_count++;
                    }
                }
            }
        }

        arc->setDelay(delay);
    }

    std::cout << "  Net arcs: " << net_arc_count << std::endl;
    std::cout << "  Cell arcs: " << cell_arc_count << std::endl;
}

// ============================================================================
// Arrival Time Propagation (Forward)
// ============================================================================

/**
 * @brief Forward propagation to calculate Arrival Time (AT)
 * @details Traverse in topological order:
 * - Set start point AT (typically 0)
 * - For each node: AT(sink) = max(AT(source) + Delay(arc))
 */
void STAEngine::updateArrivalTimes() {
    // Get topological order
    std::vector<TimingNode*> sorted_nodes = graph_->topologicalSort();
    if (sorted_nodes.empty()) {
        std::cerr << "Error: Failed to get topological order!" << std::endl;
        return;
    }

    std::cout << "  Topological order length: " << sorted_nodes.size() << std::endl;

    // Get start points (PIs and DFF/Q)
    std::vector<TimingNode*> start_points = graph_->getStartPoints();
    std::cout << "  Start points: " << start_points.size() << std::endl;

    // Set start points AT to 0.0
    for (TimingNode* start : start_points) {
        start->setArrivalTime(0.0);
        start->setSlew(0.0);  // Ideal waveform
        std::cout << "    Set AT of " << start->getName() << " = 0.0" << std::endl;
    }

    // Propagate AT forward in topological order
    size_t updated_count = 0;
    for (TimingNode* current_node : sorted_nodes) {
        // Skip start points (already set)
        if (std::find(start_points.begin(), start_points.end(), current_node) != start_points.end()) {
            continue;
        }

        // Get incoming arcs (use direct access for debugging)
        const std::vector<TimingArc*>& incoming_arcs = current_node->getIncomingArcs();

        // If node has no incoming arcs, skip (this is expected for PIs)
        if (incoming_arcs.empty()) {
            continue;
        }

        // Initialize to negative infinity (looking for maximum)
        double max_arrival = -std::numeric_limits<double>::infinity();

        // Check all incoming arcs
        for (TimingArc* arc : incoming_arcs) {
            TimingNode* prev_node = arc->getFromNode();
            if (!prev_node) {
                std::cerr << "Warning: Arc has null from_node!" << std::endl;
                continue;
            }

            double prev_at = prev_node->getArrivalTime();

            // Skip if predecessor AT is uninitialized (negative infinity)
            if (prev_at <= -TimingNode::UNINITIALIZED / 2) {
                continue;
            }

            double path_delay = prev_at + arc->getDelay();
            max_arrival = std::max(max_arrival, path_delay);
        }

        // Set AT if we found a valid path
        if (max_arrival > -std::numeric_limits<double>::infinity() / 2) {
            current_node->setArrivalTime(max_arrival);
            updated_count++;

            // Debug print first few nodes
            if (updated_count <= 5) {
                std::cout << "    Propagated AT of " << current_node->getName()
                         << " = " << max_arrival << " ns" << std::endl;
            }
        }
    }

    std::cout << "  Updated AT for " << updated_count << " nodes." << std::endl;
}

// ============================================================================
// Required Time Propagation (Backward)
// ============================================================================

/**
 * @brief Backward propagation to calculate Required Time (RAT)
 * @details Traverse in reverse topological order:
 * - Set end point RAT (typically clock_period)
 * - For each node: RAT(source) = min(RAT(sink) - Delay(arc))
 */
void STAEngine::updateRequiredTimes() {
    // Get topological order
    std::vector<TimingNode*> sorted_nodes = graph_->topologicalSort();
    if (sorted_nodes.empty()) {
        std::cerr << "Error: Failed to get topological order!" << std::endl;
        return;
    }

    // Get end points (POs and DFF/D)
    std::vector<TimingNode*> end_points = graph_->getEndPoints();
    std::cout << "  End points: " << end_points.size() << std::endl;

    // Set end points RAT to clock period
    for (TimingNode* end : end_points) {
        end->setRequiredTime(clock_period_);
        std::cout << "    Set RAT of " << end->getName() << " = " << clock_period_ << " ns" << std::endl;
    }

    // Propagate RAT backward (reverse topological order)
    size_t updated_count = 0;
    for (auto it = sorted_nodes.rbegin(); it != sorted_nodes.rend(); ++it) {
        TimingNode* current_node = *it;

        // Skip end points (already set)
        if (std::find(end_points.begin(), end_points.end(), current_node) != end_points.end()) {
            continue;
        }

        // Initialize to positive infinity (looking for minimum)
        double min_required = std::numeric_limits<double>::infinity();

        // Check all outgoing arcs
        for (TimingArc* arc : graph_->getOutgoingArcs(current_node)) {
            TimingNode* next_node = arc->getToNode();
            if (!next_node) continue;

            double next_rat = next_node->getRequiredTime();

            // Skip if successor RAT is uninitialized (positive infinity)
            if (next_rat >= TimingNode::UNINITIALIZED / 2) {
                continue;
            }

            double required_time = next_rat - arc->getDelay();
            min_required = std::min(min_required, required_time);
        }

        // Set RAT if we found a valid path
        if (min_required < std::numeric_limits<double>::infinity() / 2) {
            current_node->setRequiredTime(min_required);
            updated_count++;

            // Debug print first few nodes
            if (updated_count <= 5) {
                std::cout << "    Propagated RAT of " << current_node->getName()
                         << " = " << min_required << " ns" << std::endl;
            }
        }
    }

    std::cout << "  Updated RAT for " << updated_count << " nodes." << std::endl;
}

// ============================================================================
// Slack Calculation
// ============================================================================

/**
 * @brief Calculate timing slack for all nodes
 * @details Slack = RAT - AT (Required Time - Arrival Time)
 * - Slack > 0: Meets timing requirement
 * - Slack = 0: On critical path
 * - Slack < 0: Timing violation
 */
void STAEngine::updateSlacks() {
    size_t positive_slack = 0;
    size_t zero_slack = 0;
    size_t negative_slack = 0;

    for (const auto& node : graph_->getNodes()) {
        double at = node->getArrivalTime();
        double rat = node->getRequiredTime();

        // Skip uninitialized nodes
        if (at <= -TimingNode::UNINITIALIZED / 2 || rat >= TimingNode::UNINITIALIZED / 2) {
            continue;
        }

        double slack = rat - at;
        node->setSlack(slack);

        // Count slack categories
        if (slack > 0.0) {
            positive_slack++;
        } else if (slack == 0.0) {
            zero_slack++;
        } else {
            negative_slack++;
        }
    }

    std::cout << "  Slack distribution:" << std::endl;
    std::cout << "    Positive (\u003e 0): " << positive_slack << " nodes" << std::endl;
    std::cout << "    Zero (= 0):       " << zero_slack << " nodes (critical path)" << std::endl;
    std::cout << "    Negative (\u003c 0):  " << negative_slack << " nodes (timing violations)" << std::endl;
}

// ============================================================================
// Reporting Methods
// ============================================================================

/**
 * @brief Report timing summary
 * @details Prints WNS (Worst Negative Slack) and TNS (Total Negative Slack)
 */
void STAEngine::reportSummary() const {
    std::cout << "\n========== Timing Summary ==========" << std::endl;
    std::cout << "Clock Period: " << clock_period_ << " ns" << std::endl;

    double wns = 0.0;  // Worst Negative Slack
    double tns = 0.0;  // Total Negative Slack
    size_t violation_count = 0;

    for (const auto& node : graph_->getNodes()) {
        double slack = node->getSlack();

        // Skip uninitialized nodes
        if (slack == 0.0) {
            // Check if uninitialized (both AT and RAT at extreme values)
            if (node->getArrivalTime() <= -TimingNode::UNINITIALIZED / 2 ||
                node->getRequiredTime() >= TimingNode::UNINITIALIZED / 2) {
                continue;
            }
        }

        // Find worst (most negative) slack
        if (slack < wns) {
            wns = slack;
        }

        // Accumulate negative slacks
        if (slack < 0.0) {
            tns += slack;
            violation_count++;
        }
    }

    std::cout << "WNS (Worst Negative Slack): " << wns << " ns";
    if (wns < 0.0) {
        std::cout << " [VIOLATION]";
    }
    std::cout << std::endl;

    std::cout << "TNS (Total Negative Slack): " << tns << " ns" << std::endl;
    std::cout << "Violation Count: " << violation_count << " nodes" << std::endl;
    std::cout << "=====================================" << std::endl;
}

/**
 * @brief Dump complete graph delay information (for debug)
 */
void STAEngine::dumpGraph() const {
    std::cout << "\n========== Graph Dump ==========" << std::endl;
    std::cout << "Nodes: " << graph_->getNumNodes() << std::endl;
    std::cout << "Arcs: " << graph_->getNumArcs() << std::endl;

    std::cout << "\n--- Timing Points ---" << std::endl;
    for (const auto& node : graph_->getNodes()) {
        Pin* pin = node->getPin();
        if (!pin || !pin->getOwner()) continue;

        double at = node->getArrivalTime();
        double rat = node->getRequiredTime();
        double slack = node->getSlack();

        // Only print initialized nodes
        if (at > -TimingNode::UNINITIALIZED / 2 && rat < TimingNode::UNINITIALIZED / 2) {
            std::cout << "  " << node->getName()
                     << "  AT=" << at
                     << "  RAT=" << rat
                     << "  Slack=" << slack
                     << std::endl;
        }
    }

    std::cout << "=================================" << std::endl;
}

} // namespace mini
