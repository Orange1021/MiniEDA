# ============================================================================
# MiniEDA Integrated Flow Makefile
# High-quality build system with debug/release modes and dependency tracking
# ============================================================================

# ============================================================================
# Compiler and Compilation Options
# ============================================================================
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic
LDFLAGS := 

# Build mode (debug or release)
BUILD_MODE ?= debug

# Debug configuration (default)
ifeq ($(BUILD_MODE),debug)
    CXXFLAGS += -g -O0 -DDEBUG -DDEBUG_LOG_LEVEL=1
    BUILD_DIR := build/debug
else ifeq ($(BUILD_MODE),release)
    CXXFLAGS += -O3 -DNDEBUG -DDEBUG_LOG_LEVEL=1
    BUILD_DIR := build/release
else
    $(error Unknown BUILD_MODE: $(BUILD_MODE). Use 'debug' or 'release')
endif

# ============================================================================
# Directory Definitions
# ============================================================================
LIB_SRC_DIR := lib/src
LIB_INC_DIR := lib/include
BUILD_LIB_DIR := $(BUILD_DIR)/lib
BUILD_BIN_DIR := $(BUILD_DIR)/bin

# Application directories
PLACEMENT_DIR := apps/mini_placement
ROUTER_DIR := apps/mini_router
STA_DIR := apps/mini_sta

# Include paths
INCLUDES := -I$(LIB_INC_DIR) \
            -I$(PLACEMENT_DIR) \
            -I$(ROUTER_DIR) \
            -I$(STA_DIR)

