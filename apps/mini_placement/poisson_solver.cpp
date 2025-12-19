/**
 * @file poisson_solver.cpp
 * @brief Poisson Equation Solver Implementation using FFT
 * @details Hardcore C++ implementation with no external dependencies
 */

#include "poisson_solver.h"
#include <iomanip>
#include <algorithm>
#include <stdexcept>

namespace mini {

// ============================================================================
// Constructor and Public Interface
// ============================================================================

PoissonSolver::PoissonSolver() : bin_width_(0.0), bin_height_(0.0) {
    debugLog("PoissonSolver initialized - ready to solve electrostatic equations!");
}

void PoissonSolver::setBinSize(double bin_width, double bin_height) {
    bin_width_ = bin_width;
    bin_height_ = bin_height;
    debugLog("Bin dimensions set: " + std::to_string(bin_width_) + "x" + std::to_string(bin_height_));
}

bool PoissonSolver::solve(std::vector<Bin>& bins, int grid_width, int grid_height) {
    debugLog("Starting Poisson equation solver...");
    
    // Validate input
    if (!validateInput(bins, grid_width, grid_height)) {
        return false;
    }

    try {
        int total_bins = grid_width * grid_height;
        
        // Step 1: Extract density from bins and prepare complex data
        std::vector<Complex> spectrum(total_bins);
        for (int i = 0; i < total_bins; ++i) {
            spectrum[i] = Complex(bins[i].density, 0.0);
        }
        
        debugLog("Extracted density data from " + std::to_string(total_bins) + " bins");
        
        // Step 2: 2D FFT to frequency domain
        fft2D(spectrum, grid_width, grid_height, false); // Forward FFT
        debugLog("Completed forward 2D FFT transform");
        
        // Step 3: Apply Poisson filter in frequency domain
        applyPoissonFilter(spectrum, grid_width, grid_height);
        debugLog("Applied Poisson filter in frequency domain");
        
        // Step 4: 2D IFFT back to spatial domain
            fft2D(spectrum, grid_width, grid_height, true); // Inverse FFT
            debugLog("Completed inverse 2D FFT transform");
            
                    
        // Step 5: Calculate gradient forces
        calculateGradientForces(bins, spectrum, grid_width, grid_height);
        debugLog("Calculated electrostatic forces");
        
        // Update statistics
        updateStatistics(bins);
        
        solve_successful_ = true;
        debugLog("Poisson solver completed successfully!");
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error in PoissonSolver::solve: " << e.what() << std::endl;
        solve_successful_ = false;
        return false;
    }
}

// ============================================================================
// Static Utility Functions
// ============================================================================

bool PoissonSolver::isValidFFTSize(int width, int height) {
    // Check if both dimensions are powers of 2
    return (width > 0 && (width & (width - 1)) == 0) &&
           (height > 0 && (height & (height - 1)) == 0);
}

int PoissonSolver::nextPowerOfTwo(int size) {
    if (size <= 0) return 1;
    
    // If already power of 2, return as is
    if ((size & (size - 1)) == 0) return size;
    
    // Find next power of 2
    int power = 1;
    while (power < size) {
        power <<= 1;
    }
    return power;
}

void PoissonSolver::exportPotentialMap(const std::vector<Bin>& bins,
                                      int grid_width, int grid_height,
                                      const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        return;
    }

    file << "x,y,potential\n";
    
    for (int y = 0; y < grid_height; ++y) {
        for (int x = 0; x < grid_width; ++x) {
            int idx = y * grid_width + x;
            file << bins[idx].x << "," << bins[idx].y << "," 
                 << bins[idx].electro_potential << "\n";
        }
    }
    
    file.close();
    std::cout << "[PoissonSolver] Potential map exported to " << filename << std::endl;
}

