# MiniEDA

A lightweight EDA (Electronic Design Automation) toolchain for static timing analysis, chip placement optimization, and routing of digital circuits.

## Project Overview

MiniEDA is an educational and experimental EDA toolchain project implementing key functionalities in the digital integrated circuit design flow. This project is developed in C++17, providing a clear code structure and industrial-grade programming standards.

## Main Features

### Implemented Core Modules

- **Cell Module**: Digital circuit logic unit data model
  - Supports basic logic gates (AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF)
  - Supports sequential elements (DFF flip-flop)
  - Supports combinational logic (MUX multiplexer)
  - I/O port management with coordinate support
  - Pin direction and timing parameters

- **Net Module**: Circuit connection (wire) data model
  - Driver-load connection management
  - Fanout calculation
  - Timing parameters (wire capacitance, wire delay)
  - Clock signal identification

- **NetlistDB Module**: Netlist database management system
  - Cell/Net/Pin container management
  - O(1) fast lookup using hash tables
  - Topology queries (fanin/fanout)
  - Netlist validation and statistics
  - 42 public interface methods

- **VerilogParser Module**: Gate-level Verilog netlist parser
  - Lexical analysis with comment removal
  - Syntax analysis (module/port/wire/instance)
  - Supports 15+ gate types (AND, OR, NAND, NOR, XOR, DFF, etc.)
  - Complete error reporting system
  - Successfully parsed ISCAS s27, s344, s349, s526 circuits

- **LibertyParser Module**: Liberty (.lib) format library parser
  - Robust hierarchical parser with Group/Attribute distinction
  - Supports NLDM (Non-Linear Delay Model) lookup tables
  - Industrial-grade error handling with line/column reporting
  - Complete cell, pin, and timing arc parsing
  - Supports 135+ Nangate 45nm library cells

- **LefParser Module**: LEF (Library Exchange Format) physical library parser
  - Industrial-grade parser supporting Nangate 45nm library (134 macros)
  - Robust tokenizer handling separators and complex LEF constructs
  - Complete macro, pin, and port geometric information extraction
  - OBS block processing and antenna rule support

- **Geometry Module**: Core geometry library for placement calculations
  - 2D Point and Rect data structures with vector operations
  - Manhattan and Euclidean distance calculations
  - Bounding box operations and overlap detection

### MiniSTA - Static Timing Analysis

- Timing graph data structures with slew propagation
- STA core engine (AT/RAT/Slack calculation)
- NLDM-based Delay Model with Liberty integration
- Setup/Hold analysis with WNS/TNS reporting
- CellMapper module for intelligent cell type mapping
- Post-placement timing analysis with HPWL-based wire delay
- Complete Setup + Hold Analysis

### MiniPlacement - Chip Placement Optimization

- LEF Parser Integration for industrial-grade physical library support
- MacroMapper module for intelligent cell type mapping (100% mapping rate)
- Real physical dimensions from LEF instead of area estimation
- Three-stage placement pipeline:
  - Force-directed global placement for wirelength optimization
  - Tetris-like legalization for overlap removal
  - Detailed placement for local optimization
- Python matplotlib-based visualization
- Multi-strategy cell mapping with drive strength variants support

### MiniRouter - A* Maze Routing

- 3D routing grid with Layer 0 (M1) and Layer 1 (M2) support
  - M1: Horizontal routing preferred
  - M2: Vertical routing preferred
- A* pathfinding algorithm with configurable costs
- Via-aware routing (layer changes with via cost)
- Cell obstacles on Layer 0 to prevent routing through cells
- Star topology decomposition for multi-pin nets
- Pin location extraction from LEF physical data

### MiniFlow - Integrated EDA Flow

- Complete pipeline: Verilog → Placement → Routing → Timing Analysis
- Coordinate back-annotation for accurate timing analysis
- HPWL-based wire delay calculation using actual placement
- Automated visualization with smart label filtering
- Supports all ISCAS benchmark circuits (s27, s344, s349, s526)

## Project Structure

