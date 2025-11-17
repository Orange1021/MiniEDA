# MiniEDA

一个轻量级的 EDA（电子设计自动化）工具链，用于数字电路的静态时序分析和芯片布局优化。

## 项目简介

MiniEDA 是一个教学和实验性质的 EDA 工具链项目，旨在实现数字集成电路设计流程中的关键功能。本项目采用 C++17 开发，提供清晰的代码结构和工业级的编程规范，适合用于学习 EDA 工具开发和数字电路设计。

## 主要功能

### ✅ 已实现

- **Cell 模块**：数字电路逻辑单元数据模型
  - 支持基本逻辑门（AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF）
  - 支持时序元件（DFF 触发器）
  - 支持组合逻辑（MUX 多路选择器）
  - I/O 端口管理
  - 引脚方向和时序参数

- **Net 模块**：电路连接（线网）数据模型
  - 驱动-负载连接管理
  - 扇出计算
  - 时序参数（线网电容、线延迟）
  - 时钟信号识别

- **NetlistDB 模块**：网表数据库管理系统
  - Cell/Net/Pin 容器管理
  - O(1) 快速查找（使用哈希表）
  - 拓扑查询（扇入/扇出）
  - 网表验证与统计
  - 42 个公共接口方法

- **VerilogParser 模块**：门级 Verilog 网表解析器
  - 词法分析（支持注释移除）
  - 语法分析（module/port/wire/instance）
  - 支持 15+ 种门类型（AND, OR, NAND, NOR, XOR, DFF 等）
  - 完整的错误报告系统
  - 成功解析 ISCAS s27 标准测试电路

### 🚧 开发中

- **MiniSTA**：静态时序分析工具
- **MiniPlacement**：芯片布局优化工具

## 项目结构

```
MiniEDA/
├── lib/                    # 核心库
│   ├── include/           # 头文件
│   │   ├── cell.h         # 逻辑单元模型
│   │   ├── net.h          # 线网模型
│   │   ├── netlist_db.h   # 网表数据库 ✅
│   │   └── verilog_parser.h  # Verilog 解析器 ✅
│   └── src/               # 实现文件
│       ├── cell.cpp
│       ├── net.cpp
│       ├── netlist_db.cpp    # 464 行
│       └── verilog_parser.cpp # 780 行
├── apps/                  # 应用程序
│   ├── mini_sta/         # 静态时序分析工具（待实现）
│   └── mini_placement/   # 布局优化工具（待实现）
├── build/                # 构建输出目录
│   ├── bin/             # 可执行文件
│   └── lib/             # 目标文件
├── test_netlist_db.cpp   # NetlistDB 测试程序
├── test_verilog_parser.cpp # VerilogParser 测试程序
└── Makefile             # 构建配置
```

## 编译与安装

### 前置要求

- C++17 兼容的编译器（推荐 g++ 7.0 或更高版本）
- GNU Make

### 编译步骤

```bash
# 克隆仓库
git clone https://github.com/Orange1021/MiniEDA.git
cd MiniEDA

# 编译项目
make

# 清理构建文件
make clean
```

编译成功后，可执行文件将生成在 `build/bin/` 目录下。

## 使用示例

### NetlistDB 测试

```bash
# 编译并运行 NetlistDB 测试
g++ -std=c++17 -I./lib/include test_netlist_db.cpp \
    build/lib/cell.o build/lib/net.o build/lib/netlist_db.o \
    -o build/bin/test_netlist_db

./build/bin/test_netlist_db
```

### ISCAS 基准测试

详细的 VerilogParser 测试包括多个 ISCAS 标准电路：

```bash
# 编译
make clean && make build/lib/cell.o build/lib/net.o build/lib/netlist_db.o build/lib/verilog_parser.o
g++ -std=c++17 -Wall -Wextra -I./lib/include \
    build/lib/cell.o build/lib/net.o build/lib/netlist_db.o build/lib/verilog_parser.o \
    test/test_verilog_parser.cpp -o build/bin/test_verilog_parser

# 运行测试
./build/bin/test_verilog_parser
```

**测试结果（ISCAS 基准测试集）：**

| 电路 | 门数 | 线网数 | 解析时间 | 状态 |
|------|------|--------|----------|------|
| s27  | 20   | 18     | 0.17 ms  | ✅ 通过 |
| s344 | 197  | 185    | 1.36 ms  | ✅ 通过 |
| s349 | 198  | 186    | 1.22 ms  | ✅ 通过 |
| s382 | 190  | 183    | 1.26 ms  | ✅ 通过 |

本项目使用 ISCAS（International Symposium on Circuits and Systems）标准测试集。
测试集包含真实工业电路，是 EDA 工具性能评估的黄金标准。

**测试集位置**：`benchmarks/ISCAS/Verilog/`

## 技术特性

