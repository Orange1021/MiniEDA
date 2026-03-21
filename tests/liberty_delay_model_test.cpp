#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>

#include "delay_model.h"
#include "cell.h"
#include "liberty_parser.h"

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

std::string basicLiberty() {
    return R"(library(testlib) {
  time_unit : "1ns";
  cell(AND) {
    area : 1.0;
    pin(A) {
      direction : input;
      capacitance : 0.002;
    }
    pin(B) {
      direction : input;
      capacitance : 0.002;
    }
    pin(Y) {
      direction : output;
      capacitance : 0.003;
      timing() {
        related_pin : "A";
        timing_sense : positive_unate;
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

}  // namespace

TEST(LibertyParserTest, ParsesMinimalLibraryAndTimingData) {
    const std::string path = writeTempLiberty(basicLiberty(), "liberty_basic");
    ASSERT_FALSE(path.empty());

    LibertyParser parser;
    std::unique_ptr<Library> lib = parser.parseFile(path);
    ASSERT_NE(lib, nullptr) << parser.getError();

    EXPECT_EQ(lib->getName(), "testlib");
    EXPECT_EQ(lib->getCellCount(), 1u);

    const LibCell* and_cell = lib->getCell("AND");
    ASSERT_NE(and_cell, nullptr);
    EXPECT_NE(and_cell->getPin("A"), nullptr);
    EXPECT_NE(and_cell->getPin("B"), nullptr);

    const LibPin* y = and_cell->getPin("Y");
    ASSERT_NE(y, nullptr);
    EXPECT_TRUE(y->isOutput());
    ASSERT_EQ(y->timing_arcs.size(), 1u);
    EXPECT_TRUE(y->timing_arcs[0].cell_delay.isValid());
    EXPECT_TRUE(y->timing_arcs[0].rise_transition.isValid());
}

TEST(LibertyParserTest, ReturnsNullForInvalidLibrarySyntax) {
    const std::string bad = "library(bad) { cell(X) { pin(A) { direction : input; }";
    const std::string path = writeTempLiberty(bad, "liberty_bad");
    ASSERT_FALSE(path.empty());

    LibertyParser parser;
    std::unique_ptr<Library> lib = parser.parseFile(path);
    EXPECT_EQ(lib, nullptr);
    EXPECT_FALSE(parser.getError().empty());
}

TEST(TableDelayModelTest, UsesLookupInterpolationAndUnitConversion) {
    const std::string path = writeTempLiberty(basicLiberty(), "delay_interp");
    ASSERT_FALSE(path.empty());

    LibertyParser parser;
    std::unique_ptr<Library> lib = parser.parseFile(path);
    ASSERT_NE(lib, nullptr) << parser.getError();

    TableDelayModel model(lib.get());
    Cell and_cell("u1", CellType::AND);

    // Mid-point interpolation: expected 0.25 ns -> 0.25e-9 s
    const double delay_s = model.calculateCellDelay(&and_cell, 0.06, 0.06);
    EXPECT_NEAR(delay_s, 0.25e-9, 1e-12);

    // Mid-point interpolation on rise_transition: 0.125 ns -> 0.125e-9 s
    const double slew_s = model.calculateOutputSlew(&and_cell, 0.06, 0.06);
    EXPECT_NEAR(slew_s, 0.125e-9, 1e-12);
}

TEST(TableDelayModelTest, ClampsOutOfRangeLookupInputs) {
    const std::string path = writeTempLiberty(basicLiberty(), "delay_clamp");
    ASSERT_FALSE(path.empty());

    LibertyParser parser;
    std::unique_ptr<Library> lib = parser.parseFile(path);
    ASSERT_NE(lib, nullptr) << parser.getError();

    TableDelayModel model(lib.get());
    Cell and_cell("u2", CellType::AND);

    // Out-of-range lookup should clamp to table max corner (0.40 ns).
    const double delay_s = model.calculateCellDelay(&and_cell, 10.0, 10.0);
    EXPECT_NEAR(delay_s, 0.40e-9, 1e-12);
}
