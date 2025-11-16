/**
 * @file netlist_db.cpp
 * @brief Netlist Database Management System Implementation
 */

#include "netlist_db.h"
#include <iostream>
#include <algorithm>
#include <iomanip>

namespace mini {

// ============================================================================
// Constructor & Destructor
// ============================================================================

NetlistDB::NetlistDB(const std::string& design_name)
    : design_name_(design_name)
{
}

NetlistDB::~NetlistDB() {
    clear();
}

// ============================================================================
// Cell Management
// ============================================================================

Cell* NetlistDB::createCell(const std::string& name, CellType type) {
    // Check if cell with this name already exists
    if (hasCell(name)) {
        std::cerr << "Warning: Cell '" << name << "' already exists!" << std::endl;
        return nullptr;
    }

    // Create new cell
    auto cell = std::make_unique<Cell>(name, type);
    Cell* cell_ptr = cell.get();

    // Add to container and map
    cells_.push_back(std::move(cell));
    cell_map_[name] = cell_ptr;

    return cell_ptr;
}

Cell* NetlistDB::getCell(const std::string& name) const {
    auto it = cell_map_.find(name);
    if (it != cell_map_.end()) {
        return it->second;
    }
    return nullptr;
}

bool NetlistDB::hasCell(const std::string& name) const {
    return cell_map_.find(name) != cell_map_.end();
}

std::vector<Cell*> NetlistDB::getCellsByType(CellType type) const {
    std::vector<Cell*> result;
    for (const auto& cell : cells_) {
        if (cell->getType() == type) {
            result.push_back(cell.get());
        }
    }
    return result;
}

std::vector<Cell*> NetlistDB::getInputPorts() const {
    return getCellsByType(CellType::INPUT);
}

std::vector<Cell*> NetlistDB::getOutputPorts() const {
    return getCellsByType(CellType::OUTPUT);
}

// ============================================================================
// Net Management
// ============================================================================

Net* NetlistDB::createNet(const std::string& name) {
    // Check if net with this name already exists
    if (hasNet(name)) {
        std::cerr << "Warning: Net '" << name << "' already exists!" << std::endl;
        return nullptr;
    }

    // Create new net
    auto net = std::make_unique<Net>(name);
    Net* net_ptr = net.get();

    // Add to container and map
    nets_.push_back(std::move(net));
    net_map_[name] = net_ptr;

    return net_ptr;
}

Net* NetlistDB::getNet(const std::string& name) const {
    auto it = net_map_.find(name);
    if (it != net_map_.end()) {
        return it->second;
    }
    return nullptr;
}

bool NetlistDB::hasNet(const std::string& name) const {
    return net_map_.find(name) != net_map_.end();
}

// ============================================================================
// Connection Management
// ============================================================================

bool NetlistDB::connect(const std::string& cell_name,
                       const std::string& pin_name,
                       const std::string& net_name) {
    // Find cell
    Cell* cell = getCell(cell_name);
    if (cell == nullptr) {
        std::cerr << "Error: Cell '" << cell_name << "' not found!" << std::endl;
        return false;
    }

    // Find pin
    Pin* pin = cell->getPin(pin_name);
    if (pin == nullptr) {
        std::cerr << "Error: Pin '" << pin_name << "' not found in cell '"
                  << cell_name << "'!" << std::endl;
        return false;
    }

    // Find net
    Net* net = getNet(net_name);
    if (net == nullptr) {
        std::cerr << "Error: Net '" << net_name << "' not found!" << std::endl;
        return false;
    }

    // Connect pin to net
    return connect(pin, net);
}

bool NetlistDB::connect(Pin* pin, Net* net) {
    if (pin == nullptr || net == nullptr) {
        std::cerr << "Error: Cannot connect null pin or net!" << std::endl;
        return false;
    }

    // Connect based on pin direction
    if (pin->isOutput()) {
        // Output pin is a driver
        net->setDriver(pin);
    } else if (pin->isInput()) {
        // Input pin is a load
        net->addLoad(pin);
    } else {
        // Bidirectional pins - treat as load for now
        net->addLoad(pin);
    }

    return true;
}

bool NetlistDB::disconnect(const std::string& cell_name, const std::string& pin_name) {
    // Find cell
    Cell* cell = getCell(cell_name);
    if (cell == nullptr) {
        std::cerr << "Error: Cell '" << cell_name << "' not found!" << std::endl;
        return false;
    }

    // Find pin
    Pin* pin = cell->getPin(pin_name);
    if (pin == nullptr) {
        std::cerr << "Error: Pin '" << pin_name << "' not found in cell '"
                  << cell_name << "'!" << std::endl;
        return false;
    }

    return disconnect(pin);
}

bool NetlistDB::disconnect(Pin* pin) {
    if (pin == nullptr) {
        std::cerr << "Error: Cannot disconnect null pin!" << std::endl;
        return false;
    }

    // Get the connected net
    Net* net = pin->getNet();
    if (net == nullptr) {
        // Pin is not connected to any net
        return true;
    }

    // Disconnect based on pin direction
    if (pin->isOutput()) {
        // Output pin is a driver
        if (net->getDriver() == pin) {
            net->setDriver(nullptr);
        }
    } else {
        // Input or bidirectional pin is a load
        net->removeLoad(pin);
    }

    return true;
}

// ============================================================================
// Topology Query
// ============================================================================

std::vector<Cell*> NetlistDB::getFanoutCells(const Cell* cell) const {
    if (cell == nullptr) {
        return {};
    }

    std::unordered_set<Cell*> fanout_set;

    // Get all output pins of the cell
    std::vector<Pin*> output_pins = cell->getOutputPins();

    // For each output pin, find connected nets and their load pins
    for (Pin* out_pin : output_pins) {
        Net* net = out_pin->getNet();
        if (net == nullptr) {
            continue;
        }

        // Get all load pins on this net
        const std::vector<Pin*>& loads = net->getLoads();
        for (Pin* load_pin : loads) {
            if (load_pin != nullptr) {
                Cell* load_cell = load_pin->getOwner();
                if (load_cell != nullptr && load_cell != cell) {
                    fanout_set.insert(load_cell);
                }
            }
        }
    }

    // Convert set to vector
    return std::vector<Cell*>(fanout_set.begin(), fanout_set.end());
}

std::vector<Cell*> NetlistDB::getFaninCells(const Cell* cell) const {
    if (cell == nullptr) {
        return {};
    }

    std::unordered_set<Cell*> fanin_set;

    // Get all input pins of the cell
    std::vector<Pin*> input_pins = cell->getInputPins();

    // For each input pin, find connected net and its driver pin
    for (Pin* in_pin : input_pins) {
        Net* net = in_pin->getNet();
        if (net == nullptr) {
            continue;
        }

        // Get driver pin on this net
        Pin* driver_pin = net->getDriver();
        if (driver_pin != nullptr) {
            Cell* driver_cell = driver_pin->getOwner();
            if (driver_cell != nullptr && driver_cell != cell) {
                fanin_set.insert(driver_cell);
            }
        }
    }

    // Convert set to vector
    return std::vector<Cell*>(fanin_set.begin(), fanin_set.end());
}

bool NetlistDB::isDirectlyConnected(const Cell* from, const Cell* to) const {
    if (from == nullptr || to == nullptr) {
        return false;
    }

    // Get fanout cells of 'from'
    std::vector<Cell*> fanout = getFanoutCells(from);

    // Check if 'to' is in the fanout
    return std::find(fanout.begin(), fanout.end(), to) != fanout.end();
}

std::vector<Net*> NetlistDB::getConnectedNets(const Cell* cell) const {
    if (cell == nullptr) {
        return {};
    }

    std::unordered_set<Net*> net_set;

    // Get all pins of the cell
    const std::vector<std::unique_ptr<Pin>>& pins = cell->getPins();

    // For each pin, get its connected net
    for (const auto& pin : pins) {
        Net* net = pin->getNet();
        if (net != nullptr) {
            net_set.insert(net);
        }
    }

    // Convert set to vector
    return std::vector<Net*>(net_set.begin(), net_set.end());
}

// ============================================================================
// Validation & Statistics
// ============================================================================

bool NetlistDB::validate() const {
    std::vector<std::string> errors;

    // Run all validation checks
    size_t total_errors = 0;
    total_errors += checkFloatingPins(errors);
    total_errors += checkMultiDriverNets(errors);
    total_errors += checkNetsWithoutDriver(errors);

    // Print errors if any
    if (total_errors > 0) {
        std::cerr << "Netlist validation failed with " << total_errors
                  << " error(s):" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  - " << error << std::endl;
        }
        return false;
    }

