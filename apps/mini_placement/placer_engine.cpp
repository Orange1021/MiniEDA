/**
 * @file placer_engine.cpp
 * @brief Placement Engine Implementation for MiniPlacement
 */

#include "placer_engine.h"
#include "global_placer.h"
#include "abacus_legalizer.h"
#include "greedy_legalizer.h"
#include "legalizer.h"
#include "../../lib/include/hpwl_calculator.h"
#include "../../lib/include/visualizer.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <limits>
#include <map>
#include <chrono>
#include <iomanip>

namespace mini {

PlacerEngine::PlacerEngine(PlacerDB* db)
    : db_(db), viz_(nullptr), current_hpwl_(0.0), global_placer_(nullptr), leg_algo_(LegalizationAlgorithm::ABACUS) {
}
PlacerEngine::~PlacerEngine() {
    delete global_placer_;
}

double PlacerEngine::calculateHPWL() const {
    if (!db_) return 0.0;

    NetlistDB* netlist_db = db_->getNetlistDB();
    if (!netlist_db) return 0.0;
    
    return HPWLCalculator::calculateHPWL(netlist_db, db_);
}

void PlacerEngine::runGlobalPlacementWithAlgorithm(const std::string& algorithm) {
    if (!db_) return;
    
    std::cout << "\n=== Starting Global Placement ===" << std::endl;
    std::cout << "Algorithm: " << algorithm << std::endl;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    if (algorithm == "basic") {
        std::cout << "Mode: Basic Force-Directed (Baseline)" << std::endl;
        runBasicStrategy();
    } else if (algorithm == "nesterov") {
        std::cout << "Mode: Electrostatic Field (Nesterov)" << std::endl;
        runElectrostaticStrategy();
    } else if (algorithm == "hybrid") {
        std::cout << "Mode: Hybrid Cascade (Warmup + Nesterov)" << std::endl;
        runHybridStrategy();
    } else {
        std::cerr << "Warning: Unknown algorithm '" << algorithm << "', falling back to basic" << std::endl;
        runBasicStrategy();
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    std::cout << "Global Placement completed in " << duration.count() << " ms" << std::endl;
    std::cout << "Final HPWL: " << current_hpwl_ << std::endl;
}

void PlacerEngine::runGlobalPlacement() {
    if (!db_) return;
    
    std::cout << "Starting Global Placement (Quadratic Wirelength Optimization)..." << std::endl;
    
    // Initialize with current HPWL
    current_hpwl_ = calculateHPWL();
    std::cout << "Initial HPWL: " << current_hpwl_ << std::endl;
    
    // Force-directed placement iterations
    const int max_iterations = 50;  // Quadratic convergence is fast
    
    for (int iter = 0; iter < max_iterations; ++iter) {
        // Single iteration of force-directed algorithm
        solveForceDirectedIteration(iter);
        
        // Calculate new HPWL
        double new_hpwl = calculateHPWL();
        
        // Print progress
        if (iter % 5 == 0) {
            std::cout << "Iteration " << iter << ": HPWL = " << new_hpwl << std::endl;
            
            // Visualize every 5 iterations if visualizer is available
            if (viz_) {
                std::string filename = "quadratic_iter_" + std::to_string(iter);
                viz_->drawPlacementWithRunId(filename, run_id_);
            }
        }
        
        // Check convergence
        double improvement = current_hpwl_ - new_hpwl;
        if (std::abs(improvement) < current_hpwl_ * 0.0001) {  // 0.01% improvement threshold
            std::cout << "Converged at iteration " << iter << std::endl;
            break;
        }
        
        current_hpwl_ = new_hpwl;
    }
    
    std::cout << "Global Placement completed. Final HPWL: " << current_hpwl_ << std::endl;
    std::cout << "Improvement: " << (calculateHPWL() - current_hpwl_) << std::endl;
}

void PlacerEngine::runLegalization() {
    if (!db_) return;
    
    std::cout << ">>> Starting Legalization..." << std::endl;
    
    std::unique_ptr<Legalizer> legalizer;
    
    switch (leg_algo_) {
        case LegalizationAlgorithm::GREEDY_TETRIS:
            std::cout << "Algorithm: Greedy (Tetris)" << std::endl;
            legalizer = std::make_unique<GreedyLegalizer>(db_);
            break;
            
        case LegalizationAlgorithm::ABACUS:
            std::cout << "Algorithm: Abacus (Optimization-based)" << std::endl;
            legalizer = std::make_unique<AbacusLegalizer>(db_);
            break;
    }
    
    if (legalizer) {
        legalizer->setVerbose(true);  // Enable verbose for debugging
        legalizer->run();
    }
    
    // Calculate and report final HPWL
    double final_hpwl = calculateHPWL();
    std::cout << "  Final HPWL: " << final_hpwl << std::endl;
    
    // Update cached HPWL to maintain consistency
    current_hpwl_ = final_hpwl;
    
    // Verify no overlaps
    bool has_overlaps = hasOverlaps();
    std::cout << "  Overlap check: " << (has_overlaps ? "FOUND OVERLAPS!" : "No overlaps") << std::endl;
    
    // Visualize the result
    if (viz_) {
        viz_->drawPlacementWithRunId("legalized", run_id_);
    }
    
    std::cout << "Legalization completed!" << std::endl;
}

void PlacerEngine::runDetailedPlacement() {
    if (!db_) return;
    
    std::cout << "Starting Detailed Placement Optimization..." << std::endl;
    
    // Initialize detailed placer with verbose mode
    detailed_placer_ = std::make_unique<DetailedPlacer>(db_, db_->getNetlistDB());
    detailed_placer_->setVerbose(true);
    detailed_placer_->setIterations(3);
    
    // Run detailed placement optimization
    detailed_placer_->run();
    
    // Update current HPWL
    current_hpwl_ = detailed_placer_->getFinalHPWL();
    
    std::cout << "  Final HPWL after detailed placement: " << current_hpwl_ << std::endl;
    
    // Check for overlaps after detailed placement
    bool has_overlaps = hasOverlaps();
    std::cout << "  Overlap check after detailed placement: " << (has_overlaps ? "FOUND OVERLAPS!" : "No overlaps") << std::endl;
    
    // Visualize final result
    if (viz_) {
        viz_->drawPlacementWithRunId("detailed", run_id_);
    }
}

void PlacerEngine::solveForceDirectedIteration(int iter) {
    if (!db_) return;
    
    NetlistDB* netlist_db = db_->getNetlistDB();
    if (!netlist_db) return;
    
    // Get all movable cells
    auto cells = db_->getAllCells();
    std::unordered_map<Cell*, Point> next_positions;
    
    // Calculate target position for each movable cell
    for (Cell* cell : cells) {
        const auto& info = db_->getCellInfo(cell);
        if (info.fixed) continue;  // Skip fixed cells
        
        double sum_x = 0.0, sum_y = 0.0;
        double total_weight = 0.0;
        
        // Iterate through all pins of this cell
        for (const auto& pin_ptr : cell->getPins()) {
            Pin* pin = pin_ptr.get();
            if (!pin) continue;
            
            Net* net = pin->getNet();
            if (!net) continue;
            
            // Calculate weight for this net (1 / (pin_count - 1))
            size_t pin_count = net->getLoads().size() + (net->getDriver() ? 1 : 0);
            if (pin_count <= 1) continue;  // Skip single-pin nets
            
            double weight = 1.0 / (pin_count - 1);
            
            // Iterate through all other pins on this net
            // Check driver
            Pin* driver = net->getDriver();
            if (driver && driver != pin) {
                Cell* other_cell = driver->getOwner();
                if (other_cell) {
                    Point center = db_->getCellCenter(other_cell);
                    sum_x += weight * center.x;
                    sum_y += weight * center.y;
                    total_weight += weight;
                }
            }
            
            // Check loads
            for (Pin* load : net->getLoads()) {
                if (load && load != pin) {
                    Cell* other_cell = load->getOwner();
                    if (other_cell) {
                        Point center = db_->getCellCenter(other_cell);
                        sum_x += weight * center.x;
                        sum_y += weight * center.y;
                        total_weight += weight;
                    }
                }
            }
        }
        
        // Calculate new position as weighted average
        if (total_weight > 0.0) {
            double new_x = sum_x / total_weight;
            double new_y = sum_y / total_weight;
            next_positions[cell] = Point(new_x, new_y);
        }
    }
    
    // Apply new positions
    for (const auto& [cell, new_pos] : next_positions) {
        const auto& info = db_->getCellInfo(cell);
        double new_x = new_pos.x - info.width / 2.0;  // Convert center to lower-left
        double new_y = new_pos.y - info.height / 2.0;
        db_->placeCell(cell, new_x, new_y);
    }
    
    // Print progress
    if (iter % 10 == 0) {
        std::cout << "  Force-directed iteration " << iter 
                  << " (moved " << next_positions.size() << " cells)" << std::endl;
    }
}

void PlacerEngine::calculateNetBoundingBox(const Net* net, 
                                          double& min_x, double& max_x,
                                          double& min_y, double& max_y) const {
    // Initialize to opposite extremes
    min_x = std::numeric_limits<double>::max();
    max_x = std::numeric_limits<double>::lowest();
    min_y = std::numeric_limits<double>::max();
    max_y = std::numeric_limits<double>::lowest();
    
    // Iterate through all pins connected to this net
    // Check driver pin
    Pin* driver = net->getDriver();
    if (driver) {
        Cell* cell = driver->getOwner();
        if (cell) {
            Point center = db_->getCellCenter(cell);
            min_x = max_x = center.x;
            min_y = max_y = center.y;
        }
    }
    
    // Check all load pins
    for (Pin* load : net->getLoads()) {
        if (load) {
            Cell* cell = load->getOwner();
            if (cell) {
                Point center = db_->getCellCenter(cell);
                min_x = std::min(min_x, center.x);
                max_x = std::max(max_x, center.x);
                min_y = std::min(min_y, center.y);
                max_y = std::max(max_y, center.y);
            }
        }
    }
}

bool PlacerEngine::hasOverlaps() const {
    if (!db_) return false;
    
    // Check all pairs of cells for overlaps
    auto cells = db_->getAllCells();
    int overlap_count = 0;
    
    for (size_t i = 0; i < cells.size(); ++i) {
        for (size_t j = i + 1; j < cells.size(); ++j) {
            const auto& info1 = db_->getCellInfo(cells[i]);
            const auto& info2 = db_->getCellInfo(cells[j]);
            
            // Check if rectangles overlap
            double right1 = info1.x + info1.width;
            double right2 = info2.x + info2.width;
            double top1 = info1.y + info1.height;
            double top2 = info2.y + info2.height;
            
            // Add small epsilon to handle floating-point precision issues
            // True overlap means actual intersection, not just edge touching
            const double EPSILON = 1e-9;
            
            bool x_overlap = !(right1 <= info2.x + EPSILON || right2 <= info1.x + EPSILON);
            bool y_overlap = !(top1 <= info2.y + EPSILON || top2 <= info1.y + EPSILON);
            
            if (x_overlap && y_overlap) {
                overlap_count++;
                if (overlap_count <= 3) {  // Only print first 3 overlaps to avoid spam
                    std::cout << "    OVERLAP DETECTED: " << cells[i]->getName() 
                              << " and " << cells[j]->getName() << std::endl;
                    std::cout << "      " << cells[i]->getName() << ": (" << info1.x 
                              << "," << info1.y << ") size=" << info1.width << "x" << info1.height 
                              << " -> x:[" << info1.x << "," << right1 << "] y:[" << info1.y << "," << top1 << "]" << std::endl;
                    std::cout << "      " << cells[j]->getName() << ": (" << info2.x 
                              << "," << info2.y << ") size=" << info2.width << "x" << info2.height 
                              << " -> x:[" << info2.x << "," << right2 << "] y:[" << info2.y << "," << top2 << "]" << std::endl;
                    std::cout << "      x_overlap=" << x_overlap << " (right1=" << right1 << " <= x2=" << info2.x << "=" << (right1 <= info2.x) 
                              << " OR right2=" << right2 << " <= x1=" << info1.x << "=" << (right2 <= info1.x) << ")" << std::endl;
                    std::cout << "      y_overlap=" << y_overlap << " (top1=" << top1 << " <= y2=" << info2.y << "=" << (top1 <= info2.y) 
                              << " OR top2=" << top2 << " <= y1=" << info1.y << "=" << (top2 <= info1.y) << ")" << std::endl;
                }
            }
        }
    }
    
    if (overlap_count > 0) {
        std::cout << "    Total overlaps found: " << overlap_count << std::endl;
    }
    
    return overlap_count > 0;
}

double PlacerEngine::calculateTotalOverlap() const {
    if (!db_) return 0.0;
    
    double total_overlap = 0.0;
    auto cells = db_->getAllCells();
    
    for (size_t i = 0; i < cells.size(); ++i) {
        for (size_t j = i + 1; j < cells.size(); ++j) {
            const auto& info1 = db_->getCellInfo(cells[i]);
            const auto& info2 = db_->getCellInfo(cells[j]);
            
            // Calculate overlap area
            double x_overlap = std::min(info1.x + info1.width, info2.x + info2.width) - 
                              std::max(info1.x, info2.x);
            double y_overlap = std::min(info1.y + info1.height, info2.y + info2.height) - 
                              std::max(info1.y, info2.y);
            
            if (x_overlap > 0 && y_overlap > 0) {
                total_overlap += x_overlap * y_overlap;
            }
        }
    }
    
    return total_overlap;
}

void PlacerEngine::runBasicStrategy() {
    std::cout << "  Running Basic Force-Directed Algorithm..." << std::endl;
    
    current_hpwl_ = calculateHPWL();
    std::cout << "  Initial HPWL: " << current_hpwl_ << std::endl;
    
    const int max_iterations = 50;
    
    for (int iter = 0; iter < max_iterations; ++iter) {
        solveForceDirectedIteration(iter);
        
        double new_hpwl = calculateHPWL();
        
        if (iter % 5 == 0) {
            std::cout << "  Iteration " << iter << ": HPWL = " << new_hpwl << std::endl;
            
            if (viz_) {
                std::string filename = "quadratic_iter_" + std::to_string(iter);
                viz_->drawPlacementWithRunId(filename, run_id_);
            }
        }
        
        double improvement = current_hpwl_ - new_hpwl;
        if (std::abs(improvement) < current_hpwl_ * 0.0001) {
            std::cout << "  Converged at iteration " << iter << std::endl;
            break;
        }
        
        current_hpwl_ = new_hpwl;
    }
    
    std::cout << "  Basic strategy completed. Final HPWL: " << current_hpwl_ << std::endl;
}

void PlacerEngine::runElectrostaticStrategy() {
    std::cout << "  Running Electrostatic Field Algorithm (Nesterov)..." << std::endl;
    
    if (!global_placer_) {
        global_placer_ = new GlobalPlacer(db_, db_->getNetlistDB());
        
        const int grid_size = 64;
        const double target_density = 0.7;
        
        if (!global_placer_->initialize(grid_size, target_density)) {
            std::cerr << "  Error: Failed to initialize GlobalPlacer" << std::endl;
            return;
        }
        
        global_placer_->setMaxIterations(200);
        global_placer_->setInitialLambda(0.0001);
        global_placer_->setLambdaGrowthRate(1.05);
        global_placer_->setLearningRate(0.1);
        global_placer_->setMomentum(0.9);
        global_placer_->setVerbose(true);
        
        if (viz_) {
            global_placer_->setVisualizer(viz_);
            global_placer_->setRunId(run_id_);
        }
    }
    
    global_placer_->runPlacement();
    
    current_hpwl_ = calculateHPWL();
    std::cout << "  Electrostatic strategy completed. Final HPWL: " << current_hpwl_ << std::endl;
}

void PlacerEngine::runHybridStrategy() {
    std::cout << "  Running Hybrid Cascade Algorithm..." << std::endl;
    
    std::cout << "\n  === Phase 1: Warm-up (Basic Force-Directed) ===" << std::endl;
    const int max_warmup_iterations = 15;  // Reduce maximum iterations
    const double stop_hpwl_ratio = 0.3;   // Stop when HPWL drops to 30% of initial value
    
    current_hpwl_ = calculateHPWL();
    double initial_hpwl = current_hpwl_;
    std::cout << "  Initial HPWL: " << current_hpwl_ << std::endl;
    
    for (int iter = 0; iter < max_warmup_iterations; ++iter) {
        solveForceDirectedIteration(iter);
        
        double new_hpwl = calculateHPWL();
        double current_ratio = new_hpwl / initial_hpwl;
        
        if (iter % 5 == 0 || iter == max_warmup_iterations - 1) {
            std::cout << "  Warmup iteration " << iter << ": HPWL = " << new_hpwl 
                      << " (ratio: " << std::fixed << std::setprecision(3) << current_ratio << ")" << std::endl;
            
            if (viz_) {
                std::string filename = "warmup_iter_" + std::to_string(iter);
                viz_->drawPlacementWithRunId(filename, run_id_);
            }
        }
        
        current_hpwl_ = new_hpwl;
        
        // [Safety Valve #2] Gentle stop condition: avoid over-compression
        if (current_ratio <= stop_hpwl_ratio) {
            std::cout << "  Warm-up stopped early: HPWL ratio reached " << current_ratio << std::endl;
            break;
        }
    }
    
    std::cout << "  Warm-up completed. Cells are initially spread." << std::endl;
    std::cout << "  HPWL after warm-up: " << current_hpwl_ << std::endl;
    
    // =========================================================
    // [Key Fix] Explicit synchronization: commit warm-up results to Cell objects
    // =========================================================
    db_->commitPlacement();
    std::cout << "  >> Synchronization: Warm-up results committed to Netlist." << std::endl;
    
    std::cout << "\n  === Phase 2: Refinement (Electrostatic Nesterov) ===" << std::endl;
    
    if (!global_placer_) {
        global_placer_ = new GlobalPlacer(db_, db_->getNetlistDB());
        
        const int grid_size = 64;
        const double target_density = 0.7;
        
        if (!global_placer_->initialize(grid_size, target_density)) {
            std::cerr << "  Error: Failed to initialize GlobalPlacer" << std::endl;
            return;
        }
        
        global_placer_->setMaxIterations(200);
        global_placer_->setVerbose(true);
        
        // === Apply gentle parameters (gas diffusion mode) ===
        global_placer_->setAggressiveParameters();
        
        if (viz_) {
            global_placer_->setVisualizer(viz_);
            global_placer_->setRunId(run_id_);
            // Set warm-up mode flag, enable conservative Lambda tuning
            global_placer_->setWarmupMode(true);
        }
    }
    
    global_placer_->runPlacement();
    
    current_hpwl_ = calculateHPWL();
    std::cout << "  Hybrid strategy completed. Final HPWL: " << current_hpwl_ << std::endl;
}

} // namespace mini