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
  - Supports Nangate 45nm library cells

- **LefParser Module**: LEF (Library Exchange Format) physical library parser
  - Industrial-grade parser supporting Nangate 45nm library
  - Robust tokenizer handling separators and complex LEF constructs
  - Complete macro, pin, and port geometric information extraction
  - OBS block processing and antenna rule support

- **Geometry Module**: Core geometry library for placement calculations
  - 2D Point and Rect data structures with vector operations
  - Manhattan and Euclidean distance calculations
  - Bounding box operations and overlap detection

- **SteinerTree Module**: Minimum Spanning Tree builder for multi-pin nets
  - Prim's algorithm for efficient MST construction
  - Decomposes multi-pin nets into 2-pin segments
  - Supports both Manhattan and Euclidean distance calculations

- **CSVExporter Module**: CSV export utility for data visualization
  - Exports placement results for Python matplotlib visualization
  - Automatic directory creation and file management

### MiniSTA - Industrial-Grade Static Timing Analysis

- **Advanced Timing Graph**: Multi-level slew propagation with Min/Max path analysis
- **STA Core Engine**: Complete AT/RAT/Slack calculation with topological sorting
- **NLDM-based Delay Model**: Full Liberty library integration with lookup tables
- **Dynamic Constraint Checking**: Setup/Hold analysis with slew-dependent constraint tables
- **Industrial Constraints**: Clock uncertainty, input/output delays, and boundary constraints
- **CellMapper Module**: Intelligent cell type mapping with Nangate 45nm library support
- **LibertyPinMapper Module**: Pin name translation between Liberty and Verilog conventions
- **Constraint Lookup Tables**: State machine parser for rise_constraint/fall_constraint
- **Post-Placement Analysis**: HPWL-based wire delay calculation with coordinate back-annotation
- **Complete Timing Flow**: Setup + Hold + Clock Uncertainty + Boundary Delays
- **Path Analysis**: Timing path extraction and critical path identification
- **Timing Reporting**: Comprehensive timing reports with slack analysis

### MiniPlacement - Chip Placement Optimization

- **MacroMapper Module**: Intelligent cell type mapping
  - Maps Liberty cell types to LEF macros
  - Handles drive strength variants
  - Fallback strategies for unknown cell types

- **Global Placement Algorithms**:
  - Basic force-directed placement with quadratic wirelength
  - Momentum electrostatic placement with density control
  - Hybrid cascade placement (warm-up + refinement)

- **DensityGrid Module**: Bin-based density distribution
  - Electrostatic potential calculation
  - Density overflow management

- **PoissonSolver Module**: FFT-accelerated Poisson equation solver
  - Radix-2 Cooley-Tukey FFT algorithm
  - O(N log N) complexity for electrostatic force calculation
  - Power-of-2 grid size requirement for FFT

- **Legalization Algorithms**:
  - Greedy (Tetris) legalization: fast but aggressive HPWL reduction
  - Abacus optimization-based legalization with right-to-left compaction
  - Capacity-aware row distribution to prevent boundary overflow
  - Floating-point precision handling for site alignment

- **Detailed Placement**: Equal-width cell swapping strategy
- **OverlapDetector Utility**: Comprehensive overlap analysis and boundary touch detection
- **Zero-overlap Guarantee**: Through spatial conservation principles
- **Python Visualization**: Matplotlib-based visualization with algorithm comparison
- **Unified HPWL Calculator**: Consistent wire length estimation across all modules

### MiniRouter - Advanced A* Maze Routing

- **3D routing grid** with Layer 0 (M1) and Layer 1 (M2) support
  - M1: Horizontal routing preferred
  - M2: Vertical routing preferred
