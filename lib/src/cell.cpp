/**
 * @file cell.cpp
 * @brief Cell (Logic Gate) Data Model Implementation
 */

#include "cell.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace mini {

// ============================================================================
// Pin Class Implementation
// ============================================================================

Pin::Pin(const std::string& name, PinDirection direction, Cell* owner)
    : name_(name)
    , direction_(direction)
    , owner_(owner)
    , net_(nullptr)
{
}

// ============================================================================
// Cell Class Implementation
// ============================================================================

Cell::Cell(const std::string& name, CellType type)
    : name_(name)
    , type_(type)
    , x_(0.0)
    , y_(0.0)
    , placed_(false)
    , delay_(0.0)
{
}

Cell::~Cell() = default;

Pin* Cell::addPin(const std::string& pin_name, PinDirection direction) {
    // Check if pin with same name already exists
    if (pin_map_.find(pin_name) != pin_map_.end()) {
        throw std::runtime_error("Pin '" + pin_name + "' already exists in cell '" + name_ + "'");
    }

    // Create new pin
    auto pin = std::make_unique<Pin>(pin_name, direction, this);
    Pin* pin_ptr = pin.get();

    // Add to containers
    pins_.push_back(std::move(pin));
    pin_map_[pin_name] = pin_ptr;

    return pin_ptr;
}

Pin* Cell::getPin(const std::string& pin_name) const {
    auto it = pin_map_.find(pin_name);
    if (it != pin_map_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<Pin*> Cell::getInputPins() const {
    std::vector<Pin*> input_pins;
    for (const auto& pin : pins_) {
        if (pin->isInput()) {
            input_pins.push_back(pin.get());
        }
    }
    return input_pins;
}

std::vector<Pin*> Cell::getOutputPins() const {
    std::vector<Pin*> output_pins;
    for (const auto& pin : pins_) {
        if (pin->isOutput()) {
            output_pins.push_back(pin.get());
        }
    }
    return output_pins;
}

bool Cell::isCombinational() const {
    switch (type_) {
        case CellType::AND:
        case CellType::OR:
        case CellType::NOT:
        case CellType::NAND:
        case CellType::NOR:
        case CellType::XOR:
        case CellType::XNOR:
        case CellType::BUF:
        case CellType::MUX:
            return true;
        default:
            return false;
    }
}

bool Cell::isSequential() const {
    switch (type_) {
        case CellType::DFF:
            return true;
        default:
            return false;
    }
}

bool Cell::isPort() const {
    return type_ == CellType::INPUT || type_ == CellType::OUTPUT;
}

std::string Cell::getTypeString() const {
    return cellTypeToString(type_);
}

void Cell::print() const {
    std::cout << "Cell: " << name_ << std::endl;
    std::cout << "  Type: " << getTypeString() << std::endl;
    std::cout << "  Pins: " << pins_.size() << std::endl;

    for (const auto& pin : pins_) {
        std::cout << "    - " << pin->getName()
                  << " (" << (pin->isInput() ? "INPUT" : "OUTPUT") << ")";
        if (pin->isConnected()) {
            std::cout << " [Connected]";
        }
        std::cout << std::endl;
    }

    if (placed_) {
        std::cout << "  Position: (" << x_ << ", " << y_ << ")" << std::endl;
    } else {
        std::cout << "  Position: Not placed" << std::endl;
    }

    if (delay_ > 0.0) {
        std::cout << "  Delay: " << delay_ << " ns" << std::endl;
    }
}

// ============================================================================
// Utility Functions Implementation
// ============================================================================

std::string cellTypeToString(CellType type) {
    switch (type) {
        case CellType::UNKNOWN:  return "UNKNOWN";
        case CellType::AND:      return "AND";
        case CellType::OR:       return "OR";
        case CellType::NOT:      return "NOT";
        case CellType::NAND:     return "NAND";
        case CellType::NOR:      return "NOR";
        case CellType::XOR:      return "XOR";
        case CellType::XNOR:     return "XNOR";
        case CellType::BUF:      return "BUF";
        case CellType::DFF:      return "DFF";
        case CellType::MUX:      return "MUX";
        case CellType::INPUT:    return "INPUT";
        case CellType::OUTPUT:   return "OUTPUT";
        default:                 return "UNKNOWN";
    }
}

CellType stringToCellType(const std::string& type_str) {
    // Convert to uppercase for comparison
    std::string upper_str = type_str;
    std::transform(upper_str.begin(), upper_str.end(), upper_str.begin(), ::toupper);

    if (upper_str == "AND")         return CellType::AND;
    if (upper_str == "OR")          return CellType::OR;
    if (upper_str == "NOT" || upper_str == "INV")
                                    return CellType::NOT;
    if (upper_str == "NAND")        return CellType::NAND;
    if (upper_str == "NOR")         return CellType::NOR;
    if (upper_str == "XOR")         return CellType::XOR;
    if (upper_str == "XNOR")        return CellType::XNOR;
    if (upper_str == "BUF" || upper_str == "BUFFER")
                                    return CellType::BUF;
    if (upper_str == "DFF")         return CellType::DFF;
    if (upper_str == "MUX")         return CellType::MUX;
    if (upper_str == "INPUT")       return CellType::INPUT;
    if (upper_str == "OUTPUT")      return CellType::OUTPUT;

    return CellType::UNKNOWN;
}

} // namespace mini