void PoissonSolver::exportForceField(const std::vector<Bin>& bins,
                                    int grid_width, int grid_height,
                                    const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
        return;
    }

    file << "x,y,force_x,force_y,magnitude\n";
    
    for (int y = 0; y < grid_height; ++y) {
        for (int x = 0; x < grid_width; ++x) {
            int idx = y * grid_width + x;
            double fx = bins[idx].electro_force_x;
            double fy = bins[idx].electro_force_y;
            double magnitude = std::sqrt(fx*fx + fy*fy);
            
            file << bins[idx].x << "," << bins[idx].y << "," 
                 << fx << "," << fy << "," << magnitude << "\n";
        }
    }
    
    file.close();
    std::cout << "[PoissonSolver] Force field exported to " << filename << std::endl;
}

void PoissonSolver::printStatistics() const {
    std::cout << "\n=== Poisson Solver Statistics ===" << std::endl;
    std::cout << "Solution status: " << (solve_successful_ ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Maximum potential: " << std::fixed << std::setprecision(6) << max_potential_ << std::endl;
    std::cout << "Maximum force magnitude: " << std::fixed << std::setprecision(6) << max_force_magnitude_ << std::endl;
    std::cout << "Average force magnitude: " << std::fixed << std::setprecision(6) << avg_force_magnitude_ << std::endl;
    std::cout << "=================================" << std::endl;
}

// ============================================================================
// Core FFT Algorithms - Hardcore Implementation!
// ============================================================================

void PoissonSolver::fft1D(std::vector<Complex>& data, bool invert) {
    int n = data.size();
    
    // Base case
    if (n == 1) return;
    
    // Recursive Cooley-Tukey algorithm (Radix-2)
    std::vector<Complex> even(n / 2);
    std::vector<Complex> odd(n / 2);
    
    // Split into even and odd indices
    for (int i = 0; 2 * i < n; ++i) {
        even[i] = data[2 * i];
        odd[i] = data[2 * i + 1];
    }
    
    // Recursive FFT on halves
    fft1D(even, invert);
    fft1D(odd, invert);
    
    // Combine results
    double angle = TWO_PI / n * (invert ? -1 : 1);
    Complex w(1.0, 0.0);
    Complex wn(std::cos(angle), std::sin(angle));
    
    for (int i = 0; 2 * i < n; ++i) {
        data[i] = even[i] + w * odd[i];
        data[i + n / 2] = even[i] - w * odd[i];
        
        w *= wn;
    }
    
    // NOTE: For 2D FFT, we handle scaling at the end of fft2D, not here
    // This prevents double-scaling in the 2D case
}

void PoissonSolver::fft2D(std::vector<Complex>& data, int width, int height, bool invert) {
    // Step 1: Apply 1D FFT to each row
    for (int y = 0; y < height; ++y) {
        // Extract row
        std::vector<Complex> row = extractRow(data, y, width);
        
        // Apply 1D FFT to row
        fft1D(row, invert);
        
        // Insert processed row back
        insertRow(data, y, width, row);
    }
    
    // Step 2: Apply 1D FFT to each column
    for (int x = 0; x < width; ++x) {
        // Extract column
        std::vector<Complex> column = extractColumn(data, x, width, height);
        
        // Apply 1D FFT to column
        fft1D(column, invert);
        
        // Insert processed column back
        insertColumn(data, x, width, height, column);
    }
    
    // Step 3: For inverse FFT, apply proper 2D scaling
    if (invert) {
        double scale = 1.0 / (width * height);
        for (auto& val : data) {
            val *= scale;
        }
    }
}

std::vector<Complex> PoissonSolver::extractRow(const std::vector<Complex>& data,
                                              int row, int width) {
    std::vector<Complex> row_data(width);
    int start_idx = row * width;
    
    for (int x = 0; x < width; ++x) {
        row_data[x] = data[start_idx + x];
    }
    
    return row_data;
}

std::vector<Complex> PoissonSolver::extractColumn(const std::vector<Complex>& data,
                                                  int col, int width, int height) {
    std::vector<Complex> col_data(height);
    
    for (int y = 0; y < height; ++y) {
        col_data[y] = data[y * width + col];
    }
    
    return col_data;
}

void PoissonSolver::insertRow(std::vector<Complex>& data, int row, int width,
                             const std::vector<Complex>& row_data) {
    int start_idx = row * width;
    
    for (int x = 0; x < width; ++x) {
        data[start_idx + x] = row_data[x];
    }
}

void PoissonSolver::insertColumn(std::vector<Complex>& data, int col, int width, int height,
                                const std::vector<Complex>& col_data) {
    for (int y = 0; y < height; ++y) {
        data[y * width + col] = col_data[y];
    }
}

// ============================================================================
// Poisson Equation Solver Core
// ============================================================================

void PoissonSolver::applyPoissonFilter(std::vector<Complex>& spectrum,
                                       int width, int height) {
    debugLog("Applying Poisson filter in frequency domain...");
    
    
    
    for (int v = 0; v < height; ++v) {
        for (int u = 0; u < width; ++u) {
            int idx = v * width + u;
            
            // Handle DC component (u=0, v=0) - set to 0 (reference potential)
            if (u == 0 && v == 0) {
                spectrum[idx] = Complex(0.0, 0.0);
                continue;
            }
            
            // Calculate frequency indices (handle negative frequencies)
            int freq_u = getFrequencyIndex(u, width);
            int freq_v = getFrequencyIndex(v, height);
            
            // Apply Poisson filter: Φ̂ = ρ̂ / (u² + v²)
            // Note: freq_u, freq_v are in FFT index units, need to convert to actual frequencies
            double freq_u_double = static_cast<double>(freq_u);
            double freq_v_double = static_cast<double>(freq_v);
            double denominator = freq_u_double * freq_u_double + freq_v_double * freq_v_double;
            
            if (denominator > 0.0) {
                // Apply filter with proper scaling
                spectrum[idx] = spectrum[idx] / denominator;
                
                
            } else {
                // Avoid division by zero (shouldn't happen except DC)
                spectrum[idx] = Complex(0.0, 0.0);
            }
        }
    }
    
    debugLog("Poisson filter applied successfully");
}

int PoissonSolver::getFrequencyIndex(int index, int grid_size) const {
    // FFT frequency arrangement: [0, 1, 2, ..., N/2, -N/2+1, ..., -1]
    // But for Poisson equation, we need actual frequencies, not indices
    if (index <= grid_size / 2) {
        return index;  // Positive frequencies: 0, 1, 2, ..., N/2
    } else {
        return index - grid_size;  // Negative frequencies: -N/2+1, ..., -1
    }
}

void PoissonSolver::calculateGradientForces(std::vector<Bin>& bins,
                                           const std::vector<Complex>& potential,
                                           int width, int height) {
    debugLog("Calculating gradient forces...");
    
    
    
    // Reset statistics
    max_potential_ = 0.0;
    max_force_magnitude_ = 0.0;
    double total_force_magnitude = 0.0;
    int count = 0;
    
    // Use provided bin dimensions or calculate from bins if not set
    
        double bin_width = bin_width_;
    
        double bin_height = bin_height_;
    
        
    
        // If bin dimensions not set, calculate from bins (fallback)
    
        if (bin_width_ < 1e-9 && bins.size() >= 2 && width > 1) {
    
            // Calculate bin_width from adjacent bins in same row
    
            bin_width = bins[1].x - bins[0].x;
    
            if (bin_width < 1e-9) bin_width = 1.0;
    
        }
    
        
    
        if (bin_height_ < 1e-9 && bins.size() > width) {
    
            // Calculate bin_height from first bin of next row
    
            bin_height = bins[width].y - bins[0].y;
    
            if (bin_height < 1e-9) bin_height = 1.0;
    
        }
    
    double dx = 2.0 * bin_width;   // Central difference: 2 * bin_width
    double dy = 2.0 * bin_height;  // Central difference: 2 * bin_height
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int idx = y * width + x;
            
            // Store potential for visualization
            double phi = potential[idx].real();
            bins[idx].electro_potential = phi;
            max_potential_ = std::max(max_potential_, std::abs(phi));
            
            
            
            // Calculate X gradient: ∂Φ/∂x
            double grad_x;
            if (x == 0) {
                // Left boundary: forward difference
                int right_idx = y * width + (x + 1);
                grad_x = (potential[right_idx].real() - potential[idx].real()) / bin_width;
            } else if (x == width - 1) {
                // Right boundary: backward difference
                int left_idx = y * width + (x - 1);
                grad_x = (potential[idx].real() - potential[left_idx].real()) / bin_width;
            } else {
                // Interior: central difference
                int left_idx = y * width + (x - 1);
                int right_idx = y * width + (x + 1);
                grad_x = (potential[right_idx].real() - potential[left_idx].real()) / dx;
            }
            bins[idx].electro_force_x = -grad_x;  // F_x = -∂Φ/∂x
            
            // Calculate Y gradient: ∂Φ/∂y
            double grad_y;
            if (y == 0) {
                // Top boundary: forward difference
                int down_idx = (y + 1) * width + x;
                grad_y = (potential[down_idx].real() - potential[idx].real()) / bin_height;
            } else if (y == height - 1) {
                // Bottom boundary: backward difference
                int up_idx = (y - 1) * width + x;
                grad_y = (potential[idx].real() - potential[up_idx].real()) / bin_height;
            } else {
                // Interior: central difference
                int up_idx = (y - 1) * width + x;
                int down_idx = (y + 1) * width + x;
                grad_y = (potential[down_idx].real() - potential[up_idx].real()) / dy;
            }
            bins[idx].electro_force_y = -grad_y;  // F_y = -∂Φ/∂y
            
            // Update force statistics
            double force_mag = std::sqrt(grad_x * grad_x + grad_y * grad_y);
            
            
            max_force_magnitude_ = std::max(max_force_magnitude_, force_mag);
            total_force_magnitude += force_mag;
            count++;
        }
    }
    
    avg_force_magnitude_ = (count > 0) ? (total_force_magnitude / count) : 0.0;
    
    debugLog("Gradient forces calculated with proper scaling");
}

