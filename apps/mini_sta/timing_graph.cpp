/**
 * @file timing_graph.cpp
 * @brief Timing Graph Implementation
 * @details Core implementation of timing graph construction and algorithms
 */

#include "timing_graph.h"
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/cell.h"
#include "../../lib/include/net.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace mini {

// ============================================================================
// Constructor & Destructor
// ============================================================================

/**
 * @brief Constructor
 */
TimingGraph::TimingGraph(NetlistDB* netlist)
    : netlist_(netlist) {
}

/**
 * @brief Destructor
 */
TimingGraph::~TimingGraph() = default;

// ============================================================================
// Graph Construction - Core Method
// ============================================================================

/**
 * @brief Build timing graph from netlist (Factory Method)
 * @details Converts physical connectivity to timing graph
 * Building Pipeline:
 *  1. Create TimingNode for each Pin
 *  2. Create TimingArc (CELL_ARC) for Cell internal paths
 *  3. Create TimingArc (NET_ARC) for Net connections
 * @return true if build successful, false otherwise
 */
bool TimingGraph::buildFromNetlist() {
    clear(); // Clean up old data

    if (!netlist_) {
        std::cerr << "Error: NetlistDB pointer is null!" << std::endl;
        return false;
    }

    // Step 1: Create nodes for all pins
    std::cout << "Step 1: Creating timing nodes for all pins..." << std::endl;
    for (const auto& cell : netlist_->getCells()) {
        for (const auto& pin : cell->getPins()) {
            createNode(pin.get());
        }
    }
    std::cout << "  Created " << nodes_.size() << " timing nodes." << std::endl;

    // Step 2: Create cell arcs (internal paths in combinational cells)
    std::cout << "Step 2: Building cell arcs..." << std::endl;
    if (!buildCellArcs()) {
        std::cerr << "Error: Failed to build cell arcs!" << std::endl;
        return false;
    }
    std::cout << "  Created " << arcs_.size() << " timing arcs (including net arcs)." << std::endl;

    // Step 3: Create net arcs (connections through nets)
    std::cout << "Step 3: Building net arcs..." << std::endl;
    if (!buildNetArcs()) {
        std::cerr << "Error: Failed to build net arcs!" << std::endl;
        return false;
    }
    std::cout << "  Total arcs: " << arcs_.size() << std::endl;

    // Validate the graph
    if (!validate()) {
        std::cerr << "Error: Graph validation failed!" << std::endl;
        return false;
    }

    return true;
}

/**
 * @brief Clear all nodes and arcs
 */
void TimingGraph::clear() {
    nodes_.clear();
    arcs_.clear();
    node_map_.clear();
    pin_to_node_map_.clear();
}

// ============================================================================
// Node & Arc Creation
// ============================================================================

/**
 * @brief Create timing node for a physical pin
 * @param pin Physical pin pointer
 * @return Pointer to created TimingNode
 */
TimingNode* TimingGraph::createNode(Pin* pin) {
    if (!pin) {
        return nullptr;
    }

    // Check if node already exists
    const std::string& pin_name = pin->getName();
    if (node_map_.find(pin_name) != node_map_.end()) {
        return node_map_[pin_name];
    }

    // Create new node
    auto node = std::make_unique<TimingNode>(pin);
    TimingNode* node_ptr = node.get();

    // Store in containers
    nodes_.push_back(std::move(node));
    node_map_[pin_name] = node_ptr;
    pin_to_node_map_[pin] = node_ptr;

    return node_ptr;
}

/**
 * @brief Create timing arc between two nodes
 * @param type Arc type
 * @param from_node Source node
 * @param to_node Destination node
 * @return Pointer to created TimingArc
 */
TimingArc* TimingGraph::createArc(TimingArcType type, TimingNode* from_node, TimingNode* to_node) {
    if (!from_node || !to_node) {
        return nullptr;
    }

    // Create new arc
    auto arc = std::make_unique<TimingArc>(type, from_node, to_node);
    TimingArc* arc_ptr = arc.get();

    // Store in container
    arcs_.push_back(std::move(arc));

    // Connect nodes
    from_node->addOutgoingArc(arc_ptr);
    to_node->addIncomingArc(arc_ptr);

    return arc_ptr;
}

/**
 * @brief Build cell arcs (internal paths in cells)
 * @details For combinational cells: Input Pin -> Output Pin
 * @return true if successful, false otherwise
 */
