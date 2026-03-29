/**
 * @file abacus_legalizer.h
 * @brief Abacus Legalization Algorithm for MiniPlacement
 * @details Row-based legalization with cluster merging to preserve relative order
 */

#ifndef MINI_ABACUS_LEGALIZER_H
#define MINI_ABACUS_LEGALIZER_H

#include "legalizer.h"
#include <vector>

namespace mini {

/**
 * @struct AbacusCluster
 * @brief Cluster representation for Abacus algorithm
 * @details A cluster contains one or more cells that are treated as a unit
 */
struct AbacusCluster {
    std::vector<Cell*> cells;    ///< Cells in this cluster
    double x;                    ///< Cluster's current starting position
    double width;                ///< Cluster's total width
    double weight;               ///< Cluster's total weight (e)
    double q_score;              ///< q: Ideal position moment for quadratic optimization
    
    /**
     * @brief Constructor
     */
    AbacusCluster() : x(0.0), width(0.0), weight(0.0), q_score(0.0) {}
    
    /**
     * @brief Initialize cluster with a single cell
     * @param cell Pointer to the cell
     * @param cell_info Cell information from PlacerDB
     */
    void initialize(Cell* cell, const PlacerDB::CellInfo& cell_info);
    
    /**
     * @brief Merge another cluster into this one
     * @param other The cluster to merge (must be to the right)
     */
    void addCluster(const AbacusCluster& other);
    
    /**
     * @brief Get cluster's optimal position
     * @return Optimal x coordinate that minimizes total movement (x = q/e)
     */
    double getOptimalPosition() const {
        return (weight > 0.0) ? (q_score / weight) : x;
    }
    
    /**
     * @brief Update position to optimal position
     */
    void updateToOptimalPosition() {
        x = getOptimalPosition();
    }
};

/**
 * @struct AbacusRow
 * @brief Row representation for Abacus algorithm
 */
struct AbacusRow {
    int index;                           ///< Row index
    double y_coordinate;                 ///< Y coordinate of this row
    double min_x;                        ///< Minimum X coordinate (core left boundary)
    double max_x;                        ///< Maximum X coordinate (core right boundary)
    std::vector<Cell*> cells;            ///< Cells assigned to this row (not yet legalized)
    std::vector<AbacusCluster> clusters; ///< Clusters formed in this row
    
    /**
     * @brief Constructor
     */
    AbacusRow() : index(0), y_coordinate(0.0), min_x(0.0), max_x(0.0) {}
};

/**
 * @class AbacusLegalizer
 * @brief Abacus legalization algorithm implementation
 * @details Preserves relative ordering while resolving overlaps through cluster merging
 */
class AbacusLegalizer : public Legalizer {
public:
    /**
     * @brief Constructor
     * @param db Pointer to the placement database
     */
    using Legalizer::Legalizer; // Inherit constructor
    
    /**
     * @brief Main entry point for legalization
     */
    void run() override;
    
    /**
     * @brief Get algorithm name
     * @return Algorithm name string
     */
    std::string getAlgorithmName() const override { return "Abacus"; }

private:
    std::vector<AbacusRow> rows_;        ///< All placement rows

    // ============================================================================
    // Phase 1: Cell Distribution to Rows (Projection)
    // ============================================================================

    /**
     * @brief Distribute cells to their nearest rows
     * @details Projects cells from 2D to 1D row-based problem
     */
    void distributeCellsToRows();
    
    /**
     * @brief Initialize row information based on core area
     */
    void initializeRows();
    
    /**
     * @brief Snap cells to row Y coordinates
     * @param cell Cell to snap
     * @param row_idx Target row index
     */
    void snapCellToRow(Cell* cell, int row_idx);
    
    // ============================================================================
    // Phase 2: Row-based Legalization (Core Abacus Algorithm)
    // ============================================================================
    
    /**
     * @brief Legalize cells within a single row
     * @param row Reference to the row to legalize
     */
    void legalizeRow(AbacusRow& row);
    
    // ============================================================================
    // Phase 3: Site Alignment (Snap to Grid)
    // ============================================================================
    
    /**
     * @brief Snap cells to site grid (left-alignment scan method)
     * @details Aligns cell left edges to site boundaries while preventing overlaps
     */
    void snapToSiteGrid();
};

} // namespace mini

#endif // MINI_ABACUS_LEGALIZER_H
