#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>

#include "cell.h"
#include "delay_model.h"
#include "liberty_parser.h"
#include "netlist_db.h"
#include "sta_engine.h"
#include "timing_constraints.h"
#include "timing_graph.h"

using namespace mini;

namespace {

std::string writeTempLiberty(const std::string& body, const std::string& tag) {
    const std::string path = "/tmp/minieda_" + tag + ".lib";
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        return "";
    }
    ofs << body;
    ofs.close();
    return path;
}

std::string notLiberty() {
    return R"(library(testlib_sta) {
  time_unit : "1ns";
  cell(NOT) {
    area : 1.0;
    pin(A) {
      direction : input;
      capacitance : 0.002;
    }
    pin(Y) {
      direction : output;
      capacitance : 0.003;
      timing() {
        related_pin : "A";
        timing_sense : negative_unate;
        cell_rise(template_2x2) {
          index_1 ("0.01, 0.11");
          index_2 ("0.01, 0.11");
          values ("0.10, 0.20", \
                  "0.30, 0.40");
        }
        rise_transition(template_2x2) {
          index_1 ("0.01, 0.11");
          index_2 ("0.01, 0.11");
          values ("0.05, 0.10", \
                  "0.15, 0.20");
        }
      }
    }
  }
}
)";
}

void buildSimpleNotPath(NetlistDB& db) {
    Cell* in = db.createCell("IN", CellType::INPUT);
    Cell* inv = db.createCell("U1", CellType::NOT);
    Cell* out = db.createCell("OUT", CellType::OUTPUT);
    ASSERT_NE(in, nullptr);
    ASSERT_NE(inv, nullptr);
    ASSERT_NE(out, nullptr);

    Pin* in_y = in->addPin("Y", PinDirection::OUTPUT);
    Pin* inv_a = inv->addPin("A", PinDirection::INPUT);
    Pin* inv_y = inv->addPin("Y", PinDirection::OUTPUT);
    Pin* out_a = out->addPin("A", PinDirection::INPUT);
    ASSERT_NE(in_y, nullptr);
    ASSERT_NE(inv_a, nullptr);
    ASSERT_NE(inv_y, nullptr);
    ASSERT_NE(out_a, nullptr);

    Net* n1 = db.createNet("n1");
    Net* n2 = db.createNet("n2");
    ASSERT_NE(n1, nullptr);
    ASSERT_NE(n2, nullptr);

    ASSERT_TRUE(db.connect(in_y, n1));
    ASSERT_TRUE(db.connect(inv_a, n1));
    ASSERT_TRUE(db.connect(inv_y, n2));
    ASSERT_TRUE(db.connect(out_a, n2));
}

}  // namespace

TEST(TimingGraphTest, BuildsExpectedNodesAndArcsForSimplePath) {
    NetlistDB db("tg_simple");
    buildSimpleNotPath(db);

    const std::string lib_path = writeTempLiberty(notLiberty(), "timing_graph");
    ASSERT_FALSE(lib_path.empty());
    LibertyParser parser;
    std::unique_ptr<Library> lib = parser.parseFile(lib_path);
    ASSERT_NE(lib, nullptr) << parser.getError();

    TimingGraph graph(&db, lib.get());
    ASSERT_TRUE(graph.buildFromNetlist());

    EXPECT_EQ(graph.getNumNodes(), 4u);  // IN/Y, U1/A, U1/Y, OUT/A
    EXPECT_EQ(graph.getNumArcs(), 3u);   // 2 net arcs + 1 cell arc
    EXPECT_TRUE(graph.isDAG());
    EXPECT_EQ(graph.topologicalSort().size(), 4u);

    EXPECT_NE(graph.getNode("IN/Y"), nullptr);
    EXPECT_NE(graph.getNode("U1/A"), nullptr);
    EXPECT_NE(graph.getNode("U1/Y"), nullptr);
    EXPECT_NE(graph.getNode("OUT/A"), nullptr);

    EXPECT_EQ(graph.getStartPoints().size(), 1u);
    EXPECT_EQ(graph.getEndPoints().size(), 1u);
}

TEST(STAEngineTest, RunUpdatesATandRATWithOutputDelayConstraint) {
    NetlistDB db("sta_simple");
    buildSimpleNotPath(db);

    const std::string lib_path = writeTempLiberty(notLiberty(), "sta_engine");
    ASSERT_FALSE(lib_path.empty());
    LibertyParser parser;
    std::unique_ptr<Library> lib = parser.parseFile(lib_path);
    ASSERT_NE(lib, nullptr) << parser.getError();

    TimingGraph graph(&db, lib.get());
    ASSERT_TRUE(graph.buildFromNetlist());

    auto model = std::make_shared<TableDelayModel>(lib.get());
    STAEngine sta(&graph, model);

    TimingConstraints constraints;
    constraints.createClock("main_clk", "IN", 1.0);
    constraints.setOutputDelay("OUT", 0.2);  // RAT at OUT endpoint should become 0.8
    sta.setConstraints(&constraints);
    sta.run();

    TimingNode* in_node = graph.getNode("IN/Y");
    TimingNode* out_node = graph.getNode("OUT/A");
    ASSERT_NE(in_node, nullptr);
    ASSERT_NE(out_node, nullptr);

    EXPECT_NEAR(in_node->getArrivalTime(), 0.0, 1e-12);
    EXPECT_GT(out_node->getArrivalTime(), 0.0);
    EXPECT_NEAR(out_node->getRequiredTime(), 0.8, 1e-9);
    EXPECT_GT(out_node->getSlack(), 0.0);
}
