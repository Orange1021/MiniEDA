/**
 * @file liberty_pin_mapper.cpp
 * @brief Liberty Pin Mapper Implementation - Liberty to Verilog Pin Name Translation
 * @details Professional implementation of pin name translation with multi-level
 *          mapping strategies and comprehensive debug support.
 */

#include "../include/liberty_pin_mapper.h"
#include "debug_log.h"

namespace mini {

/**
 * @brief Constructor
 * @details Initializes default mapping rules and debug settings
 */
LibertyPinMapper::LibertyPinMapper() 
    : successful_mappings_(0), total_attempts_(0) {
    initializeDefaultMappings();
}

/**
 * @brief Core translation logic: Liberty pin name → Verilog pin name
 * @param cell_type Cell type from netlist (e.g., "NAND2", "DFF")
 * @param lib_pin_name Pin name from Liberty library (e.g., "ZN", "A")
 * @return Corresponding pin name in Verilog netlist
 * @details Translation hierarchy:
 *          1. Cell-specific mapping (highest priority)
 *          2. Global mapping (medium priority)
 *          3. Passthrough (fallback - same name)
 */
std::string LibertyPinMapper::getNetlistPinName(const std::string& cell_type, 
                                                 const std::string& lib_pin_name) const {
    total_attempts_++;
    
    // Step 1: Check cell-specific mapping rules
    // Example: While usually G -> CK, in a special latch G -> EN
    auto specific_key = std::make_pair(cell_type, lib_pin_name);
    auto specific_it = specific_map_.find(specific_key);
    if (specific_it != specific_map_.end()) {
        successful_mappings_++;
        return specific_it->second;
    }

    // Step 2: Check global mapping rules
    // Example: Handle your ZN -> Y problem across all cell types
    auto global_it = global_map_.find(lib_pin_name);
    if (global_it != global_map_.end()) {
        successful_mappings_++;
        return global_it->second;
    }

    // Step 3: Default case (Passthrough)
    // If no mapping found, assume names are identical (A -> A)
    return lib_pin_name;
}

/**
 * @brief Reverse translation: Verilog pin name → Liberty pin name
 * @param cell_type Cell type from netlist
 * @param netlist_pin_name Pin name from Verilog netlist
 * @return Corresponding pin name in Liberty library
 * @details Implements reverse lookup by scanning mapping tables
 */
std::string LibertyPinMapper::getLibPinName(const std::string& cell_type, 
                                             const std::string& netlist_pin_name) const {
    // Step 1: Check cell-specific reverse mapping
    for (const auto& entry : specific_map_) {
        if (entry.first.first == cell_type && entry.second == netlist_pin_name) {
            return entry.first.second;  // Return Liberty pin name
        }
    }

    // Step 2: Check global reverse mapping
    for (const auto& entry : global_map_) {
        if (entry.second == netlist_pin_name) {
            return entry.first;  // Return Liberty pin name
        }
    }

    // Step 3: Default case (Passthrough)
    return netlist_pin_name;
}

/**
 * @brief Add global mapping rule
 * @param lib_pin Liberty pin name
 * @param netlist_pin Verilog pin name
 * @details Example: addGlobalMapping("ZN", "Y") for all inverter outputs
 */
void LibertyPinMapper::addGlobalMapping(const std::string& lib_pin, 
                                        const std::string& netlist_pin) {
    global_map_[lib_pin] = netlist_pin;
}

/**
 * @brief Add cell-specific mapping rule
 * @param cell_type Target cell type
 * @param lib_pin Liberty pin name
 * @param netlist_pin Verilog pin name
 * @details Example: addSpecificMapping("LATCH", "G", "EN") for latch gate pin
 */
void LibertyPinMapper::addSpecificMapping(const std::string& cell_type, 
                                          const std::string& lib_pin, 
                                          const std::string& netlist_pin) {
    auto key = std::make_pair(cell_type, lib_pin);
    specific_map_[key] = netlist_pin;
    DEBUG_LOG("LibertyPinMapper", "Added specific mapping: " + cell_type + ":" + lib_pin + 
              " -> " + netlist_pin);
}

/**
 * @brief Print all current mapping rules for debugging
 */
void LibertyPinMapper::printMappings() const {
    std::cout << "\n========== PinMapper Mappings ==========" << std::endl;
    
    std::cout << "\n--- Global Mappings (" << global_map_.size() << ") ---" << std::endl;
    for (const auto& entry : global_map_) {
        std::cout << "  " << entry.first << " -> " << entry.second << std::endl;
    }
    
    std::cout << "\n--- Cell-Specific Mappings (" << specific_map_.size() << ") ---" << std::endl;
    for (const auto& entry : specific_map_) {
        std::cout << "  " << entry.first.first << ":" << entry.first.second 
                  << " -> " << entry.second << std::endl;
    }
    
    std::cout << "\n--- Statistics ---" << std::endl;
    std::cout << "  Total attempts: " << total_attempts_ << std::endl;
    std::cout << "  Successful mappings: " << successful_mappings_ << std::endl;
    if (total_attempts_ > 0) {
        double success_rate = 100.0 * successful_mappings_ / total_attempts_;
        std::cout << "  Success rate: " << std::fixed << std::setprecision(1) 
                  << success_rate << "%" << std::endl;
    }
    std::cout << "=========================================" << std::endl;
}

/**
 * @brief Initialize default mapping rules for common process libraries
 * @details Sets up standard mappings for Nangate, TSMC, GlobalFoundries, etc.
 */
void LibertyPinMapper::initializeDefaultMappings() {
    // ========================================================================
    // GLOBAL MAPPINGS: Apply to all cell types
    // ========================================================================
    
    // Standard output pin mappings (most critical for your current issue)
    addGlobalMapping("ZN", "Y");      // Inverter/Buffer output (Nangate)
    addGlobalMapping("O", "Y");       // Output pin (TSMC)
    addGlobalMapping("Z", "Y");       // Output pin (GlobalFoundries)
    
    // Standard input pin mappings (usually already match, but included for completeness)
    addGlobalMapping("I0", "A");      // Input 0 (TSMC style)
    addGlobalMapping("I1", "B");      // Input 1 (TSMC style)
    addGlobalMapping("I2", "C");      // Input 2 (TSMC style)
    addGlobalMapping("I3", "D");      // Input 3 (TSMC style)
    
    // Clock and control pin mappings
    addGlobalMapping("G", "CK");      // Gate pin (common in latches)
    addGlobalMapping("CLK", "CK");    // Clock pin alternative name
    
    // ========================================================================
    // CELL-SPECIFIC MAPPINGS: Special cases for particular cell types
    // ========================================================================
    
    // Latch-specific mappings (example of special handling)
    addSpecificMapping("LATCH", "G", "EN");     // Latch gate -> Enable
    addSpecificMapping("LATCH", "QN", "QN_B");  // Latch inverted output
    
    // Flip-flop specific mappings
    addSpecificMapping("DFF", "S", "S");        // Set pin (already matches, but explicit)
    addSpecificMapping("DFF", "R", "R");        // Reset pin (already matches, but explicit)
    addSpecificMapping("DFF", "SI", "SI");      // Scan input
    addSpecificMapping("DFF", "SE", "SE");      // Scan enable
    
    // Multiplexer specific mappings
    addSpecificMapping("MUX2", "S", "S");       // MUX select pin
    addSpecificMapping("MUX2", "D0", "A");      // MUX data input 0
    addSpecificMapping("MUX2", "D1", "B");      // MUX data input 1
    
    // Tri-state buffer specific mappings
    addSpecificMapping("TBUF", "OE", "EN");     // Output enable -> Enable
    addSpecificMapping("TBUF", "O", "Y");       // Output pin (TBUF specific)
    
    // ========================================================================
    // FUTURE EXTENSIONS: Placeholders for other process libraries
    // ========================================================================
    
    // TSMC-specific mappings (examples)
    // addGlobalMapping("A1", "A");  // TSMC input naming
    // addGlobalMapping("B1", "B");  // TSMC input naming
    
    // Intel-specific mappings (examples)
    // addGlobalMapping("IN0", "A"); // Intel input naming
    // addGlobalMapping("IN1", "B"); // Intel input naming
    
}

/**
 * @brief Debug logging helper
 * @param message Debug message to print
 */
} // namespace mini