/**
 * @file visualizer.cpp
 * @brief Visualization Implementation for MiniEDA
 */

#include "visualizer.h"
#include "../../apps/mini_placement/placer_db.h"
#include "../../apps/mini_router/routing_grid.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

// Forward declaration and include for PlacerDB
#include "../../apps/mini_placement/placer_db.h"
#include "../../apps/mini_router/maze_router.h"

namespace mini {

Visualizer::Visualizer(PlacerDB* db) : db_(db) {
}

void Visualizer::drawPlacement(const std::string& filename) {
    if (!db_) return;

    // Check if filename contains a run_id (format: runid_imagename.png)
    size_t underscore_pos = filename.find('_');
    std::string run_id;
    std::string image_name;
    
    if (underscore_pos != std::string::npos && 
        filename.substr(0, underscore_pos).length() >= 12) {  // Run IDs are at least 12 digits
        // This looks like a run_id format
        run_id = filename.substr(0, underscore_pos);
        image_name = filename.substr(underscore_pos + 1);
    } else {
        // This is a simple filename like "legalized.png", use current run_id
        // We need to get the current run_id from somewhere - for now use a default
        run_id = "current_run";
        image_name = filename;
    }
    
    // Remove .png extension if present
    size_t dot_pos = image_name.find_last_of('.');
    if (dot_pos != std::string::npos) {
        image_name = image_name.substr(0, dot_pos);
    }
    
    // Create subdirectory for this run
    std::string run_dir = "visualizations/" + run_id;
    std::string mkdir_cmd = "mkdir -p " + run_dir;
    std::system(mkdir_cmd.c_str());

    // Generate script filename and image path
    std::string script_filename = run_dir + "/plot_" + image_name + ".py";
    std::string image_path = run_dir + "/" + image_name + ".png";
    
    // Generate Python script
    generatePythonScript(script_filename, image_path);
    
    // Execute the script
    executePythonScript(script_filename);
}

void Visualizer::drawPlacementWithRunId(const std::string& filename, const std::string& run_id) {
    if (!db_) return;

    // Remove .png extension if present
    std::string image_name = filename;
    size_t dot_pos = image_name.find_last_of('.');
    if (dot_pos != std::string::npos) {
        image_name = image_name.substr(0, dot_pos);
    }
    
    // Create subdirectory for this run
    std::string run_dir = "visualizations/" + run_id;
    std::string mkdir_cmd = "mkdir -p " + run_dir;
    std::system(mkdir_cmd.c_str());

    // Generate script filename and image path
    std::string script_filename = run_dir + "/plot_" + image_name + ".py";
    std::string image_path = run_dir + "/" + image_name + ".png";
    
    // Generate Python script
    generatePythonScript(script_filename, image_path);
    
    // Execute the script
    executePythonScript(script_filename);
}

void Visualizer::exportToCSV(const std::string& filename) {
    if (!db_) return;

    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open CSV file: " << filename << std::endl;
        return;
    }

    // CSV header
    csv_file << "cell_name,x,y,width,height,fixed\n";

    // Write all cells
    for (Cell* cell : db_->getAllCells()) {
        const auto& info = db_->getCellInfo(cell);
        csv_file << cell->getName() << ","
                 << info.x << ","
                 << info.y << ","
                 << info.width << ","
                 << info.height << ","
                 << (info.fixed ? "true" : "false") << "\n";
    }

    csv_file.close();
    std::cout << "Exported placement to CSV: " << filename << std::endl;
}

void Visualizer::drawRoutedPlacement(const std::string& filename, 
                                    const std::vector<RoutingResult>& routing_results,
                                    RoutingGrid* routing_grid) {
    std::string script_filename = filename + ".py";
    std::string image_filename = filename + ".png";
    
    generateRoutedPythonScript(script_filename, image_filename, routing_results, routing_grid);
    executePythonScript(script_filename);
}

Point Visualizer::gridToPhys(const GridPoint& gp, RoutingGrid* routing_grid) {
    if (!routing_grid) {
        return Point(0.0, 0.0);  // Fallback
    }
    
    // Convert grid coordinates to physical coordinates
    // This is the inverse of physToGrid in RoutingGrid
    const Rect& core_area = routing_grid->getCoreArea();
    double pitch_x = routing_grid->getPitchX();
    double pitch_y = routing_grid->getPitchY();
    
    // Grid point to physical coordinates (remove +0.5 offset to match RoutingGrid::physToGrid)
    // Grid point represents the intersection, not the cell center
    double phys_x = core_area.x_min + gp.x * pitch_x;
    double phys_y = core_area.y_min + gp.y * pitch_y;
    
    return Point(phys_x, phys_y);
}

