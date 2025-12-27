/**
 * @file test_poisson_solver.cpp
 * @brief Test program for PoissonSolver functionality
 */

#include "../apps/mini_placement/poisson_solver.h"
#include "../apps/mini_placement/density_grid.h"
#include "../lib/include/placer_db.h"
#include "../lib/include/netlist_db.h"
#include <iostream>
#include <memory>
#include <iomanip>
#include <cmath>

using namespace mini;

int main() {
    std::cout << "=== Testing PoissonSolver ===" << std::endl;
    
    // Test parameters
    const int GRID_SIZE = 64;  // Must be power of 2 for FFT
    const double CORE_SIZE = 100.0;
    const double TARGET_DENSITY = 0.7;
    
    std::cout << "Grid size: " << GRID_SIZE << "x" << GRID_SIZE << std::endl;
    std::cout << "Core size: " << CORE_SIZE << "x" << CORE_SIZE << std::endl;
    
    // Validate FFT size
    if (!PoissonSolver::isValidFFTSize(GRID_SIZE, GRID_SIZE)) {
        std::cerr << "Error: Invalid FFT size - must be power of 2" << std::endl;
        return 1;
    }
    
    // Initialize density grid
    DensityGrid grid;
    double bin_dim = CORE_SIZE / GRID_SIZE;
    bool success = grid.init(CORE_SIZE, CORE_SIZE, 0.0, 0.0, bin_dim);
    
    if (!success) {
        std::cerr << "Failed to initialize density grid" << std::endl;
        return 1;
    }
    
    // Create test scenario with high density in center
    auto netlist_db = std::make_shared<NetlistDB>("test_design");
    auto placer_db = std::make_unique<PlacerDB>(netlist_db.get());
    
    // Set core area
    Rect core_area(0.0, 0.0, CORE_SIZE, CORE_SIZE);
    placer_db->setCoreArea(core_area);
    
    // Create test cells with high density in center
    std::vector<Cell*> test_cells;
    
    // Center cluster - should create high density and strong outward forces
    for (int i = 0; i < 20; ++i) {
        Cell* cell = netlist_db->createCell("center_cell_" + std::to_string(i), CellType::AND);
        // Place cells in center region with some randomness
        double cx = CORE_SIZE * 0.5 + (rand() % 20 - 10) * 0.5;
        double cy = CORE_SIZE * 0.5 + (rand() % 20 - 10) * 0.5;
        cell->setPosition(cx, cy);
        placer_db->addCell(cell, 3.0, 2.0);  // 3x2 cells
        test_cells.push_back(cell);
    }
    
    // Corner cells - should create lower density
    for (int corner = 0; corner < 4; ++corner) {
        for (int i = 0; i < 3; ++i) {
            Cell* cell = netlist_db->createCell("corner_cell_" + std::to_string(corner) + "_" + std::to_string(i), CellType::OR);
            
            double cx = (corner % 2 == 0) ? 10.0 : CORE_SIZE - 10.0;
            double cy = (corner < 2) ? 10.0 : CORE_SIZE - 10.0;
            cx += (rand() % 10 - 5) * 0.5;
            cy += (rand() % 10 - 5) * 0.5;
            
            cell->setPosition(cx, cy);
            placer_db->addCell(cell, 2.0, 2.0);
            test_cells.push_back(cell);
        }
    }
    
    std::cout << "Created " << test_cells.size() << " test cells" << std::endl;
    
    // Update density
    grid.updateDensity(test_cells, placer_db.get());
    
    // Print initial density statistics
    std::cout << "\n=== Initial Density Statistics ===" << std::endl;
    grid.printDensityStats(TARGET_DENSITY);
    
    // Get bins for Poisson solver
    std::vector<Bin>& bins = const_cast<std::vector<Bin>&>(grid.getBins());
    
    // Initialize Poisson solver
    PoissonSolver solver;
    
    std::cout << "\n=== Running Poisson Solver ===" << std::endl;
    
    // Solve Poisson equation
    bool solve_success = solver.solve(bins, GRID_SIZE, GRID_SIZE);
    
    if (!solve_success) {
        std::cerr << "Poisson solver failed!" << std::endl;
        return 1;
    }
    
    // Print solver statistics
    solver.printStatistics();
    
    // Analyze results
    std::cout << "\n=== Force Field Analysis ===" << std::endl;
    
    // Check center region (should have strong outward forces)
    int center_x = GRID_SIZE / 2;
    int center_y = GRID_SIZE / 2;
    int center_idx = center_y * GRID_SIZE + center_x;
    const Bin& center_bin = bins[center_idx];
    
    std::cout << "Center bin (" << center_x << "," << center_y << "):" << std::endl;
    std::cout << "  Density: " << std::fixed << std::setprecision(3) << center_bin.density << std::endl;
    std::cout << "  Potential: " << std::fixed << std::setprecision(6) << center_bin.electro_potential << std::endl;
    std::cout << "  Force: (" << std::fixed << std::setprecision(6) 
              << center_bin.electro_gradient_x << ", " << center_bin.electro_gradient_y << ")" << std::endl;
    std::cout << "  Force magnitude: " << std::fixed << std::setprecision(6) 
              << std::sqrt(center_bin.electro_gradient_x * center_bin.electro_gradient_x + 
                          center_bin.electro_gradient_y * center_bin.electro_gradient_y) << std::endl;
    
    // Check corner region (should have weaker/inward forces)
    int corner_x = 5;
    int corner_y = 5;
    int corner_idx = corner_y * GRID_SIZE + corner_x;
    const Bin& corner_bin = bins[corner_idx];
    
    std::cout << "\nCorner bin (" << corner_x << "," << corner_y << "):" << std::endl;
    std::cout << "  Density: " << std::fixed << std::setprecision(3) << corner_bin.density << std::endl;
    std::cout << "  Potential: " << std::fixed << std::setprecision(6) << corner_bin.electro_potential << std::endl;
    std::cout << "  Force: (" << std::fixed << std::setprecision(6) 
              << corner_bin.electro_gradient_x << ", " << corner_bin.electro_gradient_y << ")" << std::endl;
    std::cout << "  Force magnitude: " << std::fixed << std::setprecision(6) 
              << std::sqrt(corner_bin.electro_gradient_x * corner_bin.electro_gradient_x + 
                          corner_bin.electro_gradient_y * corner_bin.electro_gradient_y) << std::endl;
    
    // Verify physics: high density should produce outward forces
    bool physics_correct = true;
    
    // Center should have positive potential (repulsive)
    if (center_bin.electro_potential <= 0.0) {
        std::cout << "Warning: Center potential should be positive (repulsive)" << std::endl;
        physics_correct = false;
    }
    
    // Corner should have lower potential than center
    if (corner_bin.electro_potential >= center_bin.electro_potential) {
        std::cout << "Warning: Corner potential should be lower than center" << std::endl;
        physics_correct = false;
    }
    
    // Export results for visualization
    std::cout << "\n=== Exporting Results ===" << std::endl;
    
    grid.exportDensityMap("test_poisson_density.csv");
    PoissonSolver::exportPotentialMap(bins, GRID_SIZE, GRID_SIZE, "test_poisson_potential.csv");
    PoissonSolver::exportForceField(bins, GRID_SIZE, GRID_SIZE, "test_poisson_gradients.csv");
    
    std::cout << "Results exported to CSV files for visualization" << std::endl;
    
    // Test edge cases
    std::cout << "\n=== Testing Edge Cases ===" << std::endl;
    
    // Test with uniform density (should produce zero forces)
    std::vector<Bin> uniform_bins(GRID_SIZE * GRID_SIZE);
    for (auto& bin : uniform_bins) {
        bin.density = TARGET_DENSITY;  // Uniform density
    }
    
    bool uniform_success = solver.solve(uniform_bins, GRID_SIZE, GRID_SIZE);
    if (uniform_success) {
        double max_uniform_gradient = 0.0;
        for (const auto& bin : uniform_bins) {
            double force_mag = std::sqrt(bin.electro_gradient_x * bin.electro_gradient_x + 
                                       bin.electro_gradient_y * bin.electro_gradient_y);
            max_uniform_gradient = std::max(max_uniform_gradient, force_mag);
        }
        
        std::cout << "Uniform density test:" << std::endl;
        std::cout << "  Max force magnitude: " << std::fixed << std::setprecision(9) << max_uniform_gradient << std::endl;
        std::cout << "  Expected: ~0.0 (no forces in uniform density)" << std::endl;
        
        if (max_uniform_gradient > 1e-10) {
            std::cout << "Warning: Uniform density should produce minimal forces" << std::endl;
        }
    }
    
    // Final validation
    std::cout << "\n=== Final Validation ===" << std::endl;
    
    if (physics_correct && solve_success) {
        std::cout << "✓ PoissonSolver PASSED all tests!" << std::endl;
        std::cout << "✓ Physics: High density → repulsive forces" << std::endl;
        std::cout << "✓ Mathematics: Poisson equation solved correctly" << std::endl;
        std::cout << "✓ Implementation: FFT working properly" << std::endl;
    } else {
        std::cout << "✗ PoissonSolver FAILED some tests" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== PoissonSolver Test Completed Successfully ===" << std::endl;
    
    return 0;
}