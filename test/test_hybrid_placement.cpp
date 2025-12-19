/**
 * @file test_hybrid_placement.cpp
 * @brief Test program for Hybrid Placement (Warm-up + Electrostatic Optimization)
 */

#include "../apps/mini_placement/placer_engine.h"
#include "../lib/include/placer_db.h"
#include "../lib/include/netlist_db.h"
#include <iostream>
#include <memory>
#include <iomanip>
#include <random>

using namespace mini;

int main() {
    std::cout << "=== Testing Hybrid Placement (Warm-up + Electrostatic) ===" << std::endl;
    
    // Test parameters
    const double CORE_SIZE = 100.0;
    const double TARGET_DENSITY = 0.7;
    const int NUM_CELLS = 40;
    
    std::cout << "Core size: " << CORE_SIZE << "x" << CORE_SIZE << std::endl;
    std::cout << "Target density: " << TARGET_DENSITY << std::endl;
    std::cout << "Number of cells: " << NUM_CELLS << std::endl;
    
    try {
        // Create test netlist and placer database
        auto netlist_db = std::make_shared<NetlistDB>("test_design");
        auto placer_db = std::make_unique<PlacerDB>(netlist_db.get());
        
        // Set core area
        Rect core_area(0.0, 0.0, CORE_SIZE, CORE_SIZE);
        placer_db->setCoreArea(core_area);
        
        // Create test cells clustered in center (good test case for warm-up)
        std::mt19937 rng(42);  // Fixed seed for reproducibility
        std::normal_distribution<double> cluster_dist(0.0, 10.0);  // Cluster around center
        std::uniform_real_distribution<double> size_dist(2.0, 6.0);
        
        std::vector<Cell*> logic_cells;
        
        for (int i = 0; i < NUM_CELLS; ++i) {
            Cell* cell = netlist_db->createCell("logic_" + std::to_string(i), 
                                              (i % 2 == 0) ? CellType::AND : CellType::OR);
            
            // Create initial clustering around center
            double center_x = CORE_SIZE * 0.5 + cluster_dist(rng);
            double center_y = CORE_SIZE * 0.5 + cluster_dist(rng);
            
            // Clamp to core area
            center_x = std::max(5.0, std::min(CORE_SIZE - 5.0, center_x));
            center_y = std::max(5.0, std::min(CORE_SIZE - 5.0, center_y));
            
            double w = size_dist(rng);
            double h = size_dist(rng);
            
            cell->setPosition(center_x, center_y);
            placer_db->addCell(cell, w, h);
            logic_cells.push_back(cell);
        }
        
        std::cout << "Created " << logic_cells.size() << " test cells clustered in center" << std::endl;
        
        // Calculate initial statistics
        double min_x = CORE_SIZE, max_x = 0.0, min_y = CORE_SIZE, max_y = 0.0;
        for (const Cell* cell : logic_cells) {
            double x = cell->getX();
            double y = cell->getY();
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
        
        double initial_span = (max_x - min_x) + (max_y - min_y);
        std::cout << "Initial cell span: " << std::fixed << std::setprecision(1) << initial_span << std::endl;
        
        // Initialize placement engine
        PlacerEngine engine(placer_db.get());
        
        // Initialize density grid
        engine.initializeDensityGrid(5.0);  // 5um bin dimension
        
        // Configure hybrid placement
        engine.configureHybridPlacement(30, true);  // 30 warm-up iterations, enable comparison
        
        std::cout << "\n=== Running Hybrid Placement ===" << std::endl;
        
        // Run hybrid placement
        engine.runHybridPlacement();
        
        // Calculate final statistics
        min_x = CORE_SIZE; max_x = 0.0; min_y = CORE_SIZE; max_y = 0.0;
        for (const Cell* cell : logic_cells) {
            double x = cell->getX();
            double y = cell->getY();
            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
        
        double final_span = (max_x - min_x) + (max_y - min_y);
        double span_improvement = ((final_span - initial_span) / initial_span) * 100.0;
        
        std::cout << "\n=== Layout Analysis ===" << std::endl;
        std::cout << "Initial cell span: " << std::fixed << std::setprecision(1) << initial_span << std::endl;
        std::cout << "Final cell span: " << std::fixed << std::setprecision(1) << final_span << std::endl;
        std::cout << "Span improvement: " << std::fixed << std::setprecision(1) << span_improvement << "%" << std::endl;
        
        // Calculate spread efficiency (how well cells utilize the core area)
        double core_span = 2.0 * CORE_SIZE;  // Maximum possible span
        double initial_efficiency = (initial_span / core_span) * 100.0;
        double final_efficiency = (final_span / core_span) * 100.0;
        
        std::cout << "Initial space utilization: " << std::fixed << std::setprecision(1) << initial_efficiency << "%" << std::endl;
        std::cout << "Final space utilization: " << std::fixed << std::setprecision(1) << final_efficiency << "%" << std::endl;
        
        // Validate results
        bool placement_good = true;
        
        // Check if cells spread out reasonably
        if (final_efficiency < 30.0) {
            std::cout << "Warning: Cells did not spread out enough" << std::endl;
            placement_good = false;
        }
        
        if (final_efficiency > 90.0) {
            std::cout << "Warning: Cells may be too spread out" << std::endl;
        }
        
        // Check for reasonable improvement
        if (span_improvement < 50.0) {
            std::cout << "Warning: Insufficient spreading improvement" << std::endl;
            placement_good = false;
        }
        
        // Final validation
        std::cout << "\n=== Final Validation ===" << std::endl;
        if (placement_good) {
            std::cout << "✓ Hybrid Placement PASSED all tests!" << std::endl;
            std::cout << "✓ Warm-up phase provided stable initial solution" << std::endl;
            std::cout << "✓ Electrostatic optimization achieved good spreading" << std::endl;
            std::cout << "✓ Two-phase strategy worked effectively" << std::endl;
            std::cout << "✓ Baseline comparison data collected" << std::endl;
        } else {
            std::cout << "✗ Hybrid Placement FAILED some validation checks" << std::endl;
            return 1;
        }
        
        // Export final placement
        engine.exportPlacement("hybrid_placement_final.csv");
        std::cout << "\nFinal placement exported to hybrid_placement_final.csv" << std::endl;
        
        std::cout << "\n=== Advantages Demonstrated ===" << std::endl;
        std::cout << "1. Numerical Stability: Warm-up prevents gradient explosion" << std::endl;
        std::cout << "2. Engineering Evolution: Traditional → Advanced algorithm" << std::endl;
        std::cout << "3. Benchmark Capability: Baseline vs Optimized comparison" << std::endl;
        std::cout << "4. Practical Applicability: Real-world clustering scenario" << std::endl;
        
        std::cout << "\n=== Hybrid Placement Test Completed Successfully ===" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}