    std::cout << "Netlist validation passed!" << std::endl;
    return true;
}

size_t NetlistDB::checkFloatingPins(std::vector<std::string>& errors) const {
    size_t error_count = 0;

    for (const auto& cell : cells_) {
        // Skip INPUT ports (they don't need drivers)
        if (cell->getType() == CellType::INPUT) {
            continue;
        }

        const std::vector<std::unique_ptr<Pin>>& pins = cell->getPins();
        for (const auto& pin : pins) {
            // Skip OUTPUT ports of OUTPUT cells (they don't need to be connected)
            if (cell->getType() == CellType::OUTPUT && pin->isOutput()) {
                continue;
            }

            if (!pin->isConnected()) {
                errors.push_back("Floating pin: " + cell->getName() + "." + pin->getName());
                error_count++;
            }
        }
    }

    return error_count;
}

size_t NetlistDB::checkMultiDriverNets(std::vector<std::string>& errors) const {
    size_t error_count = 0;

    // Note: Current Net implementation only allows one driver,
    // so this check is for future extensions or external modifications
    for (const auto& net : nets_) {
        // For now, we just verify that if there's a driver, it's an output pin
        Pin* driver = net->getDriver();
        if (driver != nullptr && !driver->isOutput()) {
            errors.push_back("Net '" + net->getName() +
                           "' has a non-output pin as driver: " +
                           driver->getOwner()->getName() + "." + driver->getName());
            error_count++;
        }
    }

    return error_count;
}

