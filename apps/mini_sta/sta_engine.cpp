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
#include "../../lib/include/liberty.h"
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
      constraints_(nullptr),
      wire_resistance_per_unit_(0.005),  // Default: 5 Ω/μm = 0.005 kΩ/μm
      wire_cap_per_unit_(0.0002),        // Default: 0.2 fF/μm = 0.0002 pF/μm
      default_input_slew_max_(0.05e-9),  // Default: 50ps
      default_input_slew_min_(0.01e-9)   // Default: 10ps
{
}

void STAEngine::setPhysicalParameters(double wire_resistance_per_unit, double wire_cap_per_unit,
                                     double default_input_slew_max, double default_input_slew_min) {
    wire_resistance_per_unit_ = wire_resistance_per_unit;
    wire_cap_per_unit_ = wire_cap_per_unit;
    default_input_slew_max_ = default_input_slew_max;
    default_input_slew_min_ = default_input_slew_min;
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
            // NET_ARC: Calculate point-to-point interconnect delay with Slew degradation
            TimingNode* from_node = arc->getFromNode();
            TimingNode* to_node = arc->getToNode();
            
            if (from_node && to_node) {
                Pin* driver_pin = from_node->getPin();
                Pin* load_pin = to_node->getPin();
                
                if (driver_pin && load_pin) {
                    // Get input slew from driver (both min and max)
                    double input_slew_max = getValidInputSlew(from_node->getSlewMax(), true);
                    double input_slew_min = getValidInputSlew(from_node->getSlewMin(), false);

                    // Physical parameters from configuration
                    double wire_r_per_unit = wire_resistance_per_unit_;
                    double wire_c_per_unit = wire_cap_per_unit_;
                    
                    // Calculate Elmore delay and slew degradation for max path
                    auto [interconnect_delay_max, output_slew_max] = delay_model_->calculateInterconnectDelay(
                        driver_pin, load_pin, input_slew_max, wire_r_per_unit, wire_c_per_unit);
                    
                    // Calculate Elmore delay and slew degradation for min path
                    // Note: Min path values are calculated but not used in current implementation
                    // TODO: Add setOutputSlewMin() to TimingArc to support min slew
                    [[maybe_unused]] auto [interconnect_delay_min, output_slew_min] = delay_model_->calculateInterconnectDelay(
                        driver_pin, load_pin, input_slew_min, wire_r_per_unit, wire_c_per_unit);
                    
                    // For now, use max delay (both paths share same physical delay)
                    delay = interconnect_delay_max;
                    
                    // Store both slew values in the arc (we need to extend TimingArc to support this)
                    arc->setOutputSlew(output_slew_max);  // Legacy: store max slew
                    // TODO: Add setOutputSlewMin() to TimingArc
                    net_arc_count++;
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
                        double input_slew = getValidInputSlew(from_node->getSlew(), true);

                        // Calculate realistic load capacitance (Pin Cap + Wire Cap)
                        // Uses Liberty library for pin capacitance and physical wire length
                        double wire_cap_per_unit = wire_cap_per_unit_;  // Use configured value
                        double load_cap = calculateNetLoadCapacitance(to_node, wire_cap_per_unit);

                        // [FIXED] Calculate delay using correct timing arc from the graph
                        double output_slew = default_input_slew_min_;  // Use configured default
                        
                        if (arc->getLibTiming()) {
                            // Use the pre-bound LibTiming pointer - O(1) access!
                            const LibTiming* lib_timing = arc->getLibTiming();
                            
                            // Determine input edge direction (simplified: assume positive for now)
                            SignalEdge input_edge = SignalEdge::RISE;
                            
                            // Determine output edge based on timing sense
                            TableDelayModel* table_model = dynamic_cast<TableDelayModel*>(delay_model_.get());
                            if (table_model) {
                                SignalEdge output_edge = table_model->determineOutputEdge(
                                    lib_timing->timing_sense, input_edge);
                                
                                // Select correct delay table based on output edge
                                const LookupTable* delay_table = nullptr;
                                if (output_edge == SignalEdge::RISE) {
                                    delay_table = &lib_timing->cell_delay;  // Legacy: use cell_delay for rise
                                } else {
                                    // For fall edge, we need cell_fall table (if available)
                                    // Note: Some libraries only provide cell_delay, use it as fallback
                                    delay_table = &lib_timing->cell_delay;  // Fallback to cell_delay
                                }
                                
                                if (delay_table && delay_table->isValid()) {
                                    delay = delay_table->lookup(input_slew, load_cap) * 1e-9;
                                } else {
                                    delay = default_input_slew_min_;  // Use configured default
                                }
                                
                                // Calculate output slew using the correct NLDM table
                                output_slew = table_model->calculateOutputSlewWithEdge(
                                    lib_timing, input_slew, load_cap, output_edge);
                            } else {
                                delay = default_input_slew_min_;  // Use configured default
                            }
                        } else {
                            // Fallback for NET_ARC or missing Liberty data
                            delay = delay_model_->calculateCellDelay(cell, input_slew, load_cap);
                            output_slew = delay_model_->calculateOutputSlew(cell, input_slew, load_cap);
                        }
                        
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
                
                // If no specific delay for this port, use default (from AppConfig)
                if (input_delay == 0.0) {
                    // Get default from constraints (will be extended to store default values)
                    // For now, we use 0.0 as default - this can be enhanced later
                }
                
                at_value = input_delay;

                if (input_delay > 0.0) {
                    std::cout << "    Set AT of " << start->getName()
                             << " = " << at_value << " ns (input delay)" << std::endl;
                }
            }
        }

        start->setArrivalTime(at_value);
        start->setArrivalTimeMax(at_value);  // Setup analysis (max path)
        start->setArrivalTimeMin(at_value);  // Hold analysis (min path)
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
        double best_slew_max = 0.0;  // Slew from critical (slowest) path
        double best_slew_min = 0.0;  // Slew from fastest path

        // ==================== Setup (Max) ====================
        if (max_arrival > -std::numeric_limits<double>::infinity() / 2) {
            current_node->setArrivalTimeMax(max_arrival);
            updated = true;

            // Get best slew from the critical path
            best_slew_max = getBestSlewFromCriticalPath(incoming_arcs, max_arrival, true);
        }

        // ==================== Hold (Min) ====================
        if (min_arrival < std::numeric_limits<double>::infinity() / 2) {
            current_node->setArrivalTimeMin(min_arrival);
            updated = true;

            // Get best slew from the fastest path
            best_slew_min = getBestSlewFromCriticalPath(incoming_arcs, min_arrival, false);
        }

        // Propagate slew if we found valid paths
        if (updated) {
            current_node->setSlewMax(best_slew_max);
            current_node->setSlewMin(best_slew_min);
            updated_count++;

            // Debug print first few nodes
            if (updated_count <= 5) {
                std::cout << "    Propagated AT of " << current_node->getName()
                         << " [Setup=" << max_arrival << "ns, Hold=" << min_arrival << "ns"
                         << ", SlewMax=" << (best_slew_max * 1e9) << "ps"
                         << ", SlewMin=" << (best_slew_min * 1e9) << "ps]" << std::endl;
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

    // Get clock period from constraints
    double clock_period = 0.0;
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

    // [NEW] Apply dynamic Setup/Hold constraints for DFF endpoints
    if (constraints_) {
        checkSetupHoldConstraints(sorted_nodes);
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

    // Get clock period from constraints
    double clock_period = 0.0;
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
 * @brief Calculate realistic load capacitance for a timing node
 * @details Computes: C_load = Sum(C_pin) + C_wire
 * Strategy:
 * 1. Pin Capacitance: Query Liberty library for each fanout pin
 * 2. Wire Capacitance: wire_length * wire_cap_per_unit
 * @param node Timing node (output pin)
 * @param wire_cap_per_unit Unit wire capacitance (pF/μm)
 * @return Total load capacitance in farads
 */
double STAEngine::calculateNetLoadCapacitance(TimingNode* node, double wire_cap_per_unit) const {
    if (!node || !node->getPin()) {
        return 1e-15;  // 1fF minimum fallback
    }

    Pin* output_pin = node->getPin();
    Net* net = output_pin->getNet();
    
    double total_cap = 0.0;

    // ========================================================================
    // 1. Calculate Pin Capacitance (from Liberty library)
    // ========================================================================
    for (TimingArc* fanout_arc : node->getOutgoingArcs()) {
        TimingNode* fanout_node = fanout_arc->getToNode();
        if (!fanout_node || !fanout_node->getPin()) continue;

        Pin* fanout_pin = fanout_node->getPin();
        Cell* fanout_cell = fanout_pin->getOwner();
        if (!fanout_cell) continue;

        // Get Liberty cell data through delay model
        TableDelayModel* table_model = dynamic_cast<TableDelayModel*>(delay_model_.get());
        if (!table_model) {
            total_cap += 0.002 * 1e-12;  // Fallback: 2fF per pin
            continue;
        }

        // Map cell type to Liberty cell
        std::string cell_type = fanout_cell->getTypeString();
        const Library* lib = table_model->getLibrary();
        if (!lib) {
            total_cap += 0.002 * 1e-12;  // Fallback
            continue;
        }

        const LibCell* lib_cell = lib->getCell(cell_type);
        if (!lib_cell) {
            total_cap += 0.002 * 1e-12;  // Fallback
            continue;
        }

        // Get pin capacitance from Liberty
        std::string pin_name = fanout_pin->getName();
        const LibPin* lib_pin = lib_cell->getPin(pin_name);
        if (lib_pin && lib_pin->capacitance > 0.0) {
            total_cap += lib_pin->capacitance * 1e-12;  // Convert pF to F
        } else {
            total_cap += 0.002 * 1e-12;  // Fallback: 2fF
        }
    }

    // ========================================================================
    // 2. Calculate Wire Capacitance (from physical routing)
    // ========================================================================
    if (net && net->hasWireLength()) {
        // Use actual routed wire length
        double wire_length = net->getWireLength();  // in μm
        double wire_cap = wire_length * wire_cap_per_unit;  // in pF
        total_cap += wire_cap * 1e-12;  // Convert pF to F
    } else if (net) {
        // Fallback: estimate from HPWL if placement exists
        // For now, use a conservative estimate
        double estimated_wire_cap = 0.001;  // 1fF default
        total_cap += estimated_wire_cap * 1e-12;
    }

    // ========================================================================
    // 3. Ensure minimum load capacitance
    // ========================================================================
    if (total_cap < 1e-15) {  // Less than 1fF
        total_cap = 1e-15;  // 1fF minimum
    }

    return total_cap;
}

/**
 * @brief Get valid input slew with default fallback
 */
double STAEngine::getValidInputSlew(double input_slew, bool is_max) const {
    if (input_slew <= 0.0) {
        return is_max ? default_input_slew_max_ : default_input_slew_min_;
    }
    return input_slew;
}

/**
 * @brief Get best slew from critical path
 */
double STAEngine::getBestSlewFromCriticalPath(const std::vector<TimingArc*>& incoming_arcs,
                                               double target_arrival,
                                               bool is_max) const {
    // Find the arc that gave us the target arrival (critical or fastest path)
    for (TimingArc* arc : incoming_arcs) {
        TimingNode* prev_node = arc->getFromNode();
        if (!prev_node) continue;

        double prev_at;
        if (is_max) {
            prev_at = prev_node->getArrivalTimeMax();
            if (prev_at <= -TimingNode::UNINITIALIZED / 2) continue;
        } else {
            prev_at = prev_node->getArrivalTimeMin();
            if (prev_at >= TimingNode::UNINITIALIZED / 2) continue;
        }

        double path_delay = prev_at + arc->getDelay();
        if (std::abs(path_delay - target_arrival) < 1e-15) {  // Found the target path
            double best_slew = arc->getOutputSlew();
            if (best_slew <= 0.0) {
                // Fallback: use previous node's slew
                best_slew = getValidInputSlew(is_max ? prev_node->getSlewMax() : prev_node->getSlewMin(), is_max);
            }
            return best_slew;
        }
    }

    // If no matching arc found, return default
    return getValidInputSlew(0.0, is_max);
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

// ============================================================================
// [NEW] Setup/Hold Constraint Checking with Liberty Lookup Tables
// ============================================================================

/**
 * @brief [NEW] Check Setup/Hold constraints using Liberty lookup tables
 * @details This is the key function that bridges theoretical STA to industrial reality:
 * 
 * Traditional STA: Required_Time = Clock_Period - Fixed_Setup_Time
 * Industrial STA:  Required_Time = Clock_Edge - Setup_Table(data_slew, clk_slew) - Uncertainty
 * 
 * The Setup/Hold times are NOT constants! They depend on:
 * 1. Data signal slew (slower data = more setup time needed)
 * 2. Clock signal slew (slower clock = more setup time needed) 
 * 3. Process/voltage/temperature conditions (embedded in Liberty tables)
 */
void STAEngine::checkSetupHoldConstraints(const std::vector<TimingNode*>& sorted_nodes) {
    std::cout << "\n[Step 3.5] Checking Setup/Hold Constraints with Liberty Lookup Tables..." << std::endl;
    
    // Get library from delay model for constraint lookup
    auto* table_delay_model = dynamic_cast<TableDelayModel*>(delay_model_.get());
    if (!table_delay_model) {
        std::cout << "  Warning: No TableDelayModel available, skipping constraint lookup" << std::endl;
        return;
    }
    
    const Library* library = table_delay_model->getLibrary();
    if (!library) {
        std::cout << "  Warning: No Liberty library available, skipping constraint lookup" << std::endl;
        return;
    }
    
    double clock_period = constraints_->getMainClockPeriod();
    double clock_uncertainty = constraints_->getClockUncertainty();
    size_t constraint_checks = 0;
    size_t setup_violations = 0;
    size_t hold_violations = 0;
    
    // Iterate through all nodes to find DFF data pins for constraint checking
    for (TimingNode* node : sorted_nodes) {
        if (!node || !node->getPin()) continue;
        
        Pin* pin = node->getPin();
        Cell* cell = pin->getOwner();
        if (!cell || cell->getType() != CellType::DFF) continue;
        
        // We're interested in DFF data pins (D input) for setup/hold checks
        if (pin->getName() != "D") continue;
        
        // Find corresponding clock pin (CK) of the same DFF
        TimingNode* clk_node = nullptr;
        for (TimingNode* other_node : sorted_nodes) {
            if (!other_node || !other_node->getPin()) continue;
            
            Pin* other_pin = other_node->getPin();
            Cell* other_cell = other_pin->getOwner();
            if (other_cell == cell && other_pin->getName() == "CK") {
                clk_node = other_node;
                break;
            }
        }
        
        if (!clk_node) {
            std::cout << "  Warning: No clock node found for DFF " << cell->getName() << std::endl;
            continue;
        }
        
        // Get Liberty cell for constraint tables using CellMapper
        const CellMapper* cell_mapper = table_delay_model->getCellMapper();
        if (!cell_mapper) {
            std::cout << "  Warning: No CellMapper available in delay model" << std::endl;
            continue;
        }
        
        const LibCell* lib_cell = cell_mapper->findWithWarning(cell->getTypeString(), "DFF constraint check");
        if (!lib_cell) {
            std::cout << "    Available cells in library: " << library->getCellCount() << " total" << std::endl;
            if (constraint_checks == 0) { // Only print once
                library->printCellNames();
            }
            continue;
        }
        
        // Get data pin (D) for constraint lookup
        const LibPin* data_pin = lib_cell->getPin("D");
        if (!data_pin) {
            std::cout << "  Warning: No D pin found in Liberty cell " << lib_cell->name << std::endl;
            continue;
        }
        
        // Get clock pin (CK) for constraint lookup  
        const LibPin* clk_pin = lib_cell->getPin("CK");
        if (!clk_pin) {
            std::cout << "  Warning: No CK pin found in Liberty cell " << lib_cell->name << std::endl;
            continue;
        }
        
        // Prepare slew values for lookup (convert from seconds to nanoseconds for Liberty tables)
        double data_slew_ns = node->getSlewMax() * 1e9;  // Data slew in ns
        double clk_slew_ns = clk_node->getSlewMax() * 1e9;   // Clock slew in ns
        
        // Clamp to reasonable ranges to avoid lookup errors
        data_slew_ns = std::max(0.001, std::min(2.0, data_slew_ns));
        clk_slew_ns = std::max(0.001, std::min(2.0, clk_slew_ns));

        // Helper lambda to lookup constraint time
        auto lookupConstraintTime = [&](const std::string& constraint_type, double default_time) -> double {
            const LibConstraint* constraint = data_pin->getBestConstraint(constraint_type);
            double constraint_time = default_time;

            if (constraint && constraint->constraint_table.isValid()) {
                constraint_time = constraint->constraint_table.lookup(data_slew_ns, clk_slew_ns);
                // [NEW] Debug: Show which constraint was selected
                if (!constraint->when_condition.empty()) {
                    std::cout << "    Using " << constraint_type << " constraint with when: "
                              << constraint->when_condition << std::endl;
                }
            } else {
                std::cout << "  Warning: No " << constraint_type << " table found for " << lib_cell->name
                          << ", using default " << constraint_time << " ns" << std::endl;
            }

            return constraint_time;
        };

        // ==================== Setup Constraint Check ====================
        double lib_setup_time = lookupConstraintTime("setup_rising", 0.1); // Default: 100ps

        // Check if clock and data arrival times are initialized
        double clk_at_max = clk_node->getArrivalTimeMax();
        double data_at_max = node->getArrivalTimeMax();
        
        if (clk_at_max <= -TimingNode::UNINITIALIZED / 2 || 
            data_at_max <= -TimingNode::UNINITIALIZED / 2) {
            std::cout << "  Skipping DFF " << cell->getName() 
                      << ": Clock or data arrival time not initialized" << std::endl;
            continue;
        }

        // Calculate Setup Required Time
        // Formula: Required_Time = Clock_Capture_Edge - Setup_Time - Uncertainty
        double clk_arrival = clk_at_max * 1e9; // Convert to ns
        double setup_required = clk_arrival + clock_period - lib_setup_time - clock_uncertainty;

        // Calculate Setup Slack
        double data_arrival = data_at_max * 1e9; // Convert to ns
        double setup_slack = setup_required - data_arrival;

        // Update node's setup slack
        node->setSlackSetup(setup_slack * 1e-9); // Convert back to seconds

        if (setup_slack < 0) setup_violations++;

        // ==================== Hold Constraint Check ====================
        double lib_hold_time = lookupConstraintTime("hold_rising", 0.05); // Default: 50ps

        // Check if data arrival time min is initialized
        double data_at_min = node->getArrivalTimeMin();
        
        if (data_at_min >= TimingNode::UNINITIALIZED / 2) {
            std::cout << "  Skipping DFF " << cell->getName() 
                      << ": Data arrival time min not initialized" << std::endl;
            continue;
        }

        // Calculate Hold Required Time
        // Formula: Required_Time = Clock_Launch_Edge + Hold_Time + Uncertainty
        double hold_required = clk_arrival + lib_hold_time + clock_uncertainty;

        // Calculate Hold Slack
        double data_arrival_min = data_at_min * 1e9; // Convert to ns
        double hold_slack = data_arrival_min - hold_required;

        // Update node's hold slack
        node->setSlackHold(hold_slack * 1e-9); // Convert back to seconds
        
        if (hold_slack < 0) hold_violations++;
        
        // Report constraint check details for first few DFFs
        if (constraint_checks < 3) {
            std::cout << "  DFF " << cell->getName() << " constraints:" << std::endl;
            std::cout << "    Data slew: " << data_slew_ns << " ns, Clock slew: " << clk_slew_ns << " ns" << std::endl;
            std::cout << "    Setup time: " << lib_setup_time << " ns (from lookup)" << std::endl;
            std::cout << "    Hold time: " << lib_hold_time << " ns (from lookup)" << std::endl;
            std::cout << "    Setup slack: " << setup_slack << " ns" << (setup_slack < 0 ? " [VIOLATION]" : "") << std::endl;
            std::cout << "    Hold slack: " << hold_slack << " ns" << (hold_slack < 0 ? " [VIOLATION]" : "") << std::endl;
        }
        
        constraint_checks++;
    }
    
    std::cout << "  Checked " << constraint_checks << " DFF constraints" << std::endl;
    std::cout << "  Setup violations: " << setup_violations << std::endl;
    std::cout << "  Hold violations: " << hold_violations << std::endl;
    
    if (constraint_checks == 0) {
        std::cout << "  Note: No DFF endpoints found for constraint checking" << std::endl;
    }
}

} // namespace mini