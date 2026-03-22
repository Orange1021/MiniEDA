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

- **3D routing grid** with configurable layers (3-12 layers supported)
  - M1/M3/M5... (even layers): Horizontal routing preferred
  - M2/M4/M6... (odd layers): Vertical routing preferred
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
- **Multi-layer Support**: Dynamic layer configuration for different circuit scales

### MiniFlow - Integrated EDA Flow

- Complete pipeline: Verilog → Placement → Routing → Timing Analysis
- Coordinate back-annotation for accurate timing analysis
- HPWL-based wire delay calculation using actual placement
- Automated visualization with smart label filtering
- Supports ISCAS benchmark circuits (s27, s344, s349, s526, s1196, s1238, s1423, s1488, s1494, s5378)
- Configurable routing layers (3-12 layers) for different circuit scales

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
├── tests/                         # GoogleTest unit/integration tests
│   ├── netlist_db_test.cpp       # NetlistDB tests
│   ├── verilog_parser_test.cpp   # Verilog parser tests
│   ├── liberty_delay_model_test.cpp # Liberty + delay model tests
│   ├── timing_graph_sta_test.cpp # Timing graph + STA tests
│   ├── routing_test.cpp          # RoutingGrid + MazeRouter tests
│   ├── placement_test.cpp        # Density/Poisson/Legalizer tests
│   ├── lef_mapper_test.cpp       # LEF parser + mapper tests
│   └── mini_flow_smoke_test.cpp  # End-to-end mini_flow smoke test (s27)
├── benchmarks/                    # Test benchmarks
│   └── ISCAS/                     # ISCAS standard test suite
│       └── Verilog/               # Verilog circuits
├── build/                         # Build output (CMake presets)
│   └── cmake/
│       ├── debug/                 # Debug preset output
│       ├── release/               # Release preset output
│       ├── asan/                  # ASan/UBSan preset output
│       └── test/                  # GoogleTest preset output
├── visualizations/                # Generated visualizations
│   ├── plot_placement.py         # Placement visualization
│   ├── plot_routing.py          # Routing visualization
│   ├── visualize_density.py      # Density visualization
│   ├── s27/                      # s27 circuit results
│   ├── s344/                     # s344 circuit results
│   ├── s349/                     # s349 circuit results
│   ├── s526/                     # s526 circuit results
│   ├── s1196/                    # s1196 circuit results
│   ├── s1238/                    # s1238 circuit results
│   ├── s1423/                    # s1423 circuit results
│   ├── s1488/                    # s1488 circuit results
│   └── s1494/                    # s1494 circuit results
├── log/                           # Log output directory
├── txt/                           # Text output directory
├── CMakeLists.txt                 # Build configuration (CMake)
├── CMakePresets.json              # Standard debug/release/asan/test presets
├── simple_circuits.sh            # Simple circuit tests (3-layer)
├── medium_circuits.sh            # Medium circuit tests (5-layer)
├── hard_circuits.sh              # Hard circuit tests (8-layer)
└── extreme_circuits.sh           # Extreme circuit tests (12-layer)
```

## Build and Installation

### Prerequisites

- C++17 compatible compiler (g++ 7.0 or higher recommended)
- CMake (3.16 or higher recommended)
- Python 3 with matplotlib (for visualization)
- Internet access for first-time GoogleTest fetch (only when `MINIEDA_ENABLE_TESTING=ON`)

### Build Steps

```bash
# Release build
cmake --preset release
cmake --build --preset release -j$(nproc)

# Debug build with sanitizer (ASan + UBSan)
cmake --preset asan
cmake --build --preset asan -j$(nproc)

# Debug build with GoogleTest unit tests
cmake --preset test
cmake --build --preset test -j$(nproc)
ctest --test-dir build/cmake/test --output-on-failure

# Clean build files
rm -rf build/cmake

# Run regression by circuit scale
./simple_circuits.sh   # small circuits, 3 routing layers
./medium_circuits.sh   # medium circuits, 5 routing layers
./hard_circuits.sh     # large circuits, 8 routing layers
./extreme_circuits.sh  # very large circuits, 12 routing layers
```

If LeakSanitizer reports ptrace-related errors in instrumented environments, run with `ASAN_OPTIONS=detect_leaks=0`.

After successful compilation, executables will be in the `build/cmake/debug/bin/` or `build/cmake/release/bin/` directory.

### CI

GitHub Actions workflow is provided at `.github/workflows/ci.yml`.
It runs:
- GoogleTest unit tests (`cmake --preset test` + `ctest`)
- Release build + smoke test (`benchmarks/s27.v`)
- ASan/UBSan build + runtime check (`ASAN_OPTIONS=detect_leaks=0`)

## Usage Examples

### MiniFlow - Integrated EDA Flow

```bash
# Run complete flow (placement + routing + timing)
./build/cmake/release/bin/mini_flow \
  -v benchmarks/s27.v

# Run with custom utilization
./build/cmake/release/bin/mini_flow \
  -v benchmarks/s344.v \
  -util 0.7 \
  -clk 8.0

# Run with specific placement algorithm
MINIEDA_PLACEMENT_ALGO=basic ./build/cmake/release/bin/mini_flow \
  -v benchmarks/s27.v
