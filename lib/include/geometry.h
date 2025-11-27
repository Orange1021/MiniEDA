/**
 * @file geometry.h
 * @brief Core Geometry Library for MiniPlacement
 * @details Lightweight, high-performance geometric primitives
 */

#ifndef MINI_GEOMETRY_H
#define MINI_GEOMETRY_H

#include <cmath>
#include <algorithm>

namespace mini {

// ============================================================================
// Point Class
// ============================================================================

/**
 * @struct Point
 * @brief 2D Point with vector operations support
 */
struct Point {
    double x, y;

    // Constructors
    Point() : x(0.0), y(0.0) {}
    Point(double x_val, double y_val) : x(x_val), y(y_val) {}

    // Vector operations
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }

    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }

    Point& operator+=(const Point& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point& operator-=(const Point& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Point operator*(double scalar) const {
        return Point(x * scalar, y * scalar);
    }

    Point operator/(double scalar) const {
        return Point(x / scalar, y / scalar);
    }

    Point& operator*=(double scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Point& operator/=(double scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Distance calculations
    inline double manhattanDistance(const Point& other) const {
        return std::abs(x - other.x) + std::abs(y - other.y);
    }

    inline double euclideanDistance(const Point& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

// ============================================================================
// Rect Class
// ============================================================================

/**
 * @struct Rect
 * @brief 2D Rectangle with geometric operations
 */
struct Rect {
    double x_min, y_min, x_max, y_max;

    // Constructor
    Rect(double xmin, double ymin, double xmax, double ymax) 
        : x_min(xmin), y_min(ymin), x_max(xmax), y_max(ymax) {
        // Ensure proper ordering
        if (x_min > x_max) std::swap(x_min, x_max);
        if (y_min > y_max) std::swap(y_min, y_max);
    }

    // Accessors
    inline double width() const {
        return x_max - x_min;
    }

    inline double height() const {
        return y_max - y_min;
    }

    inline Point center() const {
        return Point((x_min + x_max) / 2.0, (y_min + y_max) / 2.0);
    }

    // Geometric tests
    inline bool contains(const Point& point) const {
        return point.x >= x_min && point.x <= x_max &&
               point.y >= y_min && point.y <= y_max;
    }

    inline bool overlaps(const Rect& other) const {
        return !(x_max < other.x_min || other.x_max < x_min ||
                 y_max < other.y_min || other.y_max < y_min);
    }

    // Bounding box union
    inline void unionWith(const Rect& other) {
        x_min = std::min(x_min, other.x_min);
        y_min = std::min(y_min, other.y_min);
        x_max = std::max(x_max, other.x_max);
        y_max = std::max(y_max, other.y_max);
    }

    // Move operations
    inline void translate(double dx, double dy) {
        x_min += dx;
        y_min += dy;
        x_max += dx;
        y_max += dy;
    }
};

} // namespace mini

#endif // MINI_GEOMETRY_H