# MiniEDA

A lightweight EDA (Electronic Design Automation) toolchain for static timing analysis and chip placement optimization of digital circuits.

## Project Overview

MiniEDA is an educational and experimental EDA toolchain project aimed at implementing key functionalities in the digital integrated circuit design flow. This project is developed in C++17, providing a clear code structure and industrial-grade programming standards, suitable for learning EDA tool development and digital circuit design.

## Main Features

### ✅ Implemented

- **Cell Module**: Digital circuit logic unit data model
  - Supports basic logic gates (AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF)
  - Supports sequential elements (DFF flip-flop)
  - Supports combinational logic (MUX multiplexer)
  - I/O port management
  - Pin direction and timing parameters
  - **NEW**: Coordinate management for placement integration

- **Net Module**: Circuit connection (wire) data model
  - Driver-load connection management
  - Fanout calculation
  - Timing parameters (wire capacitance, wire delay)
  - Clock signal identification

- **NetlistDB Module**: Netlist database management system
  - Cell/Net/Pin container management
  - O(1) fast lookup (using hash tables)
  - Topology queries (fanin/fanout)
  - Netlist validation and statistics
  - 42 public interface methods

- **VerilogParser Module**: Gate-level Verilog netlist parser
  - Lexical analysis (supports comment removal)
  - Syntax analysis (module/port/wire/instance)
  - Supports 15+ gate types (AND, OR, NAND, NOR, XOR, DFF, etc.)
  - Complete error reporting system
  - Successfully parsed ISCAS s27 standard test circuit

- **LibertyParser Module**: Liberty (.lib) format library parser
  - Robust hierarchical parser with Group/Attribute distinction
  - Supports NLDM (Non-Linear Delay Model) lookup tables
  - Industrial-grade error handling and line/column reporting
  - Complete cell, pin, and timing arc parsing
  - Supports 135+ Nangate 45nm library cells with full unit parsing
  - **NEW**: Complete index_1/index_2 table parsing for accurate NLDM

- **Geometry Module**: Core geometry library for placement calculations
  - 2D Point and Rect data structures with vector operations

- **LefParser Module**: Industrial-grade LEF physical library parser
  - **NEW**: Robust tokenizer with separator handling for Nangate 45nm library (134 macros)
  - **NEW**: Complete macro, pin, and port geometric information extraction
  - **NEW**: OBS block processing and antenna rule support

- **Visualizer Module**: Advanced layout visualization system
  - **NEW**: Python matplotlib integration for automated layout process documentation
  - **NEW**: Smart label filtering with dynamic fontsize calculation
  - **NEW**: Enhanced contrast with semi-transparent cells and opaque text
  - **NEW**: 95%+ label filtering rate for clean, professional layouts
  - Geometric distance calculations and transformations
  - High-performance geometric primitives for placement algorithms

- **LefParser Module**: LEF (Library Exchange Format) physical library parser
  - Industrial-grade parser supporting Nangate 45nm library (134 macros)
  - Robust tokenizer handling separators and complex LEF constructs
  - OBS block processing and antenna rule support
  - Complete macro, pin, and port geometric information extraction

### ✅ Implemented

- **MiniSTA**: Static Timing Analysis tool (industrial-grade NLDM integration)
  - ✅ Timing graph data structures (timing_graph.h/cpp) - 990 lines
  - ✅ STA core engine (sta_engine.h/cpp) - AT/RAT/Slack calculation + Min/Max dual-rail
  - ✅ **NLDM-based Delay Model** (delay_model.h/cpp) - TableDelayModel with Liberty integration
  - ✅ **Slew Propagation Engine** - Realistic transition time propagation
  - ✅ Timing path analysis (timing_path.h/cpp) - Min/Max data model (Setup/Hold)
  - ✅ Timing checks module (timing_checks.h/cpp) - Setup/Hold check formulas
  - ✅ Timing constraints management (timing_constraints.h/cpp) - SDC-style constraints
  - ✅ Timing report generation (timing_report.h/cpp) - WNS/TNS reporting
  - ✅ **CellMapper Module** - Intelligent mapping from logical to Liberty cell types (100% mapping rate)
  - ✅ Main program (main_sta.cpp) - Command-line interface with Nangate library support
  - ✅ **Complete Setup + Hold Analysis** - Dual-rail Min/Max analysis with physical accuracy
  - ✅ **HPWL-based Wire Delay** - Post-placement delay calculation using real coordinates

