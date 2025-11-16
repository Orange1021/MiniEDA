/**
 * @file netlist_db.h
 * @brief Netlist Database Management System
 * @details Centralized container for managing Cells, Nets, and their connections
 */

#ifndef MINI_NETLIST_DB_H
#define MINI_NETLIST_DB_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "cell.h"
#include "net.h"

namespace mini {

/**
 * @brief Database statistics structure
 */
struct NetlistStats {
    size_t num_cells;                   ///< Total number of cells
    size_t num_nets;                    ///< Total number of nets
    size_t num_pins;                    ///< Total number of pins
    size_t num_combinational_cells;     ///< Number of combinational cells
    size_t num_sequential_cells;        ///< Number of sequential cells
    size_t num_input_ports;             ///< Number of input ports
    size_t num_output_ports;            ///< Number of output ports
    double avg_fanout;                  ///< Average fanout
    size_t max_fanout;                  ///< Maximum fanout
};

/**
 * @brief Netlist Database Class
 * @details Central repository for all circuit objects
 */
class NetlistDB {
public:
    // ============================================================================
    // Constructor & Destructor
    // ============================================================================

    /**
     * @brief Constructor
     * @param design_name Top-level design name
     */
    explicit NetlistDB(const std::string& design_name = "top");

    /**
     * @brief Destructor
     */
    ~NetlistDB();

    // Disable copy, enable move
    NetlistDB(const NetlistDB&) = delete;
    NetlistDB& operator=(const NetlistDB&) = delete;
    NetlistDB(NetlistDB&&) = default;
    NetlistDB& operator=(NetlistDB&&) = default;

    // ============================================================================
    // Basic Accessors
    // ============================================================================

    /**
     * @brief Get design name
     */
    const std::string& getDesignName() const { return design_name_; }

    /**
     * @brief Set design name
     */
    void setDesignName(const std::string& name) { design_name_ = name; }

    // ============================================================================
    // Cell Management
    // ============================================================================

    /**
     * @brief Create a new cell
     * @param name Cell instance name (must be unique)
     * @param type Cell type
     * @return Pointer to the created cell, nullptr if name already exists
     */
    Cell* createCell(const std::string& name, CellType type);

    /**
     * @brief Get cell by name
     * @param name Cell instance name
     * @return Pointer to the cell, nullptr if not found
     */
    Cell* getCell(const std::string& name) const;

    /**
     * @brief Check if cell exists
     * @param name Cell instance name
     * @return true if exists, false otherwise
     */
    bool hasCell(const std::string& name) const;

    /**
     * @brief Get all cells
     * @return Const reference to cell container
     */
    const std::vector<std::unique_ptr<Cell>>& getCells() const { return cells_; }

    /**
     * @brief Get number of cells
     * @return Total cell count
     */
    size_t getNumCells() const { return cells_.size(); }

    /**
     * @brief Get cells by type
     * @param type Target cell type
     * @return Vector of cell pointers matching the type
     */
    std::vector<Cell*> getCellsByType(CellType type) const;

    /**
     * @brief Get all input port cells
     * @return Vector of input port cell pointers
     */
    std::vector<Cell*> getInputPorts() const;

    /**
     * @brief Get all output port cells
     * @return Vector of output port cell pointers
     */
    std::vector<Cell*> getOutputPorts() const;

    // ============================================================================
    // Net Management
    // ============================================================================

    /**
     * @brief Create a new net
     * @param name Net name (must be unique)
     * @return Pointer to the created net, nullptr if name already exists
     */
    Net* createNet(const std::string& name);

    /**
     * @brief Get net by name
     * @param name Net name
     * @return Pointer to the net, nullptr if not found
     */
    Net* getNet(const std::string& name) const;

    /**
     * @brief Check if net exists
     * @param name Net name
     * @return true if exists, false otherwise
     */
    bool hasNet(const std::string& name) const;

    /**
     * @brief Get all nets
     * @return Const reference to net container
     */
    const std::vector<std::unique_ptr<Net>>& getNets() const { return nets_; }

    /**
     * @brief Get number of nets
     * @return Total net count
     */
    size_t getNumNets() const { return nets_.size(); }

