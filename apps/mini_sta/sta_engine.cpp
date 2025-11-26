/**
 * @file sta_engine.cpp
 * @brief STA Engine Implementation
 * @details Core algorithms for Static Timing Analysis
 */

#include "sta_engine.h"
#include "timing_graph.h"
#include "timing_path.h"
#include "delay_model.h"
#include "timing_constraints.h"
#include "timing_checks.h"
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
      constraints_(nullptr) {
}

/**
 * @brief Set timing constraints for analysis
 */
void STAEngine::setConstraints(TimingConstraints* constraints) {
    constraints_ = constraints;
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
            // CELL_ARC: Calculate cell delay with NLDM
            TimingNode* from_node = arc->getFromNode();
            TimingNode* to_node = arc->getToNode();

            if (from_node && to_node) {
                Pin* to_pin = to_node->getPin();
                if (to_pin) {
                    Cell* cell = to_pin->getOwner();
                    if (cell) {
                        // Get input slew from previous node (this is the key!)
                        double input_slew = from_node->getSlew();
                        if (input_slew <= 0.0) {
                            input_slew = 0.05 * 1e-9;  // Default: 50ps if not set
                        }

                        // Calculate load capacitance
                        double load_cap = 0.0;
                        
                        // Sum capacitance of all fanout pins connected to this output
                        for (TimingArc* fanout_arc : to_node->getOutgoingArcs()) {
                            TimingNode* fanout_node = fanout_arc->getToNode();
                            if (fanout_node && fanout_node->getPin()) {
                                Pin* fanout_pin = fanout_node->getPin();
                                // Add pin capacitance (for now, use a simplified estimate)
                                // In a real implementation, this would come from Liberty library
                                load_cap += 0.002 * 1e-12;  // 2fF per pin (typical)
                            }
                        }
                        
                        // Add wire capacitance (simplified)
                        load_cap += 0.001 * 1e-12;  // 1fF wire capacitance
                        
                        // Ensure minimum load capacitance
                        if (load_cap < 0.001 * 1e-12) {
                            load_cap = 0.001 * 1e-12;  // 1fF minimum
                        }

                        // Calculate delay using NLDM lookup
                        delay = delay_model_->calculateCellDelay(cell, input_slew, load_cap);
                        
                        // Calculate output slew (this is crucial for next stage!)
                        double output_slew = delay_model_->calculateOutputSlew(cell, input_slew, load_cap);
                        
                        // Store the output slew in the arc for later use
                        arc->setOutputSlew(output_slew);
                        
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

    // Set start points AT (use input delays from constraints if available)
    for (TimingNode* start : start_points) {
        double at_value = 0.0;

        // Check if constraints are available and node is a primary input
        if (constraints_ && start->getPin()) {
            Cell* owner = start->getPin()->getOwner();
            if (owner && owner->getType() == CellType::INPUT) {
                // For input ports, use input delay constraint
                std::string port_name = owner->getName();
                double input_delay = constraints_->getInputDelay(port_name);
                at_value = input_delay;

                if (input_delay > 0.0) {
                    std::cout << "    Set AT of " << start->getName()
                             << " = " << at_value << " ns (input delay)" << std::endl;
                }
            }
        }

        start->setArrivalTime(at_value);
        start->setSlew(0.0);  // Ideal waveform

        if (at_value == 0.0) {
            std::cout << "    Set AT of " << start->getName() << " = 0.0" << std::endl;
        }
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

        // ==================== Setup Analysis (Max Path) ====================
        // Initialize to negative infinity (looking for maximum)
        double max_arrival = -std::numeric_limits<double>::infinity();

        // ==================== Hold Analysis (Min Path) ====================
        // Initialize to positive infinity (looking for minimum)
        double min_arrival = std::numeric_limits<double>::infinity();

        // Check all incoming arcs
        for (TimingArc* arc : incoming_arcs) {
            TimingNode* prev_node = arc->getFromNode();
            if (!prev_node) {
                std::cerr << "Warning: Arc has null from_node!" << std::endl;
                continue;
            }

            // ==================== Max Path (Setup) ====================
            double prev_at_max = prev_node->getArrivalTimeMax();

            // Skip if predecessor AT_max is uninitialized (negative infinity)
            if (prev_at_max > -TimingNode::UNINITIALIZED / 2) {
                double path_delay_max = prev_at_max + arc->getDelay();
                max_arrival = std::max(max_arrival, path_delay_max);
            }

            // ==================== Min Path (Hold) ====================
            double prev_at_min = prev_node->getArrivalTimeMin();

            // Skip if predecessor AT_min is uninitialized (positive infinity)
            if (prev_at_min < TimingNode::UNINITIALIZED / 2) {
                double path_delay_min = prev_at_min + arc->getDelay();
                min_arrival = std::min(min_arrival, path_delay_min);
            }
        }

        // Set AT if we found valid paths
        bool updated = false;
        double best_slew = 0.0;  // Slew from the best (critical) path

        // ==================== Setup (Max) ====================
        if (max_arrival > -std::numeric_limits<double>::infinity() / 2) {
            current_node->setArrivalTimeMax(max_arrival);
            updated = true;
            
            // Find the arc that gave us the max arrival (critical path)
            for (TimingArc* arc : incoming_arcs) {
                TimingNode* prev_node = arc->getFromNode();
                if (!prev_node) continue;
                
                double prev_at_max = prev_node->getArrivalTimeMax();
                if (prev_at_max > -TimingNode::UNINITIALIZED / 2) {
                    double path_delay = prev_at_max + arc->getDelay();
                    if (std::abs(path_delay - max_arrival) < 1e-15) {  // Found the critical path
                        best_slew = arc->getOutputSlew();
                        if (best_slew <= 0.0) {
                            // Fallback: use previous node's slew
                            best_slew = prev_node->getSlew();
                            if (best_slew <= 0.0) {
                                best_slew = 0.05 * 1e-9;  // Default: 50ps
                            }
                        }
                        break;
                    }
                }
            }
        }

        // ==================== Hold (Min) ====================
        if (min_arrival < std::numeric_limits<double>::infinity() / 2) {
            current_node->setArrivalTimeMin(min_arrival);
            updated = true;
        }

        // Propagate slew if we found valid paths
        if (updated) {
            current_node->setSlew(best_slew);
            updated_count++;

            // Debug print first few nodes
            if (updated_count <= 5) {
                std::cout << "    Propagated AT of " << current_node->getName()
                         << " [Setup=" << max_arrival << "ns, Hold=" << min_arrival << "ns, Slew=" 
                         << (best_slew * 1e9) << "ps]" << std::endl;
            }
        }
    }

    std::cout << "  Updated AT (Setup) for " << updated_count << " nodes." << std::endl;
    std::cout << "  Updated AT (Hold) for " << updated_count << " nodes." << std::endl;
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

    // Get clock period from constraints or use default
    double clock_period = 10.0;  // Default
    if (constraints_) {
        clock_period = constraints_->getMainClockPeriod();
    }

    // Set end points RAT (use output delays from constraints if available)
    for (TimingNode* end : end_points) {
        double rat_value = clock_period;

        // Check if constraints are available and node is a primary output
        if (constraints_ && end->getPin()) {
            Cell* owner = end->getPin()->getOwner();
            if (owner && owner->getType() == CellType::OUTPUT) {
                // For output ports, subtract output delay from clock period
                std::string port_name = owner->getName();
                double output_delay = constraints_->getOutputDelay(port_name);
                rat_value = clock_period - output_delay;

                if (output_delay > 0.0) {
                    std::cout << "    Set RAT of " << end->getName()
                             << " = " << rat_value << " ns (clock period - output delay)" << std::endl;
                }
            }
        }

        end->setRequiredTime(rat_value);

        if (rat_value == clock_period) {
            std::cout << "    Set RAT of " << end->getName() << " = " << rat_value << " ns" << std::endl;
        }
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
 *          for both Setup (Max path) and Hold (Min path) analysis
 */
void STAEngine::reportSummary() const {
    std::cout << "\n========== Timing Summary ==========" << std::endl;

    // Get clock period from constraints or use default
    double clock_period = 10.0;  // Default
    if (constraints_) {
        clock_period = constraints_->getMainClockPeriod();
    }
    std::cout << "Clock Period: " << clock_period << " ns" << std::endl;
    std::cout << std::endl;

    // ==================== Setup Analysis (Max Path) ====================
    double wns_setup = 0.0;        // Worst Negative Slack (Setup)
    double tns_setup = 0.0;        // Total Negative Slack (Setup)
    size_t setup_violations = 0;

    // ==================== Hold Analysis (Min Path) ====================
    double wns_hold = 0.0;         // Worst Negative Slack (Hold)
    double tns_hold = 0.0;         // Total Negative Slack (Hold)
    size_t hold_violations = 0;

    for (const auto& node : graph_->getNodes()) {
        // Setup (Max path)
        double slack_setup = node->getSlackSetup();

        // Skip uninitialized nodes
        if (slack_setup != 0.0 ||
            (node->getArrivalTimeMax() > -TimingNode::UNINITIALIZED / 2 &&
             node->getRequiredTimeMax() < TimingNode::UNINITIALIZED / 2)) {

            // Find worst Setup slack
            if (slack_setup < wns_setup) {
                wns_setup = slack_setup;
            }

            // Accumulate Setup negative slacks
            if (slack_setup < 0.0) {
                tns_setup += slack_setup;
                setup_violations++;
            }
        }

        // Hold (Min path)
        double slack_hold = node->getSlackHold();

        // Skip uninitialized nodes
        if (slack_hold != 0.0 ||
            (node->getArrivalTimeMin() < TimingNode::UNINITIALIZED / 2 &&
             node->getRequiredTimeMin() > -TimingNode::UNINITIALIZED / 2)) {

            // Find worst Hold slack (note: Hold violation is negative when too fast)
            if (slack_hold < wns_hold) {
                wns_hold = slack_hold;
            }

            // Accumulate Hold negative slacks
            if (slack_hold < 0.0) {
                tns_hold += slack_hold;
                hold_violations++;
            }
        }
    }

    // Report Setup Analysis results
    std::cout << "Setup Analysis (Max Path - Signal Too Slow):" << std::endl;
    std::cout << "  WNS (Worst Negative Slack): " << wns_setup << " ns";
    if (wns_setup < 0.0) {
        std::cout << " [VIOLATION]";
    }
    std::cout << std::endl;

    std::cout << "  TNS (Total Negative Slack): " << tns_setup << " ns" << std::endl;
    std::cout << "  Violation Count: " << setup_violations << " nodes" << std::endl;
    std::cout << std::endl;

    // Report Hold Analysis results
    std::cout << "Hold Analysis (Min Path - Signal Too Fast):" << std::endl;
    std::cout << "  WNS (Worst Negative Slack): " << wns_hold << " ns";
    if (wns_hold < 0.0) {
        std::cout << " [VIOLATION - RACE CONDITION]";
    }
    std::cout << std::endl;

    std::cout << "  TNS (Total Negative Slack): " << tns_hold << " ns" << std::endl;
    std::cout << "  Violation Count: " << hold_violations << " nodes" << std::endl;
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