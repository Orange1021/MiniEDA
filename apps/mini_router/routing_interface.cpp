/**
 * @file routing_interface.cpp
 * @brief High-level Routing Interface Implementation
 */

#include "routing_interface.h"
#include "maze_router.h"
#include "routing_grid.h"
#include "../../lib/include/lef_parser.h"
#include "../../lib/include/liberty_parser.h"
#include "../../lib/include/lef_pin_mapper.h"
#include "../../lib/include/visualizer.h"
#include "../../lib/include/placer_db.h"
#include "../../apps/mini_placement/macro_mapper.h"
#include <iostream>
#include <iomanip>

namespace mini {

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

std::unordered_map<std::string, Point> buildPinLocations(
    PlacerDB& placer_db,
    const LefLibrary& lef_lib,
    const MacroMapper& macro_mapper) {
    
    std::cout << "Building pin locations map..." << std::endl;
    
    // Create PinMapper for unified key generation
    auto pin_mapper = std::make_unique<LefPinMapper>(lef_lib, macro_mapper);
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

std::vector<RoutingResult> RoutingInterface::runRouting(
    const RoutingConfig& config,
    std::shared_ptr<NetlistDB> netlist_db,
    std::unique_ptr<PlacerDB>& placer_db) {
    
    std::vector<RoutingResult> results;
    
    try {
        // Phase 1: Parse LEF library
        if (config.verbose) {
            std::cout << "Parsing LEF library: " << config.lef_file << std::endl;
        }
        LefParser lef_parser(config.lef_file, config.verbose);
        auto lef_lib = std::make_unique<LefLibrary>(lef_parser.parse());
        if (!lef_lib) {
            std::cerr << "Error: Failed to parse LEF file" << std::endl;
            return results;
        }
        
        // Phase 2: Parse Liberty library
        if (config.verbose) {
            std::cout << "Parsing Liberty library: " << config.liberty_file << std::endl;
        }
        LibertyParser lib_parser;
        auto library = lib_parser.parseFile(config.liberty_file);
        if (!library) {
            std::cerr << "Error: Failed to parse Liberty file: " << lib_parser.getError() << std::endl;
            return results;
        }
        
        // Phase 3: Place I/O ports
        placeIOPorts(*placer_db);
        
        // Phase 4: Initialize routing grid
        RoutingGrid routing_grid;
        Rect core_area = placer_db->getCoreArea();
        double pitch = 0.095;  // Standard pitch
        routing_grid.init(core_area, pitch, pitch);
        
        if (config.verbose) {
            std::cout << "  Routing grid initialized: " << routing_grid.getGridWidth()
                      << "x" << routing_grid.getGridHeight() << " x" << routing_grid.getNumLayers()
                      << " layers" << std::endl;
        }
        
        // Phase 5: Add cell obstacles (skip I/O ports)
        if (config.verbose) {
            std::cout << "  Adding cell obstacles to routing grid..." << std::endl;
        }
        int obstacle_count = 0;
        for (Cell* cell : placer_db->getAllCells()) {
            if (cell->isPort()) continue;  // Skip I/O ports

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
        if (config.verbose) {
            std::cout << "    Added " << obstacle_count << " cell obstacles on Layer 0" << std::endl;
        }
        
        // Phase 6: Build pin locations map
        MacroMapper macro_mapper(*lef_lib);
        auto pin_locations = buildPinLocations(*placer_db, *lef_lib, macro_mapper);
        
        // Phase 7: Run maze routing
        LefPinMapper pin_mapper(*lef_lib, macro_mapper);
        MazeRouter router(&routing_grid, &pin_mapper);
        router.setViaCost(config.via_cost);
        router.setWireCostPerUnit(config.wire_cost);
        
        if (config.verbose) {
            std::cout << "Routing " << netlist_db->getNumNets() << " nets..." << std::endl;
        }
        
        for (const auto& net_ptr : netlist_db->getNets()) {
            Net* net = net_ptr.get();
            if (!net) continue;
            
            // Skip power/ground nets
            if (net->getName() == "VDD" || net->getName() == "VSS" || net->getName() == "GND") {
                continue;
            }
            
            RoutingResult result = router.routeNet(net, pin_locations);
            if (result.success) {
                results.push_back(result);
            }
        }
        
        if (config.verbose) {
            double total_wirelength;
            int total_vias, routed_count, failed_count;
            getRoutingStatistics(results, total_wirelength, total_vias, routed_count, failed_count);
            
            std::cout << "  Routing completed: " << routed_count << " successful, " 
                      << failed_count << " failed" << std::endl;
            std::cout << "  Statistics:" << std::endl;
            std::cout << "    Total wirelength: " << total_wirelength << std::endl;
            std::cout << "    Total vias: " << total_vias << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in routing: " << e.what() << std::endl;
    }
    
    return results;
}

std::vector<RoutingResult> RoutingInterface::runRoutingWithVisualization(
    const RoutingConfig& config,
    std::shared_ptr<NetlistDB> netlist_db,
    std::unique_ptr<PlacerDB>& placer_db,
    Visualizer* visualizer) {
    
    // Run routing first
    auto results = runRouting(config, netlist_db, placer_db);
    
    // Generate visualization if requested
    if (visualizer && !results.empty()) {
        // Create routing grid for visualization
        LefParser lef_parser(config.lef_file, config.verbose);
        auto lef_lib = std::make_unique<LefLibrary>(lef_parser.parse());
        
        if (lef_lib) {
            RoutingGrid routing_grid;
            Rect core_area = placer_db->getCoreArea();
            double pitch = 0.095;
            routing_grid.init(core_area, pitch, pitch);
            
            std::string output_name = "visualizations/" + config.run_id + "/post_routing";
            visualizer->drawRoutedPlacement(output_name, results, &routing_grid);
        }
    }
    
    return results;
}

void RoutingInterface::getRoutingStatistics(
    const std::vector<RoutingResult>& results,
    double& total_wirelength,
    int& total_vias,
    int& routed_count,
    int& failed_count) {
    
    total_wirelength = 0.0;
    total_vias = 0;
    routed_count = static_cast<int>(results.size());
    failed_count = 0;
    
    for (const auto& result : results) {
        total_wirelength += result.total_wirelength;
        total_vias += result.total_vias;
    }
}

} // namespace mini