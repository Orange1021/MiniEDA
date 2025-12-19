/**
 * @file density_grid.h
 * @brief Density Grid for Electrostatic Placement
 * @details Implements bin-based density calculation for modern placement algorithms
 *          Based on ePlace-style electrostatic field model
 */

#ifndef MINI_DENSITY_GRID_H
#define MINI_DENSITY_GRID_H

#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "../../lib/include/placer_db.h"
#include "../../lib/include/geometry.h"

namespace mini {

/**
 * @struct Bin
 * @brief Single bin in the density grid
 * @details Contains density, electrostatic potential and force information
 */
struct Bin {
    double x, y;                    ///< Bin center coordinates
    double density = 0.0;           ///< Current density (Cell Area / Bin Area)
    double electro_potential = 0.0; ///< Electrostatic potential (for Poisson equation)
    double electro_force_x = 0.0;   ///< Electric field force X component
    double electro_force_y = 0.0;   ///< Electric field force Y component
    
    /**
     * @brief Check if this bin is overcrowded
     * @param target_density Target utilization (e.g., 0.7)
     * @return true if density exceeds target
     */
    bool isOvercrowded(double target_density = 0.7) const {
        return density > target_density;
    }
    
    /**
     * @brief Get density overflow amount
     * @param target_density Target utilization
     * @return Amount by which density exceeds target (0 if not overcrowded)
     */
    double getOverflow(double target_density = 0.7) const {
        return std::max(0.0, density - target_density);
    }
};

/**
 * @class DensityGrid
 * @brief Bin-based density grid for electrostatic placement
 * @details Discretizes core area into bins and calculates cell density distribution
 *          Serves as the foundation for Poisson equation solving and force calculation
 */
class DensityGrid {
public:
    /**
     * @brief Constructor
     */
    DensityGrid();
    
    /**
     * @brief Destructor
     */
    ~DensityGrid() = default;

    // Disable copy, enable move
    DensityGrid(const DensityGrid&) = delete;
    DensityGrid& operator=(const DensityGrid&) = delete;
    DensityGrid(DensityGrid&&) = default;
    DensityGrid& operator=(DensityGrid&&) = default;

    /**
     * @brief Initialize the density grid
     * @param core_w Core area width
     * @param core_h Core area height
     * @param core_x_min Core area X minimum
     * @param core_y_min Core area Y minimum
     * @param bin_dim Bin dimension (typically 2-4x row height)
     * @return true if initialization successful
     */
    bool init(double core_w, double core_h, double core_x_min, double core_y_min, double bin_dim);

    /**
     * @brief Initialize the density grid with exact grid dimensions
     * @param grid_x Exact number of bins in X direction
     * @param grid_y Exact number of bins in Y direction
     * @param core_w Core area width
     * @param core_h Core area height
     * @param core_x_min Core area X minimum
     * @param core_y_min Core area Y minimum
     * @return true if initialization successful
     */
    bool init(int grid_x, int grid_y, double core_w, double core_h, double core_x_min, double core_y_min);

    /**
     * @brief Core function: Update density distribution
     * @param cells Vector of cell pointers to calculate density for
     * @param placer_db PlacerDB for accessing cell dimensions
     * @details Projects cell areas onto bins with exact overlap calculation
     */
    void updateDensity(const std::vector<Cell*>& cells, PlacerDB* placer_db);

    /**
     * @brief Get bin reference by grid indices
     * @param grid_x X grid index
     * @param grid_y Y grid index
     * @return Reference to bin (throws if out of bounds)
     */
    Bin& getBin(int grid_x, int grid_y);

    /**
     * @brief Get bin reference by grid indices (const version)
     */
    const Bin& getBin(int grid_x, int grid_y) const;

    /**
     * @brief Get bin index from world coordinates
     * @param x World X coordinate
     * @param y World Y coordinate
     * @return Pair of (grid_x, grid_y) indices
     * @details Clamps to valid range if coordinate is outside core
     */
    std::pair<int, int> getBinIndex(double x, double y) const;

