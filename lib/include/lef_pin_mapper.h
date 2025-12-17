/**
 * @file pin_mapper.h
 * @brief Pin Mapper - Unified Pin Name Mapping and Key Generation
 * @details Centralized module for mapping logical pin names to physical LEF pin names
 *          and generating consistent lookup keys across the entire EDA flow.
 */

#ifndef MINI_LEF_PIN_MAPPER_H
#define MINI_LEF_PIN_MAPPER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "lef_parser.h"
#include "liberty.h"
#include "cell.h"

// Forward declaration to avoid circular dependency
namespace mini {
class MacroMapper;
}

// Include MacroMapper definition for full implementation
#include "../../apps/mini_placement/macro_mapper.h"

namespace mini {

/**
 * @class LefPinMapper
 * @brief Unified LEF physical pin mapping and key generation system
 * @details This class provides a single source of truth for:
 *          1. Mapping logical pin names (Verilog) to physical pin names (LEF)
 *          2. Generating consistent lookup keys for pin location maps
 *          3. Eliminating mapping logic duplication between modules
 */
class LefPinMapper {
public:
    /**
     * @brief Constructor
     * @param lef_lib LEF library containing physical pin definitions
     * @param macro_mapper Cell type to LEF macro mapper
     */
    LefPinMapper(const LefLibrary& lef_lib, const MacroMapper& macro_mapper);

    /**
     * @brief Get physical pin name from logical pin name
     * @param cell_type Cell type (e.g., "NAND2", "DFF")
     * @param logical_pin_name Logical pin name from netlist (e.g., "Y", "A", "CK")
     * @return Physical pin name from LEF (e.g., "ZN", "A1", "CP")
     * @details Uses the same heuristic mapping logic as buildPinLocationMap
     */
    std::string getPhysicalPinName(const std::string& cell_type, 
                                  const std::string& logical_pin_name) const;

    /**
     * @brief Generate consistent lookup key for pin location maps
     * @param cell_name Cell instance name (e.g., "U1", "DFF_0")
     * @param physical_pin_name Physical pin name from LEF (e.g., "ZN", "A1")
     * @return Consistent key string (e.g., "U1:ZN", "DFF_0:A1")
     */
    std::string makeKey(const std::string& cell_name, 
                       const std::string& physical_pin_name);

    /**
     * @brief Get lookup key directly from cell instance and logical pin
     * @param cell_name Cell instance name (e.g., "U1", "DFF_0")
     * @param cell_type Cell type (e.g., "NAND2", "DFF")
     * @param logical_pin_name Logical pin name from netlist (e.g., "Y", "A")
     * @return Consistent key string (e.g., "U1:ZN", "DFF_0:CP")
     */
    std::string getKeyFromLogicalPin(const std::string& cell_name,
                                     const std::string& cell_type,
                                     const std::string& logical_pin_name);

    /**
     * @brief Enable/disable debug output
     * @param enabled True to enable debug logging
     */
    void setDebugMode(bool enabled) { debug_enabled_ = enabled; }

    /**
     * @brief Debug logging helper
     * @param message Debug message to print
     */
    void debugLog(const std::string& message) const;

    /**
     * @brief Get mapping statistics
     * @return Pair of (successful_mappings, total_attempts)
     */
    std::pair<size_t, size_t> getStats() const { return {successful_mappings_, total_attempts_}; }

    /**
     * @brief Reset mapping statistics
     */
    void resetStats() { successful_mappings_ = 0; total_attempts_ = 0; }

public:
    // [New] Ultimate Key generator
    // Directly pass Cell and Pin objects, PinMapper internally determines if it's I/O or StdCell
    std::string getPinKey(Cell* cell, Pin* pin) const;

private:
    // Core data
    LefLibrary& lef_lib_;
    MacroMapper& macro_mapper_;
    
    // Debug flag
    bool debug_enabled_ = false;
    
    // Statistics
    mutable size_t successful_mappings_ = 0;
    mutable size_t total_attempts_ = 0;

    /**
     * @brief Try direct pin name match in LEF macro
     * @param macro Pointer to LEF macro
     * @param pin_name Pin name to search for
     * @return Pointer to LEF port if found, nullptr otherwise
     */
    const LefPort* tryDirectMatch(const LefMacro* macro, const std::string& pin_name) const;

    /**
     * @brief Try heuristic pin name mapping
     * @param macro Pointer to LEF macro
     * @param netlist_pin_name Pin name from netlist
     * @return Pointer to LEF port if found, nullptr otherwise
     */
    const LefPort* tryHeuristicMapping(const LefMacro* macro, const std::string& netlist_pin_name) const;

    
};

} // namespace mini

#endif // MINI_LEF_PIN_MAPPER_H