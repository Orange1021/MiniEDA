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

# 核心库源文件
LIB_SRCS := $(wildcard $(LIB_SRC_DIR)/*.cpp)
LIB_OBJS := $(patsubst $(LIB_SRC_DIR)/%.cpp, $(BUILD_LIB_DIR)/%.o, $(LIB_SRCS))

# MiniSTA 源文件
STA_SRCS := $(wildcard $(STA_DIR)/*.cpp)
STA_OBJS := $(patsubst $(STA_DIR)/%.cpp, $(BUILD_LIB_DIR)/sta_%.o, $(STA_SRCS))

# MiniPlacement 源文件
PLACER_SRCS := $(wildcard $(PLACER_DIR)/*.cpp)
PLACER_OBJS := $(patsubst $(PLACER_DIR)/%.cpp, $(BUILD_LIB_DIR)/placer_%.o, $(PLACER_SRCS))

# 可执行文件
STA_BIN := $(BUILD_BIN_DIR)/mini_sta
PLACER_BIN := $(BUILD_BIN_DIR)/mini_placement

# 默认目标：编译所有
.PHONY: all
all: $(STA_BIN) $(PLACER_BIN)

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

# 只编译 MiniSTA
.PHONY: sta
sta: $(STA_BIN)

# 只编译 MiniPlacement
.PHONY: placement
placement: $(PLACER_BIN)

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

# 显示帮助信息
.PHONY: help
help:
	@echo "可用的 make 目标："
	@echo "  all         - 编译所有应用程序 (默认)"
	@echo "  sta         - 只编译 MiniSTA"
	@echo "  placement   - 只编译 MiniPlacement"
	@echo "  clean       - 清理所有编译产物"
	@echo "  run-sta     - 运行 MiniSTA (使用 ARGS=... 传递参数)"
	@echo "  run-placement - 运行 MiniPlacement (使用 ARGS=... 传递参数)"
	@echo "  help        - 显示此帮助信息"