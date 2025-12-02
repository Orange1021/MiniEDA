/**
 * @file main_router.cpp
 * @brief MiniRouter - Main Entry Point for Routing Flow
 * @details Complete flow: Parse -> Place -> Route -> Visualize
 */

#include <iostream>
#include <string>
#include <memory>
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <algorithm>
#include <cctype>

// Core libraries
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/verilog_parser.h"
#include "../../lib/include/liberty_parser.h"
#include "../../lib/include/lef_parser.h"
#include "../../lib/include/visualizer.h"
#include "../../lib/include/pin_mapper.h"

// Placement modules
#include "../mini_placement/placer_db.h"
#include "../mini_placement/placer_engine.h"
#include "../mini_placement/macro_mapper.h"

// Routing modules
#include "routing_grid.h"
#include "maze_router.h"

using namespace mini;

void printUsage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <verilog_file>" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -v <file>        Verilog netlist file (required)" << std::endl;
    std::cout << "  -lib <file>      Liberty library file (required)" << std::endl;
    std::cout << "  -lef <file>      LEF physical library file (optional)" << std::endl;
    std::cout << "  -util <value>    Target utilization (default: 0.7)" << std::endl;
    std::cout << "  -rowheight <val> Row height in micrometers (default: 3.0)" << std::endl;
    std::cout << "  -pitch <value>   Routing pitch in micrometers (default: 0.19)" << std::endl;
    std::cout << "  -help            Show this help message" << std::endl;
}

struct RouterConfig {
    std::string verilog_file;
    std::string liberty_file = "benchmarks/NangateOpenCellLibrary_typical.lib";
    std::string lef_file;
    double utilization = 0.7;
    double row_height = 3.0;
    double routing_pitch = 0.19;  // Nangate 45nm pitch
    bool verbose = false;
};

bool parseArguments(int argc, char* argv[], RouterConfig& config) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-help" || arg == "-h") {
            return false;
        } else if (arg == "-v" && i + 1 < argc) {
            config.verilog_file = argv[++i];
        } else if (arg == "-lib" && i + 1 < argc) {
            config.liberty_file = argv[++i];
        } else if (arg == "-lef" && i + 1 < argc) {
            config.lef_file = argv[++i];
        } else if (arg == "-util" && i + 1 < argc) {
            config.utilization = std::stod(argv[++i]);
        } else if (arg == "-rowheight" && i + 1 < argc) {
            config.row_height = std::stod(argv[++i]);
        } else if (arg == "-pitch" && i + 1 < argc) {
            config.routing_pitch = std::stod(argv[++i]);
        } else if (arg == "-verbose") {
            config.verbose = true;
        } else if (arg[0] != '-') {
            // Positional argument (verilog file without -v flag)
            if (config.verilog_file.empty()) {
                config.verilog_file = arg;
            }
        }
    }
    
    return !config.verilog_file.empty();
}

void placeIOPorts(PlacerDB* placer_db, NetlistDB* netlist_db) {
    const Rect& core_area = placer_db->getCoreArea();
    double core_height = core_area.height();
    
    std::cout << "   Debug: Core area = (" << core_area.x_min << "," << core_area.y_min 
              << ") to (" << core_area.x_max << "," << core_area.y_max << ")" << std::endl;
    std::cout << "   Debug: Core height = " << core_height << std::endl;
    
    std::vector<Cell*> input_ports;
    std::vector<Cell*> output_ports;
    
    // Separate I/O ports
    for (const auto& cell_ptr : netlist_db->getCells()) {
        Cell* cell = cell_ptr.get();
        if (cell->getType() == CellType::INPUT) {
            input_ports.push_back(cell);
        } else if (cell->getType() == CellType::OUTPUT) {
            output_ports.push_back(cell);
        }
    }
    
    std::cout << "   Debug: Found " << input_ports.size() << " inputs, " 
              << output_ports.size() << " outputs" << std::endl;
    
    // Place inputs along left edge
    for (size_t i = 0; i < input_ports.size(); ++i) {
        Cell* port = input_ports[i];
        double spacing = core_height / (input_ports.size() + 1);
        double y_pos = core_area.y_min + spacing * (i + 1);
        
        std::cout << "   Debug: Placing input " << port->getName() 
                  << " at (" << core_area.x_min << ", " << y_pos << ")" << std::endl;
        
        // Use standard cell size for I/O ports
        placer_db->placeCell(port, core_area.x_min, y_pos);
    }
    
    // Place outputs along right edge
    for (size_t i = 0; i < output_ports.size(); ++i) {
        Cell* port = output_ports[i];
        double spacing = core_height / (output_ports.size() + 1);
        double y_pos = core_area.y_min + spacing * (i + 1);
        
        // Position at the edge, accounting for cell width
        auto& cell_info = placer_db->getCellInfo(port);
        double x_pos = core_area.x_max - cell_info.width;
        
        std::cout << "   Debug: Placing output " << port->getName() 
                  << " at (" << x_pos << ", " << y_pos << ")" << std::endl;
        
        placer_db->placeCell(port, x_pos, y_pos);
    }
}