    // ============================================================================
    // Connection Management
    // ============================================================================

    /**
     * @brief Connect a pin to a net
     * @param cell_name Cell instance name
     * @param pin_name Pin name
     * @param net_name Net name
     * @return true if connection successful, false otherwise
     */
    bool connect(const std::string& cell_name,
                 const std::string& pin_name,
                 const std::string& net_name);

    /**
     * @brief Connect a pin to a net (using pointers)
     * @param pin Pin pointer
     * @param net Net pointer
     * @return true if connection successful, false otherwise
     */
    bool connect(Pin* pin, Net* net);

    /**
     * @brief Disconnect a pin from its net
     * @param cell_name Cell instance name
     * @param pin_name Pin name
     * @return true if disconnection successful, false otherwise
     */
    bool disconnect(const std::string& cell_name, const std::string& pin_name);

    /**
     * @brief Disconnect a pin from its net (using pointer)
     * @param pin Pin pointer
     * @return true if disconnection successful, false otherwise
     */
    bool disconnect(Pin* pin);

    // ============================================================================
    // Topology Query
    // ============================================================================

    /**
     * @brief Get all cells in fanout cone of given cell
     * @param cell Source cell pointer
     * @return Vector of fanout cell pointers
     */
    std::vector<Cell*> getFanoutCells(const Cell* cell) const;

    /**
     * @brief Get all cells in fanin cone of given cell
     * @param cell Target cell pointer
     * @return Vector of fanin cell pointers
     */
    std::vector<Cell*> getFaninCells(const Cell* cell) const;

    /**
     * @brief Check if two cells are directly connected
     * @param from Source cell
     * @param to Target cell
     * @return true if there's a direct net connection, false otherwise
     */
    bool isDirectlyConnected(const Cell* from, const Cell* to) const;

    /**
     * @brief Get all nets connected to a cell
     * @param cell Cell pointer
     * @return Vector of connected net pointers
     */
    std::vector<Net*> getConnectedNets(const Cell* cell) const;

    // ============================================================================
    // Validation & Statistics
    // ============================================================================

    /**
     * @brief Validate netlist integrity
     * @return true if valid, false if errors detected
     */
    bool validate() const;

    /**
     * @brief Get database statistics
     * @return NetlistStats structure
     */
    NetlistStats getStatistics() const;

    /**
     * @brief Print database statistics to stdout
     */
    void printStatistics() const;

    /**
     * @brief Print detailed netlist information (for debugging)
     */
    void printNetlist() const;

    // ============================================================================
    // Database Maintenance
    // ============================================================================

    /**
     * @brief Clear all data
     */
    void clear();

    /**
     * @brief Check if database is empty
     * @return true if empty, false otherwise
     */
    bool isEmpty() const { return cells_.empty() && nets_.empty(); }

private:
    // ============================================================================
    // Private Data Members
    // ============================================================================

    std::string design_name_;                           ///< Top-level design name

    // Object containers (ownership)
    std::vector<std::unique_ptr<Cell>> cells_;          ///< All cells
    std::vector<std::unique_ptr<Net>> nets_;            ///< All nets

    // Fast lookup maps (non-owning pointers)
    std::unordered_map<std::string, Cell*> cell_map_;   ///< Name -> Cell mapping
    std::unordered_map<std::string, Net*> net_map_;     ///< Name -> Net mapping

    // ============================================================================
    // Private Helper Methods
    // ============================================================================

    /**
     * @brief Check for floating pins (unconnected pins)
     * @param errors Output vector for error messages
     * @return Number of errors found
     */
    size_t checkFloatingPins(std::vector<std::string>& errors) const;

    /**
     * @brief Check for multi-driver nets
     * @param errors Output vector for error messages
     * @return Number of errors found
     */
    size_t checkMultiDriverNets(std::vector<std::string>& errors) const;

    /**
     * @brief Check for nets without drivers
     * @param errors Output vector for error messages
     * @return Number of errors found
     */
    size_t checkNetsWithoutDriver(std::vector<std::string>& errors) const;
};

} // namespace mini

#endif // MINI_NETLIST_DB_H
