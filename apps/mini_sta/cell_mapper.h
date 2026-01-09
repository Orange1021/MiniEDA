/**
 * @file cell_mapper.h
 * @brief Cell Mapper - Logical to Physical Cell Type Mapping for STA
 * @details Provides intelligent mapping between logical cell types (Verilog) 
 *          and physical library cells (Liberty) with configurable strategies.
 */

#ifndef MINI_CELL_MAPPER_H
#define MINI_CELL_MAPPER_H

#include <string>
#include <vector>
#include <iostream>
#include "../../lib/include/liberty.h"

namespace mini {

/**
 * @class CellMapper
 * @brief Intelligent mapper from logical cell types to physical library cells
 * @details This class acts as a translator between the logical world (Verilog)
 *          and the physical world (Liberty), implementing various matching strategies.
 */
class CellMapper {
public:
    /**
     * @brief Constructor
     * @param library Reference to the Liberty library containing physical cells
     */
    explicit CellMapper(const Library& library);

    /**
     * @brief Map logical cell type to best matching physical cell
     * @param cell_type Logical cell type from netlist (e.g., "NAND2")
     * @return Pointer to matched LibCell, nullptr if no match found
     * @details Matching strategy:
     *          1. Exact match with cell_type
     *          2. Try common drive strength suffixes (_X1, _X2, etc.)
     *          3. Fallback to alternative naming conventions
     */
    const LibCell* mapType(const std::string& cell_type) const;

    /**
     * @brief Find cell with warning message
     * @param cell_type Logical cell type from netlist
     * @param context Optional context string for warning message
     * @return Pointer to matched LibCell, nullptr if no match found
     * @details Prints warning to stderr if cell not found
     */
    const LibCell* findWithWarning(const std::string& cell_type,
                                   const std::string& context = "") const;

    /**
     * @brief Find cell with fallback to library direct lookup
     * @param cell_type Logical cell type from netlist
     * @return Pointer to matched LibCell, nullptr if no match found
     * @details Tries CellMapper first, then falls back to direct library lookup
     */
    const LibCell* findWithLibraryFallback(const std::string& cell_type, 
                                           const Library* library) const;

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

private:
    const Library& library_;
    bool debug_enabled_;
    
    // Statistics
    mutable size_t successful_mappings_;
    mutable size_t total_attempts_;
    
    // Common drive strength suffixes in order of preference
    std::vector<std::string> drive_strength_suffixes_ = {
        "", "_X1", "_X2", "_X4", "_X8", "_X16", "_X32"
    };
    
