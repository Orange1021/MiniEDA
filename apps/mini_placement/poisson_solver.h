/**
 * @file poisson_solver.h
 * @brief Poisson Equation Solver using Spectral Method (FFT)
 * @details Implements ePlace-style electrostatic placement using FFT-based Poisson solver
 *          Calculates electrostatic potential and forces from density distribution
 */

#ifndef MINI_POISSON_SOLVER_H
#define MINI_POISSON_SOLVER_H

#include <vector>
#include <complex>
#include <cmath>
#include <memory>
#include <string>
#include <fstream>
#include <iostream>
#include "density_grid.h"

namespace mini {

// Mathematical constants
const double PI = 3.14159265358979323846;
const double TWO_PI = 2.0 * PI;

// Type aliases for clarity
using Complex = std::complex<double>;

/**
 * @class PoissonSolver
 * @brief FFT-based Poisson equation solver for electrostatic placement
 * @details Uses spectral method to solve ∇²Φ = -ρ and calculate forces F = -∇Φ
 *          Complexity: O(N log N) where N is number of grid points
 */
class PoissonSolver {
public:
    /**
     * @brief Constructor
     */
    PoissonSolver();

    /**
     * @brief Destructor
     */
    ~PoissonSolver() = default;

    // Disable copy, enable move
    PoissonSolver(const PoissonSolver&) = delete;
    PoissonSolver& operator=(const PoissonSolver&) = delete;
    PoissonSolver(PoissonSolver&&) = default;
    PoissonSolver& operator=(PoissonSolver&&) = default;

    /**
     * @brief Set bin dimensions for accurate force calculation
     * @param bin_width Physical width of each bin
     * @param bin_height Physical height of each bin
     */
    void setBinSize(double bin_width, double bin_height);
    
    /**
     * @brief Core function: Solve Poisson equation and calculate forces
     * @param bins Density grid bins (input: density, output: potential + forces)
     * @param grid_width Grid width (must be power of 2)
     * @param grid_height Grid height (must be power of 2)
     * @return true if solving successful
     * @details Process:
     * 1. Extract density from bins
     * 2. 2D FFT to frequency domain
     * 3. Apply Poisson filter: Φ̂ = ρ̂ / (u² + v²)
     * 4. 2D IFFT back to spatial domain
     * 5. Calculate gradient forces: F = -∇Φ
     */
    bool solve(std::vector<Bin>& bins, int grid_width, int grid_height);

    /**
     * @brief Check if grid dimensions are suitable for FFT (power of 2)
     * @param width Grid width
     * @param height Grid height
     * @return true if both dimensions are powers of 2
     */
    static bool isValidFFTSize(int width, int height);

    /**
     * @brief Find next suitable FFT size (power of 2)
     * @param size Requested size
     * @return Next power of 2 size >= requested
     */
    static int nextPowerOfTwo(int size);

    /**
     * @brief Export electrostatic potential map to CSV
     * @param bins Bin array with calculated potentials
     * @param grid_width Grid width
     * @param grid_height Grid height
     * @param filename Output filename
     */
    static void exportPotentialMap(const std::vector<Bin>& bins, 
                                  int grid_width, int grid_height,
                                  const std::string& filename);

    /**
     * @brief Export force field to CSV for visualization
     * @param bins Bin array with calculated forces
     * @param grid_width Grid width
     * @param grid_height Grid height
     * @param filename Output filename
     */
    static void exportGradientField(const std::vector<Bin>& bins,
                                 int grid_width, int grid_height,
                                 const std::string& filename);

    /**
     * @brief Print solver statistics
     */
    void printStatistics() const;

    // Accessors
    double getMaxPotential() const { return max_potential_; }
    double getMaxForceMagnitude() const { return max_gradient_magnitude_; }
    double getAverageForceMagnitude() const { return avg_gradient_magnitude_; }

private:
    // Statistics
    double max_potential_ = 0.0;
    double max_gradient_magnitude_ = 0.0;
    double avg_gradient_magnitude_ = 0.0;
    bool solve_successful_ = false;
    
    // Physical dimensions
    double bin_width_ = 0.0;
    double bin_height_ = 0.0;