```
MiniEDA/
├── lib/                            # Core library
│   ├── include/                   # Header files
│   │   ├── cell.h                 # Logic cell model
│   │   ├── net.h                  # Net model
│   │   ├── netlist_db.h           # Netlist database
│   │   ├── verilog_parser.h       # Verilog parser
│   │   ├── liberty.h              # Liberty library data model
│   │   ├── liberty_parser.h       # Liberty parser
│   │   ├── lef_parser.h           # LEF physical library parser
│   │   ├── geometry.h             # Core geometry library
│   │   ├── visualizer.h           # Python visualization bridge
│   │   └── pin_mapper.h           # Pin mapping utilities
│   └── src/                       # Implementation files
│       ├── cell.cpp               # Cell implementation
│       ├── net.cpp                # Net implementation
│       ├── netlist_db.cpp         # Database management
│       ├── verilog_parser.cpp     # Verilog parsing
│       ├── liberty.cpp            # Library data structures
│       ├── liberty_parser.cpp     # Liberty parsing engine
│       ├── lef_parser.cpp         # LEF parsing
│       ├── geometry.cpp           # 2D geometric operations
│       ├── visualizer.cpp         # Visualization
│       └── pin_mapper.cpp         # Pin mapping
├── apps/                          # Applications
│   ├── mini_sta/                  # Static timing analysis
│   │   ├── sta_engine.h/.cpp      # STA core engine
│   │   ├── timing_graph.h/.cpp    # Timing graph
│   │   ├── delay_model.h/.cpp     # Delay models
│   │   ├── timing_constraints.h/.cpp # Timing constraints
│   │   ├── timing_checks.h/.cpp   # Timing checks
│   │   ├── timing_report.h/.cpp   # Timing report
│   │   ├── timing_path.h/.cpp     # Timing paths
│   │   ├── cell_mapper.h/.cpp     # Cell mapping
│   │   └── main_sta.cpp           # Main program
│   ├── mini_placement/            # Placement optimization
│   │   ├── placer_engine.h/.cpp   # Placement engine
│   │   ├── placer_db.h/.cpp       # Physical database
│   │   ├── visualizer.h/.cpp      # Visualization
│   │   ├── macro_mapper.h/.cpp    # Macro mapping
│   │   ├── placement_interface.h/.cpp # High-level interface
│   │   └── main_placer.cpp        # Main program
│   ├── mini_router/               # A* maze routing
│   │   ├── maze_router.h/.cpp     # A* routing engine
│   │   ├── routing_grid.h/.cpp    # 3D routing grid
│   │   ├── main_router.cpp        # Main program
│   │   └── pin_mapper.h           # Pin location mapping
│   └── main_flow.cpp              # Integrated flow
├── test/                          # Test programs
│   ├── test_netlist_db.cpp        # NetlistDB tests
│   ├── test_verilog_parser.cpp    # Verilog parser tests
│   ├── test_liberty_parser.cpp    # Liberty parser tests
│   └── test_repaired_features.cpp # Feature tests
├── benchmarks/                    # Test benchmarks
│   ├── sample.lib                 # Sample Liberty library
│   ├── sample.lef                 # Sample LEF library
│   ├── NangateOpenCellLibrary_typical.lib # Nangate 45nm library
│   ├── NangateOpenCellLibrary.macro.lef # Nangate macros
│   └── ISCAS/                     # ISCAS standard test suite
│       └── Verilog/               # Verilog circuits
├── build/                         # Build output
│   ├── bin/                       # Executables
│   └── lib/                       # Object files
└── Makefile                       # Build configuration
```

## Build and Installation

### Prerequisites

- C++17 compatible compiler (g++ 7.0 or higher recommended)
- GNU Make
- Python 3 with matplotlib (for visualization)

### Build Steps

```bash
# Build all components
make

# Build specific components
make sta          # MiniSTA only
make placement    # MiniPlacement only
make router       # MiniRouter only
make flow         # MiniFlow only
make test         # Test programs only

# Clean build files
make clean
```

After successful compilation, executables will be in the `build/bin/` directory.

## Usage Examples

### MiniSTA - Static Timing Analysis

```bash
# Run STA with Liberty library
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s27.v -clk 5.0 -lib benchmarks/sample.lib

# Run STA with Nangate 45nm library
./build/bin/mini_sta benchmarks/ISCAS/Verilog/s344.v \
  -clk 8.0 \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib

# View help
./build/bin/mini_sta -help
```

**Command Line Options:**
- `-clk <period>` : Clock period in ns (default: 10.0)
- `-lib <file>`  : Liberty library file (required)
- `-help`        : Show help message

### MiniPlacement - Chip Placement Optimization

```bash
# Run placement (requires Liberty library)
./build/bin/mini_placement \
  -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/sample.lib

# Run with LEF physical library (Nangate 45nm)
./build/bin/mini_placement \
  -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/sample.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef

# Run with custom utilization
./build/bin/mini_placement \
  -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/sample.lib \
  -util 0.8 \
  -rowheight 2.5

# View help
./build/bin/mini_placement -help
```

