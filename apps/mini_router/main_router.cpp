/**
 * @file main_router.cpp
 * @brief MiniRouter v2.0 - Main Program (Pipeline Pattern)
 */

#include "maze_router.h"
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../../lib/include/liberty_parser.h"
#include "../../lib/include/lef_parser.h"
#include "../../lib/include/pin_mapper.h"
#include "../../lib/include/visualizer.h"
#include "../../apps/mini_placement/placement_interface.h"
#include "../../apps/mini_placement/placer_db.h"
#include "../../apps/mini_placement/macro_mapper.h"
#include <iostream>
#include <memory>
#include <chrono>

using namespace mini;

void printUsage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <verilog_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v <file>        Verilog netlist file (required)" << std::endl;
    std::cout << "  -lib <file>      Liberty library file (required)" << std::endl;
    std::cout << "  -lef <file>      LEF physical library file (optional)" << std::endl;
    std::cout << "  -util <value>    Target utilization (default: 0.7)" << std::endl;
    std::cout << "  -via_cost <cost> Via penalty cost (default: 10.0)" << std::endl;
    std::cout << "  -wire_cost <cost> Wire cost per unit (default: 1.0)" << std::endl;
    std::cout << "  -help            Show this help message" << std::endl;
}

struct RouterConfig {
    std::string verilog_file;
    std::string liberty_file = "benchmarks/NangateOpenCellLibrary_typical.lib";
    std::string lef_file;
    double utilization = 0.7;
    double via_cost = 10.0;
    double wire_cost = 1.0;
};

bool parseArguments(int argc, char* argv[], RouterConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-help") {
            return false;
        } else if (arg == "-v" && i + 1 < argc) {
            config.verilog_file = argv[++i];
        } else if (arg == "-lib" && i + 1 < argc) {
            config.liberty_file = argv[++i];
        } else if (arg == "-lef" && i + 1 < argc) {
            config.lef_file = argv[++i];
        } else if (arg == "-util" && i + 1 < argc) {
            config.utilization = std::stod(argv[++i]);
        } else if (arg == "-via_cost" && i + 1 < argc) {
            config.via_cost = std::stod(argv[++i]);
        } else if (arg == "-wire_cost" && i + 1 < argc) {
            config.wire_cost = std::stod(argv[++i]);
        } else if (arg[0] != '-') {
            config.verilog_file = arg;
        } else {
            std::cerr << "Error: Unknown option " << arg << std::endl;
            return false;
        }
    }
    
    return !config.verilog_file.empty();
}

/**
 * @brief Auto-detect row height from LEF library
 * @param lef_lib LEF library to analyze
 * @return Row height in micrometers
 */
double autoDetectRowHeight(const LefLibrary& lef_lib) {
    std::cout << "Auto-detecting row height from LEF library..." << std::endl;
    
    // Find the first macro and get its height
    for (const auto& pair : lef_lib.macros) {
        const LefMacro& macro = pair.second;
        double height = macro.height;
        std::cout << "  Found macro '" << macro.name 
                  << "' with height: " << height << " um" << std::endl;
        return height;
    }
    
    // Fallback: use default height
    std::cout << "  Warning: No CORE macro found, using default height 3.0 um" << std::endl;
    return 3.0;
}

/**
 * @brief Helper function to place I/O ports
 * @param placer_db Placement database
 */
