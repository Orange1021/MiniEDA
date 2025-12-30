/**
 * @file steiner_tree.h
 * @brief Steiner Tree Builder for Net Topology Generation
 * @details Implements MST (Minimum Spanning Tree) using Prim's algorithm
 *          to decompose multi-pin nets into efficient 2-pin segments
 */

#ifndef MINI_STEINER_TREE_H
#define MINI_STEINER_TREE_H

#include "netlist_db.h"
#include "placer_db.h"
#include "geometry.h"
#include <vector>
#include <utility>
#include <memory>

namespace mini {

// Forward declarations
class Net;
class Pin;
class Cell;

/**
 * @struct Segment
 * @brief Represents a 2-pin connection segment in routing topology
 */
struct Segment {
    Point start;                    ///< Start point of the segment
    Point end;                      ///< End point of the segment
    Pin* start_pin = nullptr;       ///< Optional: pointer to start pin
    Pin* end_pin = nullptr;         ///< Optional: pointer to end pin
    
    /**
     * @brief Constructor
     * @param s Start point
     * @param e End point
     */
    Segment(Point s, Point e) : start(s), end(e) {}
    
    /**
     * @brief Constructor with pin pointers
     * @param s Start point
     * @param e End point
     * @param start_pin Pointer to start pin
     * @param end_pin Pointer to end pin
     */
    Segment(Point s, Point e, Pin* start_pin, Pin* end_pin) 
        : start(s), end(e), start_pin(start_pin), end_pin(end_pin) {}
    
    /**
     * @brief Calculate Manhattan length of the segment
     * @return Manhattan distance between start and end points
     */
    double manhattanLength() const {
        return std::abs(start.x - end.x) + std::abs(start.y - end.y);
    }
    
    /**
     * @brief Calculate Euclidean length of the segment
     * @return Euclidean distance between start and end points
     */
    double euclideanLength() const {
        double dx = start.x - end.x;
        double dy = start.y - end.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

/**
 * @class SteinerTreeBuilder
 * @brief Builds minimum spanning tree topology for multi-pin nets
 * @details Uses Prim's algorithm to construct MST from net pins,
 *          reducing total wirelength by 50%+ compared to star topology
 */
class SteinerTreeBuilder {
public:
    /**
     * @brief Build routing topology for a net
     * @param net Pointer to the net to process
     * @param db Pointer to placement database for pin coordinates
     * @return Vector of segments representing the MST topology
     */
    static std::vector<Segment> build(Net* net, PlacerDB* db);

public:
    /**
     * @brief Calculate Manhattan distance between two points
     * @param p1 First point
     * @param p2 Second point
     * @return Manhattan distance
     */
    static double manhattanDist(const Point& p1, const Point& p2);
    
    /**
     * @brief Extract pin coordinates from net
     * @param net Pointer to the net
     * @param db Pointer to placement database
     * @param points Output vector of pin coordinates
     * @param pins Output vector of corresponding pin pointers
     * @return Number of valid pins extracted
     */
static size_t extractPinPoints(Net* net, PlacerDB* db, 
                                         std::vector<Point>& nodes, std::vector<Pin*>& pins);

private:
    
    /**
     * @brief Validate input parameters
     * @param net Pointer to the net
     * @param db Pointer to placement database
     * @return True if valid, false otherwise
     */
    static bool validateInput(Net* net, PlacerDB* db);
    
    /**
     * @brief Print topology statistics
     * @param segments Vector of segments
     * @param original_star_length Total length of star topology (for comparison)
     */
    static void printTopologyStats(const std::vector<Segment>& segments, 
                                  double original_star_length);
};

/**
 * @struct TopologyStats
 * @brief Statistics for routing topology analysis
 */
struct TopologyStats {
    size_t num_pins;               ///< Number of pins in the net
    size_t num_segments;           ///< Number of segments in MST
    double mst_total_length;       ///< Total length of MST topology
    double star_total_length;      ///< Total length of star topology
    double length_reduction;       ///< Percentage reduction in wirelength
    double steiner_ratio;          ///< MST length / optimal Steiner tree length (estimated)
};

/**
 * @brief Analyze topology quality and statistics
 * @param net Pointer to the analyzed net
 * @param segments MST segments
 * @param db Placement database
 * @return Topology statistics
 */
TopologyStats analyzeTopology(Net* net, const std::vector<Segment>& segments, PlacerDB* db);

} // namespace mini

#endif // MINI_STEINER_TREE_H