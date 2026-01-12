// @file steiner_tree.cpp
// @brief Steiner Tree Builder Implementation using Prim's MST Algorithm
// @details Transforms multi-pin nets into efficient 2-pin segments

#include "steiner_tree.h"
#include "lef_parser.h"
#include "lef_pin_mapper.h"
#include "../include/debug_log.h"
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace mini {

Point SteinerTreeBuilder::getPinAbsolutePosition(Pin* pin, PlacerDB* db, 
                                                const LefLibrary* lef_lib, 
                                                const MacroMapper* macro_mapper) {
    if (!pin || !pin->getOwner() || !db) {
        return Point(0, 0);  // Fallback
    }
    
    Cell* cell = pin->getOwner();
    
    // Get cell origin from Cell object directly (more reliable than PlacerDB)
    double cell_origin_x = cell->getX();
    double cell_origin_y = cell->getY();
    
    // Default: use cell center (fallback for backward compatibility)
    double offset_x = 0.0;
    double offset_y = 0.0;
    
    // Try to get precise pin offset from LEF data
    if (lef_lib && macro_mapper) {
        std::string cell_type_name = cellTypeToString(cell->getType());
        const LefMacro* macro = macro_mapper->mapType(cell_type_name);
        
        if (macro) {
            // Try direct pin name match first
            const LefPort* port = macro->getPin(pin->getName());
            
            // If direct match fails, try heuristic mapping (common pin naming conventions)
            if (!port) {
                std::string pin_name = pin->getName();
                // Try common input pin mappings
                if (pin_name == "A" || pin_name == "B" || pin_name == "C") {
                    port = macro->getPin(pin_name + "1");
                }
                // Try common output pin mappings  
                if (!port && pin_name == "Y") {
                    port = macro->getPin("ZN");
                }
                // Try clock pin mappings
                if (!port && (pin_name == "CK" || pin_name == "CLK")) {
                    port = macro->getPin("CP");
                }
            }
            
            if (port && !port->rects.empty()) {
                // Use the center of the first rectangle as pin offset
                const Rect& rect = port->rects[0];
                offset_x = (rect.x_min + rect.x_max) / 2.0;
                offset_y = (rect.y_min + rect.y_max) / 2.0;
            }
        }
    }
    
    // If no LEF data available, fall back to cell center
    if (offset_x == 0.0 && offset_y == 0.0) {
        auto cell_info = db->getCellInfo(cell);
        offset_x = cell_info.width / 2.0;
        offset_y = cell_info.height / 2.0;
    }
    
    return Point(cell_origin_x + offset_x, cell_origin_y + offset_y);
}

size_t SteinerTreeBuilder::extractPinPoints(Net* net, PlacerDB* db, 
                                           std::vector<Point>& points, 
                                           std::vector<Pin*>& pins) {
    points.clear();
    pins.clear();
    
    // Extract driver pin (source)
    if (net->getDriver()) {
        Pin* driver_pin = net->getDriver();
        Cell* owner = driver_pin->getOwner();
        if (owner && db) {
            auto info = db->getCellInfo(owner);
            // Use cell center as pin coordinate for simplicity
            // Advanced approach: use pin relative offset from LEF
            Point center = Point(info.x + info.width/2, info.y + info.height/2);
            points.push_back(center);
            pins.push_back(driver_pin);
        }
    }
    
    // Extract load pins (sinks)
    for (Pin* load_pin : net->getLoads()) {
        Cell* owner = load_pin->getOwner();
        if (owner && db) {
            auto info = db->getCellInfo(owner);
            Point center = Point(info.x + info.width/2, info.y + info.height/2);
            points.push_back(center);
            pins.push_back(load_pin);
        }
    }
    
    return points.size();
}

bool SteinerTreeBuilder::validateInput(Net* net, PlacerDB* db) {
    if (!net) {
        std::cerr << "Error: Null net pointer" << std::endl;
        return false;
    }
    
    if (!db) {
        std::cerr << "Error: Null placement database pointer" << std::endl;
        return false;
    }
    
    if (!net->getDriver() && net->getLoads().empty()) {
        std::cerr << "Warning: Net " << net->getName() << " has no pins" << std::endl;
        return false;
    }
    
    return true;
}

std::vector<Segment> SteinerTreeBuilder::build(Net* net, PlacerDB* db, 
                                             const LefLibrary* lef_lib,
                                             const MacroMapper* macro_mapper) {
    std::vector<Segment> segments;
    
    // Input validation
    if (!validateInput(net, db)) {
        return segments;
    }
    
    // Extract pin points for MST construction using PRECISE pin coordinates
    // This is the critical fix: use actual pin locations instead of cell centers
    std::vector<Point> pin_points;
    std::vector<Pin*> pin_ptrs;
    
    // Extract driver pin with precise coordinates
    if (net->getDriver()) {
        Pin* driver_pin = net->getDriver();
        Point driver_pos = getPinAbsolutePosition(driver_pin, db, lef_lib, macro_mapper);
        pin_points.push_back(driver_pos);
        pin_ptrs.push_back(driver_pin);
        
        ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "SteinerTree", "Driver pin " + driver_pin->getName() + 
                   " at (" + std::to_string(driver_pos.x) + ", " + std::to_string(driver_pos.y) + ")");
    }
    
    // Extract load pins with precise coordinates
    for (Pin* load_pin : net->getLoads()) {
        Point load_pos = getPinAbsolutePosition(load_pin, db, lef_lib, macro_mapper);
        pin_points.push_back(load_pos);
        pin_ptrs.push_back(load_pin);
        
        ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "SteinerTree", "Load pin " + load_pin->getName() + 
                   " at (" + std::to_string(load_pos.x) + ", " + std::to_string(load_pos.y) + ")");
    }
    
    if (pin_points.size() < 2) {
        ROUTING_LOG("SteinerTree", "Warning: Net " + net->getName() + 
                   " has fewer than 2 pins, skipping MST construction");
        return segments;
    }
    
    ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "SteinerTree", "Building MST for " + std::to_string(pin_points.size()) + 
               " pins in net " + net->getName());
    
    // Build Minimum Spanning Tree using Prim's algorithm
    size_t n = pin_points.size();
    std::vector<bool> connected(n, false);
    std::vector<int> parent(n, -1);
    std::vector<double> min_dist(n, std::numeric_limits<double>::max());
    
    min_dist[0] = 0;
    
    for (size_t i = 0; i < n; ++i) {
        // Find the closest unconnected vertex
        int u = -1;
        for (size_t v = 0; v < n; ++v) {
            if (!connected[v] && (u == -1 || min_dist[v] < min_dist[u])) {
                u = v;
            }
        }
        
        if (u == -1 || min_dist[u] == std::numeric_limits<double>::max()) {
            break;  // Graph is disconnected
        }
        
        connected[u] = true;
        
        if (parent[u] != -1) {
            Segment seg(pin_points[parent[u]], pin_points[u], 
                       pin_ptrs[parent[u]], pin_ptrs[u]);
            segments.push_back(seg);
            
            ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "SteinerTree", 
                          "Segment: " + pin_ptrs[parent[u]]->getName() + 
                          " -> " + pin_ptrs[u]->getName() +
                          " length=" + std::to_string(seg.manhattanLength()));
        }
        
        // Update distances to remaining vertices
        for (size_t v = 0; v < n; ++v) {
            if (!connected[v]) {
                double dist = pin_points[u].manhattanDistance(pin_points[v]);
                if (dist < min_dist[v]) {
                    min_dist[v] = dist;
                    parent[v] = u;
                }
            }
        }
    }
    
    ROUTING_LOG_IF(ROUTING_LOG_LEVEL >= 2, "SteinerTree", "MST complete: " + std::to_string(segments.size()) + " segments created");
    return segments;
}

} // namespace mini