- **PathFinder Iterative Algorithm**: Congestion-aware optimization
- **Smart Access Point Finder**: 5x5 search radius for pin access flexibility
- **Exponential Penalty Growth**: History cost decay for conflict resolution
- **Randomized Net Ordering**: Breaks persistent deadlocks
- **Complete Conflict Tracking**: Best solution preservation
- **Zero-conflict Routing**: Achieved on ISCAS benchmarks
- **Via-aware Routing**: Configurable cost models
- **Cell Obstacles**: On Layer 0 to prevent routing through cells
- **Star Topology Decomposition**: For multi-pin nets
- **Pin Location Extraction**: From LEF physical data

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
│   │   ├── app_config.h           # Unified application configuration
│   │   ├── arg_parser.h           # Command line argument parsing
│   │   ├── cell.h                 # Logic cell model
│   │   ├── csv_exporter.h         # CSV export utility
│   │   ├── debug_log.h            # Debug logging macros
│   │   ├── geometry.h             # Core geometry library
│   │   ├── hpwl_calculator.h      # Unified HPWL calculation
│   │   ├── lef_parser.h           # LEF physical library parser
│   │   ├── lef_pin_mapper.h       # LEF pin name mapping
│   │   ├── liberty.h              # Liberty library data model
│   │   ├── liberty_parser.h       # Liberty parser
│   │   ├── liberty_pin_mapper.h   # Liberty pin name mapping
│   │   ├── net.h                  # Net model
│   │   ├── netlist_db.h           # Netlist database
│   │   ├── placer_db.h            # Physical placement database
│   │   ├── steiner_tree.h         # Steiner tree builder
│   │   └── verilog_parser.h       # Verilog parser
│   └── src/                       # Implementation files
│       ├── app_config.cpp         # Configuration management
│       ├── arg_parser.cpp         # Argument parsing
│       ├── cell.cpp               # Cell implementation
│       ├── csv_exporter.cpp       # CSV export
│       ├── hpwl_calculator.cpp    # HPWL calculation
│       ├── lef_parser.cpp         # LEF parsing
│       ├── lef_pin_mapper.cpp     # LEF pin mapping
│       ├── liberty.cpp            # Library data structures
│       ├── liberty_parser.cpp     # Liberty parsing
│       ├── liberty_pin_mapper.cpp # Liberty pin mapping
│       ├── net.cpp                # Net implementation
│       ├── netlist_db.cpp         # Database management
│       ├── placer_db.cpp          # Physical database
│       ├── steiner_tree.cpp       # Steiner tree
│       └── verilog_parser.cpp     # Verilog parsing
├── apps/                          # Applications
│   ├── main_flow.cpp              # Integrated flow
│   ├── mini_placement/            # Placement optimization
│   │   ├── abacus_legalizer.cpp/h # Abacus legalization
│   │   ├── density_grid.cpp/h    # Density grid
│   │   ├── detailed_placer.cpp/h  # Detailed placement
│   │   ├── global_placer.cpp/h   # Global placement
│   │   ├── greedy_legalizer.cpp/h # Greedy legalization
│   │   ├── legalizer.cpp/h       # Legalization base class
│   │   ├── macro_mapper.cpp/h    # Macro mapping
│   │   ├── overlap_detector.cpp/h # Overlap detection
│   │   ├── placement_interface.cpp/h # Placement interface
│   │   ├── placer_engine.cpp/h   # Placement engine
│   │   └── poisson_solver.cpp/h  # Poisson equation solver
│   ├── mini_router/               # A* maze routing
│   │   ├── maze_router.cpp/h     # A* routing engine
│   │   ├── routing_grid.cpp/h    # 3D routing grid
│   │   └── routing_interface.cpp/h # Routing interface
│   └── mini_sta/                  # Static timing analysis
│       ├── cell_mapper.cpp/h     # Cell mapping
│       ├── delay_model.cpp/h     # Delay models
│       ├── sta_engine.cpp/h     # STA core engine
│       ├── timing_checks.cpp/h   # Timing checks
│       ├── timing_constraints.cpp/h # Timing constraints
│       ├── timing_graph.cpp/h    # Timing graph
│       ├── timing_path.cpp/h     # Timing paths
│       └── timing_report.cpp/h   # Timing report
├── test/                          # Test programs
│   ├── test_density_grid.cpp     # Density grid tests
│   ├── test_global_placer.cpp   # Global placement tests
│   ├── test_hybrid_placement.cpp # Hybrid placement tests
│   ├── test_liberty.cpp         # Liberty library tests
│   ├── test_liberty_parser.cpp   # Liberty parser tests
│   ├── test_netlist_db.cpp       # Netlist database tests
│   ├── test_poisson_solver.cpp   # Poisson solver tests
│   ├── test_sta_debug.cpp       # STA debug tests
│   ├── test_sta_full.cpp         # Complete STA tests
│   ├── test_strict_mode.v       # Strict mode test
│   ├── test_timing_graph_build.cpp # Timing graph tests
│   ├── test_verilog_parser.cpp   # Verilog parser tests
│   └── test_repaired_features.cpp # Feature tests
├── benchmarks/                    # Test benchmarks
│   └── ISCAS/                     # ISCAS standard test suite
│       └── Verilog/               # Verilog circuits
├── build/                         # Build output
│   ├── bin/                       # Executables
│   └── lib/                       # Object files
├── visualizations/                # Generated visualizations
│   ├── plot_placement.py         # Placement visualization
│   ├── plot_routing.py          # Routing visualization
│   ├── visualize_density.py      # Density visualization
│   ├── s27/                      # s27 circuit results
│   ├── s344/                     # s344 circuit results
│   ├── s349/                     # s349 circuit results
│   └── s526/                     # s526 circuit results
├── txt/                           # Text output directory
├── Makefile                       # Build configuration
└── run_tests.sh                   # Test runner script
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

