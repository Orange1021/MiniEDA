/**
 * @file placer_db.h
 * @brief Physical Database for MiniPlacement
 * @details Manages physical information (position, size) for placement
 */

#ifndef MINI_PLACER_DB_H
#define MINI_PLACER_DB_H

#include "../../lib/include/netlist_db.h"
#include "../../lib/include/geometry.h"
#include "../../lib/include/lef_parser.h"
#include <unordered_map>
#include <random>

namespace mini {

/**
 * @class PlacerDB
 * @brief Physical database for placement algorithms
 */
class PlacerDB {
public:
    /**
     * @struct CellInfo
     * @brief Physical information for a cell
     */
    struct CellInfo {
        double x, y;           // Position (lower-left corner)
        double width, height;  // Dimensions
        bool fixed;            // Whether cell position is fixed

        CellInfo() : x(0.0), y(0.0), width(0.0), height(0.0), fixed(false) {}
        CellInfo(double x_pos, double y_pos, double w, double h, bool is_fixed = false)
            : x(x_pos), y(y_pos), width(w), height(h), fixed(is_fixed) {}
    };

    /**
     * @brief Constructor
     * @param netlist_db Pointer to the netlist database
     */
    explicit PlacerDB(NetlistDB* netlist_db);

    /**
     * @brief Set the core placement area
     * @param core_area Rectangle defining the placement boundary
     */
    void setCoreArea(const Rect& core_area);

    /**
     * @brief Get the core placement area
     * @return Core area rectangle
     */
    const Rect& getCoreArea() const { return core_area_; }

    /**
     * @brief Set row height for cell size calculation
     * @param height Row height in micrometers
     */
    void setRowHeight(double height) { row_height_ = height; }

    /**
     * @brief Get row height
     * @return Row height
     */
    double getRowHeight() const { return row_height_; }

    /**
     * @brief Set site width for grid alignment
     * @param width Site width in micrometers
     */
    void setSiteWidth(double width) { site_width_ = width; }

    /**
     * @brief Get site width
     * @return Site width in micrometers
     */
    double getSiteWidth() const { return site_width_; }

    /**
     * @brief Add a cell to the physical database (legacy method)
     * @param cell Pointer to the netlist cell
     * @param area Cell area (from liberty library)
     */
    void addCell(Cell* cell, double area);

    /**
     * @brief Add a cell to the physical database with precise dimensions
     * @param cell Pointer to the netlist cell
     * @param width Cell width in micrometers
     * @param height Cell height in micrometers
     */
    void addCell(Cell* cell, double width, double height);

    /**
     * @brief Set LEF macro pointer for a cell (for future routing use)
     * @param cell Pointer to the cell
     * @param lef_macro Pointer to LEF macro
     */
    void setCellLefMacro(Cell* cell, const LefMacro* lef_macro);

    /**
     * @brief Get LEF macro pointer for a cell
     * @param cell Pointer to the cell
     * @return Pointer to LEF macro, nullptr if not available
     */
    const LefMacro* getCellLefMacro(Cell* cell) const;

    /**
     * @brief Place a cell at specific coordinates
     * @param cell Pointer to the cell
     * @param x X coordinate (lower-left corner)
     * @param y Y coordinate (lower-left corner)
     */
    void placeCell(Cell* cell, double x, double y);

    /**
     * @brief Get cell center position
     * @param cell Pointer to the cell
     * @return Center point of the cell
     */
    Point getCellCenter(Cell* cell) const;

    /**
     * @brief Get cell information
     * @param cell Pointer to the cell
     * @return Reference to cell info
     */
    const CellInfo& getCellInfo(Cell* cell) const;

    /**
     * @brief [New] Commit placement coordinates from PlacerDB to Cell objects
     * @details Synchronizes internal CellInfo positions with actual Cell objects
     *          Essential for algorithm handoff (e.g., Phase 1 → Phase 2)
     */
    void commitPlacement();

