/**
 * @file main_placer.cpp
 * @brief MiniPlacement Main Entry Point
 * @details Command line interface for the Placement tool
 */

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>

#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../../lib/include/liberty_parser.h"
#include "../../lib/include/lef_parser.h"
#include "../../lib/include/visualizer.h"
#include "../../lib/include/app_config.h"
#include "../../lib/include/arg_parser.h"
#include "placer_db.h"
#include "placer_engine.h"
#include "macro_mapper.h"

using namespace mini;

/**
 * @brief Auto-detect row height from LEF library (with fallback to config)
 */
double detectRowHeight(const AppConfig& config) {
    double row_height = config.row_height;  // Start with config value
    
    if (!config.lef_file.empty()) {
        try {
            LefParser lef_parser(config.lef_file, config.verbose);
            auto lef_library = std::make_unique<LefLibrary>(lef_parser.parse());
            if (lef_library && !lef_library->macros.empty()) {
                // Use first macro's height as row height
                const LefMacro& macro = lef_library->macros.begin()->second;
                row_height = macro.height;
                if (config.verbose) {
                    std::cout << "  Auto-detected row height: " << row_height 
                              << " um from LEF macro: " << macro.name << std::endl;
                }
            }
        } catch (const std::exception& e) {
            if (config.verbose) {
                std::cout << "  Warning: Failed to parse LEF for row height: " 
                          << e.what() << std::endl;
                std::cout << "  Using configured row height: " << row_height << " um" << std::endl;
            }
        }
    }
    
    return row_height;
}

