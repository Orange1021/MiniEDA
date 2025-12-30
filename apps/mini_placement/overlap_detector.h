/**
 * @file overlap_detector.h
 * @brief Overlap Detection Utility for Placement
 * @details Provides comprehensive overlap analysis and reporting functionality
 */

#ifndef OVERLAP_DETECTOR_H
#define OVERLAP_DETECTOR_H

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

namespace mini {

// Forward declarations
class PlacerDB;
class Cell;

/**
 * @brief Overlap information structure
 */
struct OverlapInfo {
    Cell* cell1;
    Cell* cell2;
    double overlap_area;
    double cell1_area;
    double cell2_area;
    
    OverlapInfo(Cell* c1, Cell* c2, double area, double area1, double area2)
        : cell1(c1), cell2(c2), overlap_area(area), cell1_area(area1), cell2_area(area2) {}
};

/**
 * @brief Overlap detection and analysis utility class
 * @details Provides comprehensive overlap analysis for placement validation
 */
class OverlapDetector {
public:
    /**
     * @brief Constructor
     * @param db Pointer to placement database
     */
    explicit OverlapDetector(PlacerDB* db);
    
    /**
     * @brief Destructor
     */
    ~OverlapDetector() = default;

    // Disable copy, enable move
    OverlapDetector(const OverlapDetector&) = delete;
    OverlapDetector& operator=(const OverlapDetector&) = delete;
    OverlapDetector(OverlapDetector&&) = default;
    OverlapDetector& operator=(OverlapDetector&&) = default;

    /**
     * @brief Check if any overlaps exist
     * @return True if overlaps are found
     */
    bool hasOverlaps() const;
    
    /**
     * @brief Calculate total overlap area
     * @return Total overlapping area
     */
    double calculateTotalOverlap() const;
    
    /**
     * @brief Get detailed overlap information
     * @return Vector of overlap information structures
     */
    std::vector<OverlapInfo> getOverlapDetails() const;
    
    /**
     * @brief Print overlap summary to console
     * @param verbose Enable detailed output
     */
    void printOverlapSummary(bool verbose = false) const;
    
    /**
     * @brief Generate overlap report
     * @param title Report title
     * @param verbose Enable detailed output
     */
    void generateReport(const std::string& title = "OVERLAP ANALYSIS", bool verbose = false) const;
    
    /**
     * @brief Count overlapping cell pairs
     * @return Number of overlapping cell pairs
     */
    int countOverlaps() const;
    
    /**
     * @brief Get overlapping cells set
     * @return Set of cells that are involved in overlaps
     */
    std::vector<Cell*> getOverlappingCells() const;
    
    /**
     * @brief Count boundary touching pairs (edges touching but no area overlap)
     * @return Number of boundary touching cell pairs
     */
    int countBoundaryTouches() const;

private:
    PlacerDB* db_;                    ///< Placement database
    static constexpr double EPSILON_ = 1e-9;  ///< Floating point tolerance
    
    /**
     * @brief Calculate overlap area between two cells
     * @param cell1 First cell
     * @param cell2 Second cell
     * @return Overlap area (0 if no overlap)
     */
    double calculateCellOverlap(Cell* cell1, Cell* cell2) const;
    
    /**
     * @brief Check if two cells overlap
     * @param cell1 First cell
     * @param cell2 Second cell
     * @return True if cells overlap
     */
    bool cellsOverlap(Cell* cell1, Cell* cell2) const;
};

} // namespace mini

#endif // OVERLAP_DETECTOR_H