    // Alternative naming conventions for special cases
    std::vector<std::pair<std::string, std::string>> alternative_names_ = {
        // Basic logic gates
        {"NOT", "INV"},           // NOT gate -> INV in Liberty
        {"AND", "AND2"},          // Default to 2-input
        {"NAND", "NAND2"},        // Default to 2-input
        {"OR", "OR2"},            // Default to 2-input
        {"NOR", "NOR2"},          // Default to 2-input
        {"XOR", "XOR2"},          // Default to 2-input
        {"XNOR", "XNOR2"},        // Default to 2-input
        {"BUF", "BUF_X1"},        // Buffer
        {"DFF", "DFF_X1"},        // D Flip-Flop
        
        // Sequential elements with variants
        {"DFFR", "DFFR_X1"},      // DFF with reset
        {"DFFS", "DFFS_X1"},      // DFF with set
        {"DFFRS", "DFFRS_X1"},    // DFF with reset and set
        {"SDFF", "SDFF_X1"},      // Scan DFF
        {"SDFFR", "SDFFR_X1"},    // Scan DFF with reset
        {"SDFFS", "SDFFS_X1"},    // Scan DFF with set
        {"SDFFRS", "SDFFRS_X1"},  // Scan DFF with reset and set
        
        // I/O and special cells (use LOGIC cells as fallback)
        {"INPUT", "LOGIC1_X1"},    // Use LOGIC1 as INPUT placeholder
        {"OUTPUT", "LOGIC0_X1"},   // Use LOGIC0 as OUTPUT placeholder
        {"CLKBUF", "CLKBUF_X1"},   // Clock buffer
        {"TBUF", "TBUF_X1"},       // Tri-state buffer
        {"TINV", "TINV_X1"},       // Tri-state inverter
        
        // Complex gates (AOI, OAI families)
        {"AOI21", "AOI21_X1"},     // AND-OR-INVERT 2-1
        {"AOI22", "AOI22_X1"},     // AND-OR-INVERT 2-2
        {"AOI211", "AOI211_X1"},   // AND-OR-INVERT 2-1-1
        {"AOI221", "AOI221_X1"},   // AND-OR-INVERT 2-2-1
        {"AOI222", "AOI222_X1"},   // AND-OR-INVERT 2-2-2
        {"OAI21", "OAI21_X1"},     // OR-AND-INVERT 2-1
        {"OAI22", "OAI22_X1"},     // OR-AND-INVERT 2-2
        {"OAI211", "OAI211_X1"},   // OR-AND-INVERT 2-1-1
        {"OAI221", "OAI221_X1"},   // OR-AND-INVERT 2-2-1
        {"OAI222", "OAI222_X1"},   // OR-AND-INVERT 2-2-2
        
        // Arithmetic cells
        {"HA", "HA_X1"},           // Half adder
        {"FA", "FA_X1"},           // Full adder
        
        // Latches
        {"DLAT", "TLAT_X1"},       // Transparent latch
        {"DLH", "DLH_X1"},         // Latch (alternative name)
        {"DLL", "DLL_X1"},         // Latch (alternative name)
        
        // Clock gating
        {"CLKGATE", "CLKGATE_X1"}, // Clock gate
        {"CLKGATETST", "CLKGATETST_X1"}, // Clock gate with test
        
        // Multi-input gates (3 and 4 inputs)
        {"AND3", "AND3_X1"},       // 3-input AND
        {"AND4", "AND4_X1"},       // 4-input AND
        {"NAND3", "NAND3_X1"},     // 3-input NAND
        {"NAND4", "NAND4_X1"},     // 4-input NAND
        {"OR3", "OR3_X1"},         // 3-input OR
        {"OR4", "OR4_X1"},         // 4-input OR
        {"NOR3", "NOR3_X1"},       // 3-input NOR
        {"NOR4", "NOR4_X1"},       // 4-input NOR
        {"OAI33", "OAI33_X1"},     // OR-AND-INVERT 3-3
        
        // Multiplexer
        {"MUX2", "MUX2_X1"}        // 2-to-1 multiplexer
    };

    /**
     * @brief Try exact match with given name
     * @param name Name to search for
     * @return Pointer to LibCell if found, nullptr otherwise
     */
    const LibCell* tryExactMatch(const std::string& name) const;

    /**
     * @brief Try matching with drive strength suffixes
     * @param base_name Base name without suffix
     * @return Pointer to LibCell if found, nullptr otherwise
     */
    const LibCell* tryDriveStrengthMatch(const std::string& base_name) const;

    /**
     * @brief Try alternative naming conventions
     * @param cell_type Original cell type
     * @return Pointer to LibCell if found, nullptr otherwise
     */
    const LibCell* tryAlternativeNames(const std::string& cell_type) const;

    /**
     * @brief Try pattern-based matching for complex cells
     * @param cell_type Original cell type
     * @return Pointer to LibCell if found, nullptr otherwise
     */
    const LibCell* tryPatternMatch(const std::string& cell_type) const;

    /**
     * @brief Try fuzzy matching by removing common suffixes/prefixes
     * @param cell_type Original cell type
     * @return Pointer to LibCell if found, nullptr otherwise
     */
    const LibCell* tryFuzzyMatch(const std::string& cell_type) const;

    /**
     * @brief Extract input count from cell type name
     * @param cell_type Cell type name (e.g., "AND2", "NAND3")
     * @return Number of inputs
     */
    size_t extractInputCount(const std::string& cell_type) const;

    /**
     * @brief Debug logging helper
     * @param message Debug message to print
     */
    void debugLog(const std::string& message) const;
};

} // namespace mini

#endif // MINI_CELL_MAPPER_H