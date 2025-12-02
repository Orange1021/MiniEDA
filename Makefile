# EDA 后端工具链项目 Makefile

# 编译器和编译选项
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./lib/include
LDFLAGS :=

# 目录定义
LIB_SRC_DIR := lib/src
LIB_INC_DIR := lib/include
BUILD_LIB_DIR := build/lib
BUILD_BIN_DIR := build/bin

# 应用目录
STA_DIR := apps/mini_sta
PLACER_DIR := apps/mini_placement
ROUTER_DIR := apps/mini_router

# 核心库源文件
LIB_SRCS := $(wildcard $(LIB_SRC_DIR)/*.cpp)
LIB_OBJS := $(patsubst $(LIB_SRC_DIR)/%.cpp, $(BUILD_LIB_DIR)/%.o, $(LIB_SRCS))

# MiniSTA 源文件
STA_SRCS := $(wildcard $(STA_DIR)/*.cpp)
STA_OBJS := $(patsubst $(STA_DIR)/%.cpp, $(BUILD_LIB_DIR)/sta_%.o, $(STA_SRCS))

# MiniPlacement 源文件
PLACER_SRCS := $(wildcard $(PLACER_DIR)/*.cpp)
PLACER_OBJS := $(patsubst $(PLACER_DIR)/%.cpp, $(BUILD_LIB_DIR)/placer_%.o, $(PLACER_SRCS))

# MiniRouter 源文件
ROUTER_SRCS := $(wildcard $(ROUTER_DIR)/*.cpp)
ROUTER_OBJS := $(patsubst $(ROUTER_DIR)/%.cpp, $(BUILD_LIB_DIR)/router_%.o, $(ROUTER_SRCS))

# Integrated Flow 源文件
FLOW_SRCS := apps/main_flow.cpp
FLOW_OBJS := $(patsubst apps/%.cpp, $(BUILD_LIB_DIR)/flow_%.o, $(FLOW_SRCS))

# 需要的STA和Placement对象文件（排除main函数）
FILTERED_STA_OBJS := $(filter-out $(BUILD_LIB_DIR)/sta_main_sta.o, $(STA_OBJS))
FILTERED_PLACER_OBJS := $(filter-out $(BUILD_LIB_DIR)/placer_main_placer.o, $(PLACER_OBJS))

# 可执行文件
STA_BIN := $(BUILD_BIN_DIR)/mini_sta
PLACER_BIN := $(BUILD_BIN_DIR)/mini_placement
ROUTER_BIN := $(BUILD_BIN_DIR)/mini_router
FLOW_BIN := $(BUILD_BIN_DIR)/mini_flow

# 默认目标：编译所有
.PHONY: all
all: $(STA_BIN) $(PLACER_BIN) $(ROUTER_BIN) $(FLOW_BIN)

# 编译核心库目标文件
$(BUILD_LIB_DIR)/%.o: $(LIB_SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 MiniSTA 目标文件
$(BUILD_LIB_DIR)/sta_%.o: $(STA_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 MiniPlacement 目标文件
$(BUILD_LIB_DIR)/placer_%.o: $(PLACER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 MiniRouter 目标文件
$(BUILD_LIB_DIR)/router_%.o: $(ROUTER_DIR)/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 链接 MiniSTA 可执行文件
$(STA_BIN): $(LIB_OBJS) $(STA_OBJS)
	@mkdir -p $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "MiniSTA 编译完成: $@"

# 链接 MiniPlacement 可执行文件
$(PLACER_BIN): $(LIB_OBJS) $(PLACER_OBJS)
	@mkdir -p $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "MiniPlacement 编译完成: $@"

# 链接 MiniRouter 可执行文件
$(ROUTER_BIN): $(LIB_OBJS) $(ROUTER_OBJS) $(FILTERED_PLACER_OBJS)
	@mkdir -p $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "MiniRouter 编译完成: $@"

# 编译 Integrated Flow 目标文件
$(BUILD_LIB_DIR)/flow_%.o: apps/%.cpp
	@mkdir -p $(BUILD_LIB_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 链接 Integrated Flow 可执行文件
$(FLOW_BIN): $(LIB_OBJS) $(FILTERED_STA_OBJS) $(FILTERED_PLACER_OBJS) $(FLOW_OBJS)
	@mkdir -p $(BUILD_BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "MiniFlow 编译完成: $@"

# 只编译 MiniSTA
.PHONY: sta
sta: $(STA_BIN)

# 只编译 MiniPlacement
.PHONY: placement
placement: $(PLACER_BIN)

# 只编译 MiniRouter
.PHONY: router
router: $(ROUTER_BIN)

# 只编译 Integrated Flow
.PHONY: flow
flow: $(FLOW_BIN)

# 清理编译产物
.PHONY: clean
clean:
	rm -rf $(BUILD_LIB_DIR)/*.o $(BUILD_BIN_DIR)/*
	@echo "清理完成"

# 运行 MiniSTA (可以通过 ARGS 传递参数)
.PHONY: run-sta
run-sta: $(STA_BIN)
	$(STA_BIN) $(ARGS)

# 运行 MiniPlacement (可以通过 ARGS 传递参数)
.PHONY: run-placement
run-placement: $(PLACER_BIN)
	$(PLACER_BIN) $(ARGS)

# 运行 MiniRouter (可以通过 ARGS 传递参数)
.PHONY: run-router
run-router: $(ROUTER_BIN)
	$(ROUTER_BIN) $(ARGS)

# 显示帮助信息
.PHONY: help
help:
	@echo "可用的 make 目标："
	@echo "  all         - 编译所有应用程序 (默认)"
	@echo "  sta         - 只编译 MiniSTA"
	@echo "  placement   - 只编译 MiniPlacement"
	@echo "  router      - 只编译 MiniRouter"
	@echo "  flow        - 只编译 MiniFlow"
	@echo "  clean       - 清理所有编译产物"
	@echo "  run-sta     - 运行 MiniSTA (使用 ARGS=... 传递参数)"
	@echo "  run-placement - 运行 MiniPlacement (使用 ARGS=... 传递参数)"
	@echo "  run-router  - 运行 MiniRouter (使用 ARGS=... 传递参数)"
	@echo "  help        - 显示此帮助信息"