double PoissonSolver::centralDifference(const std::vector<Complex>& data,
                                       int index, int dimension_size) const {
    // For 2D data, we need to determine if we're calculating x or y gradient
    // This is a simplified version - in practice, we'd need grid context
    
    // For now, assume 1D central difference with periodic boundary
    int left = (index - 1 + dimension_size) % dimension_size;
    int right = (index + 1) % dimension_size;
    
    return (data[right].real() - data[left].real()) / 2.0;
}

// ============================================================================
// Helper Methods
// ============================================================================

void PoissonSolver::updateStatistics(const std::vector<Bin>& bins) {
    // Statistics are already calculated in calculateGradientForces
    debugLog("Statistics updated");
}

bool PoissonSolver::validateInput(const std::vector<Bin>& bins,
                                 int width, int height) const {
    if (bins.empty()) {
        std::cerr << "Error: Empty bins array" << std::endl;
        return false;
    }
    
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Invalid grid dimensions" << std::endl;
        return false;
    }
    
    if (!isValidFFTSize(width, height)) {
        std::cerr << "Error: Grid dimensions must be powers of 2 for FFT" << std::endl;
        std::cerr << "  Current: " << width << "x" << height << std::endl;
        std::cerr << "  Suggested: " << nextPowerOfTwo(width) << "x" 
                  << nextPowerOfTwo(height) << std::endl;
        return false;
    }
    
    int expected_size = width * height;
    if (static_cast<int>(bins.size()) != expected_size) {
        std::cerr << "Error: Bins array size mismatch" << std::endl;
        std::cerr << "  Expected: " << expected_size << std::endl;
        std::cerr << "  Actual: " << bins.size() << std::endl;
        return false;
    }
    
    return true;
}

void PoissonSolver::debugLog(const std::string& message) const {
    std::cout << "[PoissonSolver] " << message << std::endl;
}

} // namespace mini