# Clean build files
make clean

# Run tests
./run_tests.sh
```

After successful compilation, executables will be in the `build/bin/` directory.

## Usage Examples

### MiniFlow - Integrated EDA Flow

```bash
# Run complete flow (placement + routing + timing)
./build/bin/mini_flow \
  -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -clk 5.0

# Run with custom utilization
./build/bin/mini_flow \
  -v benchmarks/ISCAS/Verilog/s344.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef \
  -util 0.7 \
  -clk 8.0

# Run with specific placement algorithm
MINIEDA_PLACEMENT_ALGO=basic ./build/bin/mini_flow \
  -v benchmarks/ISCAS/Verilog/s27.v \
  -lib benchmarks/NangateOpenCellLibrary_typical.lib \
  -lef benchmarks/NangateOpenCellLibrary.macro.lef
```

**Command Line Options:**
- `-v <file>`: Verilog netlist file (required)
- `-lib <file>`: Liberty library file (required)
- `-lef <file>`: LEF physical library file (required)
- `-clk <period>`: Clock period in ns (default: 10.0)
- `-uncertainty <value>`: Clock uncertainty in ns (default: 0.05)
- `-input_delay <value>`: Default input delay in ns (default: 0.0)
- `-output_delay <value>`: Default output delay in ns (default: 0.0)
- `-max_transition <value>`: Maximum transition time constraint (default: 0.5)
- `-util <value>`: Target utilization (default: 0.8)
- `-rowheight <val>`: Row height in micrometers (default: 1.4)
- `-help`: Show help message

## Testing

### Run All Tests

```bash
./run_tests.sh
```

### Individual Tests

```bash
# Run specific tests
./build/bin/test_liberty_parser
./build/bin/test_verilog_parser
./build/bin/test_netlist_db
./build/bin/test_timing_graph_build
./build/bin/test_sta_full
./build/bin/test_global_placer
./build/bin/test_poisson_solver
./build/bin/test_hybrid_placement
```

**Test Results:**

| Circuit | Gates | Nets | Placement | Routing | STA |
|---------|-------|------|-----------|---------|-----|
| s27     | 20    | 18   | PASS      | PASS    | PASS |
| s344    | 197   | 185  | PASS      | PASS    | PASS |
| s349    | 198   | 186  | PASS      | PASS    | PASS |
| s526    | 233   | 224  | PASS      | PASS    | PASS |

## Technical Features

### Core Architecture

- **Language**: C++17
- **Build System**: GNU Make
- **Code Standards**: Modern C++ (namespaces, RAII, smart pointers, const correctness)
- **Data Structures**: Hash tables for O(1) lookup, efficient graph representations
- **Design**: Modular architecture with clear separation of concerns
- **Documentation**: Professional English comments throughout

### Key Algorithms

#### MiniSTA
- **Advanced NLDM delay model**: Full Liberty lookup tables with rise/fall transition support
- **Multi-level slew propagation**: Min/Max path analysis with slew degradation
- **Dynamic constraint checking**: Setup/Hold with slew-dependent lookup tables
- **Industrial timing constraints**: Clock uncertainty, I/O delays, max transition constraints
- **State machine parser**: Handles rise_constraint/fall_constraint from Liberty libraries
- **Complete timing flow**: AAT/RAT/slack with boundary constraints and uncertainty
- **Topological analysis**: Robust sorting for complex timing graphs
- **Path analysis**: Critical path extraction and slack reporting

#### MiniPlacement
- **Force-directed global placement**: Quadratic wirelength optimization
- **Momentum electrostatic placement**: Density optimization with Nesterov method
- **Hybrid cascade placement**: Two-phase approach (warm-up + refinement)
- **FFT-accelerated Poisson solver**: O(N log N) complexity for electrostatic forces
- **Enhanced legalization**: Greedy + Abacus with capacity-aware distribution
- **Detailed placement**: Equal-width cell swapping with spatial conservation
- **Overlap detection**: Comprehensive analysis and boundary touch detection
- **Unified HPWL calculator**: Consistent wire length estimation

#### MiniRouter
- **A* pathfinding algorithm**: Optimal path search with heuristic
- **3D routing grid**: (x, y, layer) with layer-specific constraints
- **Layer-specific routing**: M1 horizontal, M2 vertical
- **PathFinder algorithm**: Iterative congestion-aware optimization
- **Smart access point finder**: 5x5 search radius for pin optimization
- **Via-aware routing**: Configurable cost models for vias
- **Cell obstacles**: Prevent routing through cells on Layer 0
- **Star topology**: Multi-pin net decomposition

## Project Statistics

- **Total Code**: 18,330 lines (14,196 .cpp + 4,134 .h)
- **Source Files**: 60 files
- **Module Distribution**:
  - Placement Module: 22 files, 5,673 lines
  - STA Module: 16 files, 4,689 lines
  - Routing Module: 6 files, 2,698 lines
  - Core Library: 32 files, 8,340 lines
  - Main Application: 1 file, 338 lines
- **Test Coverage**: ISCAS benchmark suite (100% pass rate)
- **Libraries Supported**: Nangate 45nm (Liberty cells + LEF macros)
- **Code Quality**: Professional English comments, maximum compatibility maintained
- **Memory Management**: Smart pointers for automatic memory management
- **Debug Logging**: Compile-time controlled debug output with zero overhead in production

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

**Project Status**: MiniEDA Industrial Suite - Complete EDA Flow with Zero-Conflict Routing

**Note**: This is an educational project demonstrating core EDA algorithms. The implementation features zero-conflict routing through advanced PathFinder algorithms, smart access point optimization, and comprehensive conflict resolution. The detailed placement implementation features equal-width cell swapping with zero-overlap guarantee and comprehensive overlap analysis. While physically accurate (LEF/Liberty integration, realistic constraints), some aspects like via count may differ from commercial tools due to simplified congestion modeling. The codebase uses modern C++17 standards with professional English documentation for maximum compatibility.