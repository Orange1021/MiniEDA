#ifndef MINI_TIMING_REPORT_H
#define MINI_TIMING_REPORT_H

#include <vector>
#include <string>
#include <memory>

namespace mini {

class TimingGraph;
class TimingNode;
class TimingArc;

/**
 * @struct PathPoint
 * @brief Represents a single point in a timing path (one row in the report)
 */
struct PathPoint {
    TimingNode* node;           ///< Pointer to the timing node
    TimingArc* arrival_arc;     ///< Arc through which we arrived at this point (nullptr for first point)
    double arrival_time;        ///< Path accumulated delay (AT)
    double incr_delay;          ///< Incremental delay (delay of arrival_arc)

    /**
     * @brief Constructor
     */
    PathPoint(TimingNode* n, TimingArc* arc, double at, double incr)
        : node(n), arrival_arc(arc), arrival_time(at), incr_delay(incr) {}
};

/**
 * @class TimingPath
 * @brief Represents a complete timing path from startpoint to endpoint
 */
class TimingPath {
public:
    /**
     * @brief Add a point to the path
     * @param point The path point to add
     */
    void addPoint(const PathPoint& point);

    /**
     * @brief Reverse the path order (used after back-tracing)
     */
    void reverse();

    /**
     * @brief Get the start point of the path
     * @return Pointer to the start node
     */
    TimingNode* getStartPoint() const;

    /**
     * @brief Get the end point of the path
     * @return Pointer to the end node
     */
    TimingNode* getEndPoint() const;

    /**
     * @brief Get the slack of the path
     * @return Slack value
     */
    double getSlack() const;

    /**
     * @brief Get the data arrival time
     * @return Arrival time at endpoint
     */
    double getDataArrivalTime() const;

    /**
     * @brief Get the data required time
     * @return Required time at endpoint
     */
    double getDataRequiredTime() const;

    /**
     * @brief Print the path in PrimeTime-like format
     */
    void printPath() const;

private:
    std::vector<PathPoint> points_;  ///< Collection of points in the path
};

/**
 * @class TimingReporter
 * @brief Responsible for finding and reporting timing paths
 */
class TimingReporter {
public:
    /**
     * @brief Constructor
     * @param graph Pointer to the timing graph
     */
    explicit TimingReporter(TimingGraph* graph);

    /**
     * @brief Report the top k worst timing paths
     * @param top_k Number of paths to report (default: 1)
     */
    void reportTiming(int top_k = 1);

private:
    TimingGraph* graph_;  ///< Pointer to the timing graph (non-owning)

    /**
     * @brief Generate a timing path ending at the specified endpoint
     * @details Uses back-tracing algorithm to find the critical path
     * @param endpoint The end node of the path
     * @return Unique pointer to the generated path
     */
    std::unique_ptr<TimingPath> generatePath(TimingNode* endpoint);
};

} // namespace mini

#endif // MINI_TIMING_REPORT_H