void Visualizer::generateRoutedPythonScript(const std::string& script_filename, 
                                            const std::string& image_filename,
                                            const std::vector<RoutingResult>& routing_results,
                                            RoutingGrid* routing_grid) {
    std::ofstream script_file(script_filename);
    if (!script_file.is_open()) {
        std::cerr << "Error: Cannot create Python script: " << script_filename << std::endl;
        return;
    }

    // Get core area
    const Rect& core = db_->getCoreArea();

    script_file << "#!/usr/bin/env python3\n";
    script_file << "import matplotlib.pyplot as plt\n";
    script_file << "import matplotlib.patches as patches\n\n";
    script_file << "# Create figure and axis\n";
    script_file << "fig, ax = plt.subplots(1, 1, figsize=(12, 10))\n\n";

    // Draw core area boundary
    script_file << "# Draw core area boundary\n";
    script_file << "core_rect = patches.Rectangle((" << core.x_min << ", " << core.y_min << "), ";
    script_file << core.width() << ", " << core.height() << ", ";
    script_file << "linewidth=2, edgecolor='black', facecolor='none')\n";
    script_file << "ax.add_patch(core_rect)\n\n";

    // Draw cells (gray background for routed layout)
    script_file << "# Draw cells\n";
    for (Cell* cell : db_->getAllCells()) {
        const auto& info = db_->getCellInfo(cell);
        
        script_file << "# Cell: " << cell->getName() << "\n";
        script_file << "cell_rect = patches.Rectangle((" << info.x << ", " << info.y << "), ";
        script_file << info.width << ", " << info.height << ", ";
        script_file << "linewidth=0.5, edgecolor='gray', facecolor='lightgray', alpha=0.6)\n";
        script_file << "ax.add_patch(cell_rect)\n";
    }

    // Draw routing paths
    script_file << "# Draw routing paths\n";
    for (const auto& result : routing_results) {
        if (!result.success || result.segments.empty()) continue;
        
        script_file << "# Net paths (wirelength: " << result.total_wirelength << " units, vias: " << result.total_vias << ")\n";
        
        // Draw each segment
        for (const auto& path : result.segments) {

        // Draw segments in this path
            for (size_t i = 0; i < path.size() - 1; ++i) {
                const GridPoint& curr = path[i];
                const GridPoint& next = path[i + 1];

                // Convert grid coordinates to physical coordinates using helper method
                Point phys_curr = gridToPhys(curr, routing_grid);
                Point phys_next = gridToPhys(next, routing_grid);

                // Determine color based on layer
                std::string color;
                if (curr.layer == 0) {
                    color = "blue";    // M1 - horizontal
                } else {
                    color = "red";     // M2 - vertical
                }

                script_file << "plt.plot([" << phys_curr.x << ", " << phys_next.x << "], ["
                             << phys_curr.y << ", " << phys_next.y << "], ";
                script_file << "color='" << color << "', linewidth=1.0)\n";

                // Draw via if layer changes
                if (curr.layer != next.layer) {
                    script_file << "plt.scatter(" << phys_curr.x << ", " << phys_curr.y
                                 << ", color='black', marker='o', s=10, zorder=5)\n";
                }
            }
        }
        script_file << "\n";
    }

    // Set plot properties
    script_file << "# Set plot properties\n";
    script_file << "ax.set_aspect('equal')\n";
    script_file << "ax.set_xlim(" << (core.x_min - 10) << ", " << (core.x_max + 10) << ")\n";
    script_file << "ax.set_ylim(" << (core.y_min - 10) << ", " << (core.y_max + 10) << ")\n";
    script_file << "ax.set_xlabel('X (micrometers)')\n";
    script_file << "ax.set_ylabel('Y (micrometers)')\n";
    script_file << "ax.set_title('MiniRouter - " << image_filename << "')\n";
    script_file << "ax.grid(True, alpha=0.3)\n\n";

    // Add legend
    script_file << "# Add legend\n";
    script_file << "from matplotlib.lines import Line2D\n";
    script_file << "legend_elements = [\n";
    script_file << "    Line2D([0], [0], color='blue', linewidth=1, label='Metal 1 (Horizontal)'),\n";
    script_file << "    Line2D([0], [0], color='red', linewidth=1, label='Metal 2 (Vertical)'),\n";
    script_file << "    Line2D([0], [0], marker='o', color='w', markerfacecolor='k', markersize=4, label='Via')\n";
    script_file << "]\n";
    script_file << "ax.legend(handles=legend_elements, loc='upper right')\n\n";

    // Save the plot
    script_file << "# Save the plot\n";
    script_file << "plt.tight_layout()\n";
    script_file << "plt.savefig('" << image_filename << "', dpi=150, bbox_inches='tight')\n";
    script_file << "plt.close()\n\n";
    script_file << "print(\"Routed plot saved as: " << image_filename << "\")\n";

    script_file.close();
}

