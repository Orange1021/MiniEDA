/**
 * @file timing_graph.h
 * @brief Timing Graph Container - Mirror of NetlistDB in Timing World
 * @details Container managing all TimingNode and TimingArc, core class for building timing graph
 */

#ifndef MINI_TIMING_GRAPH_H
#define MINI_TIMING_GRAPH_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "timing_path.h"

namespace mini {

// Forward declarations
class NetlistDB;
class Pin;
class Cell;
class Net;
class Library;
class CellMapper;
class LibertyPinMapper;

/**
 * @class TimingGraph
 * @brief Timing graph container that builds and holds the entire timing graph
 * @details Equivalent to NetlistDB's mirror in timing world, storing complete hierarchy of nodes and arcs
 */
class TimingGraph {
public:
    /**
     * @brief Constructor
     * @param netlist Pointer to underlying physical netlist database (read-only)
     * @param library Pointer to Liberty library for timing arc extraction (optional)
     * @warning TimingGraph knows about NetlistDB, but NetlistDB must NOT include TimingGraph headers,
     * ensuring purity of the underlying database
     */
    explicit TimingGraph(NetlistDB* netlist, Library* library = nullptr);

    /**
     * @brief Destructor
     */
    ~TimingGraph();

    // Disable copy, enable move to prevent accidental copying
    TimingGraph(const TimingGraph&) = delete;
    TimingGraph& operator=(const TimingGraph&) = delete;
    TimingGraph(TimingGraph&&) = default;
    TimingGraph& operator=(TimingGraph&&) = default;

    // ============================================================================
    // Core Method: Build timing graph from NetlistDB (Critical Design)
    // ============================================================================

    /**
     * @brief Build timing graph from physical netlist (Factory Method)
     * @details Critical building logic that transforms physical connectivity to timing logical connectivity
     * Building Pipeline:
     *  1. Traverse all Cells in NetlistDB
     *  2. Create TimingNode for each Pin of each Cell
     *  3. Create TimingArc (NET_ARC) for each Net: Driver Pin -> Load Pin
     *  4. Create TimingArc (CELL_ARC) for each Cell: Input Pin -> Output Pin
     * @return true if build successful, false otherwise
     */
    bool buildFromNetlist();

    /**
     * @brief Clear all nodes and arcs, reset graph state
     * @details Prepare for rebuilding graph later
     */
    void clear();

    // ============================================================================
    // Node Management (Non-owning Access)
    // ============================================================================

    /**
     * @brief Find TimingNode by physical pin name
     * @param pin_name Physical pin name
     * @return TimingNode pointer, nullptr if not found
     */
    TimingNode* getNode(const std::string& pin_name) const;

    /**
     * @brief Find TimingNode by physical pin pointer
     * @param pin Physical pin pointer
     * @return TimingNode pointer, nullptr if not found
     */
    TimingNode* getNode(const Pin* pin) const;

    /**
     * @brief Check if timing node exists for given pin
     * @param pin_name Pin name
     * @return true if exists, false otherwise
     */
    bool hasNode(const std::string& pin_name) const;

    /**
     * @brief Get all timing nodes (ownership remains in TimingGraph)
     * @return Const reference to node container
     */
    const std::vector<std::unique_ptr<TimingNode>>& getNodes() const { return nodes_; }

    /**
     * @brief Get number of nodes
     */
    size_t getNumNodes() const { return nodes_.size(); }

    // ============================================================================
    // Arc Management (Non-owning Access)
    // ============================================================================

    /**
     * @brief Get all timing arcs (ownership remains in TimingGraph)
     * @return Const reference to arc container
     */
    const std::vector<std::unique_ptr<TimingArc>>& getArcs() const { return arcs_; }

    /**
     * @brief Get number of arcs
     */
    size_t getNumArcs() const { return arcs_.size(); }

    /**
     * @brief Get all arcs originating from specified node
     * @param node Source node
     * @return Vector of outgoing arc pointers
     */
    std::vector<TimingArc*> getOutgoingArcs(const TimingNode* node) const;

    /**
     * @brief Get all arcs arriving at specified node
     * @param node Target node
     * @return Vector of incoming arc pointers
     */
    std::vector<TimingArc*> getIncomingArcs(const TimingNode* node) const;

    // ============================================================================
    // Core Algorithm: Topological Sort (STA Must Propagate in Topological Order)
    // ============================================================================

    /**
     * @brief Topological sorting algorithm (DFS implementation)
     * @details STA must propagate in topological order to ensure data consistency
     * Algorithm choice: DFS (Depth-First Search) or Kahn's algorithm (BFS)
     * @return Vector of TimingNode pointers in topological order
     * @note If graph contains cycles, returns empty vector (timing graph must be DAG)
     */
    std::vector<TimingNode*> topologicalSort() const;

    /**
     * @brief Topological sort using Kahn's algorithm (BFS implementation)
     * @details Alternative implementation using in-degree zeroing
     * @return Vector of TimingNode pointers in topological order
     */
    std::vector<TimingNode*> topologicalSortKahn() const;

    // ============================================================================
    // Core Algorithm: Identify Path Start and End Points
    // ============================================================================

