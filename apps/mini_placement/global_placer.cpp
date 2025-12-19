/**
 * @file global_placer.cpp
 * @brief Global Placer Implementation with Nesterov Optimization
 * @details The heart of electrostatic placement - combines wirelength and density forces
 */

#include "global_placer.h"
#include <iomanip>
#include <algorithm>
#include <chrono>
#include "../../lib/include/visualizer.h"

namespace mini {

// ============================================================================
// Constructor and Initialization
// ============================================================================

GlobalPlacer::GlobalPlacer(PlacerDB* placer_db, NetlistDB* netlist_db)
    : placer_db_(placer_db), netlist_db_(netlist_db) {
    
    if (!placer_db_ || !netlist_db_) {
        throw std::invalid_argument("PlacerDB and NetlistDB must be valid");
    }
    
    debugLog("GlobalPlacer initialized - ready for electrostatic placement!");
}

bool GlobalPlacer::initialize(int grid_size, double target_density) {
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
    current_lambda_ = initial_lambda_;
    
    // Get core area from placer database
    const Rect& core_area = placer_db_->getCoreArea();
    double core_w = core_area.width();
    double core_h = core_area.height();
    
    // Force exact grid_size × grid_size bins for Poisson solver compatibility
    double bin_w = core_w / grid_size_;
    double bin_h = core_h / grid_size_;
    
    // Create density grid with exact grid dimensions
    density_grid_ = std::make_unique<DensityGrid>();
    if (!density_grid_->init(grid_size_, grid_size_, core_w, core_h, core_area.x_min, core_area.y_min)) {
        std::cerr << "Error: Failed to initialize density grid" << std::endl;
        return false;
    }
    
    // Initialize Poisson solver
    poisson_solver_ = std::make_unique<PoissonSolver>();
    
    // Collect all movable cells
    cells_ = placer_db_->getAllCells();
    cells_.erase(
        std::remove_if(cells_.begin(), cells_.end(), 
                      [this](const Cell* cell) { return placer_db_->getCellInfo(const_cast<Cell*>(cell)).fixed; }),
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
    
    debugLog("Starting global placement optimization...");
    
    // === Auto-Normalize Initial Lambda ===
    // Run one iteration to measure force magnitudes
    density_grid_->updateDensity(cells_, placer_db_);
    std::vector<Bin> density_input = density_grid_->getBins();
    poisson_solver_->solve(density_input, grid_size_, grid_size_);
    calculateWirelengthGradients();
    
    // Calculate average force magnitudes
    double sum_wire_force = 0.0;
    double sum_dens_force = 0.0;
    int active_cells = 0;
    
    for (int i = 0; i < cells_.size(); ++i) {
        if (placer_db_->getCellInfo(cells_[i]).fixed) continue;
        
        // Wire Force
        sum_wire_force += std::abs(wire_gradients_[i].x) + std::abs(wire_gradients_[i].y);
        
        // Density Force
        auto bin_idx = density_grid_->getBinIndex(cells_[i]->getX(), cells_[i]->getY());
        const Bin& bin = density_grid_->getBin(bin_idx.first, bin_idx.second);
        sum_dens_force += std::abs(bin.electro_force_x) + std::abs(bin.electro_force_y);
        active_cells++;
    }
    
    // [冷启动策略] Zero-Start Lambda: 极小值启动，慢爬升
    // 这能防止"炸膛"，让单元先理顺拓扑关系，再慢慢膨胀
    if (sum_dens_force > 1e-6 && active_cells > 0) {
        // 1. 冷启动：几乎为 0 的 Lambda，优先优化线长
        current_lambda_ = 1e-6;  // 几乎为 0，让线长力主导
        
        // 2. 设定 Lambda 上限，防止后期斥力无限大
        double ratio = (sum_wire_force / sum_dens_force);
        double max_lambda = 0.5 * (sum_wire_force / sum_dens_force);
        
        // 3. 保存上限值供后续增长使用
        max_lambda_ = max_lambda;
        
        debugLog(">>> COLD-START Lambda Strategy:");
        debugLog("    Initial Lambda: " + std::to_string(current_lambda_) + " (near-zero)");
        debugLog("    Max Lambda: " + std::to_string(max_lambda_) + " (safety limit)");
        debugLog("    Force ratio: " + std::to_string(ratio));
        debugLog("    Expected: Phase 1 (wire optimization) -> Phase 2 (gentle inflation)");
    } else {
        debugLog(">>> Using fallback Initial Lambda: " + std::to_string(current_lambda_));
    }
    
    // === Force Magnitude Balance Diagnosis ===
    if (active_cells > 0) {
        double avg_wire_force = sum_wire_force / active_cells;
        double avg_dens_force = sum_dens_force / active_cells;
        double ratio = avg_dens_force > 1e-9 ? avg_wire_force / avg_dens_force : 0.0;
        
        debugLog("Force Balance Analysis (GAS DIFFUSION MODE):");
        debugLog("  Average Wire Force: " + std::to_string(avg_wire_force));
        debugLog("  Average Density Force: " + std::to_string(avg_dens_force));
        debugLog("  Force Ratio (Wire/Density): " + std::to_string(ratio));
        
        if (ratio > 2.0) {
            debugLog("  ⚠️  WARNING: Wire Force dominates! Cells may cluster excessively.");
        } else if (ratio < 0.5) {
            debugLog("  ⚠️  WARNING: Density Force dominates! Cells may explode to corners.");
        } else {
            debugLog("  ✅ PERFECT: Forces are balanced - expect gentle gas diffusion.");
        }
        debugLog("  🎯 EXPECTED: Gentle spreading like ink in water, uniform density distribution.");
    }
    
    debugLog("----------------------------");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < max_iterations_; ++iter) {
        auto iter_start = std::chrono::high_resolution_clock::now();
        
        // [IO 权重调度] 计算进度 (0.0 ~ 1.0)
        double progress = (double)iter / max_iterations_;
        
        // Step 1: Update density distribution
        density_grid_->updateDensity(cells_, placer_db_);
        
        // [单向阀修复] 准备"Overflow Only"密度分布
        // 计算目标密度（利用率 + 余量）
        const Rect& core_area = placer_db_->getCoreArea();
        double total_cell_area = 0.0;
        for (const auto& cell : cells_) {
            const auto& info = placer_db_->getCellInfo(cell);
            total_cell_area += info.width * info.height;
        }
        double total_core_area = core_area.width() * core_area.height();
        double utilization = total_cell_area / total_core_area;
        double target_density = utilization + 0.1;  // 稍微给点余量
        if (target_density > 1.0) target_density = 1.0;
        
        debugLog(">>> OVERFLOW-ONLY MODE: target_density=" + std::to_string(target_density) + 
                  ", utilization=" + std::to_string(utilization));
        
        // 准备泊松求解器的输入：只处理超标密度
        density_input = density_grid_->getBins();  // 拷贝一份
        for (auto& bin : density_input) {
            // [关键修复] 只关注"超标"部分，空旷区域不产生吸力
            double rho = bin.density;
            double overflow = rho - target_density;
            if (overflow < 0) overflow = 0;  // 截断负值，消除"黑洞"
            bin.density = overflow;  // 只传递正电荷到泊松求解器
        }
        
        // Step 2: Solve Poisson equation for electrostatic forces
        bool solve_success = poisson_solver_->solve(density_input, grid_size_, grid_size_);
        
        if (!solve_success) {
            std::cerr << "Warning: Poisson solver failed at iteration " << iter << std::endl;
            continue;
        }
        
        // Step 3: Calculate wirelength gradients with IO weighting
        calculateWirelengthGradients(progress);
        
        // Step 4: Nesterov update
        double total_movement = nesterovUpdate(iter);
        
        // Step 5: Update lambda (increase density penalty)
        updateLambda();
        
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
    
    // [IO 权重调度] 动态计算 IO 权重
    // 初期 (progress < 0.3): 权重极低 (0.1)，几乎忽略 IO
    // 中期 (0.3 <= progress <= 0.6): 线性过渡 0.1 -> 1.0
    // 后期 (progress > 0.6): 权重恢复 (1.0)
    double io_weight = 0.1;  
    
    if (progress_ratio > 0.6) {
        io_weight = 1.0;
    } else if (progress_ratio > 0.3) {
        // 线性过渡: 0.1 -> 1.0
        io_weight = 0.1 + (progress_ratio - 0.3) * 3.0;  
    }
    
    debugLog("IO Weight Schedule: progress=" + std::to_string(progress_ratio) + 
              ", io_weight=" + std::to_string(io_weight));
    
    // Real wirelength model: Star Model HPWL gradient with IO weighting
    // Each net pulls its cells toward the net's center of gravity
    for (const auto& net_ptr : netlist_db_->getNets()) {
        const Net* net = net_ptr.get();
        if (!net) continue;
        
        // Skip single-pin nets (no wirelength contribution)
        const auto& loads = net->getLoads();
        Pin* driver = net->getDriver();
        if (loads.empty() && !driver) continue;
        
        // 1. 检查该 Net 是否连接到 Fixed Pin (IO Pin)
        bool has_fixed_pin = false;
        if (driver && placer_db_->getCellInfo(driver->getOwner()).fixed) {
            has_fixed_pin = true;
        }
        for (Pin* load : loads) {
            if (load && placer_db_->getCellInfo(load->getOwner()).fixed) {
                has_fixed_pin = true;
                break;
            }
        }
        
        // Calculate net center of gravity
        Point net_cog = getNetCenterOfGravity(net);
        
        // 2. 计算基础权重
        double base_weight = getNetWeight(net);
        
        // 3. 应用 IO 降权
        // 如果连了 IO，就乘以 io_weight；如果是纯内部连线，权重保持 1.0
        double final_weight = has_fixed_pin ? (base_weight * io_weight) : base_weight;
        
        // Apply gradient to driver pin
        if (driver) {
            Cell* driver_cell = driver->getOwner();
            if (driver_cell && !placer_db_->getCellInfo(driver_cell).fixed) {
                // Find cell index
                auto it = std::find(cells_.begin(), cells_.end(), driver_cell);
                if (it != cells_.end()) {
                    size_t idx = it - cells_.begin();
                    Point driver_pos{driver_cell->getX(), driver_cell->getY()};
                    Point to_cog = net_cog - driver_pos;
                    wire_gradients_[idx].x += to_cog.x * final_weight;
                    wire_gradients_[idx].y += to_cog.y * final_weight;
                }
            }
        }
        
        // Apply gradient to load pins
        for (Pin* load : loads) {
            if (!load) continue;
            Cell* load_cell = load->getOwner();
            if (load_cell && !placer_db_->getCellInfo(load_cell).fixed) {
                // Find cell index
                auto it = std::find(cells_.begin(), cells_.end(), load_cell);
                if (it != cells_.end()) {
                    size_t idx = it - cells_.begin();
                    Point load_pos{load_cell->getX(), load_cell->getY()};
                    Point to_cog = net_cog - load_pos;
                    wire_gradients_[idx].x += to_cog.x * final_weight;
                    wire_gradients_[idx].y += to_cog.y * final_weight;
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
    
    // Density gradient = negative of electrostatic force
    // (Poisson solver outputs force, gradient is opposite direction)
    return Point{-bin.electro_force_x, -bin.electro_force_y};
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
// Nesterov Accelerated Gradient Optimization
// ============================================================================

double GlobalPlacer::nesterovUpdate(int iteration) {
    double total_movement = 0.0;
    
    // Calculate total gradients
    calculateTotalGradients();
    
    for (size_t i = 0; i < cells_.size(); ++i) {
        Cell* cell = cells_[i];
        if (placer_db_->getCellInfo(cell).fixed) continue;
        
        // Get density gradient
        Point density_grad = getDensityGradient(cell);
        
        // Total gradient = wire_gradient + lambda * density_gradient
        double total_grad_x = wire_gradients_[i].x + current_lambda_ * density_grad.x;
        double total_grad_y = wire_gradients_[i].y + current_lambda_ * density_grad.y;
        
        // [工业级稳定器] 梯度裁剪 (Gradient Clipping)
        // 防止中心区域的超大斥力把单元炸飞
        const Rect& core_area = placer_db_->getCoreArea();
        double max_force = core_area.width() * 0.01;  // 不超过核心宽度的1%（更保守）
        
        // 简单的裁剪逻辑
        if (total_grad_x > max_force) total_grad_x = max_force;
        if (total_grad_x < -max_force) total_grad_x = -max_force;
        if (total_grad_y > max_force) total_grad_y = max_force;
        if (total_grad_y < -max_force) total_grad_y = -max_force;
        
        // Nesterov update: v_{k+1} = momentum * v_k - learning_rate * gradient
        velocities_[i].x = momentum_ * velocities_[i].x - learning_rate_ * total_grad_x;
        velocities_[i].y = momentum_ * velocities_[i].y - learning_rate_ * total_grad_y;
        
        // [安全阀 #1] 位移限制 (Displacement Clipping) - 防止"炸膛"
        double dx = velocities_[i].x;
        double dy = velocities_[i].y;
        double dist = std::sqrt(dx*dx + dy*dy);
        
        // 设定上限：芯片宽度的 2%（对小电路更保守）
        double max_dist = core_area.width() * 0.02;
        
        // 截断移动距离和速度
        if (dist > max_dist) {
            double scale = max_dist / dist;
            dx *= scale;
            dy *= scale;
            velocities_[i].x *= scale;  // 防止动量积累过大
            velocities_[i].y *= scale;
        }
        
        // Update position: x_{k+1} = x_k + v_{k+1}
        double old_x = cell->getX();
        double old_y = cell->getY();
        
        double new_x = old_x + dx;
        double new_y = old_y + dy;
        
        cell->setPosition(new_x, new_y);
        
        // Also update PlacerDB to maintain consistency with visualization
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
        if (placer_db_->getCellInfo(cell).fixed) continue;
        
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
        
        cell->setPosition(x, y);
        
        // Also update PlacerDB to maintain consistency with visualization
        placer_db_->placeCell(cell, x, y);
    }
}

// ============================================================================
// Dynamic Lambda Adjustment
// ============================================================================

void GlobalPlacer::updateLambda() {
    // [慢爬升策略] Lambda 慢速增长，防止"炸膛"
    // 每次增长 5%，给线长力足够时间理顺拓扑关系
    double lambda_growth = 1.05;  // 慢速增长 5%
    
    if (current_lambda_ < max_lambda_) {
        current_lambda_ *= lambda_growth;
        // 确保不超过上限
        current_lambda_ = std::min(current_lambda_, max_lambda_);
    }
}

// ============================================================================
// Utility Functions
// ============================================================================

double GlobalPlacer::calculateHPWL() const {
    // True net-based HPWL calculation
    double total_hpwl = 0.0;
    
    for (const auto& net_ptr : netlist_db_->getNets()) {
        const Net* net = net_ptr.get();
        if (!net) continue;
        
        // Skip single-pin nets (no wirelength contribution)
        const auto& loads = net->getLoads();
        Pin* driver = net->getDriver();
        if (loads.empty() && !driver) continue;
        
        // Initialize bounding box to opposite extremes
        double net_min_x = std::numeric_limits<double>::max();
        double net_max_x = std::numeric_limits<double>::lowest();
        double net_min_y = std::numeric_limits<double>::max();
        double net_max_y = std::numeric_limits<double>::lowest();
        
        // Include driver pin position
        if (driver) {
            Cell* driver_cell = driver->getOwner();
            if (driver_cell) {
                const auto& cell_info = placer_db_->getCellInfo(const_cast<Cell*>(driver_cell));
                double pin_x = driver_cell->getX() + cell_info.width / 2.0;
                double pin_y = driver_cell->getY() + cell_info.height / 2.0;
                
                net_min_x = net_max_x = pin_x;
                net_min_y = net_max_y = pin_y;
            }
        }
        
        // Include load pin positions
        for (Pin* load : loads) {
            if (!load) continue;
            Cell* load_cell = load->getOwner();
            if (load_cell) {
                const auto& cell_info = placer_db_->getCellInfo(const_cast<Cell*>(load_cell));
                double pin_x = load_cell->getX() + cell_info.width / 2.0;
                double pin_y = load_cell->getY() + cell_info.height / 2.0;
                
                net_min_x = std::min(net_min_x, pin_x);
                net_max_x = std::max(net_max_x, pin_x);
                net_min_y = std::min(net_min_y, pin_y);
                net_max_y = std::max(net_max_y, pin_y);
            }
        }
        
        // Add HPWL for this net
        total_hpwl += (net_max_x - net_min_x) + (net_max_y - net_min_y);
    }
    
    return total_hpwl;
}

void GlobalPlacer::updateStatistics(int iteration, double total_movement, double step_time_ms) {
    OptimizationStats stats;
    stats.iteration = iteration;
    stats.total_movement = total_movement;
    stats.hpwl = calculateHPWL();
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
    
    // If visualizer is available, generate PNG image
    if (visualizer_) {
        std::string filename = "electro_iter_" + std::to_string(iteration);
        visualizer_->drawPlacementWithRunId(filename, run_id_);
        std::cout << "  Global placement iteration " << iteration << " PNG exported" << std::endl;
    } else {
        std::cout << "  Global placement iteration " << iteration << " CSV exported (no visualizer)" << std::endl;
    }
}

void GlobalPlacer::exportPlacement(const std::string& filename) const {
    // Create full path to visualization directory
    std::string run_dir = "visualizations/" + run_id_;
    std::string mkdir_cmd = "mkdir -p " + run_dir;
    std::system(mkdir_cmd.c_str());
    
    // Full path for CSV file
    std::string full_path = run_dir + "/" + filename;
    std::ofstream file(full_path);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << full_path << " for writing" << std::endl;
        return;
    }
    
    file << "cell_name,x,y,width,height,fixed\n";
    
    for (const auto& cell_ptr : netlist_db_->getCells()) {
        const Cell* cell = cell_ptr.get();
        if (!cell) continue;
        
        const auto& cell_info = placer_db_->getCellInfo(const_cast<Cell*>(cell));
        
        file << cell->getName() << ","
             << cell->getX() << ","
             << cell->getY() << ","
             << cell_info.width << ","
             << cell_info.height << ","
             << (cell_info.fixed ? "1" : "0") << "\n";
    }
    
    file.close();
    
    if (verbose_) {
        std::cout << "[GlobalPlacer] Placement exported to " << filename << std::endl;
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

void GlobalPlacer::setAggressiveParameters() {
    // === 温和参数调整 (气体扩散模式) ===
    learning_rate_ = 0.05;        // 降低步长：适应修复后的力计算
    initial_lambda_ = 0.01;       // 保持适中：让自动调优起作用
    lambda_growth_rate_ = 1.02;    // 温和增长：避免过度补偿
    momentum_ = 0.9;              // 标准动量：平滑收敛
    convergence_threshold_ = 0.001; // 精确收敛：追求高质量布局
    
    debugLog("Applied gentle parameters for gas diffusion mode:");
    debugLog("  Learning Rate: " + std::to_string(learning_rate_));
    debugLog("  Initial Lambda: " + std::to_string(initial_lambda_));
    debugLog("  Lambda Growth Rate: " + std::to_string(lambda_growth_rate_));
    debugLog("  Momentum: " + std::to_string(momentum_));
    debugLog("  Convergence Threshold: " + std::to_string(convergence_threshold_));
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
    std::string python_cmd = "python3 visualize_density.py " + csv_filename + " " + png_filename + " 2>/dev/null";
    
    int result = std::system(python_cmd.c_str());
    if (result == 0) {
        debugLog("Density heatmap generated: " + png_filename);
    } else {
        debugLog("Warning: Failed to generate density heatmap (python3 or matplotlib may not be available)");
    }
}



} // namespace mini