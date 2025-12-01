/**
 * @file timing_report.cpp
 * @brief Timing Report Implementation
 * @details Implementation of critical path extraction and reporting
 */

#include "timing_report.h"
#include "timing_graph.h"
#include "timing_path.h"
#include "../../lib/include/cell.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cmath>

namespace mini {

// ============================================================================
// TimingPath Implementation
// ============================================================================

void TimingPath::addPoint(const PathPoint& point) {
    points_.push_back(point);
}

void TimingPath::reverse() {
    std::reverse(points_.begin(), points_.end());
}

TimingNode* TimingPath::getStartPoint() const {
    if (points_.empty()) return nullptr;
    return points_.front().node;
}

TimingNode* TimingPath::getEndPoint() const {
    if (points_.empty()) return nullptr;
    return points_.back().node;
}

double TimingPath::getSlack() const {
    if (points_.empty()) return 0.0;
    return points_.back().node->getSlack();
}

double TimingPath::getDataArrivalTime() const {
    if (points_.empty()) return 0.0;
    return points_.back().node->getArrivalTime();
}

double TimingPath::getDataRequiredTime() const {
    if (points_.empty()) return 0.0;
    return points_.back().node->getRequiredTime();
}

void TimingPath::printPath() const {
    if (points_.empty()) {
        std::cout << "Error: Empty timing path" << std::endl;
        return;
    }

    // Print header
    std::cout << "\n";
    std::cout << std::setw(40) << std::left << "Point"
              << std::setw(12) << std::right << "Incr"
              << std::setw(12) << std::right << "Path"
              << std::endl;
    std::cout << std::string(64, '-') << std::endl;

    // Print each point in the path
    for (const auto& point : points_) {
        // Build full name: cell_name/pin_name
        Pin* pin = point.node->getPin();
        std::string point_name = pin->getOwner()->getName() + "/" + pin->getName();

        // Truncate if too long
        if (point_name.length() > 38) {
            point_name = point_name.substr(0, 35) + "...";
        }

        std::cout << std::setw(40) << std::left << point_name
                  << std::setw(12) << std::fixed << std::setprecision(2) << std::right << point.incr_delay
                  << std::setw(12) << std::fixed << std::setprecision(2) << std::right << point.arrival_time
                  << std::endl;
    }

    // Print summary
    std::cout << std::string(64, '-') << std::endl;
    std::cout << std::setw(40) << std::left << "data arrival time"
              << std::setw(24) << std::right << getDataArrivalTime() << std::endl;
    std::cout << std::endl;
    std::cout << std::setw(40) << std::left << "data required time"
              << std::setw(24) << std::right << getDataRequiredTime() << std::endl;
    std::cout << std::string(64, '-') << std::endl;

    // Print slack with (MET) or (VIOLATED) indicator
    double slack = getSlack();
    std::string slack_status = (slack >= 0) ? " (MET)" : " (VIOLATED)";
    std::cout << std::setw(40) << std::left << "slack" + slack_status
              << std::setw(24) << std::right << slack << std::endl;
    std::cout << std::endl;
}

// ============================================================================
// TimingReporter Implementation
// ============================================================================

TimingReporter::TimingReporter(TimingGraph* graph) : graph_(graph) {}

void TimingReporter::reportTiming(int top_k) {
    if (!graph_) {
        std::cerr << "Error: TimingGraph is null" << std::endl;
        return;
    }

    // Get all endpoints
    auto endpoints = graph_->getEndPoints();
    if (endpoints.empty()) {
        std::cout << "No endpoints found in timing graph" << std::endl;
        return;
    }

    std::cout << "\n=========================================="
              << "\nTiming Report (Top " << top_k << " Worst Paths)"
              << "\n==========================================" << std::endl;

    // Sort endpoints by slack (ascending, worst first)
    // Filter out uninitialized nodes first
    std::vector<TimingNode*> valid_endpoints;
    for (TimingNode* node : endpoints) {
        double at = node->getArrivalTime();
        double rat = node->getRequiredTime();
        
        // Skip uninitialized nodes (same logic as in updateSlacks)
        if (at <= -TimingNode::UNINITIALIZED / 2 || rat >= TimingNode::UNINITIALIZED / 2) {
            continue;
        }
        valid_endpoints.push_back(node);
    }
    
    std::sort(valid_endpoints.begin(), valid_endpoints.end(),
              [](TimingNode* a, TimingNode* b) {
                  return a->getSlack() < b->getSlack();
              });

    // Generate and report top_k paths
    int paths_to_report = std::min(top_k, static_cast<int>(valid_endpoints.size()));
    for (int i = 0; i < paths_to_report; ++i) {
        TimingNode* endpoint = valid_endpoints[i];

        std::cout << "\nPath " << (i + 1) << ":"
                  << "\nEndpoint: " << endpoint->getPin()->getOwner()->getName() << "/" << endpoint->getPin()->getName()
                  << "\nSlack: " << endpoint->getSlack() << " ns"
                  << std::endl;

        // Generate the critical path
        auto path = generatePath(endpoint);
        if (path) {
            path->printPath();
        }
    }
}

std::unique_ptr<TimingPath> TimingReporter::generatePath(TimingNode* endpoint) {
    auto path = std::make_unique<TimingPath>();
    TimingNode* current_node = endpoint;

    const double epsilon = 1e-6; // Tolerance for floating-point comparison

    // Back-trace from endpoint to startpoint
    while (current_node) {
        TimingArc* critical_arc = nullptr;
        TimingNode* best_prev_node = nullptr;

        double target_arrival = current_node->getArrivalTime();

        // Find the incoming arc that caused the current arrival time
        auto incoming_arcs = graph_->getIncomingArcs(current_node);
        for (auto arc : incoming_arcs) {
            TimingNode* prev_node = arc->getFromNode();

            // Check if this arc is on the critical path
            // Formula: prev_AT + arc_delay H current_AT
            double prev_at = prev_node->getArrivalTime();
            double arc_delay = arc->getDelay();
            double calculated_at = prev_at + arc_delay;

            if (std::abs(calculated_at - target_arrival) < epsilon) {
                critical_arc = arc;
                best_prev_node = prev_node;
                break; // Found the critical predecessor
            }
        }

        // Calculate incremental delay
        double incr_delay = (critical_arc) ? critical_arc->getDelay() : 0.0;

        // Add current point to path (in reverse order)
        path->addPoint(PathPoint(current_node, critical_arc, target_arrival, incr_delay));

        // Move to previous node
        current_node = best_prev_node;

        // If no predecessor found (reached startpoint), exit loop
        if (!critical_arc) {
            break;
        }
    }

    // Reverse the path to get Start -> End order
    path->reverse();

    return path;
}

} // namespace mini
