#!/bin/bash

# 困难电路测试脚本
# 作者: MiniEDA
# 用途: 批量测试大规模电路
# 布线层数: 8层

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== MiniEDA 困难电路测试 ===${NC}"

# 编译项目
echo -e "${YELLOW}编译项目...${NC}"
make BUILD_MODE=release -j$(nproc)
if [ $? -ne 0 ]; then
    echo -e "${RED}编译失败！${NC}"
    exit 1
fi
echo -e "${GREEN}编译成功！${NC}"

# 测试电路列表
circuits=("s5378" "s9234")

# 运行每个电路
for circuit in "${circuits[@]}"; do
    echo -e "${YELLOW}========================================${NC}"
    echo -e "${YELLOW}运行电路: $circuit${NC}"
    echo -e "${YELLOW}========================================${NC}"
    
    # 运行完整流程（包含布局和布线），输出保存到日志文件
    # 使用8层布线（适合大规模电路）
    ./build/release/bin/mini_flow \
        -v benchmarks/ISCAS/Verilog/${circuit}.v \
        -lib benchmarks/NangateOpenCellLibrary_typical.lib \
        -lef benchmarks/NangateOpenCellLibrary.macro.lef \
        -num_layers 8 \
        > log/${circuit}.log 2>&1
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}$circuit 运行完成！${NC}"
    else
        echo -e "${RED}$circuit 运行失败！${NC}"
    fi
    
    echo ""
done

echo -e "${GREEN}=== 所有困难电路测试完成 ===${NC}"