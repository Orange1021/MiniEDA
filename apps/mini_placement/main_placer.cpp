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
#include "placer_db.h"
#include "placer_engine.h"
#include "visualizer.h"
#include "macro_mapper.h"

using namespace mini;

void printUsage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] -v <verilog_file> -lib <liberty_file> -lef <lef_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v <file>        Verilog netlist file (required)" << std::endl;
    std::cout << "  -lib <file>      Liberty library file (required)" << std::endl;
    std::cout << "  -lef <file>      LEF physical library file (recommended for accurate sizing)" << std::endl;
    std::cout << "  -util <value>    Target utilization (default: 0.7)" << std::endl;
    std::cout << "  -rowheight <val> Row height in micrometers (default: 3.0)" << std::endl;
    std::cout << "  -help            Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << prog_name << " -v s27.v -lib sample.lib -lef nangate.lef" << std::endl;
    std::cout << "  " << prog_name << " -v s344.v -lib sample.lib -lef nangate.lef -util 0.8" << std::endl;
}

int main(int argc, char* argv[]) {
    std::cout << "MiniPlacement - Chip Placement Tool" << std::endl;
    std::cout << "===================================" << std::endl;

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    // Parse Command Line Arguments
    std::string verilog_file;
    std::string liberty_file;
    std::string lef_file;
    double utilization = 0.7;
    double row_height = 3.0;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v") {
            if (i + 1 < argc) {
                verilog_file = argv[++i];
            } else {
                std::cerr << "Error: -v requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "-lib") {
            if (i + 1 < argc) {
                liberty_file = argv[++i];
            } else {
                std::cerr << "Error: -lib requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "-util") {
            if (i + 1 < argc) {
                utilization = std::stod(argv[++i]);
                if (utilization <= 0.0 || utilization > 1.0) {
                    std::cerr << "Error: Utilization must be between 0.0 and 1.0" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -util requires a value argument" << std::endl;
                return 1;
            }
        } else if (arg == "-lef") {
            if (i + 1 < argc) {
                lef_file = argv[++i];
            } else {
                std::cerr << "Error: -lef requires a filename argument" << std::endl;
                return 1;
            }
        } else if (arg == "-rowheight") {
            if (i + 1 < argc) {
                row_height = std::stod(argv[++i]);
                if (row_height <= 0.0) {
                    std::cerr << "Error: Row height must be positive" << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Error: -rowheight requires a value argument" << std::endl;
                return 1;
            }
        } else if (arg == "-help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg[0] == '-') {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }

    // Validate required arguments
    if (verilog_file.empty()) {
        std::cerr << "Error: Verilog file is required (use -v)" << std::endl;
        return 1;
    }
    if (liberty_file.empty()) {
        std::cerr << "Error: Liberty file is required (use -lib)" << std::endl;
        return 1;
    }

    auto start_time = std::chrono::high_resolution_clock::now();

    // Generate timestamp for this run
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    timestamp << "_" << std::setfill('0') << std::setw(3) << ms.count();
    
    std::string run_id = timestamp.str();
    std::cout << "Run ID: " << run_id << std::endl;

    try {
        // 1. Parse Verilog & Build Netlist
        std::cout << "Reading Verilog file: " << verilog_file << std::endl;
        NetlistDB db("design");
        VerilogParser v_parser;
        if (!v_parser.parseFile(verilog_file, db)) {
            std::cerr << "Error: Failed to parse Verilog file" << std::endl;
            return 1;
        }

        std::cout << "  Parsed " << db.getCells().size() << " cells" << std::endl;
        std::cout << "  Parsed " << db.getNets().size() << " nets" << std::endl;

        // 2. Parse Liberty Library
        std::cout << "\nReading Liberty library: " << liberty_file << std::endl;
        LibertyParser l_parser;
        auto library = l_parser.parseFile(liberty_file);
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
        
        if (!lef_file.empty()) {
            std::cout << "\nReading LEF physical library: " << lef_file << std::endl;
            try {
                LefParser lef_parser(lef_file, false);  // verbose=false
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
        PlacerDB placer_db(&db);
        placer_db.setRowHeight(row_height);
        std::cout << "  Row height: " << row_height << " micrometers" << std::endl;

        // Add cells with dimensions from LEF or Liberty library
        double total_cell_area = 0.0;
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
                    // Estimate dimensions from area (assume square for fallback)
                    double size = std::sqrt(cell_area);
                    cell_width = size;
                    cell_height = size;
                    size_found = true;
                    liberty_matched_cells++;
                }
            }
            
            // Final fallback: use default dimensions
            if (!size_found) {
                cell_area = 10.0;  // 10 square micrometers
                double size = std::sqrt(cell_area);
                cell_width = size;
                cell_height = size;
                fallback_cells++;
                
                std::cout << "  Warning: Cell " << cell->getName() 
                          << " (" << cell->getTypeString() << ") not found in any library, using default size" << std::endl;
            }
            
            placer_db.addCell(cell, cell_width, cell_height);
            total_cell_area += cell_area;
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

        // Auto-compute core area with row snapping and original utilization
        double target_utilization = utilization;  // Use original utilization (0.7)
        double required_core_area = total_cell_area / target_utilization;
        double core_width = std::sqrt(required_core_area);  // Make it square
        
        double row_height = 3.0;  // Same as row_height set in PlacerDB
        int num_rows = static_cast<int>(std::ceil(core_width / row_height));
        double core_height = num_rows * row_height;
        
        Rect core_area(0.0, 0.0, core_width, core_height);
        placer_db.setCoreArea(core_area);

        std::cout << "  Total cell area: " << total_cell_area << " square micrometers" << std::endl;
        std::cout << "  Target utilization: " << (target_utilization * 100) << "%" << std::endl;
        std::cout << "  Core area: " << core_width << " x " << core_height 
                  << " = " << (core_width * core_height) << " square micrometers" << std::endl;
        std::cout << "  Row capacity: " << num_rows << " rows (each " << row_height << " μm tall)" << std::endl;

        // 4. Initialize Visualizer
        std::cout << "\nSetting up Visualizer..." << std::endl;
        Visualizer visualizer(&placer_db);

        // 5. Initialize Random Placement
        std::cout << "\nInitializing Random Placement..." << std::endl;
        placer_db.initializeRandom();
        visualizer.drawPlacementWithRunId("00_random", run_id);
        std::cout << "  Random placement completed, saved to " << run_id << "_00_random.png" << std::endl;

        // 6. Setup Placement Engine
        std::cout << "\nSetting up Placement Engine..." << std::endl;
        PlacerEngine engine(&placer_db);
        engine.setVisualizer(&visualizer);
        engine.setRunId(run_id);

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
        std::cout << "  - " << run_id << "_00_random.png (initial placement)" << std::endl;
        std::cout << "  - " << run_id << "_quadratic_iter_*.png (global placement progress)" << std::endl;
        std::cout << "  - " << run_id << "_legalized.png (after legalization)" << std::endl;
        std::cout << "  - " << run_id << "_detailed.png (final optimized placement)" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "\nTotal runtime: " << duration.count() << " ms" << std::endl;
    return 0;
}