void Visualizer::generatePythonScript(const std::string& script_filename, 
                                    const std::string& image_filename) {
    std::ofstream script_file(script_filename);
    if (!script_file.is_open()) {
        std::cerr << "Error: Cannot create Python script: " << script_filename << std::endl;
        return;
    }

    // Get core area
    const Rect& core = db_->getCoreArea();

    script_file << "#!/usr/bin/env python3\n";
    script_file << "import matplotlib.pyplot as plt\n";
    script_file << "import matplotlib.patches as patches\n\n";
    script_file << "# Create figure and axis\n";
    script_file << "fig, ax = plt.subplots(1, 1, figsize=(12, 10))\n\n";

    // Draw core area boundary
    script_file << "# Draw core area boundary\n";
    script_file << "core_rect = patches.Rectangle((" << core.x_min << ", " << core.y_min << "), ";
    script_file << core.width() << ", " << core.height() << ", ";
    script_file << "linewidth=2, edgecolor='black', facecolor='none')\n";
    script_file << "ax.add_patch(core_rect)\n\n";

    // Draw cells
    script_file << "# Draw cells\ncell_colors = []\n";

    // Add cells to Python script with smart filtering
    for (Cell* cell : db_->getAllCells()) {
        const auto& info = db_->getCellInfo(cell);
        
        script_file << "# Cell: " << cell->getName() << "\n";
        script_file << "cell_rect = patches.Rectangle((" << info.x << ", " << info.y << "), ";
        script_file << info.width << ", " << info.height << ", ";
        script_file << "linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.5)\n";
        script_file << "ax.add_patch(cell_rect)\n";
        
        // Dynamic fontsize based on cell width
        double font_size = std::min(6.0, info.width * 1.5);
        
        // Filter: only show labels for cells large enough
        if (font_size > 3.0) {
            script_file << "ax.text(" << (info.x + info.width/2) << ", ";
            script_file << (info.y + info.height/2) << ", '" << cell->getName() << "', ";
            script_file << "ha='center', va='center', fontsize=" << font_size << ", alpha=1.0)\n";
        }
        script_file << "\n";
    }


    // Add nets (connections)
    script_file << "# Draw nets as fly-lines\nnet_colors = []\n";

    // Get netlist from the database
    NetlistDB* netlist_db = nullptr; // We'll need to access this from PlacerDB
    // For now, we'll skip net drawing as we need access to NetlistDB

    // Set plot properties
    script_file << "# Set plot properties\n";
    script_file << "ax.set_aspect('equal')\n";
    script_file << "ax.set_xlim(" << (core.x_min - 10) << ", " << (core.x_max + 10) << ")\n";
    script_file << "ax.set_ylim(" << (core.y_min - 10) << ", " << (core.y_max + 10) << ")\n";
    script_file << "ax.set_xlabel('X (micrometers)')\n";
    script_file << "ax.set_ylabel('Y (micrometers)')\n";
    script_file << "ax.set_title('MiniPlacement - " << image_filename << "')\n";
    script_file << "ax.grid(True, alpha=0.3)\n\n";

    // Save the plot
    script_file << "# Save the plot\n";
    script_file << "plt.tight_layout()\n";
    script_file << "plt.savefig('" << image_filename << "', dpi=150, bbox_inches='tight')\n";
    script_file << "plt.close()\n\n";
    script_file << "print(\"Plot saved as: " << image_filename << "\")\n";

    script_file.close();
}

void Visualizer::executePythonScript(const std::string& script_filename) {
    std::string command = "python3 " + script_filename;
    int result = std::system(command.c_str());
    
    if (result == 0) {
        std::cout << "Successfully generated plot using: " << script_filename << std::endl;
    } else {
        std::cerr << "Error: Failed to execute Python script: " << script_filename << std::endl;
        std::cerr << "Make sure python3 and matplotlib are installed." << std::endl;
    }
}

} // namespace mini