bool TimingGraph::buildCellArcs() {
    size_t cell_arc_count = 0;

    for (const auto& cell : netlist_->getCells()) {
        // Skip I/O ports and sequential cells for now
        if (cell->isPort() || cell->isSequential()) {
            continue;
        }

        // For combinational cells, create arcs from inputs to outputs
        if (cell->isCombinational()) {
            std::vector<Pin*> input_pins = cell->getInputPins();
            std::vector<Pin*> output_pins = cell->getOutputPins();

            // For simplicity, connect all inputs to all outputs
            // This is a simplification - real tools would use timing arcs from liberty files
            for (Pin* input_pin : input_pins) {
                for (Pin* output_pin : output_pins) {
                    TimingNode* from_node = getNode(input_pin);
                    TimingNode* to_node = getNode(output_pin);

                    if (from_node && to_node) {
                        createArc(TimingArcType::CELL_ARC, from_node, to_node);
                        cell_arc_count++;
                    }
                }
            }
        }
    }

    std::cout << "  Created " << cell_arc_count << " cell arcs." << std::endl;
    return true;
}

/**
 * @brief Build net arcs (connections through nets)
 * @details For each net: Driver Pin -> Load Pins
 * @return true if successful, false otherwise
 */
bool TimingGraph::buildNetArcs() {
    size_t net_arc_count = 0;

    for (const auto& net : netlist_->getNets()) {
        // Get driver pin
        Pin* driver = net->getDriver();
        if (!driver) {
            continue;  // Skip nets without drivers
        }

        TimingNode* from_node = getNode(driver);
        if (!from_node) {
            continue;
        }

        // Connect driver to all load pins
        for (Pin* load_pin : net->getLoads()) {
            TimingNode* to_node = getNode(load_pin);
            if (to_node && to_node != from_node) {
                createArc(TimingArcType::NET_ARC, from_node, to_node);
                net_arc_count++;
            }
        }
    }

    std::cout << "  Created " << net_arc_count << " net arcs." << std::endl;
    return true;
}

// ============================================================================
// Query Methods
// ============================================================================

/**
 * @brief Get node by pin name
 */