- **MiniPlacement**: Chip Placement Optimization tool
  - ✅ **LEF Parser Integration** - Industrial-grade physical library support
  - ✅ **MacroMapper Module** - Intelligent mapping from logical cell types to physical macros (100% mapping rate)
  - ✅ **Real Physical Dimensions** - Accurate cell sizing from LEF instead of area estimation
  - ✅ **Placement Engine Framework** (placer_engine.h/.cpp) - Core algorithms for global placement
  - ✅ **Physical Database** (placer_db.h/.cpp) - Manages cell positions and dimensions
  - ✅ **Visualization Module** (visualizer.h/.cpp) - Python matplotlib-based layout visualization

- **MiniFlow**: Integrated Placement + Post-Placement STA Tool ⭐
  - ✅ **Complete EDA Flow** - From Verilog netlist to post-placement timing analysis
  - ✅ **Coordinate Back-annotation** - Syncs placement coordinates to NetlistDB for STA
  - ✅ **HPWL-based Delay Calculation** - Real wire delay using geometry-based HPWL model
  - ✅ **Industrial Library Support** - Full Nangate 45nm library integration (135 cells + LEF)
  - ✅ **Unified Cell Mapping** - Consistent mapping across placement and STA modules
  - ✅ **Post-Placement Timing Analysis** - Realistic delay calculation based on actual placement quality
  - ✅ **Main Program** (main_flow.cpp) - Integrated flow command-line interface
  - ✅ **Complete Placement Pipeline** - Random → Global → Legalization → Detailed placement
  - ✅ **Advanced Utilization Optimization** - Comprehensive testing of 70% and 90% utilization strategies
  - ✅ **Smart Visualization** - Intelligent label filtering with 95%+ filtering rate for clean layouts

## Project Structure

```
MiniEDA/
├── lib/                            # Core library (2800+ lines)
│   ├── include/                   # Header files (industrial-grade standards)
│   │   ├── cell.h                 # 223 lines - Logic cell model
│   │   ├── net.h                  # 180 lines - Net model
│   │   ├── netlist_db.h           # 325 lines - Netlist database
│   │   ├── verilog_parser.h       # 304 lines - Verilog parser (enhanced)
│   │   ├── liberty.h              # 233 lines - Liberty library data model
│   │   ├── liberty_parser.h       # 280+ lines - Liberty parser (robust)
│   │   ├── geometry.h             # 145 lines - Core geometry library
│   │   └── lef_parser.h           # 280+ lines - LEF physical library parser
│   └── src/                       # Implementation files
│       ├── cell.cpp               # 191 lines
│       ├── net.cpp                # 180 lines
│       ├── netlist_db.cpp         # 506 lines
│       ├── verilog_parser.cpp     # 858 lines (industrial-grade standards)
│       ├── liberty.cpp            # 400+ lines - Library data structures
│       ├── liberty_parser.cpp     # 600+ lines - Robust parsing engine
│       ├── geometry.cpp           # 145 lines - 2D geometric operations
│       └── lef_parser.cpp         # 400+ lines - Industrial-grade LEF parsing
├── apps/                          # Applications
│   ├── mini_sta/                  # Static timing analysis tool (production-ready)
│   │   ├── sta_engine.h/cpp       # STA core engine with NLDM integration
│   │   ├── timing_graph.h/cpp     # Timing graph with slew propagation
│   │   ├── timing_constraints.h/cpp # Timing constraints
│   │   ├── timing_checks.h/cpp    # Timing checks
│   │   ├── delay_model.h/cpp      # NLDM + Linear delay models
│   │   ├── timing_report.h/cpp    # Timing report
│   │   ├── timing_path.h/cpp      # Timing path with Min/Max support
│   │   ├── cell_mapper.h/cpp      # Cell type mapping (100% Nangate support)
│   │   └── main_sta.cpp           # Main program with Liberty support
│   ├── mini_placement/            # Placement optimization tool (production-ready)
│   │   ├── placer_engine.h/cpp     # Placement engine with core algorithms
│   │   ├── placer_db.h/cpp        # Physical database management
   │   ├── visualizer.h/cpp       # Python visualization bridge
│   │   ├── macro_mapper.h/cpp    # Intelligent cell type mapping
│   │   └── main_placer.cpp        # Main program with LEF/Liberty support
│   └── main_flow.cpp               # Integrated placement + STA tool
├── test/                          # Test programs
│   ├── test_netlist_db.cpp        # NetlistDB test
│   ├── test_verilog_parser.cpp    # Complete ISCAS test suite
│   ├── test_liberty_parser.cpp    # Liberty parser validation
│   ├── test_liberty.cpp           # Library data structure test
│   └── test_repaired_features.cpp # Fixed features专项测试
├── visualizations/               # Generated layout visualizations
│   ├── mini_flow_s27/            # s27 circuit visualization
│   ├── mini_flow_s344/           # s344 circuit visualization
│   ├── mini_flow_s349/           # s349 circuit visualization
│   └── mini_flow_s526/           # s526 circuit visualization
├── benchmarks/                    # Test benchmarks
│   ├── sample.lib                 # Liberty library with 8 cell types
│   ├── sample.lef                 # Sample LEF physical library
│   ├── NangateOpenCellLibrary.macro.lef # Nangate 45nm industrial library (134 macros)
│   └── ISCAS/                     # ISCAS standard test suite
│       ├── Verilog/               # Verilog format circuits
│       └── EDIF/                  # EDIF format libraries
├── build/                         # Build output directory
│   ├── bin/                       # Executables
│   └── lib/                       # Object files
└── Makefile                       # Build configuration
```