/**
 * @brief Find pin by enhanced heuristic name matching
 * @param macro LEF macro containing pins
 * @param netlist_pin_name Pin name from netlist (e.g., "A", "B", "Y")
 * @return Pointer to matching LefPort, or nullptr if not found
 */
const LefPort* findPinByName(const LefMacro* macro, const std::string& netlist_pin_name) {
    if (!macro) {
        std::cout << "   Debug: findPinByName called with NULL macro for pin '" << netlist_pin_name << "'" << std::endl;
        return nullptr;
    }
    
    std::cout << "   Debug: findPinByName looking for pin '" << netlist_pin_name << "' in macro '" << macro->name << "'" << std::endl;
    
    // Convert to uppercase for case-insensitive comparison
    std::string pin_upper = netlist_pin_name;
    std::transform(pin_upper.begin(), pin_upper.end(), pin_upper.begin(), ::toupper);
    
    // Strategy 1: Direct match (already tried, but include for completeness)
    const LefPort* port = macro->getPin(netlist_pin_name);
    if (port) {
        std::cout << "   Debug: Direct match found for pin '" << netlist_pin_name << "'" << std::endl;
        return port;
    }
    
    // Enhanced Nangate pin name mappings with more comprehensive coverage
    std::vector<std::pair<std::string, std::vector<std::string>>> enhanced_mappings = {
        // Output pins - try multiple possibilities
        {"Y", {"ZN", "Z", "Y", "O", "OUT"}},     // Verilog Y -> Nangate ZN/Z/Y/O/OUT
        {"Z", {"ZN", "Z", "Y", "O", "OUT"}},     // Verilog Z -> Nangate ZN/Z/Y/O/OUT
        {"Q", {"Q", "QN", "Z", "ZN", "Y"}},      // DFF output Q -> multiple possibilities
        {"QN", {"QN", "Z", "ZN", "Y", "Q"}},     // DFF inverted output QN
        
        // Input pins - single input gates with more options
        {"A", {"A1", "A", "I", "IN", "INPUT"}},  // Verilog A -> multiple Nangate variants
        {"I", {"A1", "A", "I", "IN", "INPUT"}},  // Verilog I -> multiple Nangate variants
        {"IN", {"A1", "A", "I", "IN", "INPUT"}}, // Verilog IN -> multiple Nangate variants
        
        // Input pins - multi-input gates with systematic mapping
        {"B", {"A2", "B1", "B", "IN1", "S1"}},   // Second input with priority order
        {"C", {"A3", "B1", "C1", "C", "IN2", "S2"}}, // Third input
        {"D", {"A4", "B2", "C1", "D1", "D", "IN3", "S3"}}, // Fourth input
        {"E", {"A5", "B3", "C2", "D1", "E1", "E", "IN4"}}, // Fifth input
        {"F", {"A6", "B4", "C3", "D2", "E1", "F1", "F", "IN5"}}, // Sixth input
        
        // Clock pins with comprehensive coverage
        {"CK", {"CP", "CLK", "CLOCK", "CK", "C"}},   // Clock variants
        {"CLK", {"CP", "CLK", "CLOCK", "CK", "C"}},  // Clock variants
        {"CLOCK", {"CP", "CLK", "CLOCK", "CK", "C"}},// Clock variants
        {"C", {"CP", "CLK", "CLOCK", "CK", "C", "CLKN"}}, // Clock variants
        
        // Control pins
        {"RN", {"RN", "RESET", "RESETN", "CLR", "CLRN"}}, // Reset (active low)
        {"RESET", {"RN", "RESET", "RESETN", "CLR", "CLRN"}}, // Reset
        {"SET", {"SET", "SETN", "S", "SN"}},         // Set pins
        {"EN", {"EN", "ENABLE", "OE", "ENA"}},       // Enable pins
        
        // Select pins for multiplexers
        {"S", {"S", "SEL", "SELECT", "S0", "S1"}},   // Select pins
        {"SEL", {"S", "SEL", "SELECT", "S0", "S1"}}, // Select pins
        
        // Data pins
        {"D", {"D", "DATA", "DI", "DIN"}},          // Data input
        {"DI", {"D", "DATA", "DI", "DIN"}},         // Data input variants
    };
    
    std::cout << "   Debug: Trying enhanced mappings for pin '" << netlist_pin_name << "'" << std::endl;
    for (const auto& mapping : enhanced_mappings) {
        if (mapping.first == netlist_pin_name) {
            for (const auto& target_pin : mapping.second) {
                std::cout << "   Debug: Trying mapping " << mapping.first << " -> " << target_pin << std::endl;
                port = macro->getPin(target_pin);
                if (port) {
                    std::cout << "   Debug: Enhanced mapping SUCCESS: " << mapping.first << " -> " << target_pin << std::endl;
                    return port;
                }
            }
        }
    }
    
    // Advanced pattern-based matching with multiple strategies
    if (!pin_upper.empty()) {
        // Strategy 1: Exact first character match with priority ordering
        std::vector<std::string> priority_suffixes = {"1", "2", "3", "4", "5", "6", "", "_1", "_2"};
        for (const auto& suffix : priority_suffixes) {
            std::string candidate = pin_upper + suffix;
            port = macro->getPin(candidate);
            if (port) {
                std::cout << "   Debug: Priority suffix match: " << netlist_pin_name << " -> " << candidate << std::endl;
                return port;
            }
        }
        
        // Strategy 2: Systematic pin name exploration
        for (const auto& pin_name : macro->getPinNames()) {
            std::string lef_pin_upper = pin_name;
            std::transform(lef_pin_upper.begin(), lef_pin_upper.end(), lef_pin_upper.begin(), ::toupper);
            
            // Exact match (case insensitive)
            if (pin_upper == lef_pin_upper) {
                port = macro->getPin(pin_name);
                if (port) {
                    std::cout << "   Debug: Case-insensitive match: " << netlist_pin_name << " -> " << pin_name << std::endl;
                    return port;
                }
            }
            
            // Single character prefix match (A->A1, A2, etc.)
            if (pin_upper.size() == 1 && lef_pin_upper.size() > 1 && 
                pin_upper[0] == lef_pin_upper[0]) {
                port = macro->getPin(pin_name);
                if (port) {
                    std::cout << "   Debug: Prefix match: " << netlist_pin_name << " -> " << pin_name << std::endl;
                    return port;
                }
            }
            
            // Substring matches (both directions)
            if (lef_pin_upper.find(pin_upper) != std::string::npos || 
                pin_upper.find(lef_pin_upper) != std::string::npos) {
                port = macro->getPin(pin_name);
                if (port) {
                    std::cout << "   Debug: Substring match: " << netlist_pin_name << " -> " << pin_name << std::endl;
                    return port;
                }
            }
        }
        
        // Strategy 3: Fallback by pin function type
        // Try to guess pin function based on name patterns
        std::vector<std::string> output_candidates = {"ZN", "Z", "Y", "Q", "QN", "O", "OUT"};
        std::vector<std::string> input_candidates = {"A1", "A", "I", "IN", "D", "DI"};
        std::vector<std::string> clock_candidates = {"CP", "CLK", "C", "CK"};
        
        std::vector<std::string> candidates;
        if (pin_upper == "Y" || pin_upper == "Z" || pin_upper == "Q" || pin_upper == "QN") {
            candidates = output_candidates;
        } else if (pin_upper == "A" || pin_upper == "B" || pin_upper == "C" || pin_upper == "D") {
            candidates = input_candidates;
        } else if (pin_upper == "CK" || pin_upper == "CLK" || pin_upper == "C") {
            candidates = clock_candidates;
        }
        
        for (const auto& candidate : candidates) {
            port = macro->getPin(candidate);
            if (port) {
                std::cout << "   Debug: Function-based fallback: " << netlist_pin_name << " -> " << candidate << std::endl;
                return port;
            }
        }
    }
    
    // Strategy 4: Last resort - try any available pin
    if (!macro->getPinNames().empty()) {
        const std::string& first_pin = macro->getPinNames()[0];
        port = macro->getPin(first_pin);
        if (port) {
            std::cout << "   Debug: Last resort fallback: " << netlist_pin_name << " -> " << first_pin << std::endl;
            return port;
        }
    }
    
    std::cout << "   Debug: No mapping found for pin '" << netlist_pin_name << "'" << std::endl;
    return nullptr;
}