TimingNode* TimingGraph::getNode(const std::string& pin_name) const {
    auto it = node_map_.find(pin_name);
    if (it != node_map_.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * @brief Get node by pin pointer
 */
TimingNode* TimingGraph::getNode(const Pin* pin) const {
    auto it = pin_to_node_map_.find(pin);
    if (it != pin_to_node_map_.end()) {
        return it->second;
    }
    return nullptr;
}

/**
 * @brief Check if node exists
 */
bool TimingGraph::hasNode(const std::string& pin_name) const {
    return node_map_.find(pin_name) != node_map_.end();
}

/**
 * @brief Get arcs originating from a node
 */
std::vector<TimingArc*> TimingGraph::getOutgoingArcs(const TimingNode* node) const {
    std::vector<TimingArc*> result;
    if (node) {
        const auto& arcs = node->getOutgoingArcs();
        result.insert(result.end(), arcs.begin(), arcs.end());
    }
    return result;
}

/**
 * @brief Get arcs arriving at a node
 */
std::vector<TimingArc*> TimingGraph::getIncomingArcs(const TimingNode* node) const {
    std::vector<TimingArc*> result;
    if (node) {
        const auto& arcs = node->getIncomingArcs();
        result.insert(result.end(), arcs.begin(), arcs.end());
    }
    return result;
}

// ============================================================================
// Topological Sorting - Core Algorithm
// ============================================================================

/**
 * @brief Topological sort using DFS (3-color marking)
 * @return Vector of nodes in topological order
 */
std::vector<TimingNode*> TimingGraph::topologicalSort() const {
    if (nodes_.empty()) {
        return {};
    }

    // Create ID to node mapping for arrays
    std::unordered_map<TimingNode*, int> node_to_id;
    int id = 0;
    for (const auto& node : nodes_) {
        node_to_id[node.get()] = id++;
    }

    // Three-color visited array: 0=unvisited, 1=visiting, 2=visited
    std::vector<int> visited(nodes_.size(), 0);
    std::vector<TimingNode*> order;

    // Perform DFS from each unvisited node
    for (const auto& node : nodes_) {
        if (visited[node_to_id[node.get()]] == 0) {
            dfsTopologicalSort(node.get(), visited, node_to_id, order);
        }
    }

    // Reverse to get proper topological order
    std::reverse(order.begin(), order.end());
    return order;
}

/**
 * @brief DFS helper for topological sort
 * @param node Current node
 * @param visited Three-color visited array
 * @param node_to_id Node to ID mapping
 * @param order Result order vector
 */
void TimingGraph::dfsTopologicalSort(TimingNode* node, std::vector<int>& visited,
                                     const std::unordered_map<TimingNode*, int>& node_to_id,
                                     std::vector<TimingNode*>& order) const {
    int id = node_to_id.at(node);

    if (visited[id] == 1) {
        // Found a cycle (gray node)
        std::cerr << "Error: Combinational loop detected in timing graph!" << std::endl;
        std::cerr << "  Node: " << node->getName() << std::endl;
        throw std::runtime_error("Combinational loop detected in topological sort");
    }

    if (visited[id] == 2) {
        // Already processed (black node)
        return;
    }

    // Mark as visiting (gray)
    visited[id] = 1;

    // Visit all successors
    for (TimingArc* arc : getOutgoingArcs(node)) {
        TimingNode* successor = arc->getToNode();
        if (successor) {
            dfsTopologicalSort(successor, visited, node_to_id, order);
        }
    }

    // Mark as visited (black)
    visited[id] = 2;

    // Add to order (post-order)
    order.push_back(node);
}

/**
 * @brief Topological sort using Kahn's algorithm (BFS)
 * @return Vector of nodes in topological order
 */
std::vector<TimingNode*> TimingGraph::topologicalSortKahn() const {
    // Calculate in-degrees
    std::unordered_map<TimingNode*, int> in_degree = calculateInDegrees();

    // Initialize queue with nodes of in-degree 0
    std::vector<TimingNode*> queue;
    for (const auto& node : nodes_) {
        if (in_degree[node.get()] == 0) {
            queue.push_back(node.get());
        }
    }

    // Topological order
    std::vector<TimingNode*> order;
    size_t queue_idx = 0;

    while (queue_idx < queue.size()) {
        TimingNode* node = queue[queue_idx++];
        order.push_back(node);

        // Reduce in-degree of all successors
        for (TimingArc* arc : getOutgoingArcs(node)) {
            TimingNode* successor = arc->getToNode();
            if (successor) {
                in_degree[successor]--;
                if (in_degree[successor] == 0) {
                    queue.push_back(successor);
                }
            }
        }
    }

    // Check for cycles
    if (order.size() != nodes_.size() || order.size() != queue.size()) {
        std::cerr << "Error: Cycle detected in timing graph!" << std::endl;
        throw std::runtime_error("Cycle detected in Kahn's topological sort");
    }

    return order;
}

/**
 * @brief Calculate in-degrees of all nodes
 * @return Map of node to in-degree
 */
std::unordered_map<TimingNode*, int> TimingGraph::calculateInDegrees() const {
    std::unordered_map<TimingNode*, int> in_degree;

    // Initialize all nodes to 0
    for (const auto& node : nodes_) {
        in_degree[node.get()] = 0;
    }

    // Count incoming arcs
    for (const auto& arc : arcs_) {
        TimingNode* to = arc->getToNode();
        if (to) {
            in_degree[to]++;
        }
    }

    return in_degree;
}

// ============================================================================
// Path Start/End Point Identification
// ============================================================================

/**
 * @brief Get all timing path start points (PI and DFF/Q)
 * @return Vector of start nodes
 */
std::vector<TimingNode*> TimingGraph::getStartPoints() const {
    std::vector<TimingNode*> start_points;

    for (const auto& node : nodes_) {
        Pin* pin = node->getPin();
        if (!pin) continue;

        Cell* owner = pin->getOwner();
        if (!owner) continue;

        // Primary inputs (INPUT ports)
        if (owner->getType() == CellType::INPUT) {
            start_points.push_back(node.get());
        }

        // DFF outputs (Q pin)
        if (owner->getType() == CellType::DFF && pin->getDirection() == PinDirection::OUTPUT) {
            start_points.push_back(node.get());
        }
    }

    return start_points;
}

/**
 * @brief Get all timing path end points (PO and DFF/D)
 * @return Vector of end nodes
 */
std::vector<TimingNode*> TimingGraph::getEndPoints() const {
    std::vector<TimingNode*> end_points;

    for (const auto& node : nodes_) {
        Pin* pin = node->getPin();
        if (!pin) continue;

        Cell* owner = pin->getOwner();
        if (!owner) continue;

        // Primary outputs (OUTPUT ports)
        if (owner->getType() == CellType::OUTPUT) {
            end_points.push_back(node.get());
        }

        // DFF inputs (D pin)
        if (owner->getType() == CellType::DFF && pin->getDirection() == PinDirection::INPUT) {
            end_points.push_back(node.get());
        }
    }

    return end_points;
}

/**
 * @brief Get all primary input nodes (no incoming arcs)
 */
std::vector<TimingNode*> TimingGraph::getPrimaryInputs() const {
    std::vector<TimingNode*> pis;

    for (const auto& node : nodes_) {
        if (node->getIncomingArcs().empty()) {
            pis.push_back(node.get());
        }
    }

    return pis;
}

/**
 * @brief Get all primary output nodes (no outgoing arcs)
 */
std::vector<TimingNode*> TimingGraph::getPrimaryOutputs() const {
    std::vector<TimingNode*> pos;

    for (const auto& node : nodes_) {
        if (node->getOutgoingArcs().empty()) {
            pos.push_back(node.get());
        }
    }

    return pos;
}

/**
 * @brief Get all clock source nodes
 */
std::vector<TimingNode*> TimingGraph::getClockSources() const {
    std::vector<TimingNode*> clocks;

    for (const auto& node : nodes_) {
        if (node->isClockSource()) {
            clocks.push_back(node.get());
        }
    }

    return clocks;
}

// ============================================================================
// Validation & Utility
// ============================================================================

/**
 * @brief Check if graph is a valid DAG
 */
bool TimingGraph::isDAG() const {
    try {
        std::vector<TimingNode*> order = topologicalSort();
        return !order.empty();
    } catch (const std::runtime_error& e) {
        // Cycle detected
        return false;
    }
}

/**
 * @brief Reset timing data on all nodes
 */
void TimingGraph::resetTimingData() {
    for (const auto& node : nodes_) {
        node->reset();
    }
}

/**
 * @brief Validate graph integrity
 */
bool TimingGraph::validate() const {
    if (!netlist_) {
        std::cerr << "Validation failed: Null netlist pointer" << std::endl;
        return false;
    }

    // Check if graph is DAG
    if (!isDAG()) {
        std::cerr << "Validation failed: Graph contains cycles" << std::endl;
        return false;
    }

    // Check for nodes without pins
    for (const auto& node : nodes_) {
        if (!node->getPin()) {
            std::cerr << "Validation warning: Node has null pin pointer" << std::endl;
            return false;
        }
    }

    return true;
}

// ============================================================================
// Statistics & Debug
// ============================================================================

/**
 * @brief Get graph statistics
 */
TimingGraph::TimingGraphStats TimingGraph::getStatistics() const {
    TimingGraphStats stats;
    stats.num_nodes = nodes_.size();
    stats.num_arcs = arcs_.size();
    stats.num_pi = getPrimaryInputs().size();
    stats.num_po = getPrimaryOutputs().size();
    stats.num_clock_nodes = getClockSources().size();
    return stats;
}

/**
 * @brief Print graph statistics
 */
void TimingGraph::printStatistics() const {
    TimingGraphStats stats = getStatistics();

    std::cout << "\n========== Timing Graph Statistics ==========" << std::endl;
    std::cout << "Nodes:       " << stats.num_nodes << std::endl;
    std::cout << "Arcs:        " << stats.num_arcs << std::endl;
    std::cout << "Primary Inputs:  " << stats.num_pi << std::endl;
    std::cout << "Primary Outputs: " << stats.num_po << std::endl;
    std::cout << "Clock Nodes:   " << stats.num_clock_nodes << std::endl;
    std::cout << "============================================" << std::endl;
}

/**
 * @brief Print graph topology
 */
void TimingGraph::printGraph(bool verbose) const {
    std::cout << "\n========== Timing Graph Topology ==========" << std::endl;
    std::cout << "Number of nodes: " << nodes_.size() << std::endl;
    std::cout << "Number of arcs:  " << arcs_.size() << std::endl;

    if (verbose) {
        std::cout << "\n--- Nodes ---" << std::endl;
        for (const auto& node : nodes_) {
            Pin* pin = node->getPin();
            if (pin && pin->getOwner()) {
                std::cout << "  Node: " << node->getName()
                         << " (Type: " << pin->getOwner()->getTypeString() << ")"
                         << " In: " << node->getIncomingArcs().size()
                         << " Out: " << node->getOutgoingArcs().size() << std::endl;
            }
        }

        std::cout << "\n--- Arcs ---" << std::endl;
        for (const auto& arc : arcs_) {
            std::cout << "  " << arc->getFromNode()->getName()
                     << " -> " << arc->getToNode()->getName()
                     << " (" << arc->getTypeString() << ")" << std::endl;
        }
    }

    std::cout << "==========================================" << std::endl;
}

} // namespace mini
