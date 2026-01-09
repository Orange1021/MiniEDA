/**
 * @file macro_mapper.cpp
 * @brief Macro Mapper Implementation
 */

#include "macro_mapper.h"
#include "../../lib/include/debug_log.h"
#include <algorithm>
#include <cctype>
#include <string>

namespace mini {

MacroMapper::MacroMapper(const LefLibrary& lef_lib) 
    : lef_lib_(lef_lib), successful_mappings_(0), total_attempts_(0) {
}

const LefMacro* MacroMapper::mapType(const std::string& cell_type) const {
    total_attempts_++;
    
    DEBUG_LOG("MacroMapper", "Mapping cell type: " + cell_type);
    
    // Strategy 1: Try exact match first
    const LefMacro* macro = tryExactMatch(cell_type);
    if (macro) {
        successful_mappings_++;
        DEBUG_LOG("MacroMapper", "  ✓ Exact match found: " + cell_type);
        return macro;
    }
    
    // Strategy 2: Try alternative naming conventions
    macro = tryAlternativeNames(cell_type);
    if (macro) {
        successful_mappings_++;
        DEBUG_LOG("MacroMapper", "  ✓ Alternative name match: " + cell_type + " -> " + macro->name);
        return macro;
    }
    
    // Strategy 3: Try drive strength suffixes with alternative base names
    for (const auto& alt_pair : alternative_names_) {
        if (alt_pair.first == cell_type) {
            macro = tryDriveStrengthMatch(alt_pair.second);
            if (macro) {
                successful_mappings_++;
                DEBUG_LOG("MacroMapper", "  ✓ Drive strength match: " + cell_type + " -> " + macro->name);
                return macro;
            }
        }
    }
    
    // Strategy 4: Try drive strength suffixes with original name
    macro = tryDriveStrengthMatch(cell_type);
    if (macro) {
        successful_mappings_++;
        DEBUG_LOG("MacroMapper", "  ✓ Drive strength match: " + cell_type + " -> " + macro->name);
        return macro;
    }
    
    // Strategy 5: Try pattern-based matching for complex cells
    macro = tryPatternMatch(cell_type);
    if (macro) {
        successful_mappings_++;
        DEBUG_LOG("MacroMapper", "  ✓ Pattern match: " + cell_type + " -> " + macro->name);
        return macro;
    }
    
    // Strategy 6: Try fuzzy matching (remove suffixes, prefixes)
    macro = tryFuzzyMatch(cell_type);
    if (macro) {
        successful_mappings_++;
        DEBUG_LOG("MacroMapper", "  ✓ Fuzzy match: " + cell_type + " -> " + macro->name);
        return macro;
    }
    
    DEBUG_LOG("MacroMapper", "  ✗ No match found for: " + cell_type);
    return nullptr;
}

const LefMacro* MacroMapper::tryExactMatch(const std::string& name) const {
    return lef_lib_.getMacro(name);
}

const LefMacro* MacroMapper::tryDriveStrengthMatch(const std::string& base_name) const {
    for (const std::string& suffix : drive_strength_suffixes_) {
        std::string candidate_name = base_name + suffix;
        const LefMacro* macro = lef_lib_.getMacro(candidate_name);
        if (macro) {
            DEBUG_LOG("MacroMapper", "    Trying: " + candidate_name + " - SUCCESS");
            return macro;
        } else {
            DEBUG_LOG("MacroMapper", "    Trying: " + candidate_name + " - not found");
        }
    }
    return nullptr;
}

const LefMacro* MacroMapper::tryAlternativeNames(const std::string& cell_type) const {
    for (const auto& alt_pair : alternative_names_) {
        if (alt_pair.first == cell_type) {
            const LefMacro* macro = lef_lib_.getMacro(alt_pair.second);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Alternative: " + alt_pair.second + " - SUCCESS");
                return macro;
            } else {
                DEBUG_LOG("MacroMapper", "    Alternative: " + alt_pair.second + " - not found");
            }
        }
    }
    return nullptr;
}

