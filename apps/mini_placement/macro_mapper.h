/**
 * @file macro_mapper.h
 * @brief Macro Mapper - Logical to Physical Cell Type Mapping
 * @details Provides intelligent mapping between logical cell types (Verilog) 
 *          and physical macros (LEF) with configurable suffix strategies.
 */

#ifndef MINI_MACRO_MAPPER_H
#define MINI_MACRO_MAPPER_H

#include <string>
#include <vector>
#include <iostream>
#include "../../lib/include/lef_parser.h"

namespace mini {

/**
 * @class MacroMapper
 * @brief Intelligent mapper from logical cell types to physical LEF macros
 * @details This class acts as a translator between the logical world (Verilog)
 *          and the physical world (LEF), implementing various matching strategies.
 */
class MacroMapper {
public:
    /**
     * @brief Constructor
     * @param lef_lib Reference to the LEF library containing physical macros
     */
    explicit MacroMapper(const LefLibrary& lef_lib);

    /**
     * @brief Map logical cell type to best matching physical macro
     * @param cell_type Logical cell type from netlist (e.g., "NAND2")
     * @return Pointer to matched LefMacro, nullptr if no match found
     * @details Matching strategy:
     *          1. Exact match with cell_type
     *          2. Try common drive strength suffixes (_X1, _X2, etc.)
     *          3. Fallback to alternative naming conventions
     */
    const LefMacro* mapType(const std::string& cell_type) const;

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
    const LefLibrary& lef_lib_;
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
        {"NOT", "INV"},           // NOT gate -> INV in LEF
        {"AND", "AND2"},          // Default to 2-input
        {"NAND", "NAND2"},        // Default to 2-input
        {"OR", "OR2"},            // Default to 2-input
        {"NOR", "NOR2"},          // Default to 2-input
        {"XOR", "XOR2"},          // Default to 2-input
        {"XNOR", "XNOR2"},        // Default to 2-input
        {"BUF", "BUF_X1"},        // Buffer
        {"DFF", "DFF_X1"}         // D Flip-Flop
    };

    /**
     * @brief Try exact match with given name
     * @param name Name to search for
     * @return Pointer to LefMacro if found, nullptr otherwise
     */
    const LefMacro* tryExactMatch(const std::string& name) const;

    /**
     * @brief Try matching with drive strength suffixes
     * @param base_name Base name without suffix
     * @return Pointer to LefMacro if found, nullptr otherwise
     */
    const LefMacro* tryDriveStrengthMatch(const std::string& base_name) const;

    /**
     * @brief Try alternative naming conventions
     * @param cell_type Original cell type
     * @return Pointer to LefMacro if found, nullptr otherwise
     */
    const LefMacro* tryAlternativeNames(const std::string& cell_type) const;

    /**
     * @brief Debug logging helper
     * @param message Debug message to print
     */
    void debugLog(const std::string& message) const;
};

} // namespace mini

#endif // MINI_MACRO_MAPPER_H