```

**Command Line Options:**
- `-v <file>`: Verilog netlist file (required)
- `-lib <file>`: Liberty library file (default: NangateOpenCellLibrary_typical.lib)
- `-lef <file>`: LEF physical library file (default: NangateOpenCellLibrary.macro.lef)
- `-run_id <name>` / `--run_id <name>`: Run identifier (default: `default_run`)
- `-pitch <value>`: Routing grid pitch in micrometers (default: 0.19)
- `-clk <period>`: Clock period in ns (default: 10.0)
- `-via_cost <value>`: Via penalty cost multiplier (default: 5.0)
- `-wire_cost <value>`: Wire cost per unit length (default: 1.0)
- `-uncertainty <value>`: Clock uncertainty in ns (default: 0.05)
- `-input_delay <value>`: Default input delay in ns (default: 0.0)
- `-output_delay <value>`: Default output delay in ns (default: 0.0)
- `-max_transition <value>`: Maximum transition time constraint (default: 0.5)
- `-input_delay_port <port> <value>`: Set input delay for a specific port
- `-output_delay_port <port> <value>`: Set output delay for a specific port
- `-util <value>`: Target utilization (default: 0.7)
- `-rowheight <val>`: Row height in micrometers (default: 1.4)
- `-num_layers <count>`: Number of routing layers (default: 3, supports 3-12)
- `-verbose`: Enable verbose output
- `-skip_routing`: Skip routing stage
- `-threads <count>`: Maximum parallel threads (default: 1)
- `-conv_thresh <value>`: Placement convergence threshold (default: 1e-6)
- `-max_iter <count>`: Maximum placement iterations (default: 100)
- `-grid_expand <factor>`: Routing grid expansion factor (default: 1.2)
- `-h` / `-help` / `--help`: Show help message

## Testing

### GoogleTest Unit/Integration Tests

```bash
cmake --preset test
cmake --build --preset test -j$(nproc)
ctest --test-dir build/cmake/test --output-on-failure
```

The legacy `test/` folder has been removed. Active automated tests are maintained in `tests/`.

### Run All Tests

```bash
# Simple circuits (3-layer routing)
./simple_circuits.sh

# Medium circuits (5-layer routing)
./medium_circuits.sh

# Hard circuits (8-layer routing)
./hard_circuits.sh

# Extreme circuits (12-layer routing)
./extreme_circuits.sh
```

### Test Script Overview

| Script | Routing Layers | Target Scale | Circuits |
|--------|----------------|--------------|----------|
| `simple_circuits.sh` | 3 | Small | s27, s344, s349, s526 |
| `medium_circuits.sh` | 5 | Medium | s1196, s1238, s1423, s1488, s1494 |
| `hard_circuits.sh` | 8 | Large | s5378, s9234 |
| `extreme_circuits.sh` | 12 | Very Large / Stress | s13207, s15850, s35932 |

All four scripts compile `mini_flow` in release mode first, then run full flow (placement + routing + STA) and write logs to `log/<circuit>.log`.

**Test Results:**

| Circuit | Gates | Nets | Layers | Placement | Routing | STA |
|---------|-------|------|--------|-----------|---------|-----|
| s27     | 20    | 18   | 3      | PASS      | PASS    | PASS |
| s344    | 197   | 185  | 3      | PASS      | PASS    | PASS |
| s349    | 198   | 186  | 3      | PASS      | PASS    | PASS |
| s526    | 233   | 224  | 3      | PASS      | PASS    | PASS |
| s1196   | 577   | 510  | 5      | PASS      | PASS    | PASS |
| s1238   | 556   | 489  | 5      | PASS      | PASS    | PASS |
| s1423   | 755   | 581  | 5      | PASS      | PASS    | PASS |
| s1488   | 688   | 638  | 5      | PASS      | PASS    | PASS |
| s1494   | 682   | 632  | 5      | PASS      | PASS    | PASS |
| s5378   | 3044  | -    | 8      | PASS      | PASS    | PASS |

## Technical Features

### Core Architecture

- **Language**: C++17
- **Build System**: CMake
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
- **3D routing grid**: (x, y, layer) with configurable layers (3-12)
- **Layer-specific routing**: Even layers horizontal, odd layers vertical
- **PathFinder algorithm**: Iterative congestion-aware optimization
- **Smart access point finder**: 5x5 search radius for pin optimization
- **Via-aware routing**: Configurable cost models for vias
- **Cell obstacles**: Prevent routing through cells on Layer 0
- **Star topology**: Multi-pin net decomposition
- **Multi-layer visualization**: Support for 3-12 routing layers with color-coded display

## Project Statistics

- **Total Code (lib + apps, .cpp/.h)**: 22,186 lines (14,609 .cpp + 7,577 .h)
- **Source Files (lib + apps, .cpp/.h)**: 77 files
- **Module Distribution**:
  - Placement Module: 22 files, 5,833 lines
  - STA Module: 16 files, 4,731 lines
  - Routing Module: 6 files, 2,972 lines
  - Core Library: 32 files, 8,308 lines
  - Main Application: 1 file, 342 lines
- **Automated Tests**: GoogleTest + `ctest` + CI smoke/ASan checks
- **Libraries Supported**: Nangate 45nm (Liberty cells + LEF macros)
- **Code Quality**: Professional English comments, maximum compatibility maintained
- **Memory Management**: Smart pointers for automatic memory management
- **Debug Logging**: Compile-time controlled debug output with zero overhead in production
- **Multi-layer Routing**: Support for 3-12 routing layers with automatic layer assignment
- **Test Scripts**: 4 categorized test suites for different circuit scales

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

**Project Status**: MiniEDA Industrial Suite - Complete EDA Flow with Multi-Layer Routing

**Note**: This is an educational project demonstrating core EDA algorithms. The implementation features zero-conflict routing through advanced PathFinder algorithms, smart access point optimization, and comprehensive conflict resolution. The detailed placement implementation features equal-width cell swapping with zero-overlap guarantee and comprehensive overlap analysis. The routing system supports 3-12 configurable layers with automatic layer assignment based on circuit scale. While physically accurate (LEF/Liberty integration, realistic constraints), some aspects like via count may differ from commercial tools due to simplified congestion modeling. The codebase uses modern C++17 standards with professional English documentation for maximum compatibility.