const LefMacro* MacroMapper::tryPatternMatch(const std::string& cell_type) const {
    // Pattern 1: Multi-input gates (extract number from name)
    if (cell_type.find("AND") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "AND" + std::to_string(num_inputs) + "_X1";
            const LefMacro* macro = lef_lib_.getMacro(candidate);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Pattern AND match: " + candidate + " - SUCCESS");
                return macro;
            }
        }
    }
    
    if (cell_type.find("NAND") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "NAND" + std::to_string(num_inputs) + "_X1";
            const LefMacro* macro = lef_lib_.getMacro(candidate);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Pattern NAND match: " + candidate + " - SUCCESS");
                return macro;
            }
        }
    }
    
    if (cell_type.find("OR") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "OR" + std::to_string(num_inputs) + "_X1";
            const LefMacro* macro = lef_lib_.getMacro(candidate);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Pattern OR match: " + candidate + " - SUCCESS");
                return macro;
            }
        }
    }
    
    if (cell_type.find("NOR") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "NOR" + std::to_string(num_inputs) + "_X1";
            const LefMacro* macro = lef_lib_.getMacro(candidate);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Pattern NOR match: " + candidate + " - SUCCESS");
                return macro;
            }
        }
    }
    
    // Pattern 2: Sequential elements with suffixes
    if (cell_type.find("DFF") == 0) {
        // Try different DFF variants
        std::vector<std::string> dff_variants = {"DFF_X1", "DFFR_X1", "DFFS_X1", "DFFRS_X1", 
                                                 "SDFF_X1", "SDFFR_X1", "SDFFS_X1", "SDFFRS_X1"};
        for (const auto& variant : dff_variants) {
            const LefMacro* macro = lef_lib_.getMacro(variant);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Pattern DFF match: " + variant + " - SUCCESS");
                return macro;
            }
        }
    }
    
    // Pattern 3: AOI/OAI complex gates
    if (cell_type.find("AOI") == 0 || cell_type.find("OAI") == 0) {
        std::string base_name = cell_type.substr(0, cell_type.find('_'));
        std::string candidate = base_name + "_X1";
        const LefMacro* macro = lef_lib_.getMacro(candidate);
        if (macro) {
            DEBUG_LOG("MacroMapper", "    Pattern complex gate match: " + candidate + " - SUCCESS");
            return macro;
        }
    }
    
    return nullptr;
}

const LefMacro* MacroMapper::tryFuzzyMatch(const std::string& cell_type) const {
    // Strategy 1: Remove common suffixes and try again
    std::string fuzzy_name = cell_type;
    
    // Remove common suffixes
    std::vector<std::string> suffixes_to_remove = {"_1", "_2", "_4", "_8", "_X1", "_X2", "_X4", "_X8"};
    for (const auto& suffix : suffixes_to_remove) {
        if (fuzzy_name.length() > suffix.length() && 
            fuzzy_name.substr(fuzzy_name.length() - suffix.length()) == suffix) {
            std::string base_name = fuzzy_name.substr(0, fuzzy_name.length() - suffix.length());
            const LefMacro* macro = lef_lib_.getMacro(base_name);
            if (macro) {
                DEBUG_LOG("MacroMapper", "    Fuzzy suffix removal: " + base_name + " - SUCCESS");
                return macro;
            }
        }
    }
    
    // Strategy 2: Try case-insensitive matching
    std::string lower_name = cell_type;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    
    // Check all macros in library for case-insensitive match
    for (const auto& macro_pair : lef_lib_.macros) {
        std::string macro_lower = macro_pair.first;
        std::transform(macro_lower.begin(), macro_lower.end(), macro_lower.begin(), ::tolower);
        if (macro_lower == lower_name) {
            DEBUG_LOG("MacroMapper", "    Fuzzy case-insensitive: " + macro_pair.first + " - SUCCESS");
            return &macro_pair.second;
        }
    }
    
    // Strategy 3: Try substring matching
    for (const auto& macro_pair : lef_lib_.macros) {
        if (macro_pair.first.find(cell_type) != std::string::npos || 
            cell_type.find(macro_pair.first) != std::string::npos) {
            DEBUG_LOG("MacroMapper", "    Fuzzy substring: " + macro_pair.first + " - SUCCESS");
            return &macro_pair.second;
        }
    }
    
    return nullptr;
}

size_t MacroMapper::extractInputCount(const std::string& cell_type) const {
    // Extract number from cell type name (e.g., "AND2" -> 2, "NAND3" -> 3)
    std::string digits;
    for (char c : cell_type) {
        if (isdigit(c)) {
            digits += c;
        }
    }
    
    if (!digits.empty()) {
        return std::stoi(digits);
    }
    
    // Default to 2 inputs for gates without explicit number
    return 2;
}

} // namespace mini