# ============================================================================
# Source Files
# ============================================================================
# Core library source files
LIB_SRCS := $(wildcard $(LIB_SRC_DIR)/*.cpp)
LIB_OBJS := $(patsubst $(LIB_SRC_DIR)/%.cpp,$(BUILD_LIB_DIR)/%.o,$(LIB_SRCS))

# Application source files (shared library)
# All placement files including legalizers
APP_LIB_SRCS := \
    $(STA_DIR)/cell_mapper.cpp \
    $(STA_DIR)/delay_model.cpp \
    $(STA_DIR)/sta_engine.cpp \
    $(STA_DIR)/timing_checks.cpp \
    $(STA_DIR)/timing_constraints.cpp \
    $(STA_DIR)/timing_graph.cpp \
    $(STA_DIR)/timing_path.cpp \
    $(STA_DIR)/timing_report.cpp \
    $(PLACEMENT_DIR)/placer_engine.cpp \
    $(PLACEMENT_DIR)/macro_mapper.cpp \
    $(PLACEMENT_DIR)/poisson_solver.cpp \
    $(PLACEMENT_DIR)/global_placer.cpp \
    $(PLACEMENT_DIR)/placement_interface.cpp \
    $(PLACEMENT_DIR)/detailed_placer.cpp \
    $(PLACEMENT_DIR)/legalizer.cpp \
    $(PLACEMENT_DIR)/greedy_legalizer.cpp \
    $(PLACEMENT_DIR)/abacus_legalizer.cpp \
    $(PLACEMENT_DIR)/density_grid.cpp \
    $(PLACEMENT_DIR)/overlap_detector.cpp \
    $(ROUTER_DIR)/routing_grid.cpp \
    $(ROUTER_DIR)/maze_router.cpp \
    $(ROUTER_DIR)/routing_interface.cpp

# Build application library object files using pattern rules
APP_LIB_OBJS := $(patsubst $(PLACEMENT_DIR)/%.cpp,$(BUILD_LIB_DIR)/placement_%.o,$(filter $(PLACEMENT_DIR)/%.cpp,$(APP_LIB_SRCS)))
APP_LIB_OBJS += $(patsubst $(ROUTER_DIR)/%.cpp,$(BUILD_LIB_DIR)/routing_%.o,$(filter $(ROUTER_DIR)/%.cpp,$(APP_LIB_SRCS)))
APP_LIB_OBJS += $(patsubst $(STA_DIR)/%.cpp,$(BUILD_LIB_DIR)/sta_%.o,$(filter $(STA_DIR)/%.cpp,$(APP_LIB_SRCS)))

# All library objects
ALL_LIB_OBJS := $(LIB_OBJS) $(APP_LIB_OBJS)

# Integrated Flow source files
FLOW_SRCS := apps/main_flow.cpp
FLOW_OBJS := $(patsubst apps/%.cpp,$(BUILD_LIB_DIR)/flow_%.o,$(FLOW_SRCS))

# Dependency files (for header file tracking)
DEPS := $(LIB_OBJS:.o=.d) $(APP_LIB_OBJS:.o=.d) $(FLOW_OBJS:.o=.d)

# Executable file
FLOW_BIN := $(BUILD_BIN_DIR)/mini_flow

# ============================================================================
# Build Targets
# ============================================================================

# Default target: build integrated flow
.PHONY: all
all: $(FLOW_BIN)

# Link Integrated Flow executable
$(FLOW_BIN): $(FLOW_OBJS) $(ALL_LIB_OBJS) | $(BUILD_BIN_DIR)
	@echo "Linking $@..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)
	@echo "MiniEDA Integrated Flow compilation completed: $@"

# ============================================================================
# Compilation Rules (Pattern Rules)
# ============================================================================

# Compile core library object files
$(BUILD_LIB_DIR)/%.o: $(LIB_SRC_DIR)/%.cpp | $(BUILD_LIB_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile application library object files for placement
$(BUILD_LIB_DIR)/placement_%.o: $(PLACEMENT_DIR)/%.cpp | $(BUILD_LIB_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile application library object files for router
$(BUILD_LIB_DIR)/routing_%.o: $(ROUTER_DIR)/%.cpp | $(BUILD_LIB_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile application library object files for STA
$(BUILD_LIB_DIR)/sta_%.o: $(STA_DIR)/%.cpp | $(BUILD_LIB_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Compile Integrated Flow object files
$(BUILD_LIB_DIR)/flow_%.o: apps/%.cpp | $(BUILD_LIB_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# ============================================================================
# Directory Creation
# ============================================================================

$(BUILD_LIB_DIR) $(BUILD_BIN_DIR):
	@mkdir -p $@

# ============================================================================
# Include Dependencies (Header File Tracking)
# ============================================================================

-include $(DEPS)

# ============================================================================
# Build Mode Targets
# ============================================================================

.PHONY: debug release

debug:
	@$(MAKE) BUILD_MODE=debug

release:
	@$(MAKE) BUILD_MODE=release clean all

# ============================================================================
# Utility Targets
# ============================================================================

# Build MiniFlow only
.PHONY: flow
flow: $(FLOW_BIN)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "Cleanup completed"

# Clean everything including generated files
.PHONY: distclean
distclean: clean
	@echo "Cleaning generated files..."
	@rm -rf visualizations/*/visualizations
	@rm -f *.log

# Run MiniFlow (parameters can be passed via ARGS)
.PHONY: run
run: $(FLOW_BIN)
	$(FLOW_BIN) $(ARGS)

# Run all tests
.PHONY: test
test: $(FLOW_BIN)
	@echo "Running all tests..."
	@./run_tests.sh

# Install target
.PHONY: install
install: $(FLOW_BIN)
	@echo "Installing MiniEDA to $(PREFIX)/bin..."
	@install -d $(PREFIX)/bin
	@install -m 755 $(FLOW_BIN) $(PREFIX)/bin/

# ============================================================================
# Test Targets
# ============================================================================

.PHONY: test-s27 test-s344 test-s349 test-s526

test-s27: $(FLOW_BIN)
	@echo "Running s27 test..."
	$(FLOW_BIN) -v benchmarks/ISCAS/Verilog/s27.v \
		-lib benchmarks/NangateOpenCellLibrary_typical.lib \
		-lef benchmarks/NangateOpenCellLibrary.macro.lef

test-s344: $(FLOW_BIN)
	@echo "Running s344 test..."
	$(FLOW_BIN) -v benchmarks/ISCAS/Verilog/s344.v \
		-lib benchmarks/NangateOpenCellLibrary_typical.lib \
		-lef benchmarks/NangateOpenCellLibrary.macro.lef

test-s349: $(FLOW_BIN)
	@echo "Running s349 test..."
	$(FLOW_BIN) -v benchmarks/ISCAS/Verilog/s349.v \
		-lib benchmarks/NangateOpenCellLibrary_typical.lib \
		-lef benchmarks/NangateOpenCellLibrary.macro.lef

test-s526: $(FLOW_BIN)
	@echo "Running s526 test..."
	$(FLOW_BIN) -v benchmarks/ISCAS/Verilog/s526.v \
		-lib benchmarks/NangateOpenCellLibrary_typical.lib \
		-lef benchmarks/NangateOpenCellLibrary.macro.lef

# ============================================================================
# Static Analysis Targets (Optional)
# ============================================================================

.PHONY: tidy cppcheck

tidy:
	@echo "Running clang-tidy..."
	@clang-tidy apps/ lib/src/ -- $(CXXFLAGS) $(INCLUDES) || true

cppcheck:
	@echo "Running cppcheck..."
	@cppcheck --enable=all --inconclusive --std=c++17 apps/ lib/src/ || true

# ============================================================================
# Help Target
# ============================================================================

.PHONY: help
help:
	@echo "MiniEDA Integrated Flow Makefile"
	@echo "================================="
	@echo ""
	@echo "Usage: make [target] [BUILD_MODE=debug|release]"
	@echo ""
	@echo "Build Targets:"
	@echo "  all              - Build integrated flow (default)"
	@echo "  flow             - Build MiniFlow only"
	@echo "  debug            - Build debug version"
	@echo "  release          - Build release version (optimized)"
	@echo ""
	@echo "Utility Targets:"
	@echo "  clean            - Clean all build artifacts"
	@echo "  distclean        - Remove all generated files"
	@echo "  run              - Run MiniFlow (use ARGS=... to pass parameters)"
	@echo "  install          - Install to $(PREFIX)/bin (default: /usr/local)"
	@echo "  test             - Run all tests"
	@echo ""
	@echo "Test Targets:"
	@echo "  test-s27         - Run s27 circuit test"
	@echo "  test-s344        - Run s344 circuit test"
	@echo "  test-s349        - Run s349 circuit test"
	@echo "  test-s526        - Run s526 circuit test"
	@echo ""
	@echo "Static Analysis (Optional):"
	@echo "  tidy             - Run clang-tidy"
	@echo "  cppcheck         - Run cppcheck"
	@echo ""
	@echo "Build Modes:"
	@echo "  debug            - Debug build with symbols (default)"
	@echo "  release          - Optimized release build (-O3 -DNDEBUG)"
	@echo ""
	@echo "Example usage:"
	@echo "  make                    # Build debug version"
	@echo "  make BUILD_MODE=release # Build release version"
	@echo "  make test-s27           # Run s27 test"
	@echo "  make run ARGS='...'     # Run with custom parameters"
	@echo "  make -j$(nproc)          # Parallel compilation (use all cores)"
	@echo "  make install PREFIX=~    # Install to home directory"
	@echo ""
	@echo "Environment Variables:"
	@echo "  CXX                     - C++ compiler (default: g++)"
	@echo "  BUILD_MODE              - Build mode (debug|release)"
	@echo "  PREFIX                  - Installation prefix (default: /usr/local)"
	@echo "  ARGS                    - Arguments for 'run' target"

# ============================================================================
# Phony Targets
# ============================================================================

.PHONY: all flow clean distclean run test install debug release help
.PHONY: tidy cppcheck
.PHONY: test-s27 test-s344 test-s349 test-s526

# ============================================================================
# Default Target
# ============================================================================

.DEFAULT_GOAL := all