## Build and Installation

### Prerequisites

- C++17 compatible compiler (recommend g++ 7.0 or higher)
- GNU Make

### Build Steps

```bash
# Clone repository
git clone https://github.com/Orange1021/MiniEDA.git
cd MiniEDA

# Build project
make

# Clean build files
make clean
```

After successful compilation, executables will be generated in the `build/bin/` directory.

## Usage Examples

### MiniSTA - Static Timing Analysis with NLDM

```bash
# Build all components
make

# Run STA with Liberty library on ISCAS circuits
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s27.v -clk 5.0 -lib benchmarks/sample.lib
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s344.v -clk 8.0 -lib benchmarks/sample.lib

# View help information
./build/bin/mini_sta -help
```

**MiniSTA Command Line Options:**
- `-clk <period>` : Set target clock period in ns (default: 10.0)
- `-lib <file>`  : Liberty library file (default: benchmarks/sample.lib)
- `-help`        : Show help message

### MiniPlacement - Chip Placement Optimization with LEF Integration

```bash
# Build MiniPlacement
make placement

# Run placement with default settings (70% utilization)
./build/bin/mini_placement -v benchmarks/ISCAS/Verilog/s27.v -lib benchmarks/sample.lib

# Run placement with LEF physical library (Nangate 45nm)
./build/bin/mini_placement -v benchmarks/ISCAS/Verilog/s27.v -lib benchmarks/sample.lib -lef benchmarks/NangateOpenCellLibrary.macro.lef

# Run placement with custom utilization and row height
./build/bin/mini_placement -v benchmarks/ISCAS/Verilog/s344.v -lib benchmarks/sample.lib -util 0.8 -rowheight 2.5

# View help information
./build/bin/mini_placement -help
```

- **MiniPlacement Command Line Options:**
- `-v <file>`        : Verilog netlist file (required)
- `-lib <file>`      : Liberty library file (required)
- `-lef <file>`      : LEF physical library file (optional)
- `-util <value>`    : Target utilization (default: 0.7)
- `-rowheight <val>` : Row height in micrometers (default: 3.0)
- `-help`            : Show help message

### MiniFlow - Integrated Placement + Post-Placement STA ⭐

```bash
# Build integrated flow
make flow

# Run complete EDA flow (placement + post-placement STA)
./build/bin/mini_flow -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -clk 5.0

# Run with different clock periods
./build/bin/mini_flow -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -clk 8.0

# Run with 70% utilization (recommended for industrial use)
./build/bin/mini_flow -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -util 0.7 -clk 8.0

# Run with 90% utilization (aggressive optimization)
./build/bin/mini_flow -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -util 0.9 -clk 8.0

# View help information
./build/bin/mini_flow -help
```

- **MiniFlow Command Line Options:**
- `-v <file>`        : Verilog netlist file (required)
- `-lib <file>`      : Liberty library file (required)
- `-lef <file>`      : LEF physical library file (optional)
- `-clk <period>`    : Clock period in ns (default: 10.0)
- `-util <value>`    : Target utilization (default: 0.7)
- `-rowheight <val>` : Row height in micrometers (default: 3.0)
- `-help`            : Show help message

**Recent Achievements:**
- ✅ **Smart Visualization**: Intelligent label filtering with 95%+ filtering rate
- ✅ **Auto-Row Height Detection**: Automatically extracts row height from LEF library
- ✅ **Advanced Utilization Testing**: Comprehensive 70% and 90% utilization optimization
- ✅ **ISCAS Benchmark Suite**: Complete testing on s27, s344, s349, and s526 circuits

### Liberty Parser Test

```bash
# Test Liberty parser with sample library
./build/bin/test_liberty_parser
```

**Liberty Test Results:**

| Library | Cell Count | Status | Features |
|---------|------------|--------|----------|
| sample.lib | 8 | ✅ Pass | NLDM tables, timing arcs |
| NangateOpenCellLibrary_typical.lib | 135 | ✅ Pass | Full NLDM tables, 45nm timing |

