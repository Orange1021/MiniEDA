#!/bin/bash
# Test script for placement strategy pattern
# Demonstrates A/B testing of three placement algorithms

CIRCUIT="benchmarks/ISCAS/Verilog/s27.v"
LIB="benchmarks/NangateOpenCellLibrary_typical.lib"
LEF="benchmarks/NangateOpenCellLibrary.macro.lef"
CLK="5.0"
UTIL="0.7"

echo "========================================="
echo "Placement Strategy Pattern A/B Test"
echo "========================================="
echo ""

echo "Test 1: Basic Force-Directed (Baseline)"
echo "-----------------------------------------"
MINIEDA_PLACEMENT_ALGO=basic ./build/bin/mini_flow \
    -v $CIRCUIT -lib $LIB -lef $LEF -clk $CLK -util $UTIL 2>&1 | \
    grep -E "(Algorithm:|Mode:|HPWL|completed in)"
echo ""

echo "Test 2: Electrostatic Field (Nesterov)"
echo "-----------------------------------------"
MINIEDA_PLACEMENT_ALGO=nesterov ./build/bin/mini_flow \
    -v $CIRCUIT -lib $LIB -lef $LEF -clk $CLK -util $UTIL 2>&1 | \
    grep -E "(Algorithm:|Mode:|HPWL|completed in)"
echo ""

echo "Test 3: Hybrid Cascade (Best Practice)"
echo "-----------------------------------------"
MINIEDA_PLACEMENT_ALGO=hybrid ./build/bin/mini_flow \
    -v $CIRCUIT -lib $LIB -lef $LEF -clk $CLK -util $UTIL 2>&1 | \
    grep -E "(Algorithm:|Mode:|Phase|HPWL|completed in)"
echo ""

echo "========================================="
echo "A/B Test Completed!"
echo "========================================="
