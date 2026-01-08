/**
 * @file csv_exporter.h
 * @brief CSV Export Utilities for MiniEDA
 * @details Static utilities for exporting placement and netlist data to CSV format
 */

#ifndef MINI_CSV_EXPORTER_H
#define MINI_CSV_EXPORTER_H

#include <string>
#include <vector>

namespace mini {

// Forward declarations
class PlacerDB;
class NetlistDB;

/**
 * @class CSVExporter
 * @brief Static utilities for exporting data to CSV format
 */
class CSVExporter {
public:
    /**
     * @brief Export placement data to CSV format
     * @param placer_db Pointer to the placement database
     * @param filename Output CSV filename
     * @return True if export successful, false otherwise
     * @details Creates CSV with columns: cell_name,x,y,width,height,fixed
     */
    static bool exportPlacement(PlacerDB* placer_db, const std::string& filename);

    /**
     * @brief Export netlist data to CSV format
     * @param netlist_db Pointer to the netlist database
     * @param filename Output CSV filename
     * @return True if export successful, false otherwise
     * @details Creates CSV with net connectivity information
     */
    static bool exportNetlist(NetlistDB* netlist_db, const std::string& filename);

private:
    // Helper method to create directory if it doesn't exist
    static bool ensureDirectory(const std::string& filepath);
};

} // namespace mini

#endif // MINI_CSV_EXPORTER_H