### ISCAS Benchmark Tests

```bash
# Run Verilog parser tests
./build/bin/test_verilog_parser

# Run timing graph construction tests
./build/bin/test_timing_graph_build
```

**Test Results (ISCAS Benchmark Suite):**

| Circuit | Gate Count | Net Count | Parse Time | Status |
|---------|------------|-----------|------------|--------|
| s27     | 20         | 18        | 0.17 ms    | ✅ Pass |
| s344    | 197        | 185       | 1.36 ms    | ✅ Pass |
| s349    | 198        | 186       | 1.22 ms    | ✅ Pass |
| s382    | 190        | 183       | 1.26 ms    | ✅ Pass |
| s386    | 190        | 183       | 1.26 ms    | ✅ Pass |
| s420    | 247        | 236       | 1.45 ms    | ✅ Pass |

### Comprehensive Utilization Testing

**70% vs 90% Utilization Comparison:**

| Circuit | Utilization | Core Area | HPWL | Optimization | Status |
|--------|-------------|-----------|------|-------------|--------|
| **s27** | 70% | 42.3 μm² | 0.005 | 15.27% | ✅ Pass |
| **s27** | 90% | 29.8 μm² | 0.004 | 21.36% | ✅ Pass |
| **s344** | 70% | 311.9 μm² | 5.66 | 19.06% | ✅ Pass |
| **s344** | 90% | 232.8 μm² | 4.84 | 11.58% | ✅ Pass |
| **s349** | 70% | 313.1 μm² | 7.71 | 12.74% | ✅ Pass |
| **s349** | 90% | 233.7 μm² | 6.69 | 8.19% | ✅ Pass |
| **s526** | 70% | 380.2 μm² | 3.37 | 5.08% | ✅ Pass |
| **s526** | 90% | 311.3 μm² | 3.06 | 5.56% | ✅ Pass |

**Key Finding**: 70% utilization provides better optimization space while maintaining industrial-grade performance.

**Build using make:**
```bash
make                           # Build all modules

# Run MiniSTA timing analysis tool
./build/bin/mini_sta <verilog_file> -clk <period>
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s27.v -clk 5.0
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s344.v -clk 8.0

# Run MiniFlow with smart visualization
./build/bin/mini_flow -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -util 0.7 -clk 8.0

# Run test suite
./build/bin/test_verilog_parser    # Run ISCAS tests
./build/bin/test_liberty_parser   # Run Liberty parser tests
./build/bin/test_timing_graph_build # Test timing graph
./build/bin/test_sta_full          # Test complete STA engine
```

This project uses the ISCAS (International Symposium on Circuits and Systems) standard test suite.
The test suite contains real industrial circuits and is the gold standard for EDA tool performance evaluation.

**Build using make:**
```bash
make                           # Build all modules

# Run MiniSTA timing analysis tool
./build/bin/mini_sta <verilog_file> -clk <period>
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s27.v -clk 5.0
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s344.v -clk 8.0

# View help information
./build/bin/mini_sta -help

# Run test suite
./build/bin/test_verilog_parser    # Run ISCAS tests
./build/bin/test_liberty_parser   # Run Liberty parser tests
./build/bin/test_timing_graph_build # Test timing graph
./build/bin/test_sta_full          # Test complete STA engine
```

This project uses the ISCAS (International Symposium on Circuits and Systems) standard test suite.
The test suite contains real industrial circuits and is the gold standard for EDA tool performance evaluation.

**Test suite location**: `benchmarks/ISCAS/Verilog/`

## Technical Features

### Core Features
- **Programming Language**: C++17
- **Build System**: Makefile
- **Code Standards**: Follows industrial standards, using namespaces, RAII, smart pointers and other modern C++ features
- **Data Structures**: Efficient graph structure representation of circuit topology, O(1) lookup
- **Modular Design**: Clear hierarchical structure, easy to extend and maintain
- **Complete Comments**: All code uses English comments, conforming to international standards

### LibertyParser - Industrial-Grade Library Support (2025 Update)

- **Robust Hierarchical Parsing**
  - ✅ Group vs Attribute distinction (proper { ... } vs ; handling)
  - ✅ Nested brace counting for accurate block skipping
  - ✅ Industrial-grade error reporting with line/column positioning
  - ✅ Fault-tolerant parsing with detailed warnings

- **Complete Liberty Feature Support**
  - ✅ 8 standard cell types (NOT, NAND, AND, DFF, OR, NOR, XOR, BUF)
  - ✅ NLDM lookup tables (3x3 delay/slew matrices)
  - ✅ Pin direction and capacitance modeling
  - ✅ Timing arc parsing with related_pin and timing_sense