void placeIOPorts(PlacerDB& placer_db) {
    std::cout << "Placing I/O ports..." << std::endl;
    
    auto all_cells = placer_db.getAllCells();
    std::vector<Cell*> io_ports;
    
    // Separate I/O ports from standard cells
    for (Cell* cell : all_cells) {
        if (cell->isPort()) {
            io_ports.push_back(cell);
        }
    }
    
    // Place I/O ports around the periphery
    double core_width = placer_db.getCoreArea().width();
    double core_height = placer_db.getCoreArea().height();
    
    size_t io_count = io_ports.size();
    if (io_count == 0) return;
    
    // Distribute I/O ports evenly around the perimeter
    for (size_t i = 0; i < io_count; ++i) {
        Cell* port = io_ports[i];
        double position = static_cast<double>(i) / io_count * 2.0 * M_PI;
        
        double x, y;
        if (position < M_PI / 2) {
            // Top edge
            x = position / (M_PI / 2) * core_width;
            y = core_height;
        } else if (position < M_PI) {
            // Right edge
            x = core_width;
            y = core_height - (position - M_PI / 2) / (M_PI / 2) * core_height;
        } else if (position < 3 * M_PI / 2) {
            // Bottom edge
            x = core_width - (position - M_PI) / (M_PI / 2) * core_width;
            y = 0.0;
        } else {
            // Left edge
            x = 0.0;
            y = (position - 3 * M_PI / 2) / (M_PI / 2) * core_height;
        }
        
        // **EXPERIMENT**: Force some I/O ports to different rows to create vertical routing needs
        if (i % 3 == 0) {
            y = core_height * 0.8;  // Force to upper region
        } else if (i % 3 == 1) {
            y = core_height * 0.2;  // Force to lower region
        }
        
        port->setX(x);
        port->setY(y);
        port->setPlaced(true);
    }
    
    std::cout << "  Placed " << io_count << " I/O ports" << std::endl;
}

/**
 * @brief Build pin locations map for routing (CRITICAL data bridge)
 * @param placer_db Placement database with cell positions
 * @param lef_lib LEF library for physical pin information
 * @param macro_mapper Macro mapper for cell type mapping
 * @return Map of pin keys to physical coordinates
 */
std::unordered_map<std::string, Point> buildPinLocations(
    PlacerDB& placer_db,
    const LefLibrary& lef_lib,
    const MacroMapper& macro_mapper) {
    
    std::cout << "Building pin locations map..." << std::endl;
    
    // Create PinMapper for unified key generation
    auto pin_mapper = std::make_unique<PinMapper>(lef_lib, macro_mapper);
    std::unordered_map<std::string, Point> pin_locations;
    
    auto all_cells = placer_db.getAllCells();
    size_t total_pins = 0;
    
    for (Cell* cell : all_cells) {
        if (cell->isPort()) {
            // I/O Port handling - use unified key generation
            std::string key = pin_mapper->getPinKey(cell, nullptr);  // Port has no pin
            Point center = placer_db.getCellCenter(cell);
            pin_locations[key] = center;
            total_pins++;
            
        } else {
            // Standard Cell handling
            std::string cell_type_name = cellTypeToString(cell->getType());
            const LefMacro* macro = macro_mapper.mapType(cell_type_name);
            
            if (!macro) {
                std::cout << "  Warning: No LEF macro found for cell type " << cell_type_name << std::endl;
                continue;
            }
            
            // Process each pin of the cell
            for (const auto& pin_ptr : cell->getPins()) {
                Pin* pin = pin_ptr.get();

                // Generate unified key using the unified method
                std::string key = pin_mapper->getPinKey(cell, pin);

                // Calculate pin center offset from macro origin using LEF data
                Point pin_offset = Point(0, 0);  // Default offset

                // Try to get pin geometry from LEF
                if (macro->getPin(pin->getName())) {
                    const LefPort* port = macro->getPin(pin->getName());
                    if (port && !port->rects.empty()) {
                        // Use the center of the first rectangle as pin offset
                        const Rect& rect = port->rects[0];
                        pin_offset.x = (rect.x_min + rect.x_max) / 2.0;
                        pin_offset.y = (rect.y_min + rect.y_max) / 2.0;
                    }
                }

                Point cell_origin = Point(cell->getX(), cell->getY());
                Point abs_position = cell_origin + pin_offset;

                pin_locations[key] = abs_position;
                total_pins++;
            }
        }
    }
    
    std::cout << "  Built pin locations map with " << total_pins << " pins" << std::endl;
    return pin_locations;
}