    /**
     * @brief Get bin by world coordinates
     * @param x World X coordinate
     * @param y World Y coordinate
     * @return Reference to nearest bin
     */
    Bin& getBinByCoord(double x, double y);

    /**
     * @brief Get bin by world coordinates (const version)
     */
    const Bin& getBinByCoord(double x, double y) const;

    /**
     * @brief Export density map to CSV for visualization
     * @param filename Output CSV filename
     * @details Creates CSV file with bin coordinates and density values
     *          Can be plotted with Python/Matplotlib for debugging
     */
    void exportDensityMap(const std::string& filename) const;

    /**
     * @brief Export electrostatic potential to CSV
     * @param filename Output CSV filename
     */
    void exportPotentialMap(const std::string& filename) const;

    /**
     * @brief Print density statistics
     * @param target_density Target utilization for comparison
     */
    void printDensityStats(double target_density = 0.7) const;

    /**
     * @brief Get total overflow area
     * @param target_density Target utilization
     * @return Total area exceeding target density
     */
    double getTotalOverflow(double target_density = 0.7) const;

    /**
     * @brief Get maximum density in the grid
     * @return Maximum density value
     */
    double getMaxDensity() const;

    /**
     * @brief Get average density
     * @return Average density across all bins
     */
    double getAverageDensity() const;

    // Accessors
    int getNumBinsX() const { return num_bins_x_; }
    int getNumBinsY() const { return num_bins_y_; }
    double getBinWidth() const { return bin_w_; }
    double getBinHeight() const { return bin_h_; }
    double getCoreXMin() const { return core_x_min_; }
    double getCoreYMin() const { return core_y_min_; }
    bool isInitialized() const { return initialized_; }
    size_t getTotalBins() const { return bins_.size(); }

    /**
     * @brief Get all bins (for visualization)
     * @return Const reference to bins vector
     */
    const std::vector<Bin>& getBins() const { return bins_; }

private:
    // Grid parameters
    int num_bins_x_ = 0;              ///< Number of bins in X direction
    int num_bins_y_ = 0;              ///< Number of bins in Y direction
    double bin_w_ = 0.0;              ///< Bin width
    double bin_h_ = 0.0;              ///< Bin height
    double core_x_min_ = 0.0;         ///< Core area X minimum
    double core_y_min_ = 0.0;         ///< Core area Y minimum
    double core_w_ = 0.0;             ///< Core area width
    double core_h_ = 0.0;             ///< Core area height
    bool initialized_ = false;        ///< Initialization flag

    // Data storage
    std::vector<Bin> bins_;           ///< 1D array storing 2D grid data: index = y * num_bins_x_ + x

    /**
     * @brief Calculate exact overlap area between cell and bin
     * @param cell Cell pointer (must have valid PlacerDB info)
     * @param bin_x Bin X index
     * @param bin_y Bin Y index
     * @param placer_db PlacerDB for accessing cell dimensions
     * @return Overlap area between cell and bin
     */
    double calculateCellBinOverlap(const Cell* cell, int bin_x, int bin_y, PlacerDB* placer_db) const;

    /**
     * @brief Get bin boundaries in world coordinates
     * @param bin_x Bin X index
     * @param bin_y Bin Y index
     * @param x_min Output: minimum X coordinate
     * @param x_max Output: maximum X coordinate
     * @param y_min Output: minimum Y coordinate
     * @param y_max Output: maximum Y coordinate
     */
    void getBinBounds(int bin_x, int bin_y, 
                     double& x_min, double& x_max,
                     double& y_min, double& y_max) const;

    /**
     * @brief Debug log helper
     * @param message Debug message
     */
    void debugLog(const std::string& message) const;

    // External reference to placer database for cell dimensions
    // Note: In a real implementation, this would be passed during updateDensity
    // For now, we'll assume cells have position/dimension info available
};

} // namespace mini

#endif // MINI_DENSITY_GRID_H