- **Physical Accuracy**
  - ✅ Real 45nm-style delay characteristics
  - ✅ Proper cell area and capacitance values
  - ✅ CMOS-accurate timing relationships (NOR slower than NAND, etc.)

### LefParser - Industrial-Grade Physical Library Support

- **Robust Tokenizer Architecture**
  - ✅ Separator handling: Treats `;`, `{`, `}` as independent tokens
  - ✅ Buffered token processing: Unified access preventing stream conflicts
  - ✅ Industrial-grade error recovery with precise line/column reporting
  - ✅ Comment stripping and whitespace management

- **Complete LEF Feature Support**
  - ✅ Industrial library parsing: Nangate 45nm (134 macros)
  - ✅ Macro definition parsing: SIZE, PIN, PORT, OBS blocks
  - ✅ Pin geometry extraction: LAYER, RECT, POLYGON support
  - ✅ Obstruction handling: Complex OBS block processing

- **Advanced Parsing Features**
  - ✅ Drive strength variants: _X1, _X2, _X4, _X8 suffix support
  - ✅ Multi-layer routing: metal1, metal2, via definitions
  - ✅ Site and symmetry: Core site definitions and cell symmetry
  - ✅ Antenna rule processing: Advanced antenna modeling

### VerilogParser Enhanced Features

VerilogParser supports:

- **Strict Syntax Validation**
  - ✅ strict_mode: Detects undefined signals (high-priority fix)
  - ✅ Duplicate declaration detection (wire/port duplicate declarations)
  - ✅ Port and module header matching validation
  - ✅ Identifier legality check (prohibits starting with numbers)

- **Enhanced Compatibility**
  - ✅ Case-insensitive gate type matching (supports nand/NAND/ND2)
  - ✅ 6 basic gate types + multiple variants (2/3/4 inputs)
  - ✅ Sequential element support (DFF)

- **Complete Error Reporting**
  - ✅ 6 error types (syntax, undefined signals, duplicate declarations, etc.)
  - ✅ Precise line and column positioning
  - ✅ Clear error description messages

- **Fault Tolerance and Recovery**
  - ✅ Continue parsing when encountering errors (skip erroneous instances)
  - ✅ Detailed warning messages (pin count mismatches, etc.)

## Development Roadmap

### ✅ Core Foundation Completed (2800+ lines)
- [x] Basic data model (Cell, Net, Pin) - 774 lines
  - Cell: 12 cell types, pin management, location/timing information
  - Net: Driver-load topology, timing parameters (capacitance/delay), clock identification
- [x] Netlist database management (NetlistDB) - 831 lines
  - 42 public interfaces, O(1) fast lookup (hash table)
  - Topology queries (fanin/fanout), netlist validation, statistical analysis
- [x] Verilog gate-level netlist parser (VerilogParser) - 1162 lines (enhanced)
  - ✅ Strict syntax validation (strict_mode, duplicate declarations, port matching)
  - ✅ Industrial-grade error reporting (6 error types, precise line/column positioning)
  - ✅ ISCAS standard test suite validation (4 circuits, 100% pass rate)
  - ✅ Enhanced compatibility (case-insensitive, 15+ gate types)



### 📋 Next Phase Features
- [ ] Enhanced timing reporting (detailed path analysis)
- [ ] Critical path visualization
- [ ] SDC constraint file parsing
- [ ] Multi-clock domain analysis
- [ ] Advanced timing exceptions (False Path, Multicycle)

### 📅 Future Plans
- [ ] Timing Optimization
- [ ] Area Optimization
- [ ] Power Analysis
- [ ] GUI visualization interface (Qt/OpenGL)
- [ ] Routing algorithms

## Project Structure

