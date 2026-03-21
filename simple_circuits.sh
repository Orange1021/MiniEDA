#!/bin/bash

# 简单电路测试脚本
# 作者: MiniEDA
# 用途: 批量测试小规模电路
# 布线层数: 3层

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MiniEDA 简单电路测试 ===${NC}"

# 编译项目
echo -e "${YELLOW}编译项目...${NC}"
cmake --preset release
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake 配置失败！${NC}"
    exit 1
fi

cmake --build --preset release -j$(nproc)
if [ $? -ne 0 ]; then
    echo -e "${RED}CMake 编译失败！${NC}"
    exit 1
fi
echo -e "${GREEN}编译成功！${NC}"

MINI_FLOW_BIN="./build/cmake/release/bin/mini_flow"
if [ ! -x "$MINI_FLOW_BIN" ]; then
    echo -e "${RED}找不到可执行文件: $MINI_FLOW_BIN${NC}"
    exit 1
fi

mkdir -p log

# 测试电路列表
circuits=("s27" "s344" "s349" "s526")

# 运行每个电路
for circuit in "${circuits[@]}"; do
    echo -e "${YELLOW}========================================${NC}"
    echo -e "${YELLOW}运行电路: $circuit${NC}"
    echo -e "${YELLOW}========================================${NC}"
    
    # 运行完整流程（包含布局和布线），输出保存到日志文件
    "$MINI_FLOW_BIN" \
        -v benchmarks/ISCAS/Verilog/${circuit}.v \
        -lib benchmarks/NangateOpenCellLibrary_typical.lib \
        -lef benchmarks/NangateOpenCellLibrary.macro.lef \
        > log/${circuit}.log 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}$circuit 运行完成！${NC}"
    else
        echo -e "${RED}$circuit 运行失败！${NC}"
    fi
    
    echo ""
done

echo -e "${GREEN}=== 所有测试完成 ===${NC}"
