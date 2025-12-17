/**
 * @file pin_mapper.h
 * @brief Pin Mapper - Liberty to Verilog Pin Name Translation
 * @details Professional pin name translation layer between Liberty library naming conventions
 *          and Verilog netlist naming standards. Implements Separation of Concerns principle.
 * 
 * Architecture Positioning:
 * [Liberty File] (ZN, A, B) 
 *       ↓
 * [TimingGraph Builder] 
 *       ↓ Query: "Liberty says pin ZN, what's it called in Verilog?"
 * [PinMapper] <--- Core Translation Layer
 *       ↓ Answer: "In Verilog it's called Y"
 * [NetlistDB] (Y, A, B)
 */

#ifndef MINI_LIBERTY_PIN_MAPPER_H
#define MINI_LIBERTY_PIN_MAPPER_H

#include <string>
#include <map>
#include <utility>
#include <iostream>
#include <iomanip>

namespace mini {

/**
 * @class LibertyPinMapper
 * @brief Independent utility class for Liberty pin name to Verilog pin name translation
 * @details This class solves the "where are my hands?" problem by translating between
 *          different naming conventions used by various process libraries.
 * 
 * Responsibility Separation:
 * - MacroMapper: "Who am I?" (Cell Type mapping: NAND2_X1 -> NAND2)
 * - PinMapper:   "Where are my hands?" (Pin Name mapping: ZN -> Y, I0 -> A)
 */
class LibertyPinMapper {
public:
    /**
     * @brief Constructor
     * @details Initializes default mapping rules for common process libraries
     */
    LibertyPinMapper();

    /**
     * @brief Core query function: Translate Liberty pin name to Verilog pin name
     * @param cell_type Cell type from netlist (e.g., "NAND2", "DFF")
     * @param lib_pin_name Pin name from Liberty library (e.g., "ZN", "A")
     * @return Corresponding pin name in Verilog netlist (e.g., "Y", "A")
     * @details Translation strategy:
     *          1. Check cell-specific mapping rules first
     *          2. Fall back to global mapping rules
     *          3. Default to original name if no mapping found
     */
    std::string getNetlistPinName(const std::string& cell_type, 
                                  const std::string& lib_pin_name) const;

    /**
     * @brief Reverse query: Translate Verilog pin name to Liberty pin name
     * @param cell_type Cell type from netlist
     * @param netlist_pin_name Pin name from Verilog netlist
     * @return Corresponding pin name in Liberty library
     * @details Useful for debugging and reverse lookup operations
     */
    std::string getLibPinName(const std::string& cell_type, 
                             const std::string& netlist_pin_name) const;

    /**
     * @brief Add global mapping rule (applies to all cell types)
     * @param lib_pin Liberty pin name
     * @param netlist_pin Verilog pin name
     * @details Example: addGlobalMapping("ZN", "Y") for all inverter outputs
     */
    void addGlobalMapping(const std::string& lib_pin, const std::string& netlist_pin);

    /**
     * @brief Add cell-specific mapping rule (applies only to specific cell type)
     * @param cell_type Target cell type
     * @param lib_pin Liberty pin name
     * @param netlist_pin Verilog pin name
     * @details Example: addSpecificMapping("LATCH", "G", "EN") for latch gate pin
     */
    void addSpecificMapping(const std::string& cell_type, 
                           const std::string& lib_pin, 
                           const std::string& netlist_pin);

    /**
     * @brief Enable/disable debug output for mapping process
     * @param enabled True to enable debug logging
     */
    void setDebugMode(bool enabled) { debug_enabled_ = enabled; }

    /**
     * @brief Get mapping statistics
     * @return Pair of (successful_mappings, total_attempts)
     */
    std::pair<size_t, size_t> getStats() const { return {successful_mappings_, total_attempts_}; }

    /**
     * @brief Reset mapping statistics
     */
    void resetStats() { successful_mappings_ = 0; total_attempts_ = 0; }

    /**
     * @brief Print all current mapping rules for debugging
     */
    void printMappings() const;

private:
    // ============================================================================
    // Mapping Data Structures (Two-Level Hierarchy)
    // ============================================================================

    /**
     * @brief Global mapping table: Universal rules for all cell types
     * @details Key: Liberty Pin, Value: Netlist Pin
     * Examples: "ZN" -> "Y", "G" -> "CK" (global clock pin mapping)
     */
    std::map<std::string, std::string> global_map_;

    /**
     * @brief Specific mapping table: Cell-type-specific rules
     * @details Key: (CellType, LibertyPin), Value: NetlistPin
     * Examples: ("LATCH", "G") -> "EN" (latch-specific gate pin mapping)
     */
    std::map<std::pair<std::string, std::string>, std::string> specific_map_;

    // ============================================================================
    // Configuration and Statistics
    // ============================================================================

    bool debug_enabled_;                    ///< Debug output flag
    mutable size_t successful_mappings_;    ///< Statistics: successful mappings
    mutable size_t total_attempts_;         ///< Statistics: total mapping attempts

    // ============================================================================
    // Private Helper Methods
    // ============================================================================

    /**
     * @brief Initialize default mapping rules for common process libraries
     * @details Sets up standard mappings for Nangate, TSMC, GF, etc.
     */
    void initializeDefaultMappings();

    /**
     * @brief Debug logging helper
     * @param message Debug message to print
     */
    void debugLog(const std::string& message) const;
};

} // namespace mini

#endif // MINI_LIBERTY_PIN_MAPPER_H