/**
 * @brief Open area-based pin access points on routing grid
 * @param routing_grid Reference to routing grid
 * @param placer_db Pointer to placement database
 * @param netlist_db Pointer to netlist database
 * @param lef_lib Reference to LEF library
 */
void openAreaBasedPinAccess(RoutingGrid& routing_grid,
                           PlacerDB* placer_db,
                           NetlistDB* netlist_db,
                           const LefLibrary& lef_lib) {
    std::cout << "   Opening area-based pin access points..." << std::endl;
    
    // Create macro mapper and pin mapper for unified name mapping
    MacroMapper macro_mapper(lef_lib);
    PinMapper pin_mapper(lef_lib, macro_mapper);
    
    size_t total_pins_opened = 0;
    size_t total_grid_points_opened = 0;
    
    for (const auto& cell_ptr : netlist_db->getCells()) {
        Cell* cell = cell_ptr.get();
        
        if (cell->getType() == CellType::INPUT || cell->getType() == CellType::OUTPUT) {
            // For I/O ports, use single point access (as before)
            const auto& cell_info = placer_db->getCellInfo(cell);
            Point pin_pos(cell_info.x + cell_info.width/2, cell_info.y + cell_info.height/2);
            GridPoint pin_gp = routing_grid.physToGrid(pin_pos.x, pin_pos.y, 0);
            
            if (routing_grid.isValid(pin_gp)) {
                routing_grid.setGridState(pin_gp, GridState::FREE);
                total_pins_opened++;
                total_grid_points_opened++;
            }
        } else {
            // For standard cells, use area-based access
            const auto& cell_info = placer_db->getCellInfo(cell);
            const LefMacro* macro = macro_mapper.mapType(cell->getTypeString());
            
            if (macro) {
                // For each pin of this cell
                for (const auto& pin : cell->getPins()) {
                    // Get pin geometry from LEF
                    const LefPort* port = macro->getPin(pin->getName());
                    if (!port) {
                        // Try heuristic mapping
                        port = findPinByName(macro, pin->getName());
                    }
                    
                    if (port && !port->rects.empty()) {
                        // For each rectangle in the port
                        for (const Rect& pin_rect : port->rects) {
                            // Calculate absolute physical coordinates
                            double phys_x_min = cell_info.x + pin_rect.x_min;
                            double phys_y_min = cell_info.y + pin_rect.y_min;
                            double phys_x_max = cell_info.x + pin_rect.x_max;
                            double phys_y_max = cell_info.y + pin_rect.y_max;
                            
                            // Convert to grid coordinates
                            GridPoint grid_min = routing_grid.physToGrid(phys_x_min, phys_y_min, 0);
                            GridPoint grid_max = routing_grid.physToGrid(phys_x_max, phys_y_max, 0);
                            
                            // Ensure grid coordinates are within bounds
                            grid_min.x = std::max(0, std::min(grid_min.x, routing_grid.getGridSize().first - 1));
                            grid_min.y = std::max(0, std::min(grid_min.y, routing_grid.getGridSize().second - 1));
                            grid_max.x = std::max(0, std::min(grid_max.x, routing_grid.getGridSize().first - 1));
                            grid_max.y = std::max(0, std::min(grid_max.y, routing_grid.getGridSize().second - 1));
                            
                            // Open all grid points in the rectangle area
                            for (int gx = grid_min.x; gx <= grid_max.x; gx++) {
                                for (int gy = grid_min.y; gy <= grid_max.y; gy++) {
                                    GridPoint gp(gx, gy, 0);  // M1 layer
                                    if (routing_grid.isValid(gp)) {
                                        routing_grid.setGridState(gp, GridState::FREE);
                                        total_grid_points_opened++;
                                        
                                        // Also open the point above on M2 for via access
                                        GridPoint gp_m2(gx, gy, 1);
                                        if (routing_grid.isValid(gp_m2)) {
                                            routing_grid.setGridState(gp_m2, GridState::FREE);
                                            total_grid_points_opened++;
                                        }
                                    }
                                }
                            }
                        }
                        total_pins_opened++;
                    }
                }
            }
        }
    }
    
    std::cout << "   Opened " << total_pins_opened << " pins covering " 
              << total_grid_points_opened << " grid points" << std::endl;
}

