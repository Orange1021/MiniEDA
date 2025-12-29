/**
 * @file detailed_placer.h
 * @brief Detailed Placement Algorithm for Local Optimization
 * @details Implements sliding window reordering and global swap strategies
 *          to optimize HPWL while maintaining site alignment
 */

#ifndef MINI_DETAILED_PLACER_H
#define MINI_DETAILED_PLACER_H

#include "../../lib/include/placer_db.h"
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/hpwl_calculator.h"
#include <vector>
#include <memory>

namespace mini {

/**
 * @class DetailedPlacer
 * @brief Local placement optimizer using discrete optimization techniques
 * @details Operates in discrete site-aligned space, preserves site alignment
 */
class DetailedPlacer {
public:
    /**
     * @brief Constructor
     * @param db Pointer to placement database
     * @param netlist Pointer to netlist database
     */
    DetailedPlacer(PlacerDB* db, NetlistDB* netlist);

    /**
     * @brief Destructor
     */
    ~DetailedPlacer() = default;

    // Disable copy, enable move
    DetailedPlacer(const DetailedPlacer&) = delete;
    DetailedPlacer& operator=(const DetailedPlacer&) = delete;
    DetailedPlacer(DetailedPlacer&&) = default;
    DetailedPlacer& operator=(DetailedPlacer&&) = default;

    /**
     * @brief Main entry point for detailed placement
     * @details Executes global swap followed by detailed reordering
     */
    void run();

    /**
     * @brief Set verbose mode for debugging
     * @param verbose Enable verbose output
     */
    void setVerbose(bool verbose) { verbose_ = verbose; }

    /**
     * @brief Set number of reordering iterations
     * @param iterations Number of iterations to run
     */
    void setIterations(int iterations) { iterations_ = iterations; }

    /**
     * @brief Get final HPWL after optimization
     * @return Final HPWL value
     */
    double getFinalHPWL() const { return final_hpwl_; }

private:
    PlacerDB* db_;                    ///< Placement database
    NetlistDB* netlist_;             ///< Netlist database
    bool verbose_;                    ///< Verbose output flag
    int iterations_;                  ///< Number of reordering iterations
    double initial_hpwl_;
    double final_hpwl_;               ///< Final HPWL after optimization

    // ========================================================================
    // Strategy 1: Global Swap (Equal-width cell exchange)
    // ========================================================================
    
    /**
     * @brief Global swapping strategy for long-distance optimization
     * @details Swaps cells of equal width across different rows
     */
    void globalSwap();

    /**
     * @brief Try swapping two cells and keep if HPWL improves
     * @param c1 First cell to swap
     * @param c2 Second cell to swap
     * @return True if swap was kept (HPWL improved)
     */
    bool trySwap(Cell* c1, Cell* c2);

    // ========================================================================
    // Strategy 2: Detailed Reordering (Sliding Window)
    // ========================================================================
    
    /**
     * @brief Detailed reordering using sliding window approach
     * @details Optimizes local wirelength by permuting cells within windows
     */
    void detailedReordering();

    /**
     * @brief Optimize a single window of cells
     * @param window_cells Cells in the current window
     * @param start_x Starting X coordinate of the window
     * @return True if improvement was found
     */
    bool optimizeWindow(std::vector<Cell*>& window_cells, double start_x);

    /**
     * @brief Re-pack cells in site-aligned manner
     * @details Places cells compactly from start_x, maintaining site alignment
     * @param cells Cells to re-pack
     * @param start_x Starting X coordinate (must be site-aligned)
     */
    void rePackToSites(const std::vector<Cell*>& cells, double start_x);

    /**
     * @brief Calculate HPWL for a subset of cells
     * @param cells Cells to calculate HPWL for
     * @return HPWL value for the cell subset
     */
    double calculateLocalHPWL(const std::vector<Cell*>& cells);

    // ========================================================================
    // Utility Functions
    // ========================================================================
    
    /**
     * @brief Debug logging helper
     * @param message Debug message
     */
    void debugLog(const std::string& message) const;

    /**
     * @brief Get all cells organized by rows
     * @return Vector of rows, each containing cells sorted by X coordinate
     */
    std::vector<std::vector<Cell*>> getCellsByRow();

    /**
     * @brief Check if two cells have equal width
     * @param c1 First cell
     * @param c2 Second cell
     * @return True if cells have equal width
     */
    /**
     * @brief Calculate HPWL for a single net
     * @param net Net to calculate HPWL for
     * @return HPWL value for the net
     */
    double calculateNetHPWL(Net* net);
    /**
     * @brief Check if a coordinate is aligned to site grid
     * @param x X coordinate to check
     * @param site_width Site width for alignment
     * @return True if aligned, false otherwise
     */
    bool isSiteAligned(double x, double site_width) const;

    bool hasEqualWidth(Cell* c1, Cell* c2) const;
};

} // namespace mini

#endif // MINI_DETAILED_PLACER_H