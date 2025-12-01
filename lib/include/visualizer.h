/**
 * @file visualizer.h
 * @brief Visualization Module for MiniPlacement
 * @details Python Bridge for matplotlib-based layout visualization
 */

#ifndef MINI_VISUALIZER_H
#define MINI_VISUALIZER_H

#include <string>
#include <vector>

namespace mini {

// Forward declaration to avoid circular dependency
class PlacerDB;

/**
 * @class Visualizer
 * @brief Generates Python scripts to visualize placement using matplotlib
 */
class Visualizer {
public:
    /**
     * @brief Constructor
     * @param db Pointer to the placement database
     */
    explicit Visualizer(PlacerDB* db);

    /**
     * @brief Draw current placement and save as image
     * @param filename Output filename (e.g., "iter_001.png")
     * @details Generates and executes a Python script using matplotlib
     */
    void drawPlacement(const std::string& filename);

    /**
     * @brief Draw current placement with specific run ID
     * @param filename Output filename (e.g., "legalized.png")
     * @param run_id Run ID for directory organization
     * @details Generates and executes a Python script using matplotlib
     */
    void drawPlacementWithRunId(const std::string& filename, const std::string& run_id);

    /**
     * @brief Export placement data to CSV format
     * @param filename Output CSV filename
     * @details Creates CSV file with cell positions and dimensions
     */
    void exportToCSV(const std::string& filename);

private:
    PlacerDB* db_;  // Placement database

    /**
     * @brief Generate Python script for plotting
     * @param script_filename Python script filename
     * @param image_filename Output image filename
     */
    void generatePythonScript(const std::string& script_filename, 
                            const std::string& image_filename);

    /**
     * @brief Execute Python script
     * @param script_filename Python script to execute
     */
    void executePythonScript(const std::string& script_filename);
};

} // namespace mini

#endif // MINI_VISUALIZER_H