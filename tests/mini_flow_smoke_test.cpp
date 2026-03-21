#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

fs::path findProjectRoot() {
    fs::path p = fs::current_path();
    for (int i = 0; i < 8; ++i) {
        if (fs::exists(p / "benchmarks" / "s27.v")) {
            return p;
        }
        if (!p.has_parent_path()) {
            break;
        }
        p = p.parent_path();
    }
    return {};
}

fs::path findMiniFlowBinary(const fs::path& root) {
    const std::vector<fs::path> candidates = {
        fs::current_path() / "bin" / "mini_flow",
        fs::current_path() / "../bin/mini_flow",
        root / "build" / "cmake" / "test" / "bin" / "mini_flow",
        root / "build" / "bin" / "mini_flow",
    };

    for (const auto& p : candidates) {
        if (!p.empty() && fs::exists(p)) {
            return fs::absolute(p);
        }
    }
    return {};
}

std::string slurpFile(const fs::path& path) {
    std::ifstream ifs(path);
    if (!ifs.is_open()) {
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       std::istreambuf_iterator<char>());
}

}  // namespace

TEST(MiniFlowSmokeTest, RunsS27EndToEndAndProducesRoutingOutput) {
    const fs::path root = findProjectRoot();
    if (root.empty()) {
        GTEST_SKIP() << "Could not locate project root";
    }

    const fs::path mini_flow = findMiniFlowBinary(root);
    if (mini_flow.empty()) {
        GTEST_SKIP() << "Could not locate mini_flow binary";
    }

    const fs::path verilog = root / "benchmarks" / "s27.v";
    const fs::path liberty = root / "benchmarks" / "NangateOpenCellLibrary_typical.lib";
    const fs::path lef = root / "benchmarks" / "NangateOpenCellLibrary.macro.lef";
    if (!fs::exists(verilog) || !fs::exists(liberty) || !fs::exists(lef)) {
        GTEST_SKIP() << "Required benchmark inputs are missing";
    }

    const auto epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::system_clock::now().time_since_epoch())
                           .count();
    const std::string run_id = "e2e_smoke_" + std::to_string(epoch);
    const fs::path log_path = fs::path("/tmp") / (run_id + ".log");

    const std::string cmd =
        "\"" + mini_flow.string() + "\" \"" + verilog.string() +
        "\" -lib \"" + liberty.string() +
        "\" -lef \"" + lef.string() +
        "\" -flow -run_id " + run_id +
        " > \"" + log_path.string() + "\" 2>&1";

    const int rc = std::system(cmd.c_str());
    EXPECT_EQ(rc, 0);

    const std::string log_text = slurpFile(log_path);
    EXPECT_NE(log_text.find("Integrated Flow Completed!"), std::string::npos);

    const fs::path routing_txt_in_root = root / "visualizations" / run_id / "post_routing.txt";
    const fs::path routing_txt_in_cwd = fs::current_path() / "visualizations" / run_id / "post_routing.txt";
    EXPECT_TRUE(fs::exists(routing_txt_in_root) || fs::exists(routing_txt_in_cwd));
}
