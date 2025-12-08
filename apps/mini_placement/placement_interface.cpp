/**
 * @file placement_interface.cpp
 * @brief High-level Placement Interface Implementation
 */

#include "placement_interface.h"
#include "../../lib/include/placer_db.h"
#include "placer_engine.h"
#include "macro_mapper.h"
#include "../../lib/include/visualizer.h"
#include "../../lib/include/lef_parser.h"
#include "../../lib/include/liberty_parser.h"

#include <iostream>
#include <memory>

namespace mini {

std::unique_ptr<PlacerDB> PlacementInterface::runPlacement(
    const PlacementConfig& config,
    std::shared_ptr<NetlistDB> netlist_db) {
    
    return runPlacementWithVisualization(config, netlist_db, nullptr);
}

std::unique_ptr<PlacerDB> PlacementInterface::runPlacementWithVisualization(
    const PlacementConfig& config,
    std::shared_ptr<NetlistDB> netlist_db,
    Visualizer* visualizer) {
    
    if (config.verbose) {
        std::cout << "\n=== Running Placement Flow ===" << std::endl;
    }
    
    // Parse LEF physical library (if provided)
    std::unique_ptr<LefLibrary> lef_library;
    std::unique_ptr<MacroMapper> macro_mapper;
    bool use_lef = false;
    
    if (!config.lef_file.empty()) {
        if (config.verbose) {
            std::cout << "Reading LEF physical library: " << config.lef_file << std::endl;
        }
        try {
            LefParser lef_parser(config.lef_file, false);
            lef_library = std::make_unique<LefLibrary>(lef_parser.parse());
            use_lef = true;
            
            if (config.verbose) {
                std::cout << "  Loaded LEF library with " << lef_library->getMacroCount() << " macros" << std::endl;
            }
            
            // Initialize macro mapper
            macro_mapper = std::make_unique<MacroMapper>(*lef_library);
            macro_mapper->setDebugMode(config.verbose);
            
            // Auto-detect technology row height from LEF library
            double tech_row_height = config.row_height; // Default fallback
            bool found_macro = false;
            
            // Iterate through all macros to find the first one (should be a standard cell)
            for (const auto& macro_pair : lef_library->macros) {
                const LefMacro& macro = macro_pair.second;
                if (macro.height > 0) { // Valid macro with positive height
                    tech_row_height = macro.height;
                    found_macro = true;
                    if (config.verbose) {
                        std::cout << "  Auto-detected technology row height: " << tech_row_height 
                                  << " μm from LEF macro: " << macro.name << std::endl;
                    }
                    break; // Use first valid macro found
                }
            }
            
            if (!found_macro && config.verbose) {
                std::cout << "  Warning: No valid macro found in LEF, using default row height: " 
                          << tech_row_height << " μm" << std::endl;
            }
            
            // Update config with detected row height
            const_cast<PlacementConfig&>(config).row_height = tech_row_height;
            
        } catch (const std::exception& e) {
            std::cerr << "Error parsing LEF file: " << e.what() << std::endl;
            return nullptr;
        }
    }
    
    // Initialize placement database
    auto placer_db = std::make_unique<PlacerDB>(netlist_db.get());
    
    // Parse Liberty library for cell sizing
    std::unique_ptr<Library> liberty_library;
    if (!config.liberty_file.empty()) {
        LibertyParser lib_parser;
        liberty_library = std::unique_ptr<Library>(lib_parser.parseFile(config.liberty_file));
    }
    
    // Add cells to placement database with proper sizing
    double total_cell_area = 0.0;
    int lef_matched_cells = 0;
    int liberty_matched_cells = 0;
    int fallback_cells = 0;
    
    for (const auto& cell : netlist_db->getCells()) {
        double cell_width = 0.0;
        double cell_height = 0.0;
        double cell_area = 0.0;
        
        // Try LEF first (if available)
        if (macro_mapper) {
            const LefMacro* macro = macro_mapper->mapType(cell->getTypeString());
            if (macro) {
                cell_width = macro->width;
                cell_height = macro->height;
                cell_area = cell_width * cell_height;
                placer_db->setCellLefMacro(cell.get(), macro);
                lef_matched_cells++;
            }
        }
        
        // Fallback to Liberty if LEF failed
        if (cell_area == 0.0 && liberty_library) {
            const LibCell* lib_cell = liberty_library->getCell(cell->getTypeString());
            if (lib_cell) {
                cell_area = lib_cell->area;
                // Use proper row height for Liberty cells
                double row_height = config.row_height;
                cell_width = cell_area / row_height;
                cell_height = row_height;
                liberty_matched_cells++;
            }
        }
        
        // Final fallback to default size
        if (cell_area == 0.0) {
            cell_area = 10.0;  // 10 square micrometers
            double size = std::sqrt(cell_area);
            cell_width = size;
            cell_height = size;
            fallback_cells++;
        }
        
        placer_db->addCell(cell.get(), cell_width, cell_height);
        total_cell_area += cell_area;
    }
    
    if (config.verbose) {
        std::cout << "  Cell sizing breakdown:" << std::endl;
        std::cout << "    LEF-based: " << lef_matched_cells << " cells" << std::endl;
        std::cout << "    Liberty-based: " << liberty_matched_cells << " cells" << std::endl;
        std::cout << "    Default fallback: " << fallback_cells << " cells" << std::endl;
        
        // Show macro mapping statistics if available
        if (macro_mapper) {
            auto stats = macro_mapper->getStats();
            std::cout << "  Macro mapping statistics:" << std::endl;
            std::cout << "    Mapping success rate: " << stats.first << "/" << stats.second 
                      << " (" << (100.0 * stats.first / stats.second) << "%)" << std::endl;
        }
    }
    
    // Calculate core area based on utilization with row height alignment
    double core_area_needed = total_cell_area / config.utilization;
    double core_width = std::sqrt(core_area_needed);
    
    // Align core height to row height
    double row_height = config.row_height;
    int num_rows = static_cast<int>(std::ceil(core_width / row_height));
    double core_height = num_rows * row_height;
    
    Rect core_area(0, 0, core_width, core_height);
    placer_db->setCoreArea(core_area);
    placer_db->setRowHeight(config.row_height);
    
    if (config.verbose) {
        std::cout << "  Total cell area: " << total_cell_area << " square micrometers" << std::endl;
        std::cout << "  Target utilization: " << (config.utilization * 100) << "%" << std::endl;
        std::cout << "  Core area: " << core_width << " x " << core_height 
                  << " = " << (core_width * core_height) << " square micrometers" << std::endl;
        std::cout << "  Row capacity: " << num_rows << " rows (each " << row_height << " μm tall)" << std::endl;
    }
    
    // Initialize Visualizer (after PlacerDB is created)
    std::unique_ptr<Visualizer> internal_visualizer;
    if (!visualizer) {
        if (config.verbose) {
            std::cout << "\nSetting up Visualizer..." << std::endl;
        }
        internal_visualizer = std::make_unique<Visualizer>(placer_db.get());
        visualizer = internal_visualizer.get();
    }
    
    // Initialize random placement
    if (config.verbose) {
        std::cout << "\nInitializing Random Placement..." << std::endl;
    }
    placer_db->initializeRandom();
    if (visualizer) {
        visualizer->drawPlacementWithRunId("00_random.png", config.run_id);
    }
    if (config.verbose) {
        std::cout << "  Random placement completed" << std::endl;
    }
    
    // Run placement engine
    if (config.verbose) {
        std::cout << "\nSetting up Placement Engine..." << std::endl;
    }
    PlacerEngine engine(placer_db.get());
    if (visualizer) {
        engine.setVisualizer(visualizer);
    }
    engine.setRunId(config.run_id);
    
    if (config.verbose) {
        std::cout << "\n=== Running Global Placement ===" << std::endl;
    }
    engine.runGlobalPlacement();
    
    if (config.verbose) {
        std::cout << "\n=== Running Legalization ===" << std::endl;
    }
    engine.runLegalization();
    
    if (config.verbose) {
        std::cout << "\n=== Running Detailed Placement ===" << std::endl;
    }
    engine.runDetailedPlacement();
    
    if (config.verbose) {
        std::cout << "  Final HPWL: " << engine.getCurrentHPWL() << std::endl;
    }
    
    return placer_db;
}

} // namespace mini