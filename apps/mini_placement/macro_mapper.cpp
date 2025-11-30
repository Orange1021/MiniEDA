/**
 * @file macro_mapper.cpp
 * @brief Macro Mapper Implementation
 */

#include "macro_mapper.h"
#include <algorithm>

namespace mini {

MacroMapper::MacroMapper(const LefLibrary& lef_lib) 
    : lef_lib_(lef_lib), debug_enabled_(false), successful_mappings_(0), total_attempts_(0) {
}

const LefMacro* MacroMapper::mapType(const std::string& cell_type) const {
    total_attempts_++;
    
    debugLog("Mapping cell type: " + cell_type);
    
    // Strategy 1: Try exact match first
    const LefMacro* macro = tryExactMatch(cell_type);
    if (macro) {
        successful_mappings_++;
        debugLog("  ✓ Exact match found: " + cell_type);
        return macro;
    }
    
    // Strategy 2: Try alternative naming conventions
    macro = tryAlternativeNames(cell_type);
    if (macro) {
        successful_mappings_++;
        debugLog("  ✓ Alternative name match: " + cell_type + " -> " + macro->name);
        return macro;
    }
    
    // Strategy 3: Try drive strength suffixes with alternative base names
    for (const auto& alt_pair : alternative_names_) {
        if (alt_pair.first == cell_type) {
            macro = tryDriveStrengthMatch(alt_pair.second);
            if (macro) {
                successful_mappings_++;
                debugLog("  ✓ Drive strength match: " + cell_type + " -> " + macro->name);
                return macro;
            }
        }
    }
    
    // Strategy 4: Try drive strength suffixes with original name
    macro = tryDriveStrengthMatch(cell_type);
    if (macro) {
        successful_mappings_++;
        debugLog("  ✓ Drive strength match: " + cell_type + " -> " + macro->name);
        return macro;
    }
    
    debugLog("  ✗ No match found for: " + cell_type);
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
            debugLog("    Trying: " + candidate_name + " - SUCCESS");
            return macro;
        } else {
            debugLog("    Trying: " + candidate_name + " - not found");
        }
    }
    return nullptr;
}

const LefMacro* MacroMapper::tryAlternativeNames(const std::string& cell_type) const {
    for (const auto& alt_pair : alternative_names_) {
        if (alt_pair.first == cell_type) {
            const LefMacro* macro = lef_lib_.getMacro(alt_pair.second);
            if (macro) {
                debugLog("    Alternative: " + alt_pair.second + " - SUCCESS");
                return macro;
            } else {
                debugLog("    Alternative: " + alt_pair.second + " - not found");
            }
        }
    }
    return nullptr;
}

void MacroMapper::debugLog(const std::string& message) const {
    if (debug_enabled_) {
        std::cout << "[MacroMapper] " << message << std::endl;
    }
}

} // namespace mini