std::pair<std::unordered_map<std::string, Point>, PinMapper> buildPinLocationMap(PlacerDB* placer_db,
                                                                                 NetlistDB* netlist_db,
                                                                                 const LefLibrary& lef_lib) {
    std::unordered_map<std::string, Point> pin_locations;
    
    std::cout << "=== Building Pin Location Map ===" << std::endl;
    
    // Create macro mapper and pin mapper for unified name mapping
    MacroMapper macro_mapper(lef_lib);
    macro_mapper.setDebugMode(false); // Disable debug to avoid excessive output
    PinMapper pin_mapper(lef_lib, macro_mapper);
    pin_mapper.setDebugMode(false); // Disable debug to avoid excessive output
    
    size_t debug_count = 0;
    size_t total_cells = 0;
    size_t mapped_pins = 0;
    size_t failed_pins = 0;
    
    for (const auto& cell_ptr : netlist_db->getCells()) {
        Cell* cell = cell_ptr.get();
        total_cells++;
        
        if (cell->getType() == CellType::INPUT || cell->getType() == CellType::OUTPUT) {
            // For I/O ports, use cell location directly
            const auto& cell_info = placer_db->getCellInfo(cell);
            
            // Check if cell info is valid (not default values)
            if (cell_info.width > 0 && cell_info.height > 0) {
                Point pin_pos(cell_info.x + cell_info.width/2, cell_info.y + cell_info.height/2);
                pin_locations[cell->getName()] = pin_pos;
                
                if (debug_count < 5) {
                    std::cout << "   Debug: I/O Port " << cell->getName() 
                              << " mapped to (" << pin_pos.x << ", " << pin_pos.y << ")" << std::endl;
                    debug_count++;
                }
            } else {
                // Fallback: calculate I/O position manually if cell_info is invalid
                const Rect& core_area = placer_db->getCoreArea();
                double core_height = core_area.height();
                
                // Count I/O ports to calculate spacing
                static std::vector<Cell*> input_ports, output_ports;
                static bool ports_counted = false;
                if (!ports_counted) {
                    for (const auto& cell_ptr : netlist_db->getCells()) {
                        Cell* c = cell_ptr.get();
                        if (c->getType() == CellType::INPUT) {
                            input_ports.push_back(c);
                        } else if (c->getType() == CellType::OUTPUT) {
                            output_ports.push_back(c);
                        }
                    }
                    ports_counted = true;
                }
                
                Point pin_pos;
                if (cell->getType() == CellType::INPUT) {
                    // Find this input's index
                    auto it = std::find(input_ports.begin(), input_ports.end(), cell);
                    if (it != input_ports.end()) {
                        size_t index = std::distance(input_ports.begin(), it);
                        double spacing = core_height / (input_ports.size() + 1);
                        double y_pos = core_area.y_min + spacing * (index + 1);
                        pin_pos = Point(core_area.x_min + 0.5, y_pos); // Add 0.5 for pin center
                    }
                } else if (cell->getType() == CellType::OUTPUT) {
                    // Find this output's index
                    auto it = std::find(output_ports.begin(), output_ports.end(), cell);
                    if (it != output_ports.end()) {
                        size_t index = std::distance(output_ports.begin(), it);
                        double spacing = core_height / (output_ports.size() + 1);
                        double y_pos = core_area.y_min + spacing * (index + 1);
                        pin_pos = Point(core_area.x_max - 0.5, y_pos); // Subtract 0.5 for pin center
                    }
                }
                
                pin_locations[cell->getName()] = pin_pos;
                
                if (debug_count < 5) {
                    std::cout << "   Debug: I/O Port " << cell->getName() 
                              << " (fallback) mapped to (" << pin_pos.x << ", " << pin_pos.y << ")" << std::endl;
                    debug_count++;
                }
            }
        } else {
            // Process all non-I/O cells (including UNKNOWN types which are actually standard cells)
            if (debug_count < 10) {
                std::cout << "   Debug: Processing standard cell " << cell->getName() 
                          << " of type " << static_cast<int>(cell->getType()) 
                          << " (" << cell->getTypeString() << ")" << std::endl;
                debug_count++;
            }
            // For standard cells, use unified pin mapping
            const auto& cell_info = placer_db->getCellInfo(cell);
            const LefMacro* macro = macro_mapper.mapType(cell->getTypeString());
            
            if (debug_count < 10) {
                std::cout << "   Debug: Cell " << cell->getName() 
                          << " -> Macro " << (macro ? macro->name : "NULL") << std::endl;
                debug_count++;
            }
            
            if (macro) {
                // For each pin of this cell
                for (const auto& pin : cell->getPins()) {
                    // Use PinMapper to get the correct lookup key
                    std::string lookup_key = pin_mapper.getKeyFromLogicalPin(
                        cell->getName(),           // cell instance name
                        cell->getTypeString(),    // cell type
                        pin->getName()            // logical pin name
                    );
                    
                    // Get the physical pin location using the same logic as before
                    const LefPort* port = macro->getPin(pin->getName());
                    if (!port) {
                        // Try heuristic mapping
                        port = findPinByName(macro, pin->getName());
                    }
                    
                    if (port) {
                        if (!port->rects.empty()) {
                            // Use the center of the first pin rectangle
                            const Rect& pin_rect = port->rects[0];
                            double pin_x = cell_info.x + pin_rect.x_min + pin_rect.width() / 2;
                            double pin_y = cell_info.y + pin_rect.y_min + pin_rect.height() / 2;
                            
                            pin_locations[lookup_key] = Point(pin_x, pin_y);
                            mapped_pins++;
                            
                            if (debug_count < 10) {
                                std::cout << "   Debug: Pin " << lookup_key 
                                          << " mapped to (" << pin_x << ", " << pin_y << ")" << std::endl;
                                debug_count++;
                            }
                        } else {
                            std::cout << "   Warning: Pin " << lookup_key
                                      << " found but has no geometry!" << std::endl;
                            failed_pins++;
                            
                            // Use cell center as fallback to avoid (0,0)
                            Point pin_pos(cell_info.x + cell_info.width/2, cell_info.y + cell_info.height/2);
                            pin_locations[lookup_key] = pin_pos;
                        }
                    } else {
                        // Warning for unmapped pins
                        std::cout << "   Warning: Pin " << lookup_key
                                  << " could not be mapped to any LEF pin!" << std::endl;
                        failed_pins++;
                        
                        // Use cell center as fallback to avoid (0,0)
                        Point pin_pos(cell_info.x + cell_info.width/2, cell_info.y + cell_info.height/2);
                        pin_locations[lookup_key] = pin_pos;
                    }
                }
            } else {
                // Fallback: use cell center if macro mapping fails
                Point pin_pos(cell_info.x + cell_info.width/2, cell_info.y + cell_info.height/2);
                for (const auto& pin : cell->getPins()) {
                    std::string lookup_key = pin_mapper.getKeyFromLogicalPin(
                        cell->getName(),           // cell instance name
                        cell->getTypeString(),    // cell type
                        pin->getName()            // logical pin name
                    );
                    pin_locations[lookup_key] = pin_pos;
                    
                    if (debug_count < 5) {
                        std::cout << "   Debug: Pin " << lookup_key 
                                  << " (fallback) mapped to (" << pin_pos.x << ", " << pin_pos.y << ")" << std::endl;
                        debug_count++;
                    }
                }
            }
        }
    }
    
    std::cout << "=== Pin Mapping Statistics ===" << std::endl;
    std::cout << "Total cells processed: " << total_cells << std::endl;
    std::cout << "Successfully mapped pins: " << mapped_pins << std::endl;
    std::cout << "Failed pin mappings: " << failed_pins << std::endl;
    std::cout << "Total pin locations: " << pin_locations.size() << std::endl;
    std::cout << "===============================" << std::endl;
    
    return {pin_locations, pin_mapper};
}

