/**
 * @file legalizer.h
 * @brief Abstract Base Class for Legalization Algorithms
 * @details Strategy Pattern interface for different legalization approaches
 */

#ifndef MINI_LEGALIZER_H
#define MINI_LEGALIZER_H

#include "../../lib/include/placer_db.h"
#include <iostream>

namespace mini {

/**
 * @enum LegalizationAlgorithm
 * @brief Available legalization algorithms
 */
enum class LegalizationAlgorithm {
    GREEDY_TETRIS,  ///< Traditional greedy packing (fast, lower quality)
    ABACUS          ///< Optimization-based clustering (slower, higher quality)
};

/**
 * @class Legalizer
 * @brief Abstract base class for legalization algorithms
 * @details Provides common interface for all legalization strategies
 */
class Legalizer {
public:
    /**
     * @brief Constructor
     * @param db Pointer to the placement database
     */
    explicit Legalizer(PlacerDB* db) : db_(db) {}

    /**
     * @brief Virtual destructor
     */
    virtual ~Legalizer() = default;

    /**
     * @brief Main legalization interface
     * @details Pure virtual function - must be implemented by derived classes
     */
    virtual void run() = 0;

    /**
     * @brief Set verbose mode for debugging
     * @param verbose Whether to enable verbose output
     */
    virtual void setVerbose(bool verbose) { verbose_ = verbose; }

    /**
     * @brief Get algorithm name for logging
     * @return String representation of the algorithm
     */
    virtual std::string getAlgorithmName() const = 0;

protected:
    PlacerDB* db_;        ///< Placement database
    bool verbose_ = false; ///< Verbose output flag

    /**
     * @brief Debug logging function
     * @param message Message to log
     */
    void debugLog(const std::string& message) const {
        if (verbose_) {
            std::cout << "[" << getAlgorithmName() << "] " << message << std::endl;
        }
    }

    /**
     * @brief Calculate HPWL after legalization
     * @return Total HPWL value
     */
    double calculateHPWL() const;

    /**
     * @brief Check for overlaps after legalization
     * @return True if overlaps exist
     */
    bool hasOverlaps() const;

    /**
     * @brief Export legalization result for visualization
     * @param filename Output filename
     */
    void exportResult(const std::string& filename) const;
};

} // namespace mini

#endif // MINI_LEGALIZER_H