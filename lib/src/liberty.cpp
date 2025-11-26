/**
 * @file liberty.cpp
 * @brief Liberty Library Implementation
 * @details Implementation of NLDM lookup table interpolation and library management
 */

#include "../include/liberty.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace mini {

/**
 * @brief Find insertion index for binary search
 * @param array Sorted array of values
 * @param value Value to find
 * @return Pair of (lower_index, fraction)
 * @details Uses binary search to find where value fits in the array
 */
static std::pair<size_t, double> findInsertionPoint(
    const std::vector<double>& array, double value) {

    if (array.empty()) {
        throw std::runtime_error("Empty array in lookup");
    }

    // Handle boundary conditions
    if (value <= array.front()) {
        return {0, 0.0};
    }
    if (value >= array.back()) {
        return {array.size() - 2, 1.0};
    }

    // Binary search
    size_t left = 0;
    size_t right = array.size() - 1;

    while (right - left > 1) {
        size_t mid = left + (right - left) / 2;
        if (value < array[mid]) {
            right = mid;
        } else {
            left = mid;
        }
    }

    // Calculate interpolation fraction
    double range = array[right] - array[left];
    double fraction = (range > 0.0) ? (value - array[left]) / range : 0.0;

    return {left, fraction};
}

/**
 * @brief Perform bilinear interpolation lookup
 * @param x First dimension coordinate (e.g., input slew)
 * @param y Second dimension coordinate (e.g., output capacitance)
 * @return Interpolated value
 */
double LookupTable::lookup(double x, double y) const {
    // Validate table
    if (!isValid()) {
        throw std::runtime_error("Lookup table is not valid");
    }

    if (index_1.size() < 2 || index_2.size() < 2) {
        throw std::runtime_error("Lookup table must have at least 2x2 dimensions");
    }

    // Check dimensions match
    if (values.size() != index_1.size() ||
        values[0].size() != index_2.size()) {
        throw std::runtime_error("Lookup table dimensions mismatch");
    }

    // Find insertion points
    auto [i1, frac1] = findInsertionPoint(index_1, x);
    auto [i2, frac2] = findInsertionPoint(index_2, y);

    // Ensure indices are within bounds
    if (i1 >= values.size() - 1 || i2 >= values[0].size() - 1) {
        // Return nearest boundary value
        size_t i1_clamp = std::min(i1, values.size() - 1);
        size_t i2_clamp = std::min(i2, values[0].size() - 1);
        return values[i1_clamp][i2_clamp];
    }

    // Get four corner values
    double f00 = values[i1][i2];           // Lower-left
    double f10 = values[i1 + 1][i2];       // Lower-right
    double f01 = values[i1][i2 + 1];       // Upper-left
    double f11 = values[i1 + 1][i2 + 1];   // Upper-right

    // Bilinear interpolation
    // First interpolate in x direction
    double fx0 = f00 + (f10 - f00) * frac1;  // At y lower bound
    double fx1 = f01 + (f11 - f01) * frac1;  // At y upper bound

    // Then interpolate in y direction
    double result = fx0 + (fx1 - fx0) * frac2;

    return result;
}

/**
 * @brief Get all cell names
 * @return Vector of cell names
 */
std::vector<std::string> Library::getCellNames() const {
    std::vector<std::string> names;
    names.reserve(cells_.size());
    for (const auto& pair : cells_) {
        names.push_back(pair.first);
    }
    return names;
}

} // namespace mini