    /**
     * @brief Get cell information (mutable)
     * @param cell Pointer to the cell
     * @return Reference to cell info
     */
    CellInfo& getCellInfo(Cell* cell);

    // ============ Convenience Methods for Cell Info Access ============
    /**
     * @brief Get cell X position
     * @param cell Pointer to the cell
     * @return X coordinate of cell
     */
    double getCellX(Cell* cell) const { return getCellInfo(cell).x; }

    /**
     * @brief Get cell Y position
     * @param cell Pointer to the cell
     * @return Y coordinate of cell
     */
    double getCellY(Cell* cell) const { return getCellInfo(cell).y; }

    /**
     * @brief Get cell width
     * @param cell Pointer to the cell
     * @return Width of cell
     */
    double getCellWidth(Cell* cell) const { return getCellInfo(cell).width; }

    /**
     * @brief Get cell height
     * @param cell Pointer to the cell
     * @return Height of cell
     */
    double getCellHeight(Cell* cell) const { return getCellInfo(cell).height; }

    /**
     * @brief Check if cell is fixed
     * @param cell Pointer to the cell
     * @return true if cell position is fixed
     */
    bool isCellFixed(Cell* cell) const { return getCellInfo(cell).fixed; }

    /**
     * @brief Get cell right edge X position
     * @param cell Pointer to the cell
     * @return Right edge X coordinate
     */
    double getCellRight(Cell* cell) const { 
        const auto& info = getCellInfo(cell); 
        return info.x + info.width; 
    }

    /**
     * @brief Get cell top edge Y position
     * @param cell Pointer to the cell
     * @return Top edge Y coordinate
     */
    double getCellTop(Cell* cell) const { 
        const auto& info = getCellInfo(cell); 
        return info.y + info.height; 
    }

    /**
     * @brief Initialize random placement for all cells
     */
    void initializeRandom();

    /**
     * @brief Get all cells
     * @return Vector of all cell pointers
     */
    std::vector<Cell*> getAllCells() const;

    /**
     * @brief Get netlist database
     * @return Pointer to the netlist database
     */
    NetlistDB* getNetlistDB() const { return netlist_db_; }
    /**
     * @brief Get cells organized by rows
     * @return Vector of rows, each containing cells sorted by X coordinate
     */
    std::vector<std::vector<Cell*>> getCellsByRow();

    /**
     * @brief Get total cell area
     * @return Sum of all cell areas
     */
    double getTotalCellArea() const;

    /**
     * @brief Check if a placement is valid (within core bounds)
     * @param cell Pointer to the cell
     * @return True if placement is valid
     */
    bool isValidPlacement(Cell* cell) const;

    /**
     * @brief Static comparison function for sorting cells by X coordinate
     * @param db Pointer to PlacerDB instance
     * @param a First cell
     * @param b Second cell
     * @return True if cell a's X coordinate is less than cell b's
     */
    static bool compareByX(PlacerDB* db, const Cell* a, const Cell* b);

    /**
     * @brief Check if a coordinate is aligned to site grid
     * @param value Coordinate value to check
     * @param site_width Site width for alignment
     * @param epsilon Relative tolerance (default: 1e-10)
     * @return True if aligned, false otherwise
     */
    static bool isSiteAligned(double value, double site_width, double epsilon = 1e-10);

private:
    NetlistDB* netlist_db_;                                    // Netlist database
    Rect core_area_;                                           // Placement boundary
    double row_height_;                                        // Standard row height
    double site_width_;                                        // Standard site width (for grid alignment)
    std::unordered_map<Cell*, CellInfo> cell_infos_;           // Physical data
    std::unordered_map<Cell*, const LefMacro*> cell_lef_macros_; // LEF macro pointers
    std::mt19937 random_engine_;                               // Random number generator
};

} // namespace mini

#endif // MINI_PLACER_DB_H