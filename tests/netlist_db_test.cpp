#include <gtest/gtest.h>

#include "netlist_db.h"

using namespace mini;

TEST(NetlistDBConnectionTest, ConnectAndDisconnectPins) {
    NetlistDB db("conn_test");

    Cell* input = db.createCell("in", CellType::INPUT);
    ASSERT_NE(input, nullptr);
    Pin* input_y = input->addPin("Y", PinDirection::OUTPUT);
    ASSERT_NE(input_y, nullptr);

    Cell* gate = db.createCell("u1", CellType::AND);
    ASSERT_NE(gate, nullptr);
    Pin* gate_a = gate->addPin("A", PinDirection::INPUT);
    ASSERT_NE(gate_a, nullptr);

    Net* n1 = db.createNet("n1");
    ASSERT_NE(n1, nullptr);

    EXPECT_TRUE(db.connect(input_y, n1));
    EXPECT_EQ(n1->getDriver(), input_y);
    EXPECT_EQ(input_y->getNet(), n1);

    EXPECT_TRUE(db.connect(gate_a, n1));
    EXPECT_EQ(gate_a->getNet(), n1);
    EXPECT_EQ(n1->getFanout(), 1u);

    EXPECT_TRUE(db.disconnect(gate_a));
    EXPECT_EQ(gate_a->getNet(), nullptr);
    EXPECT_EQ(n1->getFanout(), 0u);
}

TEST(NetlistDBTopologyTest, FaninAndFanoutQueries) {
    NetlistDB db("topo_test");

    Cell* src = db.createCell("src", CellType::INPUT);
    Cell* mid = db.createCell("mid", CellType::AND);
    Cell* dst = db.createCell("dst", CellType::OUTPUT);
    ASSERT_NE(src, nullptr);
    ASSERT_NE(mid, nullptr);
    ASSERT_NE(dst, nullptr);

    Pin* src_y = src->addPin("Y", PinDirection::OUTPUT);
    Pin* mid_a = mid->addPin("A", PinDirection::INPUT);
    Pin* mid_y = mid->addPin("Y", PinDirection::OUTPUT);
    Pin* dst_a = dst->addPin("A", PinDirection::INPUT);
    ASSERT_NE(src_y, nullptr);
    ASSERT_NE(mid_a, nullptr);
    ASSERT_NE(mid_y, nullptr);
    ASSERT_NE(dst_a, nullptr);

    Net* n1 = db.createNet("n1");
    Net* n2 = db.createNet("n2");
    ASSERT_NE(n1, nullptr);
    ASSERT_NE(n2, nullptr);

    ASSERT_TRUE(db.connect(src_y, n1));
    ASSERT_TRUE(db.connect(mid_a, n1));
    ASSERT_TRUE(db.connect(mid_y, n2));
    ASSERT_TRUE(db.connect(dst_a, n2));

    auto fanout_src = db.getFanoutCells(src);
    ASSERT_EQ(fanout_src.size(), 1u);
    EXPECT_EQ(fanout_src[0], mid);

    auto fanin_dst = db.getFaninCells(dst);
    ASSERT_EQ(fanin_dst.size(), 1u);
    EXPECT_EQ(fanin_dst[0], mid);
}

TEST(NetlistDBLifecycleTest, ClearEmptiesDatabaseAndAllowsReuse) {
    NetlistDB db("lifecycle_test");

    Cell* a = db.createCell("a", CellType::INPUT);
    Cell* b = db.createCell("b", CellType::OUTPUT);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);

    Pin* a_y = a->addPin("Y", PinDirection::OUTPUT);
    Pin* b_a = b->addPin("A", PinDirection::INPUT);
    Net* n = db.createNet("n");
    ASSERT_NE(a_y, nullptr);
    ASSERT_NE(b_a, nullptr);
    ASSERT_NE(n, nullptr);
    ASSERT_TRUE(db.connect(a_y, n));
    ASSERT_TRUE(db.connect(b_a, n));

    db.clear();

    EXPECT_TRUE(db.isEmpty());
    EXPECT_EQ(db.getNumCells(), 0u);
    EXPECT_EQ(db.getNumNets(), 0u);
    EXPECT_EQ(db.getCell("a"), nullptr);
    EXPECT_EQ(db.getNet("n"), nullptr);

    Cell* c = db.createCell("c", CellType::BUF);
    ASSERT_NE(c, nullptr);
    EXPECT_EQ(db.getNumCells(), 1u);
}