int main(int argc, char* argv[]) {
    std::cout << "MiniPlacement - Chip Placement Tool" << std::endl;
    std::cout << "===================================" << std::endl;

    // ========================================================================
    // 1. Parse unified configuration
    // ========================================================================
    AppConfig config;
    if (!parseAndValidate(argc, argv, config)) {
        return 1;
    }
    
    // Force placement mode
    if (!config.placement_mode) {
        std::cout << "Note: Auto-switching to placement mode" << std::endl;
        config.placement_mode = true;
        config.flow_mode = false;
        config.sta_mode = false;
        config.routing_mode = false;
    }
    
    // Auto-generate run_id from circuit name
    config.generateRunId();

    auto start_time = std::chrono::high_resolution_clock::now();

    // Use configured run_id
    std::cout << "Run ID: " << config.run_id << std::endl;

    try {
        // ========================================================================
    // 2. Parse Verilog & Build Netlist
    // ========================================================================
    std::cout << "\n=== Parsing Verilog Netlist ===" << std::endl;
    std::cout << "Reading: " << config.verilog_file << std::endl;
    
    NetlistDB db("design");
    VerilogParser v_parser;
    if (!v_parser.parseFile(config.verilog_file, db)) {
        std::cerr << "Error: Failed to parse Verilog file" << std::endl;
        return 1;
    }

    if (config.verbose) {
        std::cout << "  Parsed " << db.getCells().size() << " cells" << std::endl;
        std::cout << "  Parsed " << db.getNets().size() << " nets" << std::endl;
    }

    // ========================================================================
    // 3. Parse Liberty Library
    // ========================================================================
    std::cout << "\n=== Parsing Liberty Library ===" << std::endl;
    std::cout << "Reading: " << config.liberty_file << std::endl;
    
    LibertyParser l_parser;
    auto library = l_parser.parseFile(config.liberty_file);
        if (!library) {
            std::cerr << "Error: Failed to parse Liberty file: " << l_parser.getError() << std::endl;
            return 1;
        }

        std::cout << "  Loaded library: " << library->getName() 
                  << " with " << library->getCellCount() << " cells" << std::endl;

        // 3. Parse LEF Physical Library (if provided)
        std::unique_ptr<LefLibrary> lef_library;
        std::unique_ptr<MacroMapper> macro_mapper;
        bool use_lef = false;
        
        if (!config.lef_file.empty()) {
            std::cout << "\n=== Reading LEF Physical Library ===" << std::endl;
            std::cout << "Reading: " << config.lef_file << std::endl;
            try {
                LefParser lef_parser(config.lef_file, config.verbose);
                lef_library = std::make_unique<LefLibrary>(lef_parser.parse());
                use_lef = true;
                
                std::cout << "  Loaded LEF library with " << lef_library->getMacroCount() << " macros" << std::endl;
                
                // Initialize macro mapper
                macro_mapper = std::make_unique<MacroMapper>(*lef_library);
                macro_mapper->setDebugMode(true);  // Enable detailed mapping logs
                
                // Show some macro examples
                auto macro_names = lef_library->getMacroNames();
                std::cout << "  Available macros: ";
                for (size_t i = 0; i < std::min(size_t(5), macro_names.size()); ++i) {
                    std::cout << macro_names[i];
                    if (i < std::min(size_t(5), macro_names.size()) - 1) {
                        std::cout << ", ";
                    }
                }
                if (macro_names.size() > 5) {
                    std::cout << "... (" << (macro_names.size() - 5) << " more)";
                }
                std::cout << std::endl;
                
            } catch (const std::exception& e) {
                std::cerr << "Warning: Failed to parse LEF file: " << e.what() << std::endl;
                std::cerr << "Continuing with Liberty-based cell sizing..." << std::endl;
                use_lef = false;
            }
        }

        // 4. Initialize Physical Database
        std::cout << "\nInitializing Physical Database..." << std::endl;
        
        // Detect actual row height from LEF (override config if available)
        double actual_row_height = detectRowHeight(config);
        
        PlacerDB placer_db(&db);
        placer_db.setRowHeight(actual_row_height);
        std::cout << "  Row height: " << actual_row_height << " micrometers" << std::endl;
        
        // We'll calculate core area after adding cells

        // Add cells with dimensions from LEF or Liberty library
        int lef_matched_cells = 0;
        int liberty_matched_cells = 0;
        int fallback_cells = 0;
        
        for (const auto& cell_ptr : db.getCells()) {
            Cell* cell = cell_ptr.get();
            double cell_width = 0.0;
            double cell_height = 0.0;
            double cell_area = 0.0;
            bool size_found = false;
            
            // First try LEF library (if available)
            if (use_lef && macro_mapper) {
                const LefMacro* lef_macro = macro_mapper->mapType(cell->getTypeString());
                
                if (lef_macro) {
                    cell_width = lef_macro->width;
                    cell_height = lef_macro->height;
                    cell_area = lef_macro->getArea();
                    size_found = true;
                    lef_matched_cells++;
                    
                    // Store LEF macro pointer in PlacerDB for future use
                    placer_db.setCellLefMacro(cell, lef_macro);
                }
            }
            
            // Fallback to Liberty library if LEF not available or cell not found
            if (!size_found) {
                const LibCell* lib_cell = library->getCell(cell->getTypeString());
                if (lib_cell) {
                    cell_area = lib_cell->area;
                    size_found = true;
                    liberty_matched_cells++;
                }
            }
            
            // Final fallback: use default dimensions
            if (!size_found) {
                cell_area = 10.0;  // 10 square micrometers
                fallback_cells++;
                
                std::cout << "  Warning: Cell " << cell->getName() 
                          << " (" << cell->getTypeString() << ") not found in any library, using default size" << std::endl;
            }
            
            // Add cell with proper dimensions
            if (cell_width > 0.0 && cell_height > 0.0) {
                // Use precise dimensions from LEF or explicit calculation
                placer_db.addCell(cell, cell_width, cell_height);
                // Debug: LEF or explicit path
                std::cout << "    DEBUG: " << cell->getName() << " using LEF/explicit: " << cell_width << "x" << cell_height << std::endl;
            } else if (size_found) {
                // Use Liberty area with proper row height sizing
                placer_db.addCell(cell, cell_area);
                // Debug: Liberty area path
                if (config.verbose) {
                    std::cout << "    DEBUG: " << cell->getName() << " using Liberty area: " << cell_area << ", expected height: " << detectRowHeight(config) << std::endl;
                }
            } else {
                // Use default dimensions
                placer_db.addCell(cell, cell_width, cell_height);
                // Debug: Default path
                std::cout << "    DEBUG: " << cell->getName() << " using default: " << cell_width << "x" << cell_height << std::endl;
            }
            // cell_area will be accumulated later
        }
        
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

        // Row height already detected and set above

        // Calculate and set core area based on total cell area and utilization
        double total_cell_area = 0.0;
        for (const auto& cell_ptr : db.getCells()) {
            Cell* cell = cell_ptr.get();
            const auto& info = placer_db.getCellInfo(cell);
            total_cell_area += info.width * info.height;
        }
        
        // Calculate core area based on utilization with row height alignment
        double core_area_needed = total_cell_area / config.utilization;
        double core_width = std::sqrt(core_area_needed);
        
        // Align core height to row height
        int num_rows = static_cast<int>(std::ceil(core_width / actual_row_height));
        double core_height = num_rows * actual_row_height;
        
        Rect core_area(0, 0, core_width, core_height);
        placer_db.setCoreArea(core_area);
        
        if (config.verbose) {
            std::cout << "  Total cell area: " << total_cell_area << " square micrometers" << std::endl;
            std::cout << "  Target utilization: " << (config.utilization * 100) << "%" << std::endl;
            std::cout << "  Core area: " << core_width << " x " << core_height 
                      << " = " << (core_width * core_height) << " square micrometers" << std::endl;
            std::cout << "  Row capacity: " << num_rows << " rows (each " << actual_row_height << " μm tall)" << std::endl;
        }

        // 4. Initialize Visualizer
        std::cout << "\nSetting up Visualizer..." << std::endl;
        Visualizer visualizer(&placer_db);

        // 5. Initialize Random Placement
        std::cout << "\nInitializing Random Placement..." << std::endl;
        placer_db.initializeRandom();
        visualizer.drawPlacementWithRunId("00_random", config.run_id);
        if (config.verbose) {
            std::cout << "  Random placement completed, saved to " << config.run_id << "_00_random.png" << std::endl;
        }

        // 6. Setup Placement Engine
        std::cout << "\nSetting up Placement Engine..." << std::endl;
        PlacerEngine engine(&placer_db);
        engine.setVisualizer(&visualizer);
        engine.setRunId(config.run_id);

        // 7. Run Global Placement
        std::cout << "\n=== Running Global Placement ===" << std::endl;
        engine.runGlobalPlacement();

        // 8. Run Legalization
        std::cout << "\n=== Running Legalization ===" << std::endl;
        engine.runLegalization();

        // 9. Run Detailed Placement
        std::cout << "\n=== Running Detailed Placement ===" << std::endl;
        engine.runDetailedPlacement();

        // 10. Final Statistics
        std::cout << "\n===================================" << std::endl;
        std::cout << "MiniPlacement Completed!" << std::endl;
        std::cout << "===================================" << std::endl;
        std::cout << "Final HPWL: " << engine.getCurrentHPWL() << std::endl;
        std::cout << "Output files in visualizations/ folder:" << std::endl;
        std::cout << "  - " << config.run_id << "_00_random.png (initial placement)" << std::endl;
        std::cout << "  - " << config.run_id << "_quadratic_iter_*.png (global placement progress)" << std::endl;
        std::cout << "  - " << config.run_id << "_legalized.png (after legalization)" << std::endl;
        std::cout << "  - " << config.run_id << "_detailed.png (final optimized placement)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\nTotal runtime: " << duration.count() << " ms" << std::endl;
    return 0;
}