int main(int argc, char* argv[]) {
    RouterConfig config;
    if (!parseArguments(argc, argv, config)) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::cout << "=== MiniRouter - EDA Routing Flow ===" << std::endl;
    std::cout << "Verilog: " << config.verilog_file << std::endl;
    std::cout << "Library: " << config.liberty_file << std::endl;
    if (!config.lef_file.empty()) {
        std::cout << "LEF: " << config.lef_file << std::endl;
    }
    std::cout << "Utilization: " << config.utilization << std::endl;
    std::cout << "Routing Pitch: " << config.routing_pitch << " um" << std::endl;
    std::cout << "======================================" << std::endl;
    
    try {
        // Step 1: Parse input files
        std::cout << "\n[1/6] Parsing input files..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        NetlistDB netlist_db;
        VerilogParser verilog_parser;
        if (!verilog_parser.parseFile(config.verilog_file, netlist_db)) {
            std::cerr << "Error: Failed to parse Verilog file: " << config.verilog_file << std::endl;
            return 1;
        }
        
        LibertyParser liberty_parser;
        auto library = liberty_parser.parseFile(config.liberty_file);
        if (!library) {
            std::cerr << "Error: Failed to parse Liberty file: " << config.liberty_file << std::endl;
            return 1;
        }
        
        LefLibrary lef_lib;
        if (!config.lef_file.empty()) {
            LefParser lef_parser(config.lef_file, config.verbose);
            if (lef_parser.isOpen()) {
                lef_lib = lef_parser.parse();
            } else {
                std::cerr << "Warning: Failed to open LEF file: " << config.lef_file << std::endl;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "   Parsed " << netlist_db.getNumCells() << " cells, " 
                  << netlist_db.getNumNets() << " nets in " 
                  << std::fixed << std::setprecision(3) << elapsed.count() << "s" << std::endl;
        
        // Step 2: Run placement
        std::cout << "\n[2/6] Running placement..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        
        PlacerDB placer_db(&netlist_db);
        
        // Calculate core area based on utilization
        double total_cell_area = 0.0;
        for (const auto& cell_ptr : netlist_db.getCells()) {
            Cell* cell = cell_ptr.get();
            if (cell->getType() != CellType::INPUT && cell->getType() != CellType::OUTPUT) {
                // Simple area estimation for now
                total_cell_area += 1.0; // Will be refined with LEF data
            }
        }
        
        // Set core area based on utilization
        double core_width = std::sqrt(total_cell_area / config.utilization);
        double core_height = total_cell_area / core_width / config.utilization;
        Rect core_area(0.0, 0.0, core_width, core_height);
        placer_db.setCoreArea(core_area);
        placer_db.setRowHeight(config.row_height);
        
        // Add cells to placer database
        for (const auto& cell_ptr : netlist_db.getCells()) {
            Cell* cell = cell_ptr.get();
            if (cell->getType() != CellType::INPUT && cell->getType() != CellType::OUTPUT) {
                // Use LEF data if available, otherwise estimate
                const LefMacro* macro = lef_lib.getMacro(cell->getName());
                if (macro) {
                    placer_db.addCell(cell, macro->width, macro->height);
                    placer_db.setCellLefMacro(cell, macro);
                } else {
                    // Simple area-based sizing
                    double estimated_area = 1.0; // Default area
                    double cell_width = std::sqrt(estimated_area);
                    double cell_height = estimated_area / cell_width;
                    placer_db.addCell(cell, cell_width, cell_height);
                }
            }
        }
        
        // Initialize random placement for standard cells
        placer_db.initializeRandom();
        
        // Place I/O ports along chip edges (AFTER standard cell placement)
        placeIOPorts(&placer_db, &netlist_db);
        
        // Mark I/O ports as fixed so placement algorithms won't move them
        for (const auto& cell_ptr : netlist_db.getCells()) {
            Cell* cell = cell_ptr.get();
            if (cell->getType() == CellType::INPUT || cell->getType() == CellType::OUTPUT) {
                auto& cell_info = placer_db.getCellInfo(cell);
                cell_info.fixed = true;
            }
        }
        
        // Run placement algorithms (but don't move I/O ports)
        PlacerEngine placer_engine(&placer_db);
        placer_engine.runGlobalPlacement();
        placer_engine.runLegalization();
        
        // Verify I/O ports are still in place
        std::cout << "   Debug: Verifying I/O port positions after placement:" << std::endl;
        for (const auto& cell_ptr : netlist_db.getCells()) {
            Cell* cell = cell_ptr.get();
            if (cell->getType() == CellType::INPUT || cell->getType() == CellType::OUTPUT) {
                const auto& cell_info = placer_db.getCellInfo(cell);
                std::cout << "   " << cell->getName() << " at (" 
                          << cell_info.x << ", " << cell_info.y << ")" << std::endl;
            }
        }
        
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        std::cout << "   Placement completed in " << std::fixed << std::setprecision(3) 
                  << elapsed.count() << "s" << std::endl;
        std::cout << "   Final HPWL: " << std::fixed << std::setprecision(2) 
                  << placer_engine.calculateHPWL() << " um" << std::endl;
        
        // Step 3: Initialize routing grid
        std::cout << "\n[3/6] Initializing routing grid..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        
        RoutingGrid routing_grid;
        const Rect& placement_core_area = placer_db.getCoreArea();
        routing_grid.init(placement_core_area, config.routing_pitch, config.routing_pitch);
        
        auto grid_size = routing_grid.getGridSize();
        std::cout << "   Grid size: " << grid_size.first << " x " << grid_size.second 
                  << " x 2 layers" << std::endl;
        std::cout << "   Total grid cells: " << (grid_size.first * grid_size.second * 2) << std::endl;
        
        // Step 4: Add obstacles (cells) and open area-based access points (pins)
        std::cout << "\n[4/6] Adding obstacles and area-based pin access..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        
        // Add cell obstacles - RE-ENABLED with area-based pin access
        std::cout << "   Adding cell obstacles on M1 layer..." << std::endl;
        for (const auto& cell_ptr : netlist_db.getCells()) {
            Cell* cell = cell_ptr.get();
            if (cell->getType() != CellType::INPUT && cell->getType() != CellType::OUTPUT) {
                const auto& cell_info = placer_db.getCellInfo(cell);
                Rect cell_rect(cell_info.x, cell_info.y, 
                              cell_info.x + cell_info.width, 
                              cell_info.y + cell_info.height);
                routing_grid.addObstacle(cell_rect, 0);  // Block M1 layer
            }
        }
        
        // Place I/O ports first
        placeIOPorts(&placer_db, &netlist_db);
        
        // Open area-based pin access points (this will override obstacles at pin locations)
        openAreaBasedPinAccess(routing_grid, &placer_db, &netlist_db, lef_lib);
        
        // Build pin location map for routing (use center points for routing targets)
        auto [pin_locations, pin_mapper] = buildPinLocationMap(&placer_db, &netlist_db, lef_lib);
        
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        auto grid_stats = routing_grid.getStatistics();
        std::cout << "   Obstacles added in " << std::fixed << std::setprecision(3) 
                  << elapsed.count() << "s" << std::endl;
        std::cout << "   Grid utilization: " << std::fixed << std::setprecision(1) 
                  << (routing_grid.getUtilization() * 100) << "%" << std::endl;
        
        // Step 5: Force update pin locations with correct I/O positions
        std::cout << "\n[5/8] Force updating pin locations..." << std::endl;
        
        // Re-place I/O ports to ensure correct positions
        placeIOPorts(&placer_db, &netlist_db);
        
        // Manually calculate and update I/O pin positions
        const Rect& routing_core_area = placer_db.getCoreArea();
        std::vector<Cell*> input_ports, output_ports;
        
        for (const auto& cell_ptr : netlist_db.getCells()) {
            Cell* cell = cell_ptr.get();
            if (cell->getType() == CellType::INPUT) {
                input_ports.push_back(cell);
            } else if (cell->getType() == CellType::OUTPUT) {
                output_ports.push_back(cell);
            }
        }
        
        // Update input port positions
        double routing_core_height = routing_core_area.height();
        for (size_t i = 0; i < input_ports.size(); ++i) {
            Cell* port = input_ports[i];
            double spacing = routing_core_height / (input_ports.size() + 1);
            double y_pos = routing_core_area.y_min + spacing * (i + 1);
            Point pin_pos(routing_core_area.x_min, y_pos);
            pin_locations[port->getName()] = pin_pos;
            
            std::cout << "   Updated " << port->getName() << " to (" 
                      << pin_pos.x << ", " << pin_pos.y << ")" << std::endl;
        }
        
        // Update output port positions
        for (size_t i = 0; i < output_ports.size(); ++i) {
            Cell* port = output_ports[i];
            double spacing = routing_core_height / (output_ports.size() + 1);
            double y_pos = routing_core_area.y_min + spacing * (i + 1);
            Point pin_pos(routing_core_area.x_max, y_pos);
            pin_locations[port->getName()] = pin_pos;
            
            std::cout << "   Updated " << port->getName() << " to (" 
                      << pin_pos.x << ", " << pin_pos.y << ")" << std::endl;
        }
        
        // Step 6: Pin Accessibility Debug Check
        std::cout << "\n[6/8] Debug: Pin Accessibility Check..." << std::endl;
        
        size_t accessible_pins = 0;
        size_t total_pins = pin_locations.size();
        
        for (const auto& [pin_name, location] : pin_locations) {
            GridPoint pin_gp = routing_grid.physToGrid(location.x, location.y, 0);
            bool is_accessible = routing_grid.isFree(pin_gp);
            
            std::cout << "   Pin " << pin_name << ": (" 
                      << pin_gp.x << "," << pin_gp.y << "," << pin_gp.layer 
                      << ") -> FREE? " << (is_accessible ? "Yes" : "No") << std::endl;
            
            if (is_accessible) {
                accessible_pins++;
            } else {
                // Debug: Check what state this pin actually has
                GridState state = routing_grid.getGridState(pin_gp);
                std::cout << "     Actual state: ";
                switch (state) {
                    case GridState::FREE: std::cout << "FREE"; break;
                    case GridState::OBSTACLE: std::cout << "OBSTACLE"; break;
                    case GridState::ROUTED: std::cout << "ROUTED"; break;
                    case GridState::VIA: std::cout << "VIA"; break;
                }
                std::cout << std::endl;
                
                // Debug: Show surrounding cells
                std::cout << "     Neighbors: ";
                auto neighbors = routing_grid.getNeighbors(pin_gp);
                for (const auto& nb : neighbors) {
                    GridState nb_state = routing_grid.getGridState(nb);
                    std::cout << "(" << nb.x << "," << nb.y << "," << nb.layer << ":";
                    switch (nb_state) {
                        case GridState::FREE: std::cout << "F"; break;
                        case GridState::OBSTACLE: std::cout << "O"; break;
                        case GridState::ROUTED: std::cout << "R"; break;
                        case GridState::VIA: std::cout << "V"; break;
                    }
                    std::cout << ") ";
                }
                std::cout << std::endl;
            }
        }
        
        std::cout << "   Pin Accessibility: " << accessible_pins << "/" << total_pins 
                  << " (" << std::fixed << std::setprecision(1) 
                  << (100.0 * accessible_pins / total_pins) << "%)" << std::endl;

        // Step 7: Run routing
        std::cout << "\n[7/8] Running maze routing..." << std::endl;
        start = std::chrono::high_resolution_clock::now();
        
        // Debug: Check pin_locations content
        std::cout << "   Debug: pin_locations contains " << pin_locations.size() << " entries:" << std::endl;
        int debug_count = 0;
        for (const auto& [name, point] : pin_locations) {
            if (debug_count < 5) {
                std::cout << "     " << name << " -> (" << point.x << ", " << point.y << ")" << std::endl;
                debug_count++;
            }
        }
        
        // Create MazeRouter with the same PinMapper instance
        MazeRouter router(&routing_grid, &pin_mapper);
        router.setAlgorithm(true);  // Use A* search
        router.setMaxAttempts(3);
        
        // Collect all nets for routing
        std::vector<Net*> nets_to_route;
        for (const auto& net_ptr : netlist_db.getNets()) {
            Net* net = net_ptr.get();
            if (net->getDriver() && !net->getLoads().empty()) {
                nets_to_route.push_back(net);
                
                // Debug: Check if driver pin exists in pin_locations
                std::string driver_name = net->getDriver()->getName();
                bool driver_found = pin_locations.find(driver_name) != pin_locations.end();
                std::cout << "   Debug: Net driver " << driver_name << " found in pin_locations: " << (driver_found ? "Yes" : "No") << std::endl;
                
                if (!driver_found && debug_count < 3) {
                    std::cout << "     Available pins: ";
                    for (const auto& [name, point] : pin_locations) {
                        if (name.find(driver_name) != std::string::npos) {
                            std::cout << name << " ";
                        }
                    }
                    std::cout << std::endl;
                    debug_count++;
                }
            }
        }
        
        std::cout << "   Routing " << nets_to_route.size() << " nets..." << std::endl;
        auto routing_results = router.routeNets(nets_to_route, pin_locations);
        
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        
        // Calculate routing statistics
        size_t successful_routes = 0;
        double total_wirelength = 0.0;
        int total_vias = 0;
        
        for (const auto& result : routing_results) {
            if (result.success) {
                successful_routes++;
                total_wirelength += result.wirelength;
                total_vias += result.num_vias;
            }
        }
        
        auto router_stats = router.getStatistics();
        std::cout << "   Routing completed in " << std::fixed << std::setprecision(3) 
                  << elapsed.count() << "s" << std::endl;
        std::cout << "   Success rate: " << successful_routes << "/" << nets_to_route.size() 
                  << " (" << std::fixed << std::setprecision(1) 
                  << (100.0 * successful_routes / nets_to_route.size()) << "%)" << std::endl;
        std::cout << "   Total wirelength: " << std::fixed << std::setprecision(2) 
                  << total_wirelength << " um" << std::endl;
        std::cout << "   Total vias: " << total_vias << std::endl;
        std::cout << "   Average attempts: " << std::fixed << std::setprecision(2) 
                  << router_stats.at("avg_attempts") << std::endl;
        
        // Step 8: Generate visualization
        std::cout << "\n[8/8] Generating routed layout visualization..." << std::endl;
        Visualizer visualizer(&placer_db);
        visualizer.drawRoutedPlacement("routed_layout.png", routing_results);
        
        std::cout << "\n=== MiniRouter Flow Completed Successfully ===" << std::endl;
        std::cout << "Output saved to: visualizations/routed/routed_layout.png" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}