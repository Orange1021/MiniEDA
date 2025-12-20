/**
 * @file placer_engine.cpp
 * @brief Placement Engine Implementation for MiniPlacement
 */

#include "placer_engine.h"
#include "global_placer.h"
#include "abacus_legalizer.h"
#include "greedy_legalizer.h"
#include "legalizer.h"
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

    double total_hpwl = 0.0;
    
    // Get netlist database
    NetlistDB* netlist_db = db_->getNetlistDB();
    if (!netlist_db) return 0.0;
    
    // Iterate over all nets
    for (const auto& net_ptr : netlist_db->getNets()) {
        const Net* net = net_ptr.get();
        if (!net) continue;
        
        double min_x, max_x, min_y, max_y;
        calculateNetBoundingBox(net, min_x, max_x, min_y, max_y);
        
        // Add HPWL for this net
        total_hpwl += (max_x - min_x) + (max_y - min_y);
    }
    
    return total_hpwl;
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
    
    std::cout << "Starting Detailed Placement (Greedy Adjacent Swap)..." << std::endl;
    
    // PRE-CHECK: Verify no overlaps before detailed placement
    std::cout << "  Pre-check overlap status: ";
    if (hasOverlaps()) {
        std::cout << "FOUND OVERLAPS - This should not happen!" << std::endl;
    } else {
        std::cout << "No overlaps (good)" << std::endl;
    }
    
    // 1. Data Prep: Group cells by rows, sorted by x-coordinate
    const Rect& core_area = db_->getCoreArea();
    double row_height = db_->getRowHeight();
    double core_y_min = core_area.y_min;
    
    // Create row structure
    std::map<int, std::vector<Cell*>> rows;
    
    for (Cell* cell : db_->getAllCells()) {
        const auto& info = db_->getCellInfo(cell);
        if (info.fixed) continue;
        
        double current_y = info.y + info.height / 2.0;
        int row_idx = static_cast<int>(std::round((current_y - core_y_min) / row_height));
        rows[row_idx].push_back(cell);
    }
    
    // Sort each row by x-coordinate
    for (auto& [row_idx, cells_in_row] : rows) {
        std::sort(cells_in_row.begin(), cells_in_row.end(),
                 [this](Cell* a, Cell* b) {
                     return db_->getCellInfo(a).x < db_->getCellInfo(b).x;
                 });
    }
    
    std::cout << "  Prepared " << rows.size() << " rows for detailed optimization" << std::endl;
    
    // 2. Optimization Loop: Multiple passes over all rows
    const int max_passes = 5;
    double initial_hpwl = calculateHPWL();
    double current_hpwl = initial_hpwl;
    int total_swaps = 0;
    
    for (int pass = 0; pass < max_passes; ++pass) {
        int pass_swaps = 0;
        std::cout << "  Pass " << (pass + 1) << "/" << max_passes << std::endl;
        
        // 3. Row Traversal: Try adjacent swaps in each row
        for (auto& [row_idx, cells_in_row] : rows) {
            for (size_t i = 0; i < cells_in_row.size() - 1; ++i) {
                Cell* left_cell = cells_in_row[i];      // Originally on the left
                Cell* right_cell = cells_in_row[i + 1]; // Originally on the right
                
                // Store original cell information for potential rollback
                const auto left_orig_info = db_->getCellInfo(left_cell);
                const auto right_orig_info = db_->getCellInfo(right_cell);
                
                // Calculate reference X coordinate (starting point for this pair)
                double start_x = (i > 0) ? 
                    db_->getCellInfo(cells_in_row[i - 1]).x + 
                    db_->getCellInfo(cells_in_row[i - 1]).width : 
                    core_area.x_min;
                
                // PROACTIVE BOUNDARY CHECK: Calculate proposed positions after swap
                // After swap: right_cell becomes the new "left", left_cell becomes the new "right"
                double proposed_x_new_left = start_x;
                double proposed_x_new_right = start_x + db_->getCellInfo(right_cell).width;
                
                // RIGHT BOUNDARY VALIDATION: Ensure swap won't exceed core area
                double row_limit = core_area.x_max;
                if (proposed_x_new_right + db_->getCellInfo(left_cell).width > row_limit) {
                    // VIOLATION: Swap would cause overflow beyond core boundary
                    // ACTION: Abort this swap attempt, move to next iteration
                    continue;
                }
                
                // ADDITIONAL VALIDATION: Check for conflicts with subsequent cells in the same row
                bool has_conflict = false;
                if (i + 2 < cells_in_row.size()) {
                    Cell* next_cell = cells_in_row[i + 2];
                    const auto& next_info = db_->getCellInfo(next_cell);
                    double next_cell_left = next_info.x;
                    
                    // Check if our swapped right cell would overlap with the next cell
                    if (proposed_x_new_right + db_->getCellInfo(left_cell).width > next_cell_left) {
                        has_conflict = true;
                    }
                }
                
                if (has_conflict) {
                    // VIOLATION: Swap would cause overlap with subsequent cell
                    // ACTION: Abort this swap attempt, move to next iteration
                    continue;
                }
                
                // Measure current HPWL before making changes
                double hpwl_before = calculateHPWL();
                
                // ATOMIC SWAP EXECUTION: Exchange order in vector
                std::swap(cells_in_row[i], cells_in_row[i + 1]);
                
                // COMMIT PHYSICAL PLACEMENT: Update positions with validated coordinates
                db_->placeCell(right_cell, proposed_x_new_left, right_orig_info.y);   // Original right, now left
                db_->placeCell(left_cell, proposed_x_new_right, left_orig_info.y);    // Original left, now right
                
                // Measure new HPWL after swap
                double hpwl_after = calculateHPWL();
                
                // DECISION LOGIC: Keep swap only if it improves wirelength AND doesn't cause overlaps
                if (hpwl_after < hpwl_before - 1e-9) {  // Small epsilon for numerical stability
                    // Check for overlaps after the swap
                    if (!hasOverlaps()) {
                        // ACCEPT the swap - improvement achieved and no overlaps!
                        pass_swaps++;
                        total_swaps++;
                    } else {
                        // ROLLBACK due to overlaps: Revert both vector order and physical positions
                        std::swap(cells_in_row[i], cells_in_row[i + 1]);
                        // Restore original coordinates (safest approach, no recalculation)
                        db_->placeCell(left_cell, left_orig_info.x, left_orig_info.y);
                        db_->placeCell(right_cell, right_orig_info.x, right_orig_info.y);
                    }
                } else {
                    // SAFE ROLLBACK: Revert both vector order and physical positions
                    std::swap(cells_in_row[i], cells_in_row[i + 1]);
                    // Restore original coordinates (safest approach, no recalculation)
                    db_->placeCell(left_cell, left_orig_info.x, left_orig_info.y);
                    db_->placeCell(right_cell, right_orig_info.x, right_orig_info.y);
                }
            }
        }
        
        current_hpwl = calculateHPWL();
        std::cout << "    Pass swaps: " << pass_swaps 
                  << ", HPWL: " << current_hpwl 
                  << " (improvement: " << (initial_hpwl - current_hpwl) << ")" << std::endl;
        
        // Early termination if no improvements
        if (pass_swaps == 0) {
            std::cout << "  No improvements in pass " << (pass + 1) << ", early termination" << std::endl;
            break;
        }
    }
    
    // 4. Finalize
    double final_hpwl = calculateHPWL();
    double total_improvement = initial_hpwl - final_hpwl;
    
    // Update cached HPWL to maintain consistency
    current_hpwl_ = final_hpwl;
    
    std::cout << "Detailed Placement completed!" << std::endl;
    std::cout << "  Total swaps performed: " << total_swaps << std::endl;
    std::cout << "  Initial HPWL: " << initial_hpwl << std::endl;
    std::cout << "  Final HPWL: " << final_hpwl << std::endl;
    std::cout << "  Total improvement: " << total_improvement << " (" 
              << (total_improvement / initial_hpwl * 100.0) << "%)" << std::endl;
    
    // 5. Final overlap check
    bool has_overlaps = hasOverlaps();
    double total_overlap = calculateTotalOverlap();
    std::cout << "  Final overlap check: " << (has_overlaps ? "FOUND OVERLAPS!" : "No overlaps") << std::endl;
    if (has_overlaps) {
        std::cout << "  Total overlap area: " << total_overlap << std::endl;
    }
    
    // 6. Visualize final result
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
    const int max_warmup_iterations = 15;  // 减少最大迭代次数
    const double stop_hpwl_ratio = 0.3;   // 当HPWL降到初始值的30%时停止
    
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
        
        // [安全阀 #2] 温和停止条件：避免压得太实
        if (current_ratio <= stop_hpwl_ratio) {
            std::cout << "  Warm-up stopped early: HPWL ratio reached " << current_ratio << std::endl;
            break;
        }
    }
    
    std::cout << "  Warm-up completed. Cells are initially spread." << std::endl;
    std::cout << "  HPWL after warm-up: " << current_hpwl_ << std::endl;
    
    // =========================================================
    // 【关键修复】显式同步：将 Warm-up 结果提交给 Cell 对象
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
        
        // === 应用温和参数 (气体扩散模式) ===
        global_placer_->setAggressiveParameters();
        
        if (viz_) {
            global_placer_->setVisualizer(viz_);
            global_placer_->setRunId(run_id_);
            // 设置warm-up模式标志，启用保守的Lambda调优
            global_placer_->setWarmupMode(true);
        }
    }
    
    global_placer_->runPlacement();
    
    current_hpwl_ = calculateHPWL();
    std::cout << "  Hybrid strategy completed. Final HPWL: " << current_hpwl_ << std::endl;
}

} // namespace mini