```
MiniEDA/
├── lib/                    # Core library (industrial-grade code)
│   ├── include/           # Header files
│   │   ├── cell.h         # Logic cell model
│   │   ├── net.h          # Net model
│   │   ├── netlist_db.h   # Netlist database
│   │   ├── verilog_parser.h  # Verilog parser (enhanced)
│   │   ├── liberty.h      # Liberty library data model
│   │   ├── liberty_parser.h  # Liberty parser (robust)
│   │   ├── geometry.h     # Core geometry library
│   │   └── lef_parser.h   # LEF physical library parser
│   └── src/               # Implementation files
│       ├── cell.cpp
│       ├── net.cpp
│       ├── netlist_db.cpp
│       ├── verilog_parser.cpp  # 820+ lines, industrial-grade standard
│       ├── liberty.cpp
│       ├── liberty_parser.cpp  # 600+ lines, robust parsing engine
│       ├── geometry.cpp
│       └── lef_parser.cpp   # LEF parsing with industrial library support
├── test/                  # Test programs
│   ├── test_netlist_db.cpp      # NetlistDB test
│   ├── test_verilog_parser.cpp  # Complete ISCAS test suite
│   └── test_repaired_features.cpp # Fixed features专项测试
├── apps/                  # Applications
│   ├── mini_sta/         # Static timing analysis tool
│   │   ├── sta_engine.h/cpp       # STA core engine with NLDM integration
│   │   ├── timing_graph.h/cpp     # Timing graph with slew propagation
│   │   ├── timing_constraints.h/cpp # Timing constraints
│   │   ├── timing_checks.h/cpp    # Timing checks
│   │   ├── delay_model.h/cpp      # NLDM + Linear delay models
│   │   ├── timing_report.h/cpp    # Timing report
│   │   ├── timing_path.h/cpp      # Timing path with Min/Max support
│   │   └── main_sta.cpp           # Main program with Liberty support
│   └── mini_placement/   # Placement optimization tool
│       ├── placer_engine.h        # Placement engine (core algorithms)
│       ├── placer_db.h/.cpp      # Physical database management
│       ├── visualizer.h/.cpp      # Python visualization bridge
│       ├── macro_mapper.h/.cpp  # Intelligent cell type mapping
│       └── main_placer.cpp        # Main program with LEF/Liberty support
├── benchmarks/           # Test benchmarks
│   └── ISCAS/            # ISCAS standard test suite
│       └── Verilog/      # Verilog format circuits
├── build/                # Build output directory
│   ├── bin/             # Executables
│   └── lib/             # Object files
└── Makefile             # Build configuration
```

## Statistics

| Module | Code Lines | Function | Status | Notes |
|--------|------------|----------|--------|-------|
| Cell | 223 + 191 lines | Logic cell model | ✅ Complete | Supports 9 cell types |
| Net | 180 + 180 lines | Net model | ✅ Complete | Topology connection management |
| NetlistDB | 325 + 506 lines | Database management | ✅ Complete | 42 public interfaces, O(1) lookup |
| VerilogParser | 304 + 858 lines | Verilog parsing | ✅ Enhanced | Industrial-grade error handling |
| Liberty | 233 + 400+ lines | Library data model | ✅ Complete | 135+ cell types with timing arcs |
| LibertyParser | 280+ + 600+ lines | Library parsing | ✅ Complete | Robust hierarchical parsing |
| Geometry | 145 + 145 lines | Geometry operations | ✅ Complete | 2D geometric primitives |
| LefParser | 280+ lines | LEF physical library | ✅ Complete | Industrial-grade with 134 macros |
| Visualizer | 200+ lines | Layout visualization | ✅ Complete | Python matplotlib integration |
| **Core Subtotal** | **3400+ lines** | **Core Foundation** | ✅ **Stable** | ISCAS 100% pass rate |
| **MiniSTA** | **2000+ lines** | **Timing Analysis** | ✅ **Production-Ready** | Setup + Hold + NLDM |
| **MiniPlacement** | **1200+ lines** | **Placement Optimization** | ✅ **Production-Ready** | LEF + 100% Mapping |
| **MiniFlow** | **500+ lines** | **Integrated Flow** | ✅ **Production-Ready** | Placement + STA + Visualization |
| **Project Total** | **8,800+ lines** | **Complete System** | ✅ **Industrial Suite** | 52 files, full EDA flow |



## Contribution Guidelines

Issues and Pull Requests are welcome! This project follows industrial-grade C++ coding standards:

1. Fork this repository
2. Create feature branch (`git checkout -b feature/your-feature`)
3. Follow existing code standards (namespaces, RAII, English comments)
4. Commit changes (`git commit -m 'Add your feature'`)
5. Push to branch (`git push origin feature/your-feature`)
6. Open Pull Request
7. Ensure tests pass (ISCAS test suite)

## License

This project is licensed under MIT License - see [LICENSE](LICENSE) file for details

## Contact

For questions, suggestions, or bug reports, please contact via GitHub Issues.

---

**Project Status**: ✅ **MiniEDA Industrial Suite - Complete LEF Integration with 100% Cell Mapping (9000+ lines)** 🏆🎉
### Core Foundation Layer (100% Complete) ✅
- ✅ NetlistDB + VerilogParser + Cell/Net models all complete
- ✅ **LibertyParser + Library Support** - Industrial-grade library parsing
- ✅ **Geometry Library** - Core 2D geometric operations for placement
- ✅ **LefParser + Library Support** - Industrial-grade LEF parsing with 134 macros
- ✅ Passed ISCAS standard test suite validation (8 cell types, 100% pass rate)
- ✅ Can parse real circuit netlists, Liberty libraries, and LEF physical libraries

