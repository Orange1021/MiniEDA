#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "cell.h"
#include "netlist_db.h"
#include "placer_db.h"
#include "density_grid.h"
#include "poisson_solver.h"
#include "greedy_legalizer.h"
#include "overlap_detector.h"

using namespace mini;

TEST(DensityGridTest, DistributesCellAreaAcrossOverlappedBins) {
    NetlistDB netlist("density_test");
    Cell* c1 = netlist.createCell("U1", CellType::AND);
    ASSERT_NE(c1, nullptr);

    PlacerDB db(&netlist);
    db.setCoreArea(Rect(0.0, 0.0, 2.0, 2.0));
    db.setRowHeight(1.0);
    db.addCell(c1, 1.0, 1.0);
    db.placeCell(c1, 0.5, 0.5);

    DensityGrid grid;
    ASSERT_TRUE(grid.init(2, 2, 2.0, 2.0, 0.0, 0.0));

    std::vector<Cell*> cells{c1};
    grid.updateDensity(cells, &db);

    EXPECT_NEAR(grid.getBin(0, 0).density, 0.25, 1e-9);
    EXPECT_NEAR(grid.getBin(1, 0).density, 0.25, 1e-9);
    EXPECT_NEAR(grid.getBin(0, 1).density, 0.25, 1e-9);
    EXPECT_NEAR(grid.getBin(1, 1).density, 0.25, 1e-9);
    EXPECT_NEAR(grid.getAverageDensity(), 0.25, 1e-9);
    EXPECT_NEAR(grid.getTotalOverflow(0.20), 0.20, 1e-9);
}

TEST(PoissonSolverTest, ValidatesFftDimensionsAndSolvesSimpleCase) {
    EXPECT_TRUE(PoissonSolver::isValidFFTSize(8, 4));
    EXPECT_FALSE(PoissonSolver::isValidFFTSize(6, 4));
    EXPECT_EQ(PoissonSolver::nextPowerOfTwo(5), 8);

    std::vector<Bin> bins(4);
    bins[0].x = 0.5;
    bins[0].y = 0.5;
    bins[1].x = 1.5;
    bins[1].y = 0.5;
    bins[2].x = 0.5;
    bins[2].y = 1.5;
    bins[3].x = 1.5;
    bins[3].y = 1.5;

    bins[0].density = 1.0;
    bins[1].density = 0.0;
    bins[2].density = 0.0;
    bins[3].density = 0.0;

    PoissonSolver solver;
    solver.setBinSize(1.0, 1.0);
    ASSERT_TRUE(solver.solve(bins, 2, 2));

    for (const auto& bin : bins) {
        EXPECT_TRUE(std::isfinite(bin.electro_potential));
        EXPECT_TRUE(std::isfinite(bin.electro_gradient_x));
        EXPECT_TRUE(std::isfinite(bin.electro_gradient_y));
    }
    EXPECT_GE(solver.getMaxForceMagnitude(), 0.0);
}

TEST(GreedyLegalizerTest, RemovesOverlapsAndKeepsCellsInsideCore) {
    NetlistDB netlist("legalizer_test");
    Cell* a = netlist.createCell("A", CellType::AND);
    Cell* b = netlist.createCell("B", CellType::OR);
    Cell* c = netlist.createCell("C", CellType::NOT);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    ASSERT_NE(c, nullptr);

    PlacerDB db(&netlist);
    db.setCoreArea(Rect(0.0, 0.0, 4.0, 2.0));
    db.setRowHeight(1.0);

    db.addCell(a, 2.0, 1.0);
    db.addCell(b, 2.0, 1.0);
    db.addCell(c, 2.0, 1.0);

    db.placeCell(a, 0.0, 0.0);
    db.placeCell(b, 0.0, 0.0);
    db.placeCell(c, 0.0, 0.0);

    OverlapDetector before(&db);
    EXPECT_TRUE(before.hasOverlaps());

    GreedyLegalizer legalizer(&db);
    legalizer.run();

    OverlapDetector after(&db);
    EXPECT_FALSE(after.hasOverlaps());
    EXPECT_TRUE(db.isValidPlacement(a));
    EXPECT_TRUE(db.isValidPlacement(b));
    EXPECT_TRUE(db.isValidPlacement(c));
}
