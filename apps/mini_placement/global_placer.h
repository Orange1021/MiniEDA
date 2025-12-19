/**
 * @file global_placer.h
 * @brief Global Placer with Nesterov Accelerated Gradient Optimization
 * @details Integrates DensityGrid, PoissonSolver, and Wirelength Forces
 *          for electrostatic placement using modern optimization techniques
 */

#ifndef MINI_GLOBAL_PLACER_H
#define MINI_GLOBAL_PLACER_H

#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <iostream>
#include "../../lib/include/placer_db.h"
#include "../../lib/include/netlist_db.h"
#include "../../lib/include/geometry.h"
#include "density_grid.h"
#include "poisson_solver.h"

namespace mini {

/**
 * @struct OptimizationStats
 * @brief Statistics for each optimization iteration
 */
struct OptimizationStats {
    int iteration;
    double total_movement;
    double hpwl;
    double max_density;
    double total_overflow;
    double lambda;
    double learning_rate;
    double step_time_ms;
};

/**
 * @class GlobalPlacer
 * @brief Global placement engine using Nesterov accelerated gradient
 * @details Combines wirelength forces and electrostatic density forces
 *          to optimize cell positions while maintaining connectivity
 */
class GlobalPlacer {
public:
    /**
     * @brief Constructor
     * @param placer_db Placement database (must be valid)
     * @param netlist_db Netlist database (must be valid)
     */
    GlobalPlacer(PlacerDB* placer_db, NetlistDB* netlist_db);

    /**
     * @brief Destructor
     */
    ~GlobalPlacer() = default;

    // Disable copy, enable move
    GlobalPlacer(const GlobalPlacer&) = delete;
    GlobalPlacer& operator=(const GlobalPlacer&) = delete;
    GlobalPlacer(GlobalPlacer&&) = default;
    GlobalPlacer& operator=(GlobalPlacer&&) = default;

    /**
     * @brief Initialize the global placer
     * @param grid_size Density grid size (must be power of 2)
     * @param target_density Target utilization (e.g., 0.7)
     * @return true if initialization successful
     */
    bool initialize(int grid_size = 64, double target_density = 0.7);

    /**
     * @brief Run the global placement optimization
     * @details Main optimization loop using Nesterov accelerated gradient
     */
    void runPlacement();

    /**
     * @brief Get optimization statistics for all iterations
     * @return Vector of iteration statistics
     */
    const std::vector<OptimizationStats>& getStatistics() const { return stats_; }

    /**
     * @brief Export placement results for visualization
     * @param filename Output filename
     */
    void exportPlacement(const std::string& filename) const;

    /**
     * @brief Print final placement statistics
     */
    void printFinalStatistics() const;

    // Configuration parameters
    void setMaxIterations(int max_iter) { max_iterations_ = max_iter; }
    void setInitialLambda(double lambda) { initial_lambda_ = lambda; }
    void setLambdaGrowthRate(double rate) { lambda_growth_rate_ = rate; }
    void setLearningRate(double lr) { learning_rate_ = lr; }
    void setMomentum(double momentum) { momentum_ = momentum; }
    void setConvergenceThreshold(double threshold) { convergence_threshold_ = threshold; }
    
    /**
     * @brief [New] Apply aggressive parameters for better convergence
     * @details Shock therapy: high learning rate, high lambda, high momentum
     */
    void setAggressiveParameters();
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // Visualization support
    void setVisualizer(class Visualizer* visualizer) { visualizer_ = visualizer; }
    void setRunId(const std::string& run_id) { run_id_ = run_id; }
    
    /**
     * @brief Set warm-up mode flag for conservative Lambda tuning
     * @param is_warmup Whether this is starting from warm-up results
     */
    void setWarmupMode(bool is_warmup) { is_warmup_mode_ = is_warmup; }

private:
    // Core components
    PlacerDB* placer_db_;                    ///< Placement database
    NetlistDB* netlist_db_;                  ///< Netlist database
    std::unique_ptr<DensityGrid> density_grid_; ///< Density grid
    std::unique_ptr<PoissonSolver> poisson_solver_; ///< Poisson equation solver
    
    // Visualization support
    class Visualizer* visualizer_ = nullptr;  ///< Visualizer for PNG generation
    std::string run_id_ = "global_placer";    ///< Run ID for output files

    // Optimization state
    std::vector<Cell*> cells_;               ///< All movable cells
    std::vector<Point> velocities_;          ///< Cell velocities for Nesterov
    std::vector<Point> wire_gradients_;      ///< Wirelength gradients
    std::vector<OptimizationStats> stats_;   ///< Optimization statistics

