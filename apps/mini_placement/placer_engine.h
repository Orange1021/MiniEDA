/**
 * @file placer_engine.h
 * @brief Placement Engine Framework for MiniPlacement
 * @details Core algorithms for global placement and legalization
 */

#ifndef MINI_PLACER_ENGINE_H
#define MINI_PLACER_ENGINE_H


#include "../../lib/include/placer_db.h"
#include "placement_interface.h"
#include "detailed_placer.h"

namespace mini {

// Forward declarations
class Visualizer;
class GlobalPlacer;
class AbacusLegalizer;

/**
 * @class PlacerEngine
 * @brief Main placement engine with HPWL calculation and optimization
 * @details Supports multiple placement algorithms via Strategy Pattern
 */
class PlacerEngine {
public:
    /**
     * @brief Constructor
     * @param db Pointer to placement database
     * @param target_density Target placement density
     * @param initial_lambda Initial lambda for electrostatic forces
     * @param lambda_growth_rate Lambda growth rate
     * @param learning_rate Learning rate for optimization
     * @param momentum Momentum factor
     * @param convergence_threshold Convergence threshold
     * @param density_margin Density margin for overflow handling
     * @param max_gradient_ratio Maximum gradient ratio for safety
     * @param max_displacement_ratio Maximum displacement ratio
     */
    PlacerEngine(PlacerDB* db, double target_density, double initial_lambda, 
                 double lambda_growth_rate, double learning_rate, 
                 double momentum, double convergence_threshold, 
                 double density_margin, double max_gradient_ratio, double max_displacement_ratio);
    
    /**
     * @brief Destructor
     */
    ~PlacerEngine();
    
    /**
     * @brief Set HPWL convergence ratio
     * @param ratio HPWL convergence ratio (e.g., 0.0001 for 0.01%)
     */
    void setHPWLConvergenceRatio(double ratio) { hpwl_convergence_ratio_ = ratio; }
    void setDetailedIterations(int iterations) { detailed_iterations_ = iterations; }

    /**
     * @brief Calculate Half-Perimeter Wire Length (HPWL)
     * @return Total HPWL for all nets
     * @details HPWL = Sum((max_x - min_x) + (max_y - min_y)) for all nets
     */
    double calculateHPWL() const;

    /**
     * @brief Run complete global placement (default: basic force-directed)
     * @details Force-directed algorithm with iterative optimization
     */
    void runGlobalPlacement();

    /**
     * @brief Run global placement with specified algorithm
     * @param algorithm Algorithm selection: "basic", "nesterov", "hybrid"
     * @details Strategy Pattern implementation for algorithm switching
     */
    void runGlobalPlacementWithAlgorithm(const std::string& algorithm);

    /**
     * @brief Run legalization to align cells to rows
     * @details Tetris-like algorithm to remove overlaps
     */
    void runLegalization();

    /**
     * @brief Run detailed placement for local optimization
     * @details Local cell swapping and fine-tuning
     */
    void runDetailedPlacement();



    /**
     * @brief Set run ID for file naming
     * @param run_id Unique identifier for this run
     */
    void setRunId(const std::string& run_id) { run_id_ = run_id; }

    /**
     * @brief Set legalization algorithm
     * @param algo Legalization algorithm selection
     */
    void setLegalizationAlgorithm(LegalizationAlgorithm algo) { leg_algo_ = algo; }

    /**
     * @brief Get current HPWL value
     * @return Current HPWL
     */
    double getCurrentHPWL() const { return current_hpwl_; }

private:
    PlacerDB* db_;  // Placement database
    double current_hpwl_;             // Current HPWL value
    double hpwl_convergence_ratio_;   ///< HPWL convergence ratio
    double density_margin_;           ///< Density margin for target density calculation
    double max_gradient_ratio_;       ///< Maximum gradient as ratio of core width
    double max_displacement_ratio_;   ///< Maximum displacement as ratio of core width
    int detailed_iterations_;         ///< Number of iterations for detailed placement
    double warmup_stop_ratio_;         ///< Stop warmup when HPWL drops to this ratio of initial value
    std::string run_id_;              // Run ID for file naming
    GlobalPlacer* global_placer_;     // Pointer to advanced global placer
    LegalizationAlgorithm leg_algo_;  // Legalization algorithm selection
    std::unique_ptr<DetailedPlacer> detailed_placer_;  // Detailed placement optimizer
    
    // Placement algorithm parameters
    double target_density_;           // Target utilization for global placement
    double initial_lambda_;           // Initial density penalty factor
    double lambda_growth_rate_;       // Lambda growth rate per iteration
    double learning_rate_;            // Learning rate (step size)
    double momentum_;                 // Momentum factor
    double convergence_threshold_;    // Convergence threshold

    /**
     * @brief Single iteration of force-directed algorithm
     * @param iter Current iteration number
     */
    void solveForceDirectedIteration(int iter);

    // ========================================================================
    // Strategy Pattern: Placement Algorithms
    // ========================================================================

    /**
     * @brief Strategy A: Basic Force-Directed Placement (Baseline)
     * @details Traditional spring-force model with quadratic wirelength
     */
    void runBasicStrategy();

    /**
     * @brief Strategy B: Electrostatic Field Placement (Nesterov)
     * @details Advanced electrostatic placement with density control
     */
    void runElectrostaticStrategy();

    /**
     * @brief Strategy C: Hybrid Cascade Placement (Best Practice)
     * @details Two-phase approach: warm-up with basic + refinement with electrostatic
     */
    void runHybridStrategy();

    /**
     * @brief Calculate net bounding box
     * @param net Pointer to the net
     * @param min_x Minimum X coordinate (output)
     * @param max_x Maximum X coordinate (output)
     * @param min_y Minimum Y coordinate (output)
     * @param max_y Maximum Y coordinate (output)
     */
    void calculateNetBoundingBox(const Net* net, 
                               double& min_x, double& max_x,
                               double& min_y, double& max_y) const;

    /**
     * @brief Check if placement has overlaps
     * @return True if there are overlapping cells
     */
    bool hasOverlaps() const;

    /**
     * @brief Calculate total overlap area
     * @return Total overlapping area
     */
    double calculateTotalOverlap() const;

    /**
     * @brief Export placement to CSV and generate visualization using Python script
     * @param filename Base filename (without extension)
     */
    void exportAndVisualize(const std::string& filename) const;
};

} // namespace mini

#endif // MINI_PLACER_ENGINE_H