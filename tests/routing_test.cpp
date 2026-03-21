#include <gtest/gtest.h>

#include <cmath>
#include <stdexcept>
#include <vector>

#include "routing_grid.h"
#include "maze_router.h"
#include "lef_pin_mapper.h"
#include "macro_mapper.h"

using namespace mini;

namespace {

bool containsPoint(const std::vector<GridPoint>& points, const GridPoint& target) {
    for (const auto& p : points) {
        if (p == target) {
            return true;
        }
    }
    return false;
}

RoutingGrid makeGrid(int layers = 3) {
    RoutingGrid grid;
    grid.init(Rect(0.0, 0.0, 5.0, 5.0), 1.0, 1.0, layers);
    return grid;
}

}  // namespace

TEST(RoutingGridTest, EvenLayerNeighborsAreHorizontalPlusVia) {
    RoutingGrid grid = makeGrid(3);
    const GridPoint current(2, 2, 0);

    const auto neighbors = grid.getNeighbors(current, 1);

    EXPECT_TRUE(containsPoint(neighbors, GridPoint(1, 2, 0)));
    EXPECT_TRUE(containsPoint(neighbors, GridPoint(3, 2, 0)));
    EXPECT_TRUE(containsPoint(neighbors, GridPoint(2, 2, 1)));

    EXPECT_FALSE(containsPoint(neighbors, GridPoint(2, 1, 0)));
    EXPECT_FALSE(containsPoint(neighbors, GridPoint(2, 3, 0)));
}

TEST(RoutingGridTest, OddLayerNeighborsAreVerticalPlusVias) {
    RoutingGrid grid = makeGrid(3);
    const GridPoint current(2, 2, 1);

    const auto neighbors = grid.getNeighbors(current, 1);

    EXPECT_TRUE(containsPoint(neighbors, GridPoint(2, 1, 1)));
    EXPECT_TRUE(containsPoint(neighbors, GridPoint(2, 3, 1)));
    EXPECT_TRUE(containsPoint(neighbors, GridPoint(2, 2, 0)));
    EXPECT_TRUE(containsPoint(neighbors, GridPoint(2, 2, 2)));

    EXPECT_FALSE(containsPoint(neighbors, GridPoint(1, 2, 1)));
    EXPECT_FALSE(containsPoint(neighbors, GridPoint(3, 2, 1)));
}

TEST(RoutingGridTest, ObstacleBlocksNeighborExpansion) {
    RoutingGrid grid = makeGrid(2);
    const GridPoint blocked(3, 2, 0);
    grid.addObstacle(Rect(3.0, 2.0, 3.0, 2.0), 0);

    EXPECT_EQ(grid.getState(blocked), GridState::OBSTACLE);
    EXPECT_FALSE(grid.isFree(blocked, 1));

    const auto neighbors = grid.getNeighbors(GridPoint(2, 2, 0), 1);
    EXPECT_FALSE(containsPoint(neighbors, blocked));
}

TEST(RoutingGridTest, MovementCostIncludesHistoryAndEnemyPenalty) {
    RoutingGrid grid = makeGrid(2);
    const GridPoint from(1, 1, 0);
    const GridPoint to(2, 1, 0);

    grid.setHistoryCost(to.x, to.y, to.layer, 2.5);
    grid.setState(to, GridState::ROUTED, 7);

    const double enemy_cost = grid.calculateMovementCost(from, to, 3, 10.0);
    const double friend_cost = grid.calculateMovementCost(from, to, 7, 10.0);

    EXPECT_NEAR(enemy_cost, 12.5, 1e-9);
    EXPECT_NEAR(friend_cost, 2.5, 1e-9);
}

TEST(MazeRouterTest, ConstructorValidatesRequiredDependencies) {
    LefLibrary lef_lib;
    MacroMapper macro_mapper(lef_lib);
    LefPinMapper pin_mapper(lef_lib, macro_mapper);
    RoutingGrid grid = makeGrid(2);

    EXPECT_THROW(MazeRouter(nullptr, &pin_mapper, nullptr), std::invalid_argument);
    EXPECT_THROW(MazeRouter(&grid, nullptr, nullptr), std::invalid_argument);
}

TEST(MazeRouterTest, FindPathBuildsValidRouteAcrossLayers) {
    LefLibrary lef_lib;
    MacroMapper macro_mapper(lef_lib);
    LefPinMapper pin_mapper(lef_lib, macro_mapper);
    RoutingGrid grid = makeGrid(2);

    MazeRouter router(&grid, &pin_mapper, nullptr);

    const GridPoint start(0, 0, 0);
    const GridPoint end(4, 4, 0);
    std::vector<GridPoint> path;

    ASSERT_TRUE(router.findPath(start, end, path));
    ASSERT_FALSE(path.empty());
    EXPECT_TRUE(path.front() == start);
    EXPECT_TRUE(path.back() == end);

    for (size_t i = 1; i < path.size(); ++i) {
        const int dx = std::abs(path[i].x - path[i - 1].x);
        const int dy = std::abs(path[i].y - path[i - 1].y);
        const int dz = std::abs(path[i].layer - path[i - 1].layer);
        EXPECT_EQ(dx + dy + dz, 1);
    }
}