### 核心特性
- **编程语言**：C++17
- **构建系统**：Makefile
- **代码规范**：遵循工业标准，使用命名空间、RAII、智能指针等现代 C++ 特性
- **数据结构**：高效的图结构表示电路拓扑，O(1) 查找
- **模块化设计**：清晰的层次结构，便于扩展和维护
- **完整注释**：所有代码采用英文注释，符合国际规范

### VerilogParser 增强功能（2025 更新）

经过工业级代码标准修复，VerilogParser 现已支持：

- **严格的语法校验**
  - ✅ strict_mode：检测未定义信号（高优先级修复）
  - ✅ 重复声明检测（wire/port 重复声明）
  - ✅ 端口与模块头部匹配校验
  - ✅ 标识符合法性检查（禁止数字开头）

- **增强的兼容性**
  - ✅ 大小写不敏感的门类型匹配（支持 nand/NAND/ND2）
  - ✅ 6 种基本门类型 + 多种变体（2/3/4 输入）
  - ✅ 时序元件支持（DFF）

- **完善的错误报告**
  - ✅ 6 种错误类型（语法、未定义信号、重复声明等）
  - ✅ 精确的行号和列号定位
  - ✅ 清晰的错误描述信息

- **容错与恢复**
  - ✅ 遇到错误时继续解析（跳过错误实例）
  - ✅ 详细的警告信息（引脚数不匹配等）

## 开发路线

### ✅ 已完成（核心基础）
- [x] 基础数据模型（Cell, Pin, Net）- 530 行
- [x] 网表数据库管理（NetlistDB）- 464 行
- [x] Verilog 门级网表解析器（VerilogParser）- 820+ 行
  - [x] 语法校验增强（6 大核心问题修复）
  - [x] ISCAS 标准测试集验证（4 个电路 100% 通过率）
  - [x] 大小写兼容性支持

### 🚧 下一阶段
- [ ] 网表验证算法增强（处理浮动引脚）
- [ ] 静态时序分析引擎（TimingEngine）
- [ ] 布局算法实现（PlacementEngine）
- [ ] 时序约束文件解析（SDC）

### 📅 未来规划
- [ ] 时序优化
- [ ] 面积优化
- [ ] 功耗分析
- [ ] GUI 可视化界面

## 项目结构

```
MiniEDA/
├── lib/                    # 核心库（工业级代码）
│   ├── include/           # 头文件
│   │   ├── cell.h         # 逻辑单元模型
│   │   ├── net.h          # 线网模型
│   │   ├── netlist_db.h   # 网表数据库
│   │   └── verilog_parser.h  # Verilog 解析器（增强版）
│   └── src/               # 实现文件
│       ├── cell.cpp
│       ├── net.cpp
│       ├── netlist_db.cpp
│       └── verilog_parser.cpp  # 820+ 行，工业级标准
├── test/                  # 测试程序
│   ├── test_netlist_db.cpp      # NetlistDB 测试
│   ├── test_verilog_parser.cpp  # 完整 ISCAS 测试套件
│   └── test_repaired_features.cpp # 修复功能专项测试
├── apps/                  # 应用程序（待实现）
│   ├── mini_sta/         # 静态时序分析工具
│   └── mini_placement/   # 布局优化工具
├── benchmarks/           # 测试基准
│   └── ISCAS/            # ISCAS 标准测试集
│       └── Verilog/      # Verilog 格式电路
├── build/                # 构建输出目录
│   ├── bin/             # 可执行文件
│   └── lib/             # 目标文件
└── Makefile             # 构建配置
```

## 统计信息

| 模块 | 代码行数 | 功能 | 状态 | 备注 |
|------|---------|------|------|------|
| Cell | 170 行 | 逻辑单元模型 | ✅ 完成 | 支持 9 种单元类型 |
| Net | 180 行 | 线网模型 | ✅ 完成 | 拓扑连接管理 |
| NetlistDB | 464 行 | 数据库管理 | ✅ 完成 | 42 个公共接口 |
| VerilogParser | 820+ 行 | Verilog 解析 | ✅ 增强 | 工业级标准修复 |
| **总计** | **1600+ 行** | **核心基础** | ✅ **稳定** | 已验证 |

## 贡献指南

欢迎提交 Issue 和 Pull Request！本项目采用工业级 C++ 编码规范：

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/your-feature`)
3. 遵循现有的代码规范（命名空间、RAII、英文注释）
4. 提交更改 (`git commit -m 'Add your feature'`)
5. 推送到分支 (`git push origin feature/your-feature`)
6. 开启 Pull Request
7. 确保测试通过（ISCAS 测试集）

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 联系方式

如有问题、建议或发现 bug，欢迎通过 GitHub Issues 联系。

---

**项目状态**：✅ 核心模块（NetlistDB + VerilogParser）已完成，通过 ISCAS 标准测试集验证，可解析真实电路网表并构建数据库。

**最新更新**：2025 - VerilogParser 修复 6 个核心语法校验问题，增强错误报告，支持大小写不敏感匹配
