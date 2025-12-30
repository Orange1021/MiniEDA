// @file steiner_tree.cpp
// @brief Steiner Tree Builder Implementation using Prim's MST Algorithm
// @details Transforms multi-pin nets into efficient 2-pin segments

#include "steiner_tree.h"
#include <cmath>
#include <limits>
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace mini {

double SteinerTreeBuilder::manhattanDist(const Point& p1, const Point& p2) {
    return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
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

std::vector<Segment> SteinerTreeBuilder::build(Net* net, PlacerDB* db) {
    std::vector<Segment> segments;
    
    // Input validation
    if (!validateInput(net, db)) {
        return segments;
    }
    
    // Extract pin points for MST construction
    std::vector<Point> pin_points;
    std::vector<Pin*> pin_ptrs;
    
    if (extractPinPoints(net, db, pin_points, pin_ptrs) < 2) {
        return segments;
    }
    
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
            segments.emplace_back(pin_points[parent[u]], pin_points[u], 
                                    pin_ptrs[parent[u]], pin_ptrs[u]);
        }
        
        // Update distances to remaining vertices
        for (size_t v = 0; v < n; ++v) {
            if (!connected[v]) {
                double dist = manhattanDist(pin_points[u], pin_points[v]);
                if (dist < min_dist[v]) {
                    min_dist[v] = dist;
                    parent[v] = u;
                }
            }
        }
    }
    
    return segments;
}

} // namespace mini