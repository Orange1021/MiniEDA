/**
 * @file greedy_legalizer.h
 * @brief Greedy (Tetris-like) Legalization Algorithm
 * @details Fast but lower quality legalization using linear packing
 */

#ifndef MINI_GREEDY_LEGALIZER_H
#define MINI_GREEDY_LEGALIZER_H

#include "legalizer.h"

namespace mini {

/**
 * @class GreedyLegalizer
 * @brief Greedy/Tetris-like legalization implementation
 * @details Simple linear packing algorithm: fill rows left-to-right, top-to-bottom
 */
class GreedyLegalizer : public Legalizer {
public:
    /**
     * @brief Constructor
     * @param db Pointer to the placement database
     */
    using Legalizer::Legalizer; // Inherit constructor

    /**
     * @brief Run greedy legalization
     */
    void run() override;

    /**
     * @brief Get algorithm name
     * @return Algorithm name string
     */
    std::string getAlgorithmName() const override { return "GreedyTetris"; }

private:
    /**
     * @brief Collect all movable cells
     * @return Vector of movable cell pointers
     */
    std::vector<Cell*> collectMovableCells() const;

    /**
     * @brief Sort cells by Y then X coordinates
     * @param cells Vector of cells to sort (modified in-place)
     */
    void sortCells(std::vector<Cell*>& cells) const;

    /**
     * @brief Perform linear packing placement
     * @param cells Sorted cells to place
     * @return Number of successfully placed cells
     */
    int performLinearPacking(const std::vector<Cell*>& cells);

    /**
     * @brief Print placement statistics
     * @param cells_placed Number of cells placed
     * @param rows_used Number of rows used
     * @param max_rows Maximum available rows
     */
    void printStatistics(int cells_placed, int rows_used, int max_rows) const;
};

} // namespace mini

#endif // MINI_GREEDY_LEGALIZER_H