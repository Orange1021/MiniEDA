/**
 * @file pin_mapper.cpp
 * @brief Pin Mapper Implementation
 */

#include "pin_mapper.h"
#include <algorithm>
#include <cctype>
#include <iostream>

namespace mini {

PinMapper::PinMapper(const LefLibrary& lef_lib, const MacroMapper& macro_mapper)
    : lef_lib_(const_cast<LefLibrary&>(lef_lib)), 
      macro_mapper_(const_cast<MacroMapper&>(macro_mapper)), 
      debug_enabled_(false),
      successful_mappings_(0), total_attempts_(0) {
}

std::string PinMapper::getPhysicalPinName(const std::string& cell_type, 
                                         const std::string& logical_pin_name) {
    total_attempts_++;
    
    debugLog("Mapping pin: " + cell_type + ":" + logical_pin_name);
    
    // Get the LEF macro for this cell type
    const LefMacro* macro = macro_mapper_.mapType(cell_type);
    if (!macro) {
        debugLog("  ✗ No macro found for cell type: " + cell_type);
        return logical_pin_name; // Fallback to original name
    }
    
    // Try direct match first
    const LefPort* port = tryDirectMatch(macro, logical_pin_name);
    if (port) {
        successful_mappings_++;
        debugLog("  ✓ Direct match found: " + logical_pin_name);
        return logical_pin_name;
    }
    
    // Try heuristic mapping
    port = tryHeuristicMapping(macro, logical_pin_name);
    if (port) {
        successful_mappings_++;
        
        // Find the actual pin name in the macro
        for (const auto& pin_name : macro->getPinNames()) {
            if (macro->getPin(pin_name) == port) {
                debugLog("  ✓ Heuristic mapping: " + logical_pin_name + " -> " + pin_name);
                return pin_name;
            }
        }
    }
    
    debugLog("  ✗ No mapping found for: " + logical_pin_name);
    return logical_pin_name; // Fallback to original name
}

std::string PinMapper::makeKey(const std::string& cell_name, 
                              const std::string& physical_pin_name) {
    return cell_name + ":" + physical_pin_name;
}

std::string PinMapper::getKeyFromLogicalPin(const std::string& cell_name,
                                            const std::string& cell_type,
                                            const std::string& logical_pin_name) {
    std::string physical_pin = getPhysicalPinName(cell_type, logical_pin_name);
    return makeKey(cell_name, physical_pin);
}

const LefPort* PinMapper::tryDirectMatch(const LefMacro* macro, const std::string& pin_name) const {
    if (!macro) return nullptr;
    
    const LefPort* port = macro->getPin(pin_name);
    if (port) {
        debugLog("    Direct match success: " + pin_name);
    }
    
    return port;
}

const LefPort* PinMapper::tryHeuristicMapping(const LefMacro* macro, const std::string& netlist_pin_name) const {
    if (!macro) return nullptr;
    
    debugLog("    Trying heuristic mapping for pin: " + netlist_pin_name);
    
    // Convert to uppercase for case-insensitive comparison
    std::string pin_upper = netlist_pin_name;
    std::transform(pin_upper.begin(), pin_upper.end(), pin_upper.begin(), ::toupper);
    
    // Enhanced Nangate pin name mappings with more comprehensive coverage
    std::vector<std::pair<std::string, std::vector<std::string>>> enhanced_mappings = {
        // Output pins - try multiple possibilities
        {"Y", {"ZN", "Z", "Y", "O", "OUT"}},     // Verilog Y -> Nangate ZN/Z/Y/O/OUT
        {"Z", {"ZN", "Z", "Y", "O", "OUT"}},     // Verilog Z -> Nangate ZN/Z/Y/O/OUT
        {"Q", {"Q", "QN", "Z", "ZN", "Y"}},      // DFF output Q -> multiple possibilities
        {"QN", {"QN", "Z", "ZN", "Y", "Q"}},     // DFF inverted output QN
        
        // Input pins - single input gates with more options
        {"A", {"A1", "A", "I", "IN", "INPUT"}},  // Verilog A -> multiple Nangate variants
        {"I", {"A1", "A", "I", "IN", "INPUT"}},  // Verilog I -> multiple Nangate variants
        {"IN", {"A1", "A", "I", "IN", "INPUT"}}, // Verilog IN -> multiple Nangate variants
        
        // Input pins - multi-input gates with systematic mapping
        {"B", {"A2", "B1", "B", "IN1", "S1"}},   // Second input with priority order
        {"C", {"A3", "B1", "C1", "C", "IN2", "S2"}}, // Third input
        {"D", {"A4", "B2", "C1", "D1", "D", "IN3", "S3"}}, // Fourth input
        {"E", {"A5", "B3", "C2", "D1", "E1", "E", "IN4"}}, // Fifth input
        {"F", {"A6", "B4", "C3", "D2", "E1", "F1", "F", "IN5"}}, // Sixth input
        
        // Clock pins with comprehensive coverage
        {"CK", {"CP", "CLK", "CLOCK", "CK", "C"}},   // Clock variants
        {"CLK", {"CP", "CLK", "CLOCK", "CK", "C"}},  // Clock variants
        {"CLOCK", {"CP", "CLK", "CLOCK", "CK", "C"}},// Clock variants
        {"C", {"CP", "CLK", "CLOCK", "CK", "C", "CLKN"}}, // Clock variants
        
        // Control pins
        {"RN", {"RN", "RESET", "RESETN", "CLR", "CLRN"}}, // Reset (active low)
        {"RESET", {"RN", "RESET", "RESETN", "CLR", "CLRN"}}, // Reset
        {"SET", {"SET", "SETN", "S", "SN"}},         // Set pins
        {"EN", {"EN", "ENABLE", "OE", "ENA"}},       // Enable pins
        
        // Select pins for multiplexers
        {"S", {"S", "SEL", "SELECT", "S0", "S1"}},   // Select pins
        {"SEL", {"S", "SEL", "SELECT", "S0", "S1"}}, // Select pins
        
        // Data pins
        {"D", {"D", "DATA", "DI", "DIN"}},          // Data input
        {"DI", {"D", "DATA", "DI", "DIN"}},         // Data input variants
    };
    
    // Try enhanced mappings first
    for (const auto& mapping : enhanced_mappings) {
        if (mapping.first == netlist_pin_name) {
            for (const auto& target_pin : mapping.second) {
                const LefPort* port = macro->getPin(target_pin);
                if (port) {
                    debugLog("    Enhanced mapping success: " + mapping.first + " -> " + target_pin);
                    return port;
                }
            }
        }
    }
    
    // Advanced pattern-based matching with multiple strategies
    if (!pin_upper.empty()) {
        // Strategy 1: Exact first character match with priority ordering
        std::vector<std::string> priority_suffixes = {"1", "2", "3", "4", "5", "6", "", "_1", "_2"};
        for (const auto& suffix : priority_suffixes) {
            std::string candidate = pin_upper + suffix;
            const LefPort* port = macro->getPin(candidate);
            if (port) {
                debugLog("    Priority suffix match: " + netlist_pin_name + " -> " + candidate);
                return port;
            }
        }
        
        // Strategy 2: Systematic pin name exploration
        for (const auto& pin_name : macro->getPinNames()) {
            std::string lef_pin_upper = pin_name;
            std::transform(lef_pin_upper.begin(), lef_pin_upper.end(), lef_pin_upper.begin(), ::toupper);
            
            // Exact match (case insensitive)
            if (pin_upper == lef_pin_upper) {
                const LefPort* port = macro->getPin(pin_name);
                if (port) {
                    debugLog("    Case-insensitive match: " + netlist_pin_name + " -> " + pin_name);
                    return port;
                }
            }
            
            // Single character prefix match (A->A1, A2, etc.)
            if (pin_upper.size() == 1 && lef_pin_upper.size() > 1 && 
                pin_upper[0] == lef_pin_upper[0]) {
                const LefPort* port = macro->getPin(pin_name);
                if (port) {
                    debugLog("    Prefix match: " + netlist_pin_name + " -> " + pin_name);
                    return port;
                }
            }
            
            // Substring matches (both directions)
            if (lef_pin_upper.find(pin_upper) != std::string::npos || 
                pin_upper.find(lef_pin_upper) != std::string::npos) {
                const LefPort* port = macro->getPin(pin_name);
                if (port) {
                    debugLog("    Substring match: " + netlist_pin_name + " -> " + pin_name);
                    return port;
                }
            }
        }
        
        // Strategy 3: Fallback by pin function type
        // Try to guess pin function based on name patterns
        std::vector<std::string> output_candidates = {"ZN", "Z", "Y", "Q", "QN", "O", "OUT"};
        std::vector<std::string> input_candidates = {"A1", "A", "I", "IN", "D", "DI"};
        std::vector<std::string> clock_candidates = {"CP", "CLK", "C", "CK"};
        
        std::vector<std::string> candidates;
        if (pin_upper == "Y" || pin_upper == "Z" || pin_upper == "Q" || pin_upper == "QN") {
            candidates = output_candidates;
        } else if (pin_upper == "A" || pin_upper == "B" || pin_upper == "C" || pin_upper == "D") {
            candidates = input_candidates;
        } else if (pin_upper == "CK" || pin_upper == "CLK" || pin_upper == "C") {
            candidates = clock_candidates;
        }
        
        for (const auto& candidate : candidates) {
            const LefPort* port = macro->getPin(candidate);
            if (port) {
                debugLog("    Function-based fallback: " + netlist_pin_name + " -> " + candidate);
                return port;
            }
        }
    }
    
    // Strategy 4: Last resort - try any available pin
    if (!macro->getPinNames().empty()) {
        const std::string& first_pin = macro->getPinNames()[0];
        const LefPort* port = macro->getPin(first_pin);
        if (port) {
            debugLog("    Last resort fallback: " + netlist_pin_name + " -> " + first_pin);
            return port;
        }
    }
    
    debugLog("    No heuristic mapping found for: " + netlist_pin_name);
    return nullptr;
}

std::string PinMapper::getPinKey(Cell* cell, Pin* pin) const {
    if (!cell) return "";

    // 1. Unified handling of I/O ports (core fix for VDD:Z issue)
    if (cell->getType() == CellType::INPUT || cell->getType() == CellType::OUTPUT) {
        // For I/O, Key is always just the Cell name (e.g., "CK", "VDD")
        return cell->getName();
    }

    // 2. Unified handling of standard cells
    if (!pin) return "";
    
    // Get physical pin name (Y -> ZN)
    std::string phys_pin = const_cast<PinMapper*>(this)->getPhysicalPinName(cell->getTypeString(), pin->getName());
    
    // Combine Key (U1:ZN)
    std::string key = cell->getName() + ":" + phys_pin;
    
    return key;
}

void PinMapper::debugLog(const std::string& message) const {
    
}



} // namespace mini