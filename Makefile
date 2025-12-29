# MiniEDA Integrated Flow Makefile
# Simplified version that only builds the integrated flow

# Compiler and compilation options
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./lib/include
LDFLAGS :=

# Directory definitions
LIB_SRC_DIR := lib/src
LIB_INC_DIR := lib/include
BUILD_LIB_DIR := build/lib
BUILD_BIN_DIR := build/bin

# Application directories
PLACEMENT_DIR := apps/mini_placement
ROUTER_DIR := apps/mini_router
STA_DIR := apps/mini_sta

# Core library source files
LIB_SRCS := $(wildcard $(LIB_SRC_DIR)/*.cpp) lib/src/liberty_pin_mapper.cpp lib/src/lef_pin_mapper.cpp
LIB_OBJS := $(patsubst $(LIB_SRC_DIR)/%.cpp, $(BUILD_LIB_DIR)/%.o, $(LIB_SRCS))

# Application source files (shared library)
APP_LIB_SRCS := $(STA_DIR)/cell_mapper.cpp $(STA_DIR)/delay_model.cpp $(STA_DIR)/sta_engine.cpp \
               $(STA_DIR)/timing_checks.cpp $(STA_DIR)/timing_constraints.cpp $(STA_DIR)/timing_graph.cpp \
               $(STA_DIR)/timing_path.cpp $(STA_DIR)/timing_report.cpp \
               $(PLACEMENT_DIR)/placer_engine.cpp $(PLACEMENT_DIR)/macro_mapper.cpp $(PLACEMENT_DIR)/poisson_solver.cpp $(PLACEMENT_DIR)/global_placer.cpp \
               $(PLACEMENT_DIR)/placement_interface.cpp $(PLACEMENT_DIR)/detailed_placer.cpp \
               $(ROUTER_DIR)/routing_grid.cpp $(ROUTER_DIR)/maze_router.cpp $(ROUTER_DIR)/routing_interface.cpp

# Build application library object files
APP_LIB_OBJS := $(patsubst $(PLACEMENT_DIR)/%.cpp,$(BUILD_LIB_DIR)/placement_%.o,$(filter $(PLACEMENT_DIR)/%.cpp,$(APP_LIB_SRCS)))
APP_LIB_OBJS += $(patsubst $(ROUTER_DIR)/%.cpp,$(BUILD_LIB_DIR)/routing_%.o,$(filter $(ROUTER_DIR)/%.cpp,$(APP_LIB_SRCS)))
APP_LIB_OBJS += $(patsubst $(STA_DIR)/%.cpp,$(BUILD_LIB_DIR)/sta_%.o,$(filter $(STA_DIR)/%.cpp,$(APP_LIB_SRCS)))

# All library objects
ALL_LIB_OBJS := $(LIB_OBJS) $(APP_LIB_OBJS)

# Integrated Flow source files
FLOW_SRCS := apps/main_flow.cpp
FLOW_OBJS := $(patsubst apps/%.cpp, $(BUILD_LIB_DIR)/flow_%.o, $(FLOW_SRCS))

# Executable file
FLOW_BIN := $(BUILD_BIN_DIR)/mini_flow

# Default target: build integrated flow
.PHONY: all
all: $(FLOW_BIN)

# Compile core library object files
$(BUILD_LIB_DIR)/%.o: $(LIB_SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile application library object files for placement
$(BUILD_LIB_DIR)/placement_%.o: $(PLACEMENT_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile application library object files for router
$(BUILD_LIB_DIR)/routing_%.o: $(ROUTER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile application library object files for STA
$(BUILD_LIB_DIR)/sta_%.o: $(STA_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Integrated Flow object files
$(BUILD_LIB_DIR)/flow_%.o: apps/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile special placement files
$(BUILD_LIB_DIR)/legalizer.o: $(PLACEMENT_DIR)/legalizer.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_LIB_DIR)/greedy_legalizer.o: $(PLACEMENT_DIR)/greedy_legalizer.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_LIB_DIR)/abacus_legalizer.o: $(PLACEMENT_DIR)/abacus_legalizer.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_LIB_DIR)/density_grid.o: $(PLACEMENT_DIR)/density_grid.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link Integrated Flow executable
$(BUILD_BIN_DIR)/mini_flow: $(BUILD_LIB_DIR)/flow_main_flow.o $(ALL_LIB_OBJS) $(BUILD_LIB_DIR)/legalizer.o $(BUILD_LIB_DIR)/greedy_legalizer.o $(BUILD_LIB_DIR)/abacus_legalizer.o $(BUILD_LIB_DIR)/density_grid.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "MiniEDA Integrated Flow compilation completed: $@"

# Build MiniFlow only
.PHONY: flow
flow: $(FLOW_BIN)

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_LIB_DIR)/*.o $(BUILD_BIN_DIR)/*
	@echo "Cleanup completed"

# Run MiniFlow (parameters can be passed via ARGS)
.PHONY: run
run: $(FLOW_BIN)
	$(FLOW_BIN) $(ARGS)

# Quick test targets for common benchmarks
.PHONY: test-s27-hybrid
test-s27-hybrid: $(FLOW_BIN)
	$(FLOW_BIN) -v benchmarks/ISCAS/Verilog/s27.v \
		-lib benchmarks/NangateOpenCellLibrary_typical.lib \
		-lef benchmarks/NangateOpenCellLibrary.macro.lef \
		-clk 5.0 \
		-util 0.7

.PHONY: test-s344-hybrid
test-s344-hybrid: $(FLOW_BIN)
	$(FLOW_BIN) -v benchmarks/ISCAS/Verilog/s344.v \
		-lib benchmarks/NangateOpenCellLibrary_typical.lib \
		-lef benchmarks/NangateOpenCellLibrary.macro.lef \
		-clk 8.0 \
		-util 0.7

# Show help information
.PHONY: help
help:
	@echo "MiniEDA Integrated Flow Makefile"
	@echo "================================="
	@echo "Available make targets:"
	@echo "  all              - Build integrated flow (default)"
	@echo "  flow             - Build MiniFlow only"
	@echo "  clean            - Clean all build artifacts"
	@echo "  run              - Run MiniFlow (use ARGS=... to pass parameters)"
	@echo "  test-s27-hybrid  - Quick test with s27 circuit using hybrid placement"
	@echo "  test-s344-hybrid - Quick test with s344 circuit using hybrid placement"
	@echo "  help             - Show this help information"
	@echo ""
	@echo "Example usage:"
	@echo "  make                    # Build the integrated flow"
	@echo "  make test-s27-hybrid    # Run s27 test with hybrid placement"
	@echo "  make run ARGS='...'     # Run with custom parameters"