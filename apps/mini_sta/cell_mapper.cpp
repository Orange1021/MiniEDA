/**
 * @file cell_mapper.cpp
 * @brief Cell Mapper Implementation for STA
 */

#include "cell_mapper.h"
#include <algorithm>
#include <cctype>
#include <string>

namespace mini {

CellMapper::CellMapper(const Library& library) 
    : library_(library), debug_enabled_(false), successful_mappings_(0), total_attempts_(0) {
}

const LibCell* CellMapper::mapType(const std::string& cell_type) const {
    total_attempts_++;
    
    debugLog("Mapping cell type: " + cell_type);
    
    // Strategy 1: Try exact match first
    const LibCell* cell = tryExactMatch(cell_type);
    if (cell) {
        successful_mappings_++;
        debugLog("  ✓ Exact match found: " + cell_type);
        return cell;
    }
    
    // Strategy 2: Try alternative naming conventions
    cell = tryAlternativeNames(cell_type);
    if (cell) {
        successful_mappings_++;
        debugLog("  ✓ Alternative name match: " + cell_type + " -> " + cell->name);
        return cell;
    }
    
    // Strategy 3: Try drive strength suffixes with alternative base names
    for (const auto& alt_pair : alternative_names_) {
        if (alt_pair.first == cell_type) {
            cell = tryDriveStrengthMatch(alt_pair.second);
            if (cell) {
                successful_mappings_++;
                debugLog("  ✓ Drive strength match: " + cell_type + " -> " + cell->name);
                return cell;
            }
        }
    }
    
    // Strategy 4: Try drive strength suffixes with original name
    cell = tryDriveStrengthMatch(cell_type);
    if (cell) {
        successful_mappings_++;
        debugLog("  ✓ Drive strength match: " + cell_type + " -> " + cell->name);
        return cell;
    }
    
    // Strategy 5: Try pattern-based matching for complex cells
    cell = tryPatternMatch(cell_type);
    if (cell) {
        successful_mappings_++;
        debugLog("  ✓ Pattern match: " + cell_type + " -> " + cell->name);
        return cell;
    }
    
    // Strategy 6: Try fuzzy matching (remove suffixes, prefixes)
    cell = tryFuzzyMatch(cell_type);
    if (cell) {
        successful_mappings_++;
        debugLog("  ✓ Fuzzy match: " + cell_type + " -> " + cell->name);
        return cell;
    }
    
    debugLog("  ✗ No match found for: " + cell_type);
    return nullptr;
}

const LibCell* CellMapper::tryExactMatch(const std::string& name) const {
    return library_.getCell(name);
}

const LibCell* CellMapper::tryDriveStrengthMatch(const std::string& base_name) const {
    for (const std::string& suffix : drive_strength_suffixes_) {
        std::string candidate_name = base_name + suffix;
        const LibCell* cell = library_.getCell(candidate_name);
        if (cell) {
            debugLog("    Trying: " + candidate_name + " - SUCCESS");
            return cell;
        } else {
            debugLog("    Trying: " + candidate_name + " - not found");
        }
    }
    return nullptr;
}

const LibCell* CellMapper::tryAlternativeNames(const std::string& cell_type) const {
    for (const auto& alt_pair : alternative_names_) {
        if (alt_pair.first == cell_type) {
            const LibCell* cell = library_.getCell(alt_pair.second);
            if (cell) {
                debugLog("    Alternative: " + alt_pair.second + " - SUCCESS");
                return cell;
            } else {
                debugLog("    Alternative: " + alt_pair.second + " - not found");
            }
        }
    }
    return nullptr;
}

const LibCell* CellMapper::tryPatternMatch(const std::string& cell_type) const {
    // Pattern 1: Multi-input gates (extract number from name)
    if (cell_type.find("AND") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "AND" + std::to_string(num_inputs) + "_X1";
            const LibCell* cell = library_.getCell(candidate);
            if (cell) {
                debugLog("    Pattern AND match: " + candidate + " - SUCCESS");
                return cell;
            }
        }
    }
    
    if (cell_type.find("NAND") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "NAND" + std::to_string(num_inputs) + "_X1";
            const LibCell* cell = library_.getCell(candidate);
            if (cell) {
                debugLog("    Pattern NAND match: " + candidate + " - SUCCESS");
                return cell;
            }
        }
    }
    
    if (cell_type.find("OR") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "OR" + std::to_string(num_inputs) + "_X1";
            const LibCell* cell = library_.getCell(candidate);
            if (cell) {
                debugLog("    Pattern OR match: " + candidate + " - SUCCESS");
                return cell;
            }
        }
    }
    
    if (cell_type.find("NOR") == 0) {
        size_t num_inputs = extractInputCount(cell_type);
        if (num_inputs >= 2 && num_inputs <= 4) {
            std::string candidate = "NOR" + std::to_string(num_inputs) + "_X1";
            const LibCell* cell = library_.getCell(candidate);
            if (cell) {
                debugLog("    Pattern NOR match: " + candidate + " - SUCCESS");
                return cell;
            }
        }
    }
    
    // Pattern 2: Sequential elements with suffixes
    if (cell_type.find("DFF") == 0) {
        // Try different DFF variants
        std::vector<std::string> dff_variants = {"DFF_X1", "DFFR_X1", "DFFS_X1", "DFFRS_X1", 
                                                 "SDFF_X1", "SDFFR_X1", "SDFFS_X1", "SDFFRS_X1"};
        for (const auto& variant : dff_variants) {
            const LibCell* cell = library_.getCell(variant);
            if (cell) {
                debugLog("    Pattern DFF match: " + variant + " - SUCCESS");
                return cell;
            }
        }
    }
    
    // Pattern 3: AOI/OAI complex gates
    if (cell_type.find("AOI") == 0 || cell_type.find("OAI") == 0) {
        std::string base_name = cell_type.substr(0, cell_type.find('_'));
        std::string candidate = base_name + "_X1";
        const LibCell* cell = library_.getCell(candidate);
        if (cell) {
            debugLog("    Pattern complex gate match: " + candidate + " - SUCCESS");
            return cell;
        }
    }
    
    return nullptr;
}

const LibCell* CellMapper::tryFuzzyMatch(const std::string& cell_type) const {
    // Strategy 1: Remove common suffixes and try again
    std::string fuzzy_name = cell_type;
    
    // Remove common suffixes
    std::vector<std::string> suffixes_to_remove = {"_1", "_2", "_4", "_8", "_X1", "_X2", "_X4", "_X8"};
    for (const auto& suffix : suffixes_to_remove) {
        if (fuzzy_name.length() > suffix.length() && 
            fuzzy_name.substr(fuzzy_name.length() - suffix.length()) == suffix) {
            std::string base_name = fuzzy_name.substr(0, fuzzy_name.length() - suffix.length());
            const LibCell* cell = library_.getCell(base_name);
            if (cell) {
                debugLog("    Fuzzy suffix removal: " + base_name + " - SUCCESS");
                return cell;
            }
        }
    }
    
    // Strategy 2: Try case-insensitive matching
    std::string lower_name = cell_type;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    
    // Check all cells in library for case-insensitive match
    // Note: This is inefficient but useful for debugging
    // In a real implementation, we'd maintain a case-insensitive index
    
    // Strategy 3: Try substring matching
    // This would require iterating through all cells, which is expensive
    // For now, we'll skip this to maintain performance
    
    return nullptr;
}

size_t CellMapper::extractInputCount(const std::string& cell_type) const {
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

void CellMapper::debugLog(const std::string& message) const {
    if (debug_enabled_) {
        std::cout << "[CellMapper] " << message << std::endl;
    }
}

} // namespace mini