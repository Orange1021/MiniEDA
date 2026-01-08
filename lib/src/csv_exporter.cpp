/**
 * @file csv_exporter.cpp
 * @brief CSV Export Utilities Implementation
 */

#include "csv_exporter.h"
#include "placer_db.h"
#include "netlist_db.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

namespace mini {

bool CSVExporter::exportPlacement(PlacerDB* placer_db, const std::string& filename) {
    if (!placer_db) {
        std::cerr << "Error: PlacerDB is null" << std::endl;
        return false;
    }

    // Ensure directory exists
    if (!ensureDirectory(filename)) {
        return false;
    }

    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open CSV file: " << filename << std::endl;
        return false;
    }

    // CSV header
    csv_file << "cell_name,x,y,width,height,fixed\n";

    // Write all cells
    for (const auto& cell : placer_db->getAllCells()) {
        const auto& info = placer_db->getCellInfo(cell);
        csv_file << cell->getName() << ","
                 << info.x << ","
                 << info.y << ","
                 << info.width << ","
                 << info.height << ","
                 << (info.fixed ? "true" : "false") << "\n";
    }

    csv_file.close();
    std::cout << "Exported placement to CSV: " << filename << std::endl;
    return true;
}

bool CSVExporter::exportNetlist(NetlistDB* netlist_db, const std::string& filename) {
    if (!netlist_db) {
        std::cerr << "Error: NetlistDB is null" << std::endl;
        return false;
    }

    // Ensure directory exists
    if (!ensureDirectory(filename)) {
        return false;
    }

    std::ofstream csv_file(filename);
    if (!csv_file.is_open()) {
        std::cerr << "Error: Cannot open CSV file: " << filename << std::endl;
        return false;
    }

    // CSV header
    csv_file << "net_name,driver,load\n";

    // Write net connectivity
    for (const auto& net : netlist_db->getNets()) {
        const auto& pins = net->getAllPins();
        if (pins.empty()) continue;

        // Find driver (first pin that is an output)
        std::string driver_pin = "";
        for (const auto& pin : pins) {
            if (pin->getDirection() == PinDirection::OUTPUT) {
                driver_pin = pin->getOwner()->getName() + "." + pin->getName();
                break;
            }
        }

        // Write loads
        for (const auto& pin : pins) {
            if (pin->getDirection() == PinDirection::INPUT) {
                std::string load_pin = pin->getOwner()->getName() + "." + pin->getName();
                csv_file << net->getName() << ","
                         << driver_pin << ","
                         << load_pin << "\n";
            }
        }
    }

    csv_file.close();
    std::cout << "Exported netlist to CSV: " << filename << std::endl;
    return true;
}

bool CSVExporter::ensureDirectory(const std::string& filepath) {
    // Extract directory from filepath
    size_t last_slash = filepath.find_last_of('/');
    if (last_slash == std::string::npos) {
        // No directory, just filename
        return true;
    }

    std::string directory = filepath.substr(0, last_slash);
    
    // Create directory using mkdir command
    std::string mkdir_cmd = "mkdir -p " + directory;
    int result = std::system(mkdir_cmd.c_str());
    
    return (result == 0);
}

} // namespace mini
