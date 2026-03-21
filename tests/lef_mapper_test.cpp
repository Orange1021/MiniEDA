#include <gtest/gtest.h>

#include <fstream>
#include <string>

#include "lef_parser.h"
#include "macro_mapper.h"
#include "lef_pin_mapper.h"

using namespace mini;

namespace {

std::string writeTempLef(const std::string& body, const std::string& tag) {
    const std::string path = "/tmp/minieda_" + tag + ".lef";
    std::ofstream ofs(path, std::ios::out | std::ios::trunc);
    if (!ofs.is_open()) {
        return "";
    }
    ofs << body;
    ofs.close();
    return path;
}

std::string minimalLef() {
    return R"(VERSION 5.8 ;
MACRO NAND2_X1
  CLASS CORE ;
  ORIGIN 0 0 ;
  SIZE 1.4 BY 2.0 ;
  SYMMETRY X Y ;
  SITE coreSite ;
  PIN A1
    DIRECTION INPUT ;
    USE SIGNAL ;
    PORT
      LAYER M1 ;
      RECT 0.00 0.00 0.10 0.10 ;
    END
  END A1
  PIN A2
    DIRECTION INPUT ;
    USE SIGNAL ;
    PORT
      LAYER M1 ;
      RECT 0.20 0.00 0.30 0.10 ;
    END
  END A2
  PIN ZN
    DIRECTION OUTPUT ;
    USE SIGNAL ;
    PORT
      LAYER M1 ;
      RECT 1.20 1.80 1.30 1.90 ;
    END
  END ZN
END NAND2_X1
END LIBRARY
)";
}

}  // namespace

TEST(LefParserTest, ParsesMacroDimensionsAndPins) {
    const std::string path = writeTempLef(minimalLef(), "lef_parse");
    ASSERT_FALSE(path.empty());

    LefParser parser(path);
    LefLibrary lib = parser.parse();

    EXPECT_EQ(lib.getMacroCount(), 1u);
    const LefMacro* macro = lib.getMacro("NAND2_X1");
    ASSERT_NE(macro, nullptr);
    EXPECT_NEAR(macro->width, 1.4, 1e-9);
    EXPECT_NEAR(macro->height, 2.0, 1e-9);

    const LefPort* a1 = macro->getPin("A1");
    const LefPort* zn = macro->getPin("ZN");
    ASSERT_NE(a1, nullptr);
    ASSERT_NE(zn, nullptr);
    ASSERT_EQ(a1->getRects().size(), 1u);
    ASSERT_EQ(zn->getRects().size(), 1u);
}

TEST(MacroMapperTest, MapsByDriveStrengthAndCaseInsensitiveFallback) {
    const std::string path = writeTempLef(minimalLef(), "macro_mapper");
    ASSERT_FALSE(path.empty());

    LefParser parser(path);
    LefLibrary lib = parser.parse();
    MacroMapper mapper(lib);

    const LefMacro* mapped_by_suffix = mapper.mapType("NAND2");
    ASSERT_NE(mapped_by_suffix, nullptr);
    EXPECT_EQ(mapped_by_suffix->name, "NAND2_X1");

    const LefMacro* mapped_case_insensitive = mapper.mapType("nand2_x1");
    ASSERT_NE(mapped_case_insensitive, nullptr);
    EXPECT_EQ(mapped_case_insensitive->name, "NAND2_X1");
}

TEST(LefPinMapperTest, MapsLogicalPinsAndBuildsStableKeys) {
    const std::string path = writeTempLef(minimalLef(), "pin_mapper");
    ASSERT_FALSE(path.empty());

    LefParser parser(path);
    LefLibrary lib = parser.parse();
    MacroMapper macro_mapper(lib);
    LefPinMapper pin_mapper(lib, macro_mapper);

    EXPECT_EQ(pin_mapper.getPhysicalPinName("NAND2", "A"), "A1");
    EXPECT_EQ(pin_mapper.getPhysicalPinName("NAND2", "Y"), "ZN");
    EXPECT_EQ(pin_mapper.getKeyFromLogicalPin("U1", "NAND2", "Y"), "U1:ZN");
    EXPECT_EQ(pin_mapper.makeKey("U2", "A2"), "U2:A2");
}