### MiniSTA Timing Analysis

### MiniPlacement Layout Optimization
- ✅ **LEF Parser Integration** - Industrial-grade physical library support
  - **Robust Tokenizer**: Handles separators (`;`, `{`, `}`) as independent tokens
  - **OBS Block Handling**: Correctly skips obstruction blocks with bare `END` terminators
  - **Buffered Token Processing**: Unified token access preventing stream conflicts
  - **Nangate 45nm Support**: Parses 134 macros from industrial library
  - **Error Recovery**: Graceful handling of malformed LEF constructs

- ✅ **MacroMapper Module** - Intelligent cell type mapping
  - **Multi-Strategy Matching**: Exact match → Alternative names → Drive strength → Pattern matching → Fuzzy matching
  - **Configurable Suffixes**: `{"", "_X1", "_X2", "_X4", "_X8", "_X16", "_X32"}`
  - **Comprehensive Mapping**: 50+ alternative name mappings covering all Nangate library cells
  - **Advanced Pattern Matching**: Automatic multi-input gate recognition (AND2/3/4, NAND2/3/4, etc.)
  - **Intelligent Fuzzy Matching**: Case-insensitive and substring matching
  - **I/O Cell Support**: INPUT → LOGIC1_X1, OUTPUT → LOGIC0_X1 placeholders
  - **Mapping Statistics**: **100% success rate** with Nangate library (20/20 cells for s27, 197/197 for s344)
  - **Debug Support**: Detailed mapping logs for troubleshooting

- ✅ **Real Physical Dimensions** - Accurate cell sizing
  - **LEF-Based Sizing**: 13 cells use exact dimensions from LEF
  - **Area Accuracy**: Total area reduced from 97.5 to 91.812 square micrometers
  - **Process Variation**: Different drive strengths (_X1, _X2, _X4) with varying dimensions
  - **Pin Geometry**: Stored LefMacro pointers for future routing integration





### MiniPlacement Placement Optimization ✅
- ✅ **Geometry Library**: 2D geometric operations and transformations for placement calculations
- ✅ **Physical Database**: Manages cell positions, dimensions, and placement constraints
- ✅ **Force-Directed Global Placement**: Physics-based algorithm for wirelength optimization
- ✅ **Legalization Algorithm**: Tetris-like row alignment and overlap removal
- ✅ **Detailed Placement**: Local optimization through cell swapping and fine-tuning
- ✅ **Python Visualization**: Matplotlib-based visualization for placement process monitoring
- ✅ **Complete Placement Flow**: End-to-end pipeline from initialization to final optimization
- ✅ **LEF Integration**: Industrial-grade LEF parser supporting Nangate 45nm library (134 macros)
- ✅ **MacroMapper Module**: Intelligent mapping from logical cell types to physical macros
- ✅ **Real Physical Dimensions**: Accurate cell sizing from LEF instead of area estimation

## Latest Update: 2025 - **Advanced Placement Optimization with Smart Visualization** 🏆🎉

MiniEDA is now a **complete industrial-grade EDA suite** featuring:

### MiniSTA - Static Timing Analysis ✅ Production-Ready
1. ✅ Parsing real industrial Liberty libraries (135+ cell types)
2. ✅ NLDM-based delay calculation (non-linear, physically accurate)
3. ✅ Slew propagation through timing paths
4. ✅ Setup/Hold analysis with WNS/TNS reporting
5. ✅ Processing circuits with 500+ nodes efficiently
6. ✅ **Physical meaning**: All delays based on real CMOS characteristics

### MiniPlacement - Layout Optimization ✅ Production-Ready
1. ✅ Industrial-grade LEF parsing (Nangate 45nm, 134 macros)
2. ✅ Force-directed global placement with HPWL optimization
3. ✅ Tetris-like legalization and detailed placement
4. ✅ 100% cell mapping success rate with intelligent MacroMapper
5. ✅ Real-time visualization with Python matplotlib integration
6. ✅ **Auto-detected technology row height**: Automatically extracts row height from LEF library (1.4μm for Nangate 45nm)

### MiniFlow - Integrated EDA Flow ✅ Production-Ready
1. ✅ Complete placement → post-placement STA pipeline
2. ✅ Coordinate back-annotation for accurate timing analysis
3. ✅ HPWL-based wire delay calculation
4. ✅ **Smart visualization with intelligent filtering**:
   - Dynamic fontsize based on cell size: `min(6.0, width × 1.5)`
   - Automatic label filtering: Only cells with fontsize > 3.0pt display labels
   - Enhanced contrast: Semi-transparent cells (alpha=0.5) with opaque text (alpha=1.0)
   - 95%+ label filtering rate for clean, professional layouts