size_t NetlistDB::checkNetsWithoutDriver(std::vector<std::string>& errors) const {
    size_t error_count = 0;

    for (const auto& net : nets_) {
        if (!net->hasDriver() && net->getFanout() > 0) {
            errors.push_back("Net '" + net->getName() + "' has loads but no driver");
            error_count++;
        }
    }

    return error_count;
}

NetlistStats NetlistDB::getStatistics() const {
    NetlistStats stats;

    // Basic counts
    stats.num_cells = cells_.size();
    stats.num_nets = nets_.size();

    // Count pins and classify cells
    stats.num_pins = 0;
    stats.num_combinational_cells = 0;
    stats.num_sequential_cells = 0;
    stats.num_input_ports = 0;
    stats.num_output_ports = 0;

    for (const auto& cell : cells_) {
        stats.num_pins += cell->getPins().size();

        if (cell->isCombinational()) {
            stats.num_combinational_cells++;
        } else if (cell->isSequential()) {
            stats.num_sequential_cells++;
        }

        if (cell->getType() == CellType::INPUT) {
            stats.num_input_ports++;
        } else if (cell->getType() == CellType::OUTPUT) {
            stats.num_output_ports++;
        }
    }

    // Calculate fanout statistics
    stats.avg_fanout = 0.0;
    stats.max_fanout = 0;

    if (!nets_.empty()) {
        size_t total_fanout = 0;
        for (const auto& net : nets_) {
            size_t fanout = net->getFanout();
            total_fanout += fanout;
            if (fanout > stats.max_fanout) {
                stats.max_fanout = fanout;
            }
        }
        stats.avg_fanout = static_cast<double>(total_fanout) / nets_.size();
    }

    return stats;
}

void NetlistDB::printStatistics() const {
    NetlistStats stats = getStatistics();

    std::cout << "========================================" << std::endl;
    std::cout << "Netlist Database Statistics" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Design Name: " << design_name_ << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Cells:                " << stats.num_cells << std::endl;
    std::cout << "  - Combinational:    " << stats.num_combinational_cells << std::endl;
    std::cout << "  - Sequential:       " << stats.num_sequential_cells << std::endl;
    std::cout << "  - Input Ports:      " << stats.num_input_ports << std::endl;
    std::cout << "  - Output Ports:     " << stats.num_output_ports << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Nets:                 " << stats.num_nets << std::endl;
    std::cout << "Pins:                 " << stats.num_pins << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Average Fanout:       " << std::fixed << std::setprecision(2)
              << stats.avg_fanout << std::endl;
    std::cout << "Maximum Fanout:       " << stats.max_fanout << std::endl;
    std::cout << "========================================" << std::endl;
}

void NetlistDB::printNetlist() const {
    std::cout << "========================================" << std::endl;
    std::cout << "Netlist: " << design_name_ << std::endl;
    std::cout << "========================================" << std::endl;

    // Print all cells
    std::cout << "\n--- Cells (" << cells_.size() << ") ---" << std::endl;
    for (const auto& cell : cells_) {
        cell->print();
        std::cout << std::endl;
    }

    // Print all nets
    std::cout << "\n--- Nets (" << nets_.size() << ") ---" << std::endl;
    for (const auto& net : nets_) {
        net->print();
        std::cout << std::endl;
    }

    std::cout << "========================================" << std::endl;
}

// ============================================================================
// Database Maintenance
// ============================================================================

void NetlistDB::clear() {
    // Clear all containers
    // Note: unique_ptr will automatically delete objects
    cells_.clear();
    nets_.clear();
    cell_map_.clear();
    net_map_.clear();
}

} // namespace mini