    // ========================================================================
    // Core FFT Algorithms (Hardcore Mode - No External Dependencies!)
    // ========================================================================

    /**
     * @brief 1D Cooley-Tukey FFT (Radix-2, recursive)
     * @param data Complex vector (must be power of 2 size)
     * @param invert true for inverse FFT, false for forward FFT
     * @details Classic O(N log N) FFT implementation
     */
    void fft1D(std::vector<Complex>& data, bool invert);

    /**
     * @brief 2D FFT by applying 1D FFT on rows then columns
     * @param data 2D data stored as 1D vector (size = width * height)
     * @param width Grid width (must be power of 2)
     * @param height Grid height (must be power of 2)
     * @param invert true for inverse FFT, false for forward FFT
     */
    void fft2D(std::vector<Complex>& data, int width, int height, bool invert);

    /**
     * @brief Extract row from 2D data for 1D FFT processing
     * @param data 2D data vector
     * @param row Row index
     * @param width Grid width
     * @return Extracted row as complex vector
     */
    std::vector<Complex> extractRow(const std::vector<Complex>& data, 
                                   int row, int width);

    /**
     * @brief Extract column from 2D data for 1D FFT processing
     * @param data 2D data vector
     * @param col Column index
     * @param width Grid width
     * @param height Grid height
     * @return Extracted column as complex vector
     */
    std::vector<Complex> extractColumn(const std::vector<Complex>& data,
                                      int col, int width, int height);

    /**
     * @brief Insert processed row back into 2D data
     * @param data 2D data vector (modified)
     * @param row Row index
     * @param width Grid width
     * @param row_data Processed row data
     */
    void insertRow(std::vector<Complex>& data, int row, int width,
                   const std::vector<Complex>& row_data);

    /**
     * @brief Insert processed column back into 2D data
     * @param data 2D data vector (modified)
     * @param col Column index
     * @param width Grid width
     * @param height Grid height
     * @param col_data Processed column data
     */
    void insertColumn(std::vector<Complex>& data, int col, int width, int height,
                      const std::vector<Complex>& col_data);

    // ========================================================================
    // Poisson Equation Solver Core
    // ========================================================================

    /**
     * @brief Apply Poisson filter in frequency domain
     * @param spectrum Frequency domain data (modified in place)
     * @param width Grid width
     * @param height Grid height
     * @details Implements Φ̂ = ρ̂ / (u² + v²) where u,v are frequency indices
     *          Handles DC component (u=0, v=0) by setting to 0
     */
    void applyPoissonFilter(std::vector<Complex>& spectrum, 
                           int width, int height);

    /**
     * @brief Calculate frequency indices for given grid position
     * @param index Grid index (0 to width-1)
     * @param grid_size Grid dimension
     * @return Frequency index (handles negative frequencies)
     * @details FFT output: [0, 1, 2, ..., N/2, -N/2+1, ..., -1]
     */
    int getFrequencyIndex(int index, int grid_size) const;

    /**
     * @brief Calculate gradient forces using central differences
     * @param bins Bin array (output: electro_gradient_x, electro_gradient_y)
     * @param potential Potential values from IFFT
     * @param width Grid width
     * @param height Grid height
     * @details F_x = -∂Φ/∂x, F_y = -∂Φ/∂y using central difference
     */
    void calculateGradientForces(std::vector<Bin>& bins,
                                const std::vector<Complex>& potential,
                                int width, int height);

    /**
     * @brief Calculate gradient using central difference (1D)
     * @param data 1D data array
     * @param index Current index
     * @param size Array size
     * @return Gradient value (central difference with periodic boundary)
     */
    double centralDifference(const std::vector<Complex>& data, 
                           int index, int size) const;

    /**
     * @brief Update solver statistics after solving
     * @param bins Bin array with calculated potentials and forces
     */
    void updateStatistics(const std::vector<Bin>& bins);

    /**
     * @brief Validate grid dimensions and data
     * @param bins Bin array
     * @param width Grid width
     * @param height Grid height
     * @return true if valid
     */
    bool validateInput(const std::vector<Bin>& bins, 
                      int width, int height) const;
};

} // namespace mini

#endif // MINI_POISSON_SOLVER_H