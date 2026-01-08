/**
 * @file global_placer.cpp
 * @brief Global Placer Implementation with Momentum Optimization
 * @details The heart of electrostatic placement - combines wirelength and density gradients
 */

#include "global_placer.h"
#include <iomanip>
#include <algorithm>
#include <chrono>
#include "../../lib/include/hpwl_calculator.h"
#include "../../lib/include/csv_exporter.h"

namespace mini {

// ============================================================================
// Constructor and Initialization
// ============================================================================

GlobalPlacer::GlobalPlacer(PlacerDB* placer_db, NetlistDB* netlist_db)
    : placer_db_(placer_db), netlist_db_(netlist_db), density_margin_(0.1), max_gradient_ratio_(0.01), max_displacement_ratio_(0.02) {
    
    if (!placer_db_ || !netlist_db_) {
        throw std::invalid_argument("PlacerDB and NetlistDB must be valid");
    }
    
    debugLog("GlobalPlacer initialized - ready for electrostatic placement!");
}

bool GlobalPlacer::initialize(int grid_size, double target_density, double initial_lambda, double lambda_growth_rate, 
                               double learning_rate, double momentum, double convergence_threshold) {
    debugLog("Initializing global placer...");
    
    // Validate grid size (must be power of 2 for FFT)
    if (!PoissonSolver::isValidFFTSize(grid_size, grid_size)) {
        std::cerr << "Error: Grid size must be power of 2 for FFT" << std::endl;
        std::cerr << "  Requested: " << grid_size << "x" << grid_size << std::endl;
        std::cerr << "  Suggested: " << PoissonSolver::nextPowerOfTwo(grid_size) 
                  << "x" << PoissonSolver::nextPowerOfTwo(grid_size) << std::endl;
        return false;
    }
    
    grid_size_ = grid_size;
    target_density_ = target_density;
    initial_lambda_ = initial_lambda;
    lambda_growth_rate_ = lambda_growth_rate;
    learning_rate_ = learning_rate;
    momentum_ = momentum;
    convergence_threshold_ = convergence_threshold;
    current_lambda_ = initial_lambda;
    
    // Get core area from placer database
    const Rect& core_area = placer_db_->getCoreArea();
    double core_w = core_area.width();
    double core_h = core_area.height();
    
    // Force exact grid_size × grid_size bins for Poisson solver compatibility
    // Create density grid with exact grid dimensions
    density_grid_ = std::make_unique<DensityGrid>();
    if (!density_grid_->init(grid_size_, grid_size_, core_w, core_h, core_area.x_min, core_area.y_min)) {
        std::cerr << "Error: Failed to initialize density grid" << std::endl;
        return false;
    }
    
    // Initialize Poisson solver
    poisson_solver_ = std::make_unique<PoissonSolver>();
    
    // Set actual bin dimensions for accurate gradient calculation
    double bin_w = core_w / grid_size_;
    double bin_h = core_h / grid_size_;
    poisson_solver_->setBinSize(bin_w, bin_h);
    
    // Collect all movable cells
    cells_ = placer_db_->getAllCells();
    cells_.erase(
        std::remove_if(cells_.begin(), cells_.end(), 
                      [this](const Cell* cell) { return placer_db_->isCellFixed(const_cast<Cell*>(cell)); }),
        cells_.end()
    );
    
    debugLog("Found " + std::to_string(cells_.size()) + " movable cells");
    
    // Initialize optimization state
    initializeOptimizationState();
    
    initialized_ = validateInitialization();
    
    if (initialized_) {
        debugLog("Global placer initialized successfully!");
        debugLog("Grid size: " + std::to_string(grid_size_) + "x" + std::to_string(grid_size_));
        debugLog("Target density: " + std::to_string(target_density_));
        debugLog("Initial lambda: " + std::to_string(initial_lambda_));
    }
    
    return initialized_;
}

void GlobalPlacer::initializeOptimizationState() {
    // Initialize velocities to zero
    velocities_.resize(cells_.size(), {0.0, 0.0});
    
    // Initialize wire gradients storage
    wire_gradients_.resize(cells_.size(), {0.0, 0.0});
    
    // Clear statistics
    stats_.clear();
    
    debugLog("Optimization state initialized");
}

bool GlobalPlacer::validateInitialization() const {
    if (!placer_db_ || !netlist_db_) {
        std::cerr << "Error: Invalid database references" << std::endl;
        return false;
    }
    
    if (!density_grid_ || !poisson_solver_) {
        std::cerr << "Error: Failed to initialize core components" << std::endl;
        return false;
    }
    
    if (cells_.empty()) {
        std::cerr << "Error: No movable cells found" << std::endl;
        return false;
    }
    
    return true;
}

// ============================================================================
// Main Placement Algorithm
// ============================================================================

void GlobalPlacer::runPlacement() {
    if (!initialized_) {
        std::cerr << "Error: Global placer not initialized" << std::endl;
        return;
    }
    
    debugLog("Starting momentum-based global placement optimization...");
    
    // === Auto-Normalize Initial Lambda ===
    // Run one iteration to measure gradient magnitudes
    density_grid_->updateDensity(cells_, placer_db_);
    std::vector<Bin> density_input = density_grid_->getBins();
    poisson_solver_->solve(density_input, grid_size_, grid_size_);
    
    // Sync updated gradients back to density_grid_
    for (int y = 0; y < grid_size_; ++y) {
        for (int x = 0; x < grid_size_; ++x) {
            int idx = y * grid_size_ + x;
            Bin& grid_bin = density_grid_->getBin(x, y);
            const Bin& solved_bin = density_input[idx];
            grid_bin.electro_gradient_x = solved_bin.electro_gradient_x;
            grid_bin.electro_gradient_y = solved_bin.electro_gradient_y;
            grid_bin.electro_potential = solved_bin.electro_potential;
        }
    }
    
    calculateWirelengthGradients();
    
    // Calculate average gradient magnitudes
    double sum_wire_gradient = 0.0;
    double sum_dens_gradient = 0.0;
    int active_cells = 0;
    
    for (int i = 0; i < cells_.size(); ++i) {
        if (placer_db_->isCellFixed(cells_[i])) continue;
        
        // Wire Force
        sum_wire_gradient += std::abs(wire_gradients_[i].x) + std::abs(wire_gradients_[i].y);
        
        // Density Force
        auto bin_idx = density_grid_->getBinIndex(cells_[i]->getX(), cells_[i]->getY());
        const Bin& bin = density_grid_->getBin(bin_idx.first, bin_idx.second);
        sum_dens_gradient += std::abs(bin.electro_gradient_x) + std::abs(bin.electro_gradient_y);
        active_cells++;
    }
    
    // [Dynamic Lambda Strategy] Auto-calculate initial value based on force balance
    if (sum_dens_gradient > 1e-6 && active_cells > 0) {
        // 1. Calculate reasonable initial Lambda based on force balance
        double ratio = (sum_wire_gradient / sum_dens_gradient);
        // Make density force and wire force on the same scale
        current_lambda_ = sum_wire_gradient / sum_dens_gradient;
        
        // 2. Set Lambda upper limit to prevent infinite repulsion later
        double max_lambda = 0.5 * (sum_wire_gradient / sum_dens_gradient);
        
        // 3. Save upper limit for subsequent growth
        max_lambda_ = max_lambda;
        
        debugLog(">>> DYNAMIC Lambda Strategy:");
        debugLog("    Initial Lambda: " + std::to_string(current_lambda_) + " (auto-calculated)");
        debugLog("    Max Lambda: " + std::to_string(max_lambda_) + " (safety limit)");
        debugLog("    Force ratio: " + std::to_string(ratio));
        debugLog("    Expected: Balanced gradients from start");
    } else {
        debugLog(">>> Using fallback Initial Lambda: " + std::to_string(current_lambda_));
    }
    
    // === Force Magnitude Balance Diagnosis ===
    if (active_cells > 0) {
        double avg_wire_gradient = sum_wire_gradient / active_cells;
        double avg_dens_gradient = sum_dens_gradient / active_cells;
        double ratio = avg_dens_gradient > 1e-9 ? avg_wire_gradient / avg_dens_gradient : 0.0;
        
        debugLog("Gradient Balance Analysis (GAS DIFFUSION MODE):");
          debugLog("  Average Wire Gradient: " + std::to_string(avg_wire_gradient));
          debugLog("  Average Density Gradient: " + std::to_string(avg_dens_gradient));
          debugLog("  Gradient Ratio (Wire/Density): " + std::to_string(ratio));        
        if (ratio > 2.0) {
            debugLog("  [WARNING] Wire Gradient dominates! Cells may cluster excessively.");
        } else if (ratio < 0.5) {
            debugLog("  [WARNING] Density Gradient dominates! Cells may explode to corners.");
        } else {
            debugLog("  [PERFECT] Gradients are balanced - expect gentle gas diffusion.");
        }
        debugLog("  [EXPECTED] Gentle spreading like ink in water, uniform density distribution.");
    }
    
    debugLog("----------------------------");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < max_iterations_; ++iter) {
        auto iter_start = std::chrono::high_resolution_clock::now();
        
        // [IO Weight Schedule] Calculate progress (0.0 ~ 1.0)
        double progress = (double)iter / max_iterations_;
        
        // Step 1: Update density distribution
        density_grid_->updateDensity(cells_, placer_db_);
        
        // [One-way Valve Fix] Prepare "Overflow Only" density distribution
        // Calculate target density (utilization + margin)
        const Rect& core_area = placer_db_->getCoreArea();
        double total_cell_area = 0.0;
        for (const auto& cell : cells_) {
            const auto& info = placer_db_->getCellInfo(cell);
            total_cell_area += info.width * info.height;
        }
        double total_core_area = core_area.width() * core_area.height();
        double utilization = total_cell_area / total_core_area;
        double target_density = utilization + density_margin_;  // Use configured margin
        if (target_density > 1.0) target_density = 1.0;
        
        debugLog(">>> OVERFLOW-ONLY MODE: target_density=" + std::to_string(target_density) + 
                  ", utilization=" + std::to_string(utilization));
        
        // Prepare Poisson solver input: only handle excess density
        density_input = density_grid_->getBins();  // Make a copy
        for (auto& bin : density_input) {
            // [Key Fix] Only focus on "excess" parts, empty areas don't generate attraction
            double rho = bin.density;
            double overflow = rho - target_density;
            if (overflow < 0) overflow = 0;  // Truncate negative values, eliminate "black holes"
            bin.density = overflow;  // Only pass positive charge to Poisson solver
        }
        
        // Step 2: Solve Poisson equation for electrostatic gradients
        bool solve_success = poisson_solver_->solve(density_input, grid_size_, grid_size_);
        
        if (!solve_success) {
            std::cerr << "Warning: Poisson solver failed at iteration " << iter << std::endl;
            continue;
        }
        
        // Step 2.5: Sync updated gradients back to density_grid_
        // PoissonSolver updates density_input, but we need the gradients in density_grid_
        for (int y = 0; y < grid_size_; ++y) {
            for (int x = 0; x < grid_size_; ++x) {
                int idx = y * grid_size_ + x;
                Bin& grid_bin = density_grid_->getBin(x, y);
                const Bin& solved_bin = density_input[idx];
                grid_bin.electro_gradient_x = solved_bin.electro_gradient_x;
                grid_bin.electro_gradient_y = solved_bin.electro_gradient_y;
                grid_bin.electro_potential = solved_bin.electro_potential;
            }
        }
        
        // Step 3: Calculate wirelength gradients with IO weighting
        calculateWirelengthGradients(progress);
        
        // Step 4: Momentum update
        double total_movement = momentumUpdate(iter);
        
        // Step 5: Update lambda (increase density penalty)
        updateLambda();
        
        // [Dynamic Lambda Balance] Re-evaluate force balance every 10 iterations
        if (iter % 10 == 0 && iter > 0) {
            // Calculate average force for current frame
            double avg_wire_gradient = 0.0;
            double avg_density_gradient_raw = 0.0;
            int active_cells = 0;
            
            for (size_t i = 0; i < cells_.size(); ++i) {
                if (placer_db_->isCellFixed(cells_[i])) continue;
                
                // Calculate wire force magnitude
                double wire_gradient_mag = std::sqrt(wire_gradients_[i].x * wire_gradients_[i].x + 
                                             wire_gradients_[i].y * wire_gradients_[i].y);
                avg_wire_gradient += wire_gradient_mag;
                
                // Calculate raw density force magnitude (raw value from Solver output)
                Point density_grad = getDensityGradient(cells_[i]);
                double density_gradient_raw_mag = std::sqrt(density_grad.x * density_grad.x + 
                                                        density_grad.y * density_grad.y);
                avg_density_gradient_raw += density_gradient_raw_mag;
                
                active_cells++;
            }
            
            if (active_cells > 0) {
                avg_wire_gradient /= active_cells;
                avg_density_gradient_raw /= active_cells;
                
                // Target: Lambda * avg_density_gradient_raw ≈ avg_wire_gradient
                // i.e., target_lambda ≈ avg_wire_gradient / avg_density_gradient_raw
                double target_balance_lambda = (avg_density_gradient_raw > 1e-9) ?
                    (avg_wire_gradient / avg_density_gradient_raw) : current_lambda_;
                
                // Dynamic adjustment strategy
                double effective_density_gradient = current_lambda_ * avg_density_gradient_raw;
                double current_ratio = (effective_density_gradient > 1e-9) ? 
                    (avg_wire_gradient / effective_density_gradient) : 0.0;
                
                // Effective density force too small, accelerate Lambda growth
                if (current_ratio > 10.0) {
                    current_lambda_ *= 1.2;  // 20% growth
                }
                // Effective density force too large, slow down growth
                else if (current_ratio < 0.1) {
                    current_lambda_ *= 1.01; // 1% growth
                }
                // Near balance, normal growth
                else {
                    current_lambda_ *= 1.05; // 5% growth
                }
                
                // Dynamically update max_lambda_, allow breaking initial limit
                max_lambda_ = std::max(max_lambda_, target_balance_lambda * 2.0);
            }
        }
        
        // Step 6: Calculate timing and statistics
        auto iter_end = std::chrono::high_resolution_clock::now();
        auto iter_duration = std::chrono::duration_cast<std::chrono::milliseconds>(iter_end - iter_start);
        
        updateStatistics(iter, total_movement, iter_duration.count());
        
        // Step 7: Visualization and logging
        if (verbose_ && (iter % 10 == 0 || iter < 10)) {
            std::cout << "Iter " << std::setw(3) << iter 
                      << ": Movement=" << std::fixed << std::setprecision(3) << total_movement
                      << ", HPWL=" << std::fixed << std::setprecision(1) << current_hpwl_
                      << ", Lambda=" << std::scientific << std::setprecision(2) << current_lambda_
                      << ", MaxDensity=" << std::fixed << std::setprecision(3) << density_grid_->getMaxDensity()
                      << std::endl;
        }
        
        // Step 8: Export placement images periodically
        if (iter % 20 == 0) {
            exportPlacementImage(iter);
        }
        
        // Step 9: Check convergence
        if (checkConvergence(total_movement)) {
            debugLog("Convergence reached at iteration " + std::to_string(iter));
            break;
        }
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    debugLog("Global placement completed in " + std::to_string(total_duration.count()) + " ms");
    
    // Final export
    exportPlacementImage(max_iterations_);
    printFinalStatistics();
    
    // Export density visualization
    exportDensityVisualization();
}

// ============================================================================
// Wirelength Gradient Calculation (Star Model)
// ============================================================================

void GlobalPlacer::calculateWirelengthGradients() {
    calculateWirelengthGradients(0.0);  // Default: no progress weighting
}

void GlobalPlacer::calculateWirelengthGradients(double progress_ratio) {
    // Clear gradients
    std::fill(wire_gradients_.begin(), wire_gradients_.end(), Point{0.0, 0.0});
    
    // [IO Weight Schedule] Dynamically calculate IO weight
    // Early stage: moderate IO weight (0.5)
    // Middle stage (0.3 <= progress <= 0.6): linear transition 0.5 -> 1.0
    // Late stage (progress > 0.6): weight restored (1.0)
    double io_weight = 0.5;  
    
    if (progress_ratio > 0.6) {
        io_weight = 1.0;
    } else if (progress_ratio > 0.3) {
        // Linear transition: 0.5 -> 1.0
        io_weight = 0.5 + (progress_ratio - 0.3) * 1.67;  
    }
    
    debugLog("IO Weight Schedule: progress=" + std::to_string(progress_ratio) + 
              ", io_weight=" + std::to_string(io_weight));
    
    // Real wirelength model: Star Model HPWL gradient with IO weighting
    // Each net creates gradients pointing away from the net's center of gravity
    // (Gradient points toward increasing wirelength direction)
    for (const auto& net_ptr : netlist_db_->getNets()) {
        const Net* net = net_ptr.get();
        if (!net) continue;
        
        // Skip single-pin nets (no wirelength contribution)
        const auto& loads = net->getLoads();
        Pin* driver = net->getDriver();
        if (loads.empty() && !driver) continue;
        
        // 1. Check if this Net connects to Fixed Pin (IO Pin)
        bool has_fixed_pin = false;
        if (driver && placer_db_->isCellFixed(driver->getOwner())) {
            has_fixed_pin = true;
        }
        for (Pin* load : loads) {
            if (load && placer_db_->isCellFixed(load->getOwner())) {
                has_fixed_pin = true;
                break;
            }
        }
        
        // Calculate net center of gravity
        Point net_cog = getNetCenterOfGravity(net);
        
        // 2. Calculate base weight
        double base_weight = getNetWeight(net);
        
        // 3. Apply IO down-weighting
        // If connected to IO, multiply by io_weight; if pure internal connection, weight remains 1.0
        double final_weight = has_fixed_pin ? (base_weight * io_weight) : base_weight;
        
        // Apply gradient to driver pin
        if (driver) {
            Cell* driver_cell = driver->getOwner();
            if (driver_cell && !placer_db_->isCellFixed(driver_cell)) {
                // Find cell index
                auto it = std::find(cells_.begin(), cells_.end(), driver_cell);
                if (it != cells_.end()) {
                    size_t idx = it - cells_.begin();
                    Point driver_pos{driver_cell->getX(), driver_cell->getY()};
                    Point dist_from_cog = net_cog - driver_pos;  // Gradient points toward increasing wirelength
                    wire_gradients_[idx].x += dist_from_cog.x * final_weight;
                    wire_gradients_[idx].y += dist_from_cog.y * final_weight;
                }
            }
        }
        
        // Apply gradient to load pins
        for (Pin* load : loads) {
            if (!load) continue;
            Cell* load_cell = load->getOwner();
            if (load_cell && !placer_db_->isCellFixed(load_cell)) {
                // Find cell index
                auto it = std::find(cells_.begin(), cells_.end(), load_cell);
                if (it != cells_.end()) {
                    size_t idx = it - cells_.begin();
                    Point load_pos{load_cell->getX(), load_cell->getY()};
                    Point dist_from_cog = net_cog - load_pos;  // Gradient points toward increasing wirelength
                    wire_gradients_[idx].x += dist_from_cog.x * final_weight;
                    wire_gradients_[idx].y += dist_from_cog.y * final_weight;
                }
            }
        }
    }
}

Point GlobalPlacer::getNetCenterOfGravity(const Net* net) const {
    Point sum_pos{0.0, 0.0};
    int pin_count = 0;
    
    // Include driver pin
    Pin* driver = net->getDriver();
    if (driver) {
        Cell* driver_cell = driver->getOwner();
        if (driver_cell) {
            const auto& cell_info = placer_db_->getCellInfo(const_cast<Cell*>(driver_cell));
            sum_pos.x += driver_cell->getX() + cell_info.width / 2.0;
            sum_pos.y += driver_cell->getY() + cell_info.height / 2.0;
            pin_count++;
        }
    }
    
    // Include load pins
    for (Pin* load : net->getLoads()) {
        if (!load) continue;
        Cell* load_cell = load->getOwner();
        if (load_cell) {
            const auto& cell_info = placer_db_->getCellInfo(const_cast<Cell*>(load_cell));
            sum_pos.x += load_cell->getX() + cell_info.width / 2.0;
            sum_pos.y += load_cell->getY() + cell_info.height / 2.0;
            pin_count++;
        }
    }
    
    // Calculate average position
    if (pin_count > 0) {
        return Point{sum_pos.x / pin_count, sum_pos.y / pin_count};
    }
    
    // Fallback to core center
    const Rect& core_area = placer_db_->getCoreArea();
    return core_area.center();
}

double GlobalPlacer::getNetWeight(const Net* net) const {
    // Net weight based on fanout: higher fanout = stronger attraction
    const auto& loads = net->getLoads();
    Pin* driver = net->getDriver();
    
    int pin_count = loads.size() + (driver ? 1 : 0);
    
    // Skip single-pin nets
    if (pin_count <= 1) return 0.0;
    
    // Weight inversely proportional to fanout (standard Star Model)
    // This prevents high-fanout nets from dominating
    return 1.0 / (pin_count - 1);
}

// ============================================================================
// Density Gradient Integration
// ============================================================================

Point GlobalPlacer::getDensityGradient(const Cell* cell) const {
    // Map cell position to grid coordinates
    auto [grid_x, grid_y] = getCellGridPosition(cell);
    
    // Get bin from density grid
    const Bin& bin = density_grid_->getBin(grid_x, grid_y);
    
    // Density gradient = negative of electrostatic gradient
    // (Poisson solver outputs electrostatic gradient, true density gradient is opposite direction)
    return Point{-bin.electro_gradient_x, -bin.electro_gradient_y};
}

std::pair<int, int> GlobalPlacer::getCellGridPosition(const Cell* cell) const {
    double cell_x = cell->getX();
    double cell_y = cell->getY();
    
    // Use cell center for mapping
    const auto& cell_info = placer_db_->getCellInfo(const_cast<Cell*>(cell));
    double center_x = cell_x + cell_info.width / 2.0;
    double center_y = cell_y + cell_info.height / 2.0;
    
    return density_grid_->getBinIndex(center_x, center_y);
}

// ============================================================================
// Momentum Gradient Descent Optimization
// ============================================================================

// NOTE: This implementation uses standard Momentum Gradient Descent, not Nesterov Accelerated Gradient (NAG).
// While the original code was named "Nesterov", the actual algorithm implemented is:
//   v_{k+1} = momentum * v_k - learning_rate * gradient
//   x_{k+1} = x_k + v_{k+1}
// This is the classic Momentum update, which is widely used in commercial EDA placement tools
// due to its stability and effectiveness.

double GlobalPlacer::momentumUpdate(int iteration) {
    double total_movement = 0.0;
    
    // Calculate total gradients
    calculateTotalGradients();
    
    // [Decisive Experiment] Print key force data at 50th iteration
    if (iteration == 50) {
        double avg_wire_gradient = 0.0;
        double avg_density_gradient_raw = 0.0;
        int active_cells = 0;
        
        for (size_t i = 0; i < cells_.size(); ++i) {
            if (placer_db_->isCellFixed(cells_[i])) continue;
            
            // Calculate wire force magnitude
            double wire_gradient_mag = std::sqrt(wire_gradients_[i].x * wire_gradients_[i].x + 
                                             wire_gradients_[i].y * wire_gradients_[i].y);
            avg_wire_gradient += wire_gradient_mag;
            
            // Calculate raw density force magnitude (raw value from Solver output)
            Point density_grad = getDensityGradient(cells_[i]);
            double density_gradient_raw_mag = std::sqrt(density_grad.x * density_grad.x + 
                                                    density_grad.y * density_grad.y);
            avg_density_gradient_raw += density_gradient_raw_mag;
            
            active_cells++;
        }
        
        if (active_cells > 0) {
            avg_wire_gradient /= active_cells;
            avg_density_gradient_raw /= active_cells;
            
            double effective_density_gradient = current_lambda_ * avg_density_gradient_raw;
            double force_ratio = (avg_density_gradient_raw > 1e-9) ? (avg_wire_gradient / avg_density_gradient_raw) : 0.0;
            double effective_ratio = (effective_density_gradient > 1e-9) ? (avg_wire_gradient / effective_density_gradient) : 0.0;
            
            std::cout << "\n========== [Decisive] 50th Iteration Gradient Balance Diagnosis ==========" << std::endl;
                std::cout << "Average Wire Gradient Magnitude (Avg |∇_wire|):     " << std::fixed << std::setprecision(6) << avg_wire_gradient << std::endl;
                std::cout << "Average Raw Density Gradient Magnitude (Avg |∇_dens_raw|): " << std::fixed << std::setprecision(6) << avg_density_gradient_raw << std::endl;
                std::cout << "Current Lambda (current_lambda):        " << std::scientific << std::setprecision(3) << current_lambda_ << std::endl;
                std::cout << "Effective Density Gradient (Lambda * ∇_dens_raw):    " << std::fixed << std::setprecision(6) << effective_density_gradient << std::endl;
                std::cout << "Raw Gradient Ratio (∇_wire / ∇_dens_raw):      " << std::fixed << std::setprecision(3) << force_ratio << std::endl;
                std::cout << "Effective Gradient Ratio (∇_wire / Lambda*∇_dens):   " << std::fixed << std::setprecision(3) << effective_ratio << std::endl;
                
                // Judgment criteria
                if (effective_ratio < 0.1) {
                    std::cout << "[WARNING] Effective density force is 10x+ wire force! Cells may explode to corners!" << std::endl;
                        } else if (effective_ratio > 10.0) {
                            std::cout << "[WARNING] Wire force is 10x+ effective density force! Cells may over-cluster!" << std::endl;
                        } else {
                            std::cout << "[IDEAL] Force balance is good (around 1:1), expect gentle diffusion." << std::endl;                }            std::cout << "====================================================\n" << std::endl;
        }
    }
    
    for (size_t i = 0; i < cells_.size(); ++i) {
        Cell* cell = cells_[i];
        if (placer_db_->isCellFixed(cell)) continue;
        
        // Get density gradient
        Point density_grad = getDensityGradient(cell);
        
        // Total gradient = wire_gradient + lambda * density_gradient
        double total_grad_x = wire_gradients_[i].x + current_lambda_ * density_grad.x;
        double total_grad_y = wire_gradients_[i].y + current_lambda_ * density_grad.y;
        
        // [Industrial Stabilizer] Gradient Clipping
            // Prevent huge gradients in central area from blowing up cells
        const Rect& core_area = placer_db_->getCoreArea();
        double max_gradient = core_area.width() * max_gradient_ratio_;  // Use configured ratio
            
            // Simple clipping logic
        if (total_grad_x > max_gradient) total_grad_x = max_gradient;
        if (total_grad_x < -max_gradient) total_grad_x = -max_gradient;
        if (total_grad_y > max_gradient) total_grad_y = max_gradient;
        if (total_grad_y < -max_gradient) total_grad_y = -max_gradient;
        
        // Momentum update: v_{k+1} = momentum * v_k - learning_rate * gradient
        velocities_[i].x = momentum_ * velocities_[i].x - learning_rate_ * total_grad_x;
        velocities_[i].y = momentum_ * velocities_[i].y - learning_rate_ * total_grad_y;
        
        // [Safety Valve #1] Displacement Clipping - prevent "explosion"
        double dx = velocities_[i].x;
        double dy = velocities_[i].y;
        double dist = std::sqrt(dx*dx + dy*dy);
        
        // Set upper limit: use configured ratio of chip width
        double max_dist = core_area.width() * max_displacement_ratio_;
        
        // Truncate movement distance and velocity
        if (dist > max_dist) {
            double scale = max_dist / dist;
            dx *= scale;
            dy *= scale;
            velocities_[i].x *= scale;  // Prevent excessive momentum accumulation
            velocities_[i].y *= scale;
        }
        
        // Update position: x_{k+1} = x_k + v_{k+1}
        double old_x = cell->getX();
        double old_y = cell->getY();
        
        double new_x = old_x + dx;
        double new_y = old_y + dy;

        // Update PlacerDB (internally calls cell->setPosition())
        placer_db_->placeCell(cell, new_x, new_y);
        
        // Clamp to core area
        clampToCoreArea();
        
        // Calculate movement
        double movement = std::abs(velocities_[i].x) + std::abs(velocities_[i].y);
        total_movement += movement;
    }
    
    return total_movement;
}

void GlobalPlacer::calculateTotalGradients() {
    // Wire gradients already calculated in calculateWirelengthGradients()
    // Density gradients will be calculated on-demand in nesterovUpdate()
    // This function is a placeholder for future gradient combination logic
}

void GlobalPlacer::clampToCoreArea() {
    const Rect& core_area = placer_db_->getCoreArea();
    double core_x_max = core_area.x_min + core_area.width();
    double core_y_max = core_area.y_min + core_area.height();
    
    for (Cell* cell : cells_) {
        if (placer_db_->isCellFixed(cell)) continue;
        
        const auto& cell_info = placer_db_->getCellInfo(cell);
        
        // Get current position
        double x = cell->getX();
        double y = cell->getY();
        
        // Calculate bounds to keep cell within core area
        double min_x = core_area.x_min;
        double max_x = core_x_max - cell_info.width;
        double min_y = core_area.y_min;
        double max_y = core_y_max - cell_info.height;
        
        // Clamp position
        x = std::max(min_x, std::min(max_x, x));
        y = std::max(min_y, std::min(max_y, y));

        // Update PlacerDB (internally calls cell->setPosition())
        placer_db_->placeCell(cell, x, y);
    }
}

// ============================================================================
// Dynamic Lambda Adjustment
// ============================================================================

void GlobalPlacer::updateLambda() {
    // [Slow Climb Strategy] Lambda grows slowly to prevent "explosion"
    // Use configured lambda growth rate
    double lambda_growth = lambda_growth_rate_;
    
    if (current_lambda_ < max_lambda_) {
        current_lambda_ *= lambda_growth;
        // Ensure not exceeding upper limit
        current_lambda_ = std::min(current_lambda_, max_lambda_);
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

void GlobalPlacer::updateStatistics(int iteration, double total_movement, double step_time_ms) {
    OptimizationStats stats;
    stats.iteration = iteration;
    stats.total_movement = total_movement;
    stats.hpwl = HPWLCalculator::calculateHPWL(netlist_db_, placer_db_);
    stats.max_density = density_grid_->getMaxDensity();
    stats.total_overflow = density_grid_->getTotalOverflow(target_density_);
    stats.lambda = current_lambda_;
    stats.learning_rate = learning_rate_;
    stats.step_time_ms = step_time_ms;
    
    current_hpwl_ = stats.hpwl;
    
    stats_.push_back(stats);
}

bool GlobalPlacer::checkConvergence(double total_movement) const {
    return total_movement < convergence_threshold_;
}

void GlobalPlacer::exportPlacementImage(int iteration) const {
    // Export CSV data for external visualization (use standard naming)
    std::string csv_filename = "placement_iter_" + std::to_string(iteration) + ".csv";
    exportPlacement(csv_filename);
    
    // Generate PNG image using Python script
    std::string run_dir = "visualizations/" + run_id_;
    std::string csv_path = run_dir + "/" + csv_filename;
    std::string png_filename = "electro_iter_" + std::to_string(iteration) + ".png";
    std::string png_path = run_dir + "/" + png_filename;
    
    // Call Python script to generate visualization
    std::string plot_cmd = "cd visualizations && python3 plot_placement.py " + 
                          run_id_ + "/" + csv_filename + " " + 
                          run_id_ + "/" + png_filename +
                          " --title \"Global Placement - Iteration " + std::to_string(iteration) + "\"";
    
    int result = std::system(plot_cmd.c_str());
    if (result == 0) {
        std::cout << "  Global placement iteration " << iteration << " PNG exported" << std::endl;
    } else {
        std::cout << "  Global placement iteration " << iteration << " CSV exported (visualization failed)" << std::endl;
    }
}

void GlobalPlacer::exportPlacement(const std::string& filename) const {
    // Create full path to visualization directory
    std::string full_path = "visualizations/" + run_id_ + "/" + filename;
    
    // Export using CSVExporter
    if (CSVExporter::exportPlacement(placer_db_, full_path)) {
        if (verbose_) {
            std::cout << "[GlobalPlacer] Placement exported to " << filename << std::endl;
        }
    }
}

void GlobalPlacer::printFinalStatistics() const {
    std::cout << "\n=== Global Placement Final Statistics ===" << std::endl;
    
    if (!stats_.empty()) {
        const OptimizationStats& final = stats_.back();
        std::cout << "Final HPWL: " << std::fixed << std::setprecision(1) << final.hpwl << std::endl;
        std::cout << "Final Max Density: " << std::fixed << std::setprecision(3) << final.max_density << std::endl;
        std::cout << "Final Total Overflow: " << std::fixed << std::setprecision(3) << final.total_overflow << std::endl;
        std::cout << "Final Lambda: " << std::scientific << std::setprecision(2) << final.lambda << std::endl;
        std::cout << "Total Iterations: " << final.iteration + 1 << std::endl;
        
        if (!stats_.empty()) {
            double initial_hpwl = stats_[0].hpwl;
            double hpwl_improvement = ((initial_hpwl - final.hpwl) / initial_hpwl) * 100.0;
            std::cout << "HPWL Improvement: " << std::fixed << std::setprecision(1) << hpwl_improvement << "%" << std::endl;
        }
    }
    
    density_grid_->printDensityStats(target_density_);
    poisson_solver_->printStatistics();
    
    std::cout << "==========================================" << std::endl;
}

void GlobalPlacer::debugLog(const std::string& message) const {
    if (verbose_) {
        std::cout << "[GlobalPlacer] " << message << std::endl;
    }
}



void GlobalPlacer::exportDensityVisualization() const {
    if (!density_grid_ || !initialized_) {
        debugLog("Skipping density visualization - grid not initialized");
        return;
    }
    
    // Create output directory
    std::string run_dir = "visualizations/" + run_id_;
    std::string mkdir_cmd = "mkdir -p " + run_dir;
    std::system(mkdir_cmd.c_str());
    
    // Export density map to CSV
    std::string csv_filename = run_dir + "/density_map.csv";
    density_grid_->exportDensityMap(csv_filename);
    
    // Generate heatmap using Python script
    std::string png_filename = run_dir + "/density_heatmap.png";
    std::string python_cmd = "python3 visualizations/visualize_density.py " + csv_filename + " " + png_filename + " 2>/dev/null";
    
    int result = std::system(python_cmd.c_str());
    if (result == 0) {
        debugLog("Density heatmap generated: " + png_filename);
    } else {
        debugLog("Warning: Failed to generate density heatmap (python3 or matplotlib may not be available)");
    }
}



} // namespace mini