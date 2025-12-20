# EDA Backend Toolchain Project Makefile

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
STA_DIR := apps/mini_sta
PLACER_DIR := apps/mini_placement
ROUTER_DIR := apps/mini_router

# Core library source files
LIB_SRCS := $(wildcard $(LIB_SRC_DIR)/*.cpp) lib/src/liberty_pin_mapper.cpp lib/src/lef_pin_mapper.cpp
LIB_OBJS := $(patsubst $(LIB_SRC_DIR)/%.cpp, $(BUILD_LIB_DIR)/%.o, $(LIB_SRCS))

# Additional application source files (dependencies for lib)
APP_LIB_SRCS := $(STA_DIR)/cell_mapper.cpp $(STA_DIR)/delay_model.cpp $(STA_DIR)/sta_engine.cpp \
               $(STA_DIR)/timing_checks.cpp $(STA_DIR)/timing_constraints.cpp $(STA_DIR)/timing_graph.cpp \
               $(STA_DIR)/timing_path.cpp $(STA_DIR)/timing_report.cpp \
               $(PLACER_DIR)/placer_engine.cpp $(PLACER_DIR)/macro_mapper.cpp $(PLACER_DIR)/poisson_solver.cpp $(PLACER_DIR)/global_placer.cpp \
               $(ROUTER_DIR)/routing_grid.cpp $(ROUTER_DIR)/maze_router.cpp

# Additional dependencies for STA (needs MacroMapper)
STA_DEPS := $(BUILD_LIB_DIR)/lib_macro_mapper.o

# Build application library object files for placement
PLACER_APP_LIB_OBJS := $(patsubst $(PLACER_DIR)/%.cpp, $(BUILD_LIB_DIR)/lib_%.o, $(filter $(PLACER_DIR)/%.cpp,$(APP_LIB_SRCS)))

# Build application library object files for router
ROUTER_APP_LIB_OBJS := $(patsubst $(ROUTER_DIR)/%.cpp, $(BUILD_LIB_DIR)/lib_%.o, $(filter $(ROUTER_DIR)/%.cpp,$(APP_LIB_SRCS)))

# Build application library object files for STA
STA_APP_LIB_OBJS := $(patsubst $(STA_DIR)/%.cpp, $(BUILD_LIB_DIR)/lib_%.o, $(filter $(STA_DIR)/%.cpp,$(APP_LIB_SRCS)))

# All application library objects
APP_LIB_OBJS := $(PLACER_APP_LIB_OBJS) $(ROUTER_APP_LIB_OBJS) $(STA_APP_LIB_OBJS)

# Update LIB_OBJS to include application library files
ALL_LIB_OBJS := $(LIB_OBJS) $(APP_LIB_OBJS)

# MiniSTA source files
STA_SRCS := $(wildcard $(STA_DIR)/*.cpp)
STA_OBJS := $(patsubst $(STA_DIR)/%.cpp, $(BUILD_LIB_DIR)/sta_%.o, $(STA_SRCS))

# MiniPlacement source files (exclude files already in APP_LIB_SRCS and main function)
PLACER_SRCS := $(filter-out $(APP_LIB_SRCS) main_placer.cpp,$(wildcard $(PLACER_DIR)/*.cpp))
PLACER_OBJS := $(patsubst $(PLACER_DIR)/%.cpp, $(BUILD_LIB_DIR)/placer_%.o, $(PLACER_SRCS))

# MiniRouter source files (exclude files already in APP_LIB_SRCS and main function)
ROUTER_SRCS := $(filter-out $(APP_LIB_SRCS) main_router.cpp,$(wildcard $(ROUTER_DIR)/*.cpp))
ROUTER_OBJS := $(patsubst $(ROUTER_DIR)/%.cpp, $(BUILD_LIB_DIR)/router_%.o, $(ROUTER_SRCS))

# Integrated Flow source files
FLOW_SRCS := apps/main_flow.cpp
FLOW_OBJS := $(patsubst apps/%.cpp, $(BUILD_LIB_DIR)/flow_%.o, $(FLOW_SRCS))

# Required STA and Placement object files (exclude main functions)
FLOW_OBJS := $(filter-out $(BUILD_LIB_DIR)/sta_main_sta.o $(BUILD_LIB_DIR)/lib_main_placer.o $(BUILD_LIB_DIR)/main_router.o,$(LIB_OBJS))

# Executable files
STA_BIN := $(BUILD_BIN_DIR)/mini_sta
PLACER_BIN := $(BUILD_BIN_DIR)/mini_placement
ROUTER_BIN := $(BUILD_BIN_DIR)/mini_router
FLOW_BIN := $(BUILD_BIN_DIR)/mini_flow

# Default target: build all
.PHONY: all
all: $(STA_BIN) $(PLACER_BIN) $(ROUTER_BIN) $(FLOW_BIN)

# Compile core library object files
$(BUILD_LIB_DIR)/%.o: $(LIB_SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile MiniSTA object files
$(BUILD_LIB_DIR)/sta_%.o: $(STA_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile application library object files for placement
$(BUILD_LIB_DIR)/lib_%.o: $(PLACER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile application library object files for router
$(BUILD_LIB_DIR)/lib_%.o: $(ROUTER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile application library object files for STA
$(BUILD_LIB_DIR)/lib_%.o: $(STA_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile MiniPlacement object files
$(BUILD_LIB_DIR)/placer_%.o: $(PLACER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Legalizer base class object files
$(BUILD_LIB_DIR)/legalizer.o: $(PLACER_DIR)/legalizer.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile GreedyLegalizer object files
$(BUILD_LIB_DIR)/greedy_legalizer.o: $(PLACER_DIR)/greedy_legalizer.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile AbacusLegalizer object files
$(BUILD_LIB_DIR)/abacus_legalizer.o: $(PLACER_DIR)/abacus_legalizer.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile DensityGrid object files (special case for density_grid.cpp)
$(BUILD_LIB_DIR)/density_grid.o: $(PLACER_DIR)/density_grid.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile MiniRouter object files
$(BUILD_LIB_DIR)/router_%.o: $(ROUTER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link MiniSTA executable
$(BUILD_BIN_DIR)/mini_sta: $(BUILD_LIB_DIR)/sta_main_sta.o $(STA_OBJS) $(STA_DEPS) $(LIB_OBJS) | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "MiniSTA compilation completed: $@"

# Link MiniPlacement executable
$(BUILD_BIN_DIR)/mini_placement: $(BUILD_LIB_DIR)/lib_main_placer.o $(LIB_OBJS) $(APP_LIB_OBJS) $(BUILD_LIB_DIR)/density_grid.o $(BUILD_LIB_DIR)/legalizer.o $(BUILD_LIB_DIR)/greedy_legalizer.o $(BUILD_LIB_DIR)/abacus_legalizer.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "MiniPlacement compilation completed: $@"

# Link MiniRouter executable
$(BUILD_BIN_DIR)/mini_router: $(BUILD_LIB_DIR)/router_main_router.o $(ROUTER_OBJS) $(LIB_OBJS) $(APP_LIB_OBJS) $(BUILD_LIB_DIR)/placer_placement_interface.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "MiniRouter compilation completed: $@"

# Compile Integrated Flow object files
$(BUILD_LIB_DIR)/flow_%.o: apps/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link Integrated Flow executable
$(BUILD_BIN_DIR)/mini_flow: $(BUILD_LIB_DIR)/flow_main_flow.o $(LIB_OBJS) $(APP_LIB_OBJS) $(BUILD_LIB_DIR)/placer_placement_interface.o $(BUILD_LIB_DIR)/router_routing_interface.o $(BUILD_LIB_DIR)/density_grid.o $(BUILD_LIB_DIR)/legalizer.o $(BUILD_LIB_DIR)/greedy_legalizer.o $(BUILD_LIB_DIR)/abacus_legalizer.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "MiniFlow compilation completed: $@"

# Build MiniSTA only
.PHONY: sta
sta: $(STA_BIN)

# Build MiniPlacement only
.PHONY: placement
placement: $(PLACER_BIN)

# Build MiniRouter only
.PHONY: router
router: $(ROUTER_BIN)

# Build MiniFlow only
.PHONY: flow
flow: $(FLOW_BIN)

# Build density grid test
.PHONY: test-density
test-density: $(BUILD_BIN_DIR)/test_density_grid

$(BUILD_BIN_DIR)/test_density_grid: test/test_density_grid.cpp $(LIB_OBJS) $(BUILD_LIB_DIR)/density_grid.o $(BUILD_LIB_DIR)/lib_macro_mapper.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "Density grid test compilation completed: $@"

# Build poisson solver test
.PHONY: test-poisson
test-poisson: $(BUILD_BIN_DIR)/test_poisson_solver

$(BUILD_BIN_DIR)/test_poisson_solver: test/test_poisson_solver.cpp $(LIB_OBJS) $(BUILD_LIB_DIR)/density_grid.o $(BUILD_LIB_DIR)/lib_poisson_solver.o $(BUILD_LIB_DIR)/lib_macro_mapper.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "Poisson solver test compilation completed: $@"

# Build global placer test
.PHONY: test-global-placer
test-global-placer: $(BUILD_BIN_DIR)/test_global_placer

$(BUILD_BIN_DIR)/test_global_placer: test/test_global_placer.cpp $(LIB_OBJS) $(BUILD_LIB_DIR)/density_grid.o $(BUILD_LIB_DIR)/lib_poisson_solver.o $(BUILD_LIB_DIR)/lib_global_placer.o $(BUILD_LIB_DIR)/lib_macro_mapper.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "Global placer test compilation completed: $@"

# Build hybrid placement test
.PHONY: test-hybrid-placement
test-hybrid-placement: $(BUILD_BIN_DIR)/test_hybrid_placement

$(BUILD_BIN_DIR)/test_hybrid_placement: test/test_hybrid_placement.cpp $(LIB_OBJS) $(BUILD_LIB_DIR)/density_grid.o $(BUILD_LIB_DIR)/lib_poisson_solver.o $(BUILD_LIB_DIR)/lib_global_placer.o $(BUILD_LIB_DIR)/lib_placer_engine.o $(BUILD_LIB_DIR)/lib_macro_mapper.o | $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $< $(filter %.o,$^) -o $@ $(LDFLAGS)
	@echo "Hybrid placement test compilation completed: $@"

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_LIB_DIR)/*.o $(BUILD_BIN_DIR)/*
	@echo "Cleanup completed"

# Run MiniSTA (parameters can be passed via ARGS)
.PHONY: run-sta
run-sta: $(STA_BIN)
	$(STA_BIN) $(ARGS)

# Run MiniPlacement (parameters can be passed via ARGS)
.PHONY: run-placement
run-placement: $(PLACER_BIN)
	$(PLACER_BIN) $(ARGS)

# Run MiniRouter (parameters can be passed via ARGS)
.PHONY: run-router
run-router: $(ROUTER_BIN)
	$(ROUTER_BIN) $(ARGS)

# Show help information
.PHONY: help
help:
	@echo "Available make targets:"
	@echo "  all         - Build all applications (default)"
	@echo "  sta         - Build MiniSTA only"
	@echo "  placement   - Build MiniPlacement only"
	@echo "  router      - Build MiniRouter only"
	@echo "  flow        - Build MiniFlow only"
	@echo "  clean       - Clean all build artifacts"
	@echo "  run-sta     - Run MiniSTA (use ARGS=... to pass parameters)"
	@echo "  run-placement - Run MiniPlacement (use ARGS=... to pass parameters)"
	@echo "  run-router  - Run MiniRouter (use ARGS=... to pass parameters)"
	@echo "  help        - Show this help information"