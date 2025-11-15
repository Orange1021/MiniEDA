# MiniEDA

一个轻量级的 EDA（电子设计自动化）工具链，用于数字电路的静态时序分析和芯片布局优化。

## 项目简介

MiniEDA 是一个教学和实验性质的 EDA 工具链项目，旨在实现数字集成电路设计流程中的关键功能。本项目采用 C++17 开发，提供清晰的代码结构和工业级的编程规范，适合用于学习 EDA 工具开发和数字电路设计。

## 主要功能

### 已实现

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

### 开发中

- **Verilog Parser**：Verilog 格式电路描述文件解析器
- **Netlist Database**：网表数据库管理系统
- **MiniSTA**：静态时序分析工具
- **MiniPlacement**：芯片布局优化工具

## 项目结构

```
MiniEDA/
├── lib/                    # 核心库
│   ├── include/           # 头文件
│   │   ├── cell.h         # 逻辑单元模型
│   │   ├── net.h          # 线网模型
│   │   ├── netlist_db.h   # 网表数据库（待实现）
│   │   └── verilog_parser.h  # Verilog 解析器（待实现）
│   └── src/               # 实现文件
│       ├── cell.cpp
│       ├── net.cpp
│       ├── netlist_db.cpp
│       └── verilog_parser.cpp
├── apps/                  # 应用程序
│   ├── mini_sta/         # 静态时序分析工具
│   └── mini_placement/   # 布局优化工具
├── benchmarks/           # 测试基准
│   └── ISCAS/           # ISCAS 标准测试集
├── build/               # 构建输出目录
│   ├── bin/            # 可执行文件
│   └── lib/            # 目标文件
└── Makefile            # 构建配置
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

## 使用方法

### MiniSTA（静态时序分析）

```bash
# 运行静态时序分析（开发中）
./build/bin/mini_sta <input_file>
```

### MiniPlacement（布局优化）

```bash
# 运行芯片布局优化（开发中）
./build/bin/mini_placement <input_file>
```

## 测试基准

项目包含 ISCAS（International Symposium on Circuits and Systems）标准测试集，提供多种规模的组合和时序电路用于验证工具功能。

## 技术特性

- **编程语言**：C++17
- **构建系统**：Makefile
- **编码规范**：遵循工业标准，使用命名空间、RAII、智能指针等现代 C++ 特性
- **数据结构**：高效的图结构表示电路拓扑
- **模块化设计**：清晰的层次结构，便于扩展和维护

## 开发路线

- [x] 基础数据模型（Cell, Pin, Net）
- [ ] Verilog 解析器
- [ ] 网表数据库管理
- [ ] 静态时序分析引擎
- [ ] 布局算法实现
- [ ] 性能优化与测试

## 贡献指南

欢迎提交 Issue 和 Pull Request！

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件

## 联系方式

如有问题或建议，欢迎通过 GitHub Issues 联系。

---

**注意**：本项目处于早期开发阶段，部分功能尚未实现。