    /**
     * @brief Get all timing path start point nodes
     * @details Start points include:
     *  - PI (Primary Input) primary input ports
     *  - DFF/Q (flip-flop outputs)
     * @return Vector of start node pointers
     */
    std::vector<TimingNode*> getStartPoints() const;

    /**
     * @brief Get all timing path end point nodes
     * @details End points include:
     *  - PO (Primary Output) primary output ports
     *  - DFF/D (flip-flop inputs)
     * @return Vector of end node pointers
     */
    std::vector<TimingNode*> getEndPoints() const;

    // ============================================================================
    // Graph Topology Queries
    // ============================================================================

    /**
     * @brief Get all primary input nodes (no incoming arcs)
     * @return Vector of primary input node pointers
     */
    std::vector<TimingNode*> getPrimaryInputs() const;

    /**
     * @brief Get all primary output nodes (no outgoing arcs)
     */
    std::vector<TimingNode*> getPrimaryOutputs() const;

    /**
     * @brief Get all clock source nodes
     */
    std::vector<TimingNode*> getClockSources() const;

    /**
     * @brief Check if graph is valid DAG (Directed Acyclic Graph)
     * @return true if acyclic, false if cycles detected
     */
    bool isDAG() const;

    // ============================================================================
    // Timing Data Management and Reset
    // ============================================================================

    /**
     * @brief Reset timing data on all nodes (for new analysis round)
     * @details Calls TimingNode::reset() for each node
     */
    void resetTimingData();

    /**
     * @brief Validate graph integrity
     */
    bool validate() const;

    // ============================================================================
    // Statistics and Debug
    // ============================================================================

    /**
     * @struct TimingGraphStats
     * @brief Timing graph statistics
     */
    struct TimingGraphStats {
        size_t num_nodes;           ///< Total number of nodes
        size_t num_arcs;            ///< Total number of arcs
        size_t num_pi;              ///< Number of primary inputs
        size_t num_po;              ///< Number of primary outputs
        size_t num_clock_nodes;     ///< Number of clock nodes
    };

    /**
     * @brief Get graph statistics
     */
    TimingGraphStats getStatistics() const;

    /**
     * @brief Print statistics to stdout
     */
    void printStatistics() const;

    /**
     * @brief Print graph topology (for debugging)
     * @param verbose Print detailed node/arc information if true
     */
    void printGraph(bool verbose = false) const;

private:
    // ============================================================================
    // Private Data Members (Ownership Management)
    // ============================================================================

    NetlistDB* netlist_;                                 ///< Read-only reference to underlying physical netlist database
    Library* library_;                                   ///< Read-only reference to Liberty library (optional)
    std::unique_ptr<CellMapper> cell_mapper_;            ///< Intelligent logical→physical cell type mapper
    std::unique_ptr<LibertyPinMapper> pin_mapper_;       ///< Professional Liberty→Verilog pin name translator
    std::vector<std::unique_ptr<TimingNode>> nodes_;     ///< Owns all node memory
    std::vector<std::unique_ptr<TimingArc>> arcs_;       ///< Owns all arc memory

    // Fast lookup maps (non-owning pointers)
    std::unordered_map<std::string, TimingNode*> node_map_;         ///< Pin name → TimingNode mapping
    std::unordered_map<const Pin*, TimingNode*> pin_to_node_map_;   ///< Pin pointer → TimingNode mapping

    // ============================================================================
    // Private Helper Methods (Sub-steps of buildFromNetlist)
    // ============================================================================

    /**
     * @brief Create timing node for physical pin
     * @param pin Physical pin pointer
     * @return Created TimingNode pointer
     * @details Called by buildFromNetlist(), traverses all Cell Pins
     */
    TimingNode* createNode(Pin* pin);

    /**
     * @brief Create timing arc between two nodes
     * @param type Arc type (CELL_ARC or NET_ARC)
     * @param from_node Source node
     * @param to_node Destination node
     * @param lib_timing Pointer to Liberty timing data (for CELL_ARC)
     * @return Created TimingArc pointer
     */
    TimingArc* createArc(TimingArcType type, TimingNode* from_node, TimingNode* to_node, const LibTiming* lib_timing = nullptr);

    /**
     * @brief Build all CELL_ARC (traverse all Cell Input → Output)
     * @return true on success, false on failure
     */
    bool buildCellArcs();

    /**
     * @brief Build all NET_ARC (traverse all Net Driver → Load)
     * @return true on success, false on failure
     */
    bool buildNetArcs();

    /**
     * @brief DFS helper for topological sort
     * @param node Current node
     * @param visited Three-color marking array
     * @param node_to_id Node to ID mapping
     * @param order Result order vector
     */
    void dfsTopologicalSort(TimingNode* node, std::vector<int>& visited,
                            const std::unordered_map<TimingNode*, int>& node_to_id,
                            std::vector<TimingNode*>& order) const;

    

    /**
     * @brief Calculate node in-degrees (Kahn's algorithm helper)
     */
    std::unordered_map<TimingNode*, int> calculateInDegrees() const;
};

} // namespace mini

#endif // MINI_TIMING_GRAPH_H
