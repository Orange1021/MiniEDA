/**
 * @file test_density_grid.cpp
 * @brief Test program for DensityGrid functionality
 */

#include "../apps/mini_placement/density_grid.h"
#include "../lib/include/placer_db.h"
#include "../lib/include/netlist_db.h"
#include <iostream>
#include <memory>
#include <iomanip>

using namespace mini;

int main() {
    std::cout << "=== Testing DensityGrid ===" << std::endl;
    
    // Create a simple test scenario
    double core_w = 100.0;
    double core_h = 80.0;
    double core_x_min = 0.0;
    double core_y_min = 0.0;
    double bin_dim = 10.0;  // 10x10 bins
    
    // Initialize density grid
    DensityGrid grid;
    bool success = grid.init(core_w, core_h, core_x_min, core_y_min, bin_dim);
    
    if (!success) {
        std::cerr << "Failed to initialize density grid" << std::endl;
        return 1;
    }
    
    std::cout << "Density grid initialized successfully:" << std::endl;
    std::cout << "  Grid size: " << grid.getNumBinsX() << "x" << grid.getNumBinsY() << std::endl;
    std::cout << "  Total bins: " << grid.getTotalBins() << std::endl;
    std::cout << "  Bin dimensions: " << grid.getBinWidth() << "x" << grid.getBinHeight() << std::endl;
    
    // Create a mock PlacerDB and cells for testing
    auto netlist_db = std::make_shared<NetlistDB>("test_design");
    auto placer_db = std::make_unique<PlacerDB>(netlist_db.get());
    
    // Set core area
    Rect core_area(core_x_min, core_y_min, core_w, core_h);
    placer_db->setCoreArea(core_area);
    
    // Create some test cells
    std::vector<Cell*> test_cells;
    
    // Cell 1: Large cell in the center (should create high density)
    Cell* cell1 = netlist_db->createCell("cell1", CellType::AND);
    cell1->setPosition(45.0, 35.0);  // Center position
    placer_db->addCell(cell1, 10.0, 8.0);  // 10x8 size
    test_cells.push_back(cell1);
    
    // Cell 2: Medium cell
    Cell* cell2 = netlist_db->createCell("cell2", CellType::OR);
    cell2->setPosition(50.0, 40.0);
    placer_db->addCell(cell2, 6.0, 5.0);
    test_cells.push_back(cell2);
    
    // Cell 3: Small cell in corner (low density area)
    Cell* cell3 = netlist_db->createCell("cell3", CellType::NOT);
    cell3->setPosition(5.0, 5.0);
    placer_db->addCell(cell3, 3.0, 3.0);
    test_cells.push_back(cell3);
    
    std::cout << "\nCreated " << test_cells.size() << " test cells" << std::endl;
    
    // Update density
    grid.updateDensity(test_cells, placer_db.get());
    
    // Print statistics
    grid.printDensityStats(0.7);  // 70% target utilization
    
    // Check specific bins
    std::cout << "\n=== Specific Bin Analysis ===" << std::endl;
    
    // Center bin (should have high density)
    auto [center_x, center_y] = grid.getBinIndex(50.0, 40.0);
    const Bin& center_bin = grid.getBin(center_x, center_y);
    std::cout << "Center bin (" << center_x << "," << center_y << "):" << std::endl;
    std::cout << "  Position: (" << center_bin.x << "," << center_bin.y << ")" << std::endl;
    std::cout << "  Density: " << std::fixed << std::setprecision(3) << center_bin.density << std::endl;
    std::cout << "  Overcrowded: " << (center_bin.isOvercrowded(0.7) ? "YES" : "NO") << std::endl;
    
    // Corner bin (should have low density)
    auto [corner_x, corner_y] = grid.getBinIndex(5.0, 5.0);
    const Bin& corner_bin = grid.getBin(corner_x, corner_y);
    std::cout << "Corner bin (" << corner_x << "," << corner_y << "):" << std::endl;
    std::cout << "  Position: (" << corner_bin.x << "," << corner_bin.y << ")" << std::endl;
    std::cout << "  Density: " << std::fixed << std::setprecision(3) << corner_bin.density << std::endl;
    std::cout << "  Overcrowded: " << (corner_bin.isOvercrowded(0.7) ? "YES" : "NO") << std::endl;
    
    // Export density map for visualization
    grid.exportDensityMap("test_density_map.csv");
    std::cout << "\nDensity map exported to test_density_map.csv" << std::endl;
    
    // Test edge cases
    std::cout << "\n=== Edge Case Testing ===" << std::endl;
    
    // Test out-of-bounds coordinates
    auto [oot_x, oot_y] = grid.getBinIndex(-10.0, -10.0);
    std::cout << "Out-of-bounds (-10,-10) maps to bin (" << oot_x << "," << oot_y << ")" << std::endl;
    
    auto [oot2_x, oot2_y] = grid.getBinIndex(200.0, 200.0);
    std::cout << "Out-of-bounds (200,200) maps to bin (" << oot2_x << "," << oot2_y << ")" << std::endl;
    
    // Test statistics
    std::cout << "\n=== Summary Statistics ===" << std::endl;
    std::cout << "Max density: " << std::fixed << std::setprecision(3) << grid.getMaxDensity() << std::endl;
    std::cout << "Average density: " << std::fixed << std::setprecision(3) << grid.getAverageDensity() << std::endl;
    std::cout << "Total overflow: " << std::fixed << std::setprecision(3) << grid.getTotalOverflow(0.7) << std::endl;
    
    std::cout << "\n=== DensityGrid Test Completed Successfully ===" << std::endl;
    
    return 0;
}