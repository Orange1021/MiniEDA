/**
 * @file overlap_detector.cpp
 * @brief Overlap Detection Utility Implementation
 */

#include "overlap_detector.h"
#include "placer_db.h"
#include "../../lib/include/cell.h"
#include <algorithm>
#include <unordered_set>

namespace mini {

OverlapDetector::OverlapDetector(PlacerDB* db) : db_(db) {
    if (!db_) {
        throw std::invalid_argument("PlacerDB cannot be null");
    }
}

bool OverlapDetector::hasOverlaps() const {
    auto overlaps = getOverlapDetails();
    return !overlaps.empty();
}

double OverlapDetector::calculateTotalOverlap() const {
    double total_overlap = 0.0;
    auto all_cells = db_->getAllCells();
    
    for (size_t i = 0; i < all_cells.size(); ++i) {
        for (size_t j = i + 1; j < all_cells.size(); ++j) {
            total_overlap += calculateCellOverlap(all_cells[i], all_cells[j]);
        }
    }
    
    return total_overlap;
}

std::vector<OverlapInfo> OverlapDetector::getOverlapDetails() const {
    std::vector<OverlapInfo> overlaps;
    auto all_cells = db_->getAllCells();
    
    for (size_t i = 0; i < all_cells.size(); ++i) {
        for (size_t j = i + 1; j < all_cells.size(); ++j) {
            Cell* cell1 = all_cells[i];
            Cell* cell2 = all_cells[j];
            
            double overlap_area = calculateCellOverlap(cell1, cell2);
            
            // Only add if there's a real overlap (area > tolerance)
            if (overlap_area > EPSILON_) {
                const auto& info1 = db_->getCellInfo(cell1);
                const auto& info2 = db_->getCellInfo(cell2);
                
                overlaps.emplace_back(cell1, cell2, overlap_area,
                                     info1.width * info1.height,
                                     info2.width * info2.height);
            }
        }
    }
    
    return overlaps;
}

void OverlapDetector::printOverlapSummary(bool verbose) const {
    auto overlaps = getOverlapDetails();
    int boundary_touches = countBoundaryTouches();
    
    if (overlaps.empty() && boundary_touches == 0) {
        std::cout << "  No overlaps or boundary touches detected" << std::endl;
        return;
    }
    
    std::cout << "  Overlap Analysis Results:" << std::endl;
    std::cout << "    Total overlapping pairs: " << overlaps.size() << std::endl;
    std::cout << "    Boundary touching pairs: " << boundary_touches << std::endl;
    
    double total_area = 0.0;
    for (const auto& overlap : overlaps) {
        total_area += overlap.overlap_area;
    }
    std::cout << "    Total overlap area: " << std::fixed << std::setprecision(4) 
              << total_area << " μm²" << std::endl;
    
    if (verbose && !overlaps.empty()) {
        std::cout << "  Top 10 largest overlaps:" << std::endl;
        
        // Sort by overlap area (descending)
        auto sorted_overlaps = overlaps;
        std::sort(sorted_overlaps.begin(), sorted_overlaps.end(),
                 [](const OverlapInfo& a, const OverlapInfo& b) {
                     return a.overlap_area > b.overlap_area;
                 });
        
        int max_show = std::min(10, static_cast<int>(sorted_overlaps.size()));
        for (int i = 0; i < max_show; ++i) {
            const auto& overlap = sorted_overlaps[i];
            std::cout << "    " << (i+1) << ". " << overlap.cell1->getName() 
                      << " ↔ " << overlap.cell2->getName() << ": "
                      << std::fixed << std::setprecision(4) << overlap.overlap_area << " μm²"
                      << " (" << std::setprecision(1) 
                      << (overlap.overlap_area / overlap.cell1_area * 100.0) << "% of "
                      << overlap.cell1->getName() << ", "
                      << (overlap.overlap_area / overlap.cell2_area * 100.0) << "% of "
                      << overlap.cell2->getName() << ")" << std::endl;
        }
        
        // Statistics
        std::vector<double> areas;
        for (const auto& overlap : overlaps) {
            areas.push_back(overlap.overlap_area);
        }
        
        double avg_area = total_area / areas.size();
        double max_area = *std::max_element(areas.begin(), areas.end());
        double min_area = *std::min_element(areas.begin(), areas.end());
        
        std::cout << "  Overlap Statistics:" << std::endl;
        std::cout << "    Average overlap area: " << std::fixed << std::setprecision(4) 
                  << avg_area << " μm²" << std::endl;
        std::cout << "    Maximum overlap area: " << std::fixed << std::setprecision(4) 
                  << max_area << " μm²" << std::endl;
        std::cout << "    Minimum overlap area: " << std::fixed << std::setprecision(4) 
                  << min_area << " μm²" << std::endl;
        
        auto overlapping_cells = getOverlappingCells();
        std::cout << "    Overlapping cells: " << overlapping_cells.size() << std::endl;
    }
}

void OverlapDetector::generateReport(const std::string& title, bool verbose) const {
    std::cout << "\n=== " << title << " ===" << std::endl;
    
    auto all_cells = db_->getAllCells();
    std::cout << "Total cells analyzed: " << all_cells.size() << std::endl;
    
    printOverlapSummary(verbose);
    
    std::cout << "=== End " << title << " ===\n" << std::endl;
}

int OverlapDetector::countOverlaps() const {
    return static_cast<int>(getOverlapDetails().size());
}

std::vector<Cell*> OverlapDetector::getOverlappingCells() const {
    std::unordered_set<Cell*> overlapping_cells;
    auto overlaps = getOverlapDetails();
    
    for (const auto& overlap : overlaps) {
        overlapping_cells.insert(overlap.cell1);
        overlapping_cells.insert(overlap.cell2);
    }
    
    return std::vector<Cell*>(overlapping_cells.begin(), overlapping_cells.end());
}

int OverlapDetector::countBoundaryTouches() const {
    int boundary_touches = 0;
    auto all_cells = db_->getAllCells();
    
    for (size_t i = 0; i < all_cells.size(); ++i) {
        for (size_t j = i + 1; j < all_cells.size(); ++j) {
            Cell* cell1 = all_cells[i];
            Cell* cell2 = all_cells[j];
            
            const auto& info1 = db_->getCellInfo(cell1);
            const auto& info2 = db_->getCellInfo(cell2);
            
            // Check for boundary touching (no real overlap, but edges接触)
            double x1 = std::max(info1.x, info2.x);
            double y1 = std::max(info1.y, info2.y);
            double x2 = std::min(info1.x + info1.width, info2.x + info2.width);
            double y2 = std::min(info1.y + info1.height, info2.y + info2.height);
            
            // Calculate overlap dimensions
            double overlap_width = std::max(0.0, x2 - x1);
            double overlap_height = std::max(0.0, y2 - y1);
            double overlap_area = overlap_width * overlap_height;
            
            // Check if cells touch but don't have significant overlap
            bool x_touch = (std::abs(x2 - x1) < EPSILON_) && (overlap_height > EPSILON_);
            bool y_touch = (std::abs(y2 - y1) < EPSILON_) && (overlap_width > EPSILON_);
            
            if ((x_touch || y_touch) && overlap_area <= EPSILON_) {
                boundary_touches++;
            }
        }
    }
    
    return boundary_touches;
}

double OverlapDetector::calculateCellOverlap(Cell* cell1, Cell* cell2) const {
    const auto& info1 = db_->getCellInfo(cell1);
    const auto& info2 = db_->getCellInfo(cell2);
    
    // Calculate overlap rectangle
    double x1 = std::max(info1.x, info2.x);
    double y1 = std::max(info1.y, info2.y);
    double x2 = std::min(info1.x + info1.width, info2.x + info2.width);
    double y2 = std::min(info1.y + info1.height, info2.y + info2.height);
    
    // Calculate overlap area with proper floating-point tolerance
    // Only consider it a real overlap if area > tolerance (not just touching)
    double overlap_width = std::max(0.0, x2 - x1);
    double overlap_height = std::max(0.0, y2 - y1);
    double overlap_area = overlap_width * overlap_height;
    
    // Apply tolerance to avoid treating boundary touching as overlap
    return (overlap_area > EPSILON_) ? overlap_area : 0.0;
}

bool OverlapDetector::cellsOverlap(Cell* cell1, Cell* cell2) const {
    // Use the same logic as calculateCellOverlap for consistency
    return calculateCellOverlap(cell1, cell2) > EPSILON_;
}

} // namespace mini