int main(int argc, char* argv[]) {
    std::cout << "MiniRouter v2.0 - Pipeline Pattern Router" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    RouterConfig config;
    if (!parseArguments(argc, argv, config)) {
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // ====================================================================
        // Phase 1: 加载与配置 (Setup)
        // ====================================================================
        std::cout << "\n=== Phase 1: Loading and Configuration ===" << std::endl;
        
        // Parse Verilog netlist
        std::cout << "Parsing Verilog netlist: " << config.verilog_file << std::endl;
        VerilogParser v_parser;
        auto netlist_db = std::make_shared<NetlistDB>();
        if (!v_parser.parseFile(config.verilog_file, *netlist_db)) {
            std::cerr << "Error: Failed to parse Verilog file" << std::endl;
            return 1;
        }
        std::cout << "  Parsed " << netlist_db->getNumCells() << " cells, " 
                  << netlist_db->getNumNets() << " nets" << std::endl;
        
        // Parse Liberty library
        std::cout << "Parsing Liberty library: " << config.liberty_file << std::endl;
        LibertyParser lib_parser;
        auto library = lib_parser.parseFile(config.liberty_file);
        if (!library) {
            std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
            return 1;
        }
        std::cout << "  Loaded library: " << library->getName() 
                  << " with " << library->getCellCount() << " cells" << std::endl;
        
        // Parse LEF library (if provided)
        std::unique_ptr<LefLibrary> lef_lib;
        double row_height = 3.0;  // Default fallback
        
        if (!config.lef_file.empty()) {
            std::cout << "Parsing LEF library: " << config.lef_file << std::endl;
            LefParser lef_parser(config.lef_file, true);  // Enable verbose
            lef_lib = std::make_unique<LefLibrary>(lef_parser.parse());
            if (!lef_lib) {
                std::cerr << "Error: Failed to parse LEF file" << std::endl;
                return 1;
            }
            std::cout << "  Loaded LEF library with " << lef_lib->macros.size() << " macros" << std::endl;
            
            // Auto-detect row height from LEF
            row_height = autoDetectRowHeight(*lef_lib);
        } else {
            std::cout << "  No LEF file provided, using default row height: " << row_height << " um" << std::endl;
        }
        
        // ====================================================================
        // Phase 2: 布局 (Placement)
        // ====================================================================
        std::cout << "\n=== Phase 2: Placement ===" << std::endl;
        
        // Setup placement configuration
        PlacementConfig placement_config;
        placement_config.liberty_file = config.liberty_file;
        placement_config.lef_file = config.lef_file;
        placement_config.utilization = config.utilization;
        placement_config.row_height = row_height;
        placement_config.verbose = true;
        placement_config.run_id = "mini_router_run";
        
        // Run placement
        auto placer_db = PlacementInterface::runPlacement(placement_config, netlist_db);
        if (!placer_db) {
            std::cerr << "Error: Placement failed" << std::endl;
            return 1;
        }
        
        // Place I/O ports
        placeIOPorts(*placer_db);
        
        std::cout << "  Placement completed successfully" << std::endl;
        
        // ====================================================================
        // Phase 3: 布线准备 (Routing Prep) - CRITICAL data bridge
        // ====================================================================
        std::cout << "\n=== Phase 3: Routing Preparation ===" << std::endl;
        
        if (!lef_lib) {
            std::cerr << "Error: LEF library required for routing" << std::endl;
            return 1;
        }
        
        // Initialize MacroMapper
        MacroMapper macro_mapper(*lef_lib);
        
        // Build pin locations map
        auto pin_locations = buildPinLocations(*placer_db, *lef_lib, macro_mapper);
        
        // Initialize RoutingGrid
        RoutingGrid routing_grid;
        Rect core_area = placer_db->getCoreArea();
        double pitch = 0.095;  // Refined pitch: half of original to fix quantization error
        routing_grid.init(core_area, pitch, pitch);
        
        std::cout << "  Routing grid initialized: " << routing_grid.getGridWidth()
                  << "x" << routing_grid.getGridHeight() << " x" << routing_grid.getNumLayers()
                  << " layers" << std::endl;

        // **PHYSICALLY CORRECT**: Add cell obstacles to block routing through cells
        std::cout << "  Adding cell obstacles to routing grid..." << std::endl;
        auto all_cells = placer_db->getAllCells();
        int obstacle_count = 0;
        for (Cell* cell : all_cells) {
            if (cell->isPort()) continue;  // I/O ports should not block routing

            const auto& info = placer_db->getCellInfo(cell);
            double cell_x = cell->getX();
            double cell_y = cell->getY();
            double cell_width = info.width;
            double cell_height = info.height;

            // Create obstacle rectangle on Layer 0 (M1)
            Rect obstacle(cell_x, cell_y, cell_x + cell_width, cell_y + cell_height);
            routing_grid.addObstacle(obstacle, 0);
            obstacle_count++;
        }
        std::cout << "    Added " << obstacle_count << " cell obstacles on Layer 0" << std::endl;

        // ====================================================================
        // Phase 4: 执行与可视化 (Execution)
        // ====================================================================
        std::cout << "\n=== Phase 4: Routing Execution ===" << std::endl;
        
        // Create PinMapper and MazeRouter
        PinMapper pin_mapper(*lef_lib, macro_mapper);
        MazeRouter router(&routing_grid, &pin_mapper);
        router.setViaCost(0.1);  // Almost free vias to test if more nets need them
        router.setWireCostPerUnit(config.wire_cost);
        
        // Route all nets and collect results for visualization
        std::cout << "Routing " << netlist_db->getNumNets() << " nets..." << std::endl;
        int routed_count = 0;
        int failed_count = 0;
        std::vector<RoutingResult> routing_results;
        
        for (const auto& net_ptr : netlist_db->getNets()) {
            Net* net = net_ptr.get();
            RoutingResult result = router.routeNet(net, pin_locations);
            if (result.success) {
                routed_count++;
                routing_results.push_back(result);  // Collect successful results
            } else {
                failed_count++;
                if (!result.error_message.empty()) {
                    std::cout << "  Failed to route net " << net->getName() 
                              << ": " << result.error_message << std::endl;
                } else {
                    std::cout << "  Failed to route net " << net->getName() 
                              << ": No error message available" << std::endl;
                }
            }
        }
        
        std::cout << "  Routing completed: " << routed_count << " successful, " 
                  << failed_count << " failed" << std::endl;
        std::cout << "  Statistics:" << std::endl;
        std::cout << "    Total wirelength: " << router.getTotalWirelength() << std::endl;
        std::cout << "    Total vias: " << router.getTotalVias() << std::endl;
        
        // Generate visualization
        std::cout << "\n=== Visualization ===" << std::endl;

        // Create visualizer and generate routed layout
        Visualizer visualizer(placer_db.get());

        // Extract circuit name from verilog file path
        std::string circuit_name = "circuit";
        size_t last_slash = config.verilog_file.find_last_of("/");
        size_t last_dot = config.verilog_file.find_last_of(".");
        if (last_slash != std::string::npos && last_dot != std::string::npos && last_dot > last_slash) {
            circuit_name = config.verilog_file.substr(last_slash + 1, last_dot - last_slash - 1);
        } else if (last_dot != std::string::npos) {
            circuit_name = config.verilog_file.substr(0, last_dot);
        }

        // Generate output filename
        std::string output_name = "routed_layout_" + circuit_name;

        // Generate routed layout visualization
        visualizer.drawRoutedPlacement(output_name, routing_results, &routing_grid);
        std::cout << "  Generated routed layout visualization: " << output_name << ".png" << std::endl;
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        std::cout << "\n===========================================" << std::endl;
        std::cout << "MiniRouter v2.0 Completed!" << std::endl;
        std::cout << "Total runtime: " << duration.count() << " ms" << std::endl;
        std::cout << "===========================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}