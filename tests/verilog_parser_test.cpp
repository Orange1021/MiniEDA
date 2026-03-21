#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "netlist_db.h"
#include "verilog_parser.h"

using namespace mini;

namespace {

std::string writeTempVerilog(const std::string& body, const std::string& tag) {
    const std::string path = "/tmp/minieda_" + tag + ".v";
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        return "";
    }
    ofs << body;
    ofs.close();
    return path;
}

}  // namespace

TEST(VerilogParserTest, ParsesBasicCombinationalNetlist) {
    const std::string src = R"(module top (a, b, y);
input a, b;
output y;
wire n1;
AN2 U1 (n1, a, b);
IV U2 (y, n1);
endmodule
)";
    const std::string path = writeTempVerilog(src, "parse_basic");
    ASSERT_FALSE(path.empty());

    NetlistDB db;
    VerilogParser parser;
    EXPECT_TRUE(parser.parseFile(path, db));
    EXPECT_FALSE(parser.hasErrors());

    EXPECT_EQ(db.getDesignName(), "top");
    EXPECT_EQ(db.getNumCells(), 5u);  // 3 ports + 2 instances
    EXPECT_EQ(db.getNumNets(), 4u);   // a, b, y, n1

    Cell* u1 = db.getCell("U1");
    ASSERT_NE(u1, nullptr);
    EXPECT_EQ(u1->getType(), CellType::AND);
    ASSERT_NE(u1->getPin("Y"), nullptr);
    ASSERT_NE(u1->getPin("A"), nullptr);
    ASSERT_NE(u1->getPin("B"), nullptr);
}

TEST(VerilogParserTest, RejectsUnknownGateType) {
    const std::string src = R"(module top (a, y);
input a;
output y;
wire n1;
FOO U1 (n1, a);
endmodule
)";
    const std::string path = writeTempVerilog(src, "unknown_gate");
    ASSERT_FALSE(path.empty());

    NetlistDB db;
    VerilogParser parser;
    EXPECT_FALSE(parser.parseFile(path, db));
    ASSERT_TRUE(parser.hasErrors());

    const auto& errors = parser.getErrors();
    bool found_unknown_gate = false;
    for (const auto& e : errors) {
        if (e.type == ParseErrorType::UNKNOWN_GATE_TYPE) {
            found_unknown_gate = true;
            break;
        }
    }
    EXPECT_TRUE(found_unknown_gate);
}

TEST(VerilogParserTest, StrictModeFlagsUndefinedSignal) {
    const std::string src = R"(module top (a, y);
input a;
output y;
IV U1 (y, n1);
endmodule
)";
    const std::string path = writeTempVerilog(src, "strict_undef");
    ASSERT_FALSE(path.empty());

    NetlistDB db;
    VerilogParser parser;
    parser.setStrictMode(true);
    EXPECT_FALSE(parser.parseFile(path, db));
    ASSERT_TRUE(parser.hasErrors());

    const auto& errors = parser.getErrors();
    bool found_undefined = false;
    for (const auto& e : errors) {
        if (e.type == ParseErrorType::UNDEFINED_SIGNAL) {
            found_undefined = true;
            break;
        }
    }
    EXPECT_TRUE(found_undefined);
}
