/**
 * @file net.cpp
 * @brief Net (Wire/Signal) Data Model Implementation
 */

#include "net.h"
#include "cell.h"
#include <iostream>
#include <algorithm>

namespace mini {

// ============================================================================
// Net Class Implementation
// ============================================================================

Net::Net(const std::string& name)
    : name_(name)
    , driver_(nullptr)
    , wire_capacitance_(0.0)
    , wire_delay_(0.0)
    , wire_resistance_(0.0)
    , wire_length_(0.0)
    , is_clock_(false)
{
}

Net::~Net() {
    disconnectAll();
}

void Net::setDriver(Pin* pin) {
    // If there's an existing driver, disconnect it first
    if (driver_ != nullptr) {
        driver_->setNet(nullptr);
    }

    driver_ = pin;

    // Update the pin's net pointer
    if (driver_ != nullptr) {
        driver_->setNet(this);
    }
}

void Net::addLoad(Pin* pin) {
    if (pin == nullptr) {
        return;
    }

    // Check if pin is already in loads
    auto it = std::find(loads_.begin(), loads_.end(), pin);
    if (it != loads_.end()) {
        return; // Already exists, don't add duplicate
    }

    // Add to loads
    loads_.push_back(pin);

    // Update the pin's net pointer
    pin->setNet(this);
}

bool Net::removeLoad(Pin* pin) {
    if (pin == nullptr) {
        return false;
    }

    // Find and remove the pin
    auto it = std::find(loads_.begin(), loads_.end(), pin);
    if (it != loads_.end()) {
        // Disconnect the pin
        (*it)->setNet(nullptr);

        // Remove from vector
        loads_.erase(it);
        return true;
    }

    return false;
}

void Net::disconnectAll() {
    // Disconnect driver
    if (driver_ != nullptr) {
        driver_->setNet(nullptr);
        driver_ = nullptr;
    }

    // Disconnect all loads
    for (Pin* load : loads_) {
        if (load != nullptr) {
            load->setNet(nullptr);
        }
    }
    loads_.clear();
}

std::vector<Pin*> Net::getAllPins() const {
    std::vector<Pin*> all_pins;

    // Add driver if exists
    if (driver_ != nullptr) {
        all_pins.push_back(driver_);
    }

    // Add all loads
    all_pins.insert(all_pins.end(), loads_.begin(), loads_.end());

    return all_pins;
}

bool Net::isConnectedTo(const Pin* pin) const {
    if (pin == nullptr) {
        return false;
    }

    // Check if it's the driver
    if (driver_ == pin) {
        return true;
    }

    // Check if it's in loads
    auto it = std::find(loads_.begin(), loads_.end(), pin);
    return it != loads_.end();
}

bool Net::isConnectedTo(const Cell* cell) const {
    if (cell == nullptr) {
        return false;
    }

    // Check driver
    if (driver_ != nullptr && driver_->getOwner() == cell) {
        return true;
    }

    // Check loads
    for (const Pin* load : loads_) {
        if (load != nullptr && load->getOwner() == cell) {
            return true;
        }
    }

    return false;
}

void Net::print() const {
    std::cout << "Net: " << name_ << std::endl;

    // Print driver
    if (driver_ != nullptr) {
        std::cout << "  Driver: " << driver_->getOwner()->getName()
                  << "." << driver_->getName() << std::endl;
    } else {
        std::cout << "  Driver: None" << std::endl;
    }

    // Print loads
    std::cout << "  Loads: " << loads_.size() << " (Fanout: " << getFanout() << ")" << std::endl;
    for (const Pin* load : loads_) {
        if (load != nullptr) {
            std::cout << "    - " << load->getOwner()->getName()
                      << "." << load->getName() << std::endl;
        }
    }

    // Print timing information
    if (wire_capacitance_ > 0.0) {
        std::cout << "  Wire Capacitance: " << wire_capacitance_ << " pF" << std::endl;
    }
    if (wire_delay_ > 0.0) {
        std::cout << "  Wire Delay: " << wire_delay_ << " ns" << std::endl;
    }

    // Print attributes
    if (is_clock_) {
        std::cout << "  Type: Clock Signal" << std::endl;
    }
}

} // namespace mini