5. ✅ **Advanced utilization optimization**: Comprehensive testing of 70% and 90% utilization
6. ✅ **ISCAS benchmark validation**: Complete testing on s27, s344, s349, and s526 circuits

**Recent Achievements**:
- **Smart Label Filtering**: Achieved 92-96% label filtering rate across all test circuits
- **Utilization Optimization**: Demonstrated effective 70% and 90% utilization strategies
- **Industrial LEF Integration**: Perfect 100% cell mapping with Nangate 45nm library
- **Comprehensive Performance Analysis**: Detailed HPWL and optimization metrics for all utilization levels

**Project Statistics**:
- **Total Code**: 8,800+ lines of C++17 industrial-grade code
- **Architecture**: Modular design with 52 source files
- **Test Coverage**: ISCAS benchmark suite with 100% pass rate
- **Visualization**: Automated layout process documentation with smart filtering

**MiniEDA is a complete industrial-grade EDA toolchain!** 🏆

### MiniFlow - Integrated EDA Flow with Visualization
- ✅ **Complete Pipeline**: Verilog → Placement → Post-Placement STA → Visualization
- ✅ **Coordinate Back-annotation**: Syncs placement coordinates to NetlistDB for accurate timing
- ✅ **HPWL-based Wire Delay**: Real wire delay using geometry-based HPWL model
- ✅ **Visual Analytics**: Automated generation of placement process images
- ✅ **Multi-Circuit Support**: Tested on ISCAS benchmarks (s27, s344, s349, s526)

**Usage Examples**:
```bash
# Build complete flow
make flow

# Run integrated EDA flow with visualization
./build/bin/mini_flow -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -clk 5.0

# View generated visualizations
ls visualizations/mini_flow_s27/
# Shows: 00_random.png, quadratic_iter_*.png, legalized.png, detailed.png
```

**Complete EDA Flow**:
1. **Netlist Parsing**: Verilog → NetlistDB with library cell mapping
2. **Placement Pipeline**: Random → Global → Legalization → Detailed
3. **Timing Analysis**: Post-placement STA with real wire delays
4. **Visualization**: Automatic generation of placement process images
5. **Results**: HPWL optimization, WNS/TNS reporting, layout analytics

**MiniEDA: Complete Industrial EDA Toolchain!** 🏆🎉

**Key Achievements**:
- ✅ **Industrial Libraries**: Full Nangate 45nm support (135 Liberty cells + 134 LEF macros)
- ✅ **100% Cell Mapping**: Intelligent MacroMapper with multi-strategy matching
- ✅ **Real Physical Integration**: Accurate dimensions from LEF instead of estimation
- ✅ **Visual Analytics**: Complete placement process documentation
- ✅ **Production-Ready**: 8,800+ lines of industrial-grade C++17 code

**Technical Excellence**:
- ✅ **Modular Architecture**: Clean separation of parsing, placement, timing, and visualization
- ✅ **Industrial Standards**: LEF/Liberty parsing with robust error handling
- ✅ **Algorithm Quality**: Force-directed placement with HPWL optimization
- ✅ **Testing Coverage**: ISCAS benchmark suite with 100% validation
- ✅ **Performance**: Efficient processing of circuits with 500+ nodes
- ✅ **Modular Design**: Clear algorithm separation, easy to extend and maintain
- ✅ **LEF Integration**: Support for industrial-standard LEF format, precise physical dimensions
- ✅ **Intelligent Mapping**: Multi-strategy cell type mapping, high compatibility

**Performance Improvements**:
- **Parsing Capability**: From 1 macro → **134 macros** (13400% improvement)
- **Mapping Success Rate**: From 5% → **100%** (2000% improvement)
- **Dimension Accuracy**: Estimated area → **Real physical dimensions**
- **Comprehensive Coverage**: Complete support for all Nangate 45nm library cells

**Usage Examples**:
```bash
# Build MiniPlacement
make placement

# Run placement algorithm (with visualization)
./build/bin/mini_placement <verilog_file> -rows <row_count> -vis

# View help
./build/bin/mini_placement -help
```

**Placement Flow**:
1. **Initialization**: Random placement of all cells
2. **Global Placement**: Force-directed algorithm optimizes overall wirelength
3. **Legalization**: Linear packing algorithm eliminates overlaps
4. **Detailed Placement**: Greedy swap algorithm for local optimization
5. **Visualization**: Generate layout images for each stage

**MiniEDA is now a complete EDA tool suite** featuring industrial-grade static timing analysis and production-ready layout optimization capabilities! 🚀