    // Configuration parameters
    int grid_size_ = 64;                     ///< Density grid size (power of 2)
    double target_density_ = 0.7;            ///< Target utilization
    int max_iterations_ = 500;               ///< Maximum optimization iterations
    double initial_lambda_ = 0.0001;         ///< Initial density penalty factor
    double lambda_growth_rate_ = 1.05;       ///< Lambda growth rate per iteration
    double learning_rate_ = 0.1;            ///< Learning rate (step size)
    double momentum_ = 0.9;                  ///< Nesterov momentum factor
    double convergence_threshold_ = 0.001;   ///< Convergence threshold (much smaller for better convergence)
    bool verbose_ = true;                    ///< Verbose output

    // Current optimization state
    double current_lambda_ = 0.0001;         ///< Current density penalty factor
    double current_hpwl_ = 0.0;              ///< Current HPWL
    bool initialized_ = false;               ///< Initialization flag
    bool is_warmup_mode_ = false;            ///< Warm-up mode flag for conservative tuning
    double max_lambda_ = 1.0;                ///< Maximum lambda limit for safety

    // ========================================================================
    // Core Optimization Algorithms
    // ========================================================================

    /**
     * @brief Calculate wirelength gradients using Star Model
     * @details Each net pulls its cells toward the center of gravity
     */
    void calculateWirelengthGradients();
    
    /**
     * @brief Calculate wirelength gradients with IO weighting schedule
     * @param progress_ratio Progress from 0.0 to 1.0 for IO weight adjustment
     * @details Early iterations ignore IO pins, later iterations gradually include them
     */
    void calculateWirelengthGradients(double progress_ratio);

    /**
     * @brief Get density gradient for a cell from Poisson solver
     * @param cell Cell pointer
     * @return Density gradient (negative of electrostatic force)
     */
    Point getDensityGradient(const Cell* cell) const;

    /**
     * @brief Combined gradient calculation
     * @details Total gradient = wire gradient + lambda * density gradient
     */
    void calculateTotalGradients();

    /**
     * @brief Nesterov accelerated gradient update
     * @param iteration Current iteration number
     * @return Total movement in this iteration
     */
    double nesterovUpdate(int iteration);

    /**
     * @brief Clamp cell positions to core area
     */
    void clampToCoreArea();

    /**
     * @brief Update lambda (density penalty factor)
     * @details Gradually increase density penalty to spread cells
     */
    void updateLambda();

    // ========================================================================
    // Utility Functions
    // ========================================================================

    /**
     * @brief Calculate current HPWL
     * @return Half-perimeter wire length
     */
    double calculateHPWL() const;

    /**
     * @brief Get net center of gravity
     * @param net Net pointer
     * @return Center point of all pins in the net
     */
    Point getNetCenterOfGravity(const Net* net) const;

    /**
     * @brief Map cell position to grid coordinates
     * @param cell Cell pointer
     * @return Grid coordinate pair
     */
    std::pair<int, int> getCellGridPosition(const Cell* cell) const;

    /**
     * @brief Update optimization statistics
     * @param iteration Current iteration
     * @param total_movement Total movement in this iteration
     * @param step_time_ms Time taken for this iteration
     */
    void updateStatistics(int iteration, double total_movement, double step_time_ms);

    /**
     * @brief Check convergence criteria
     * @param total_movement Movement in current iteration
     * @return true if converged
     */
    bool checkConvergence(double total_movement) const;

    /**
     * @brief Export current placement state as image
     * @param iteration Current iteration number
     */
    void exportPlacementImage(int iteration) const;

    /**
     * @brief Export density map and generate heatmap visualization
     * @details Exports density grid to CSV and calls Python script to generate heatmap
     */
    void exportDensityVisualization() const;

    /**
     * @brief Debug log helper
     * @param message Debug message
     */
    void debugLog(const std::string& message) const;

    /**
     * @brief Validate initialization
     * @return true if properly initialized
     */
    bool validateInitialization() const;

    /**
     * @brief Initialize cell velocities and gradients
     */
    void initializeOptimizationState();

    /**
     * @brief Calculate weight for net based on fanout
     * @param net Net pointer
     * @return Weight factor (1 / (fanout - 1))
     */
    double getNetWeight(const Net* net) const;
};

} // namespace mini

#endif // MINI_GLOBAL_PLACER_H