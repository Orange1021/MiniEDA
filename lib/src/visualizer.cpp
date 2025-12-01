/**
 * @file visualizer.cpp
 * @brief Visualization Implementation for MiniEDA
 */

#include "visualizer.h"
#include <fstream>
#include <iostream>
#include <sstream>

// Forward declaration and include for PlacerDB
#include "../../apps/mini_placement/placer_db.h"

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