**Command Line Options:**
- `-v <file>`        : Verilog netlist file (required)
- `-lib <file>`      : Liberty library file (required)
- `-lef <file>`      : LEF physical library file (optional)
- `-util <value>`    : Target utilization (default: 0.7)
- `-rowheight <val>` : Row height in micrometers (default: 3.0)
- `-help`            : Show help message

### MiniRouter - A* Maze Routing

```bash
# Run router with physical libraries
./build/bin/mini_router \
  -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -via_cost 10.0

# View help
./build/bin/mini_router -help
```

**Command Line Options:**
- `-v <file>`        : Verilog netlist file (required)
- `-lib <file>`      : Liberty library file (required)
- `-lef <file>`      : LEF physical library file (required)
- `-via_cost <val>`  : Via cost multiplier (default: 10.0)
- `-help`            : Show help message

### MiniFlow - Integrated EDA Flow

```bash
# Run complete flow (placement + routing + timing)
./build/bin/mini_flow \
  -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -clk 5.0

# Run with different utilization
./build/bin/mini_flow \
  -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -util 0.7 \
  -clk 8.0

# View help
./build/bin/mini_flow -help
```

**Command Line Options:**
- `-v <file>`        : Verilog netlist file (required)
- `-lib <file>`      : Liberty library file (required)
- `-lef <file>`      : LEF physical library file (required)
- `-clk <period>`    : Clock period in ns (default: 10.0)
- `-util <value>`    : Target utilization (default: 0.7)
- `-rowheight <val>` : Row height in micrometers (default: 3.0)
- `-help`            : Show help message

## Testing

### Liberty Parser Test

```bash
./build/bin/test_liberty_parser
```

Results:
- sample.lib: 8 cell types - PASS
- NangateOpenCellLibrary_typical.lib: 135 cells - PASS

### ISCAS Benchmark Tests

```bash
# Run Verilog parser tests
./build/bin/test_verilog_parser

# Run timing graph tests
./build/bin/test_timing_graph_build

# Run complete STA tests
./build/bin/test_sta_full
```

**Test Results:**

| Circuit | Gates | Nets | Status |
|---------|-------|------|--------|
| s27     | 20    | 18   | PASS |
| s344    | 197   | 185  | PASS |
| s349    | 198   | 186  | PASS |
| s382    | 190   | 183  | PASS |
| s386    | 190   | 183  | PASS |
| s420    | 247   | 236  | PASS |

Test suite location: `benchmarks/ISCAS/Verilog/`

## Technical Features

### Core Architecture

- **Language**: C++17
- **Build System**: GNU Make
- **Code Standards**: Modern C++ (namespaces, RAII, smart pointers, const correctness)
- **Data Structures**: Hash tables for O(1) lookup, efficient graph representations
- **Design**: Modular architecture with clear separation of concerns
- **Comments**: English documentation throughout

### Key Algorithms

#### MiniSTA
- NLDM delay model with Liberty lookup tables
- Slew propagation through timing paths
- AAT/RAT/slack calculation
- Setup and hold timing checks
- Topological sorting for timing analysis

#### MiniPlacement
- Force-directed global placement
- Tetris-like legalization
- Greedy swap detailed placement
- HPWL (Half-Perimeter Wire Length) objective

#### MiniRouter
- A* pathfinding algorithm
- 3D routing grid (x, y, layer)
- Layer-specific routing constraints:
  - M1 (Layer 0): Horizontal preferred
  - M2 (Layer 1): Vertical preferred
- Cell obstacles on Layer 0
- Via-aware routing with cost model

## Project Statistics

- **Total Code**: 13,200+ lines of C++
- **Source Files**: 52 files
- **Test Coverage**: ISCAS benchmark suite (100% pass rate)
- **Libraries Supported**: Nangate 45nm (135 Liberty cells + 134 LEF macros)

## Contribution Guidelines

Issues and Pull Requests are welcome! This project follows industrial-grade C++ coding standards:

1. Fork the repository
2. Create feature branch (`git checkout -b feature/your-feature`)
3. Follow existing code standards
4. Commit changes (`git commit -m 'Add your feature'`)
5. Push to branch (`git push origin feature/your-feature`)
6. Open Pull Request
7. Ensure tests pass

## License

This project is licensed under MIT License - see LICENSE file for details.

## Contact

For questions, suggestions, or bug reports, please use GitHub Issues.

---

**Project Status**: MiniEDA Industrial Suite - Complete EDA Flow

**Note**: This is an educational project demonstrating core EDA algorithms. While physically accurate (LEF/Liberty integration, realistic constraints), some aspects like via count may differ from commercial tools due to simplified congestion modeling.
