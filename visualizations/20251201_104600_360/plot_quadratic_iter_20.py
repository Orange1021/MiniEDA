#!/usr/bin/env python3
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Create figure and axis
fig, ax = plt.subplots(1, 1, figsize=(12, 10))

# Draw core area boundary
core_rect = patches.Rectangle((0, 0), 17.1388, 18, linewidth=2, edgecolor='black', facecolor='none')
ax.add_patch(core_rect)

# Draw cells
cell_colors = []
# Cell: NOR2_26
cell_rect = patches.Rectangle((8.83169, 8.45818), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11669, 9.15818, 'NOR2_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_25
cell_rect = patches.Rectangle((8.98963, 8.38251), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.27463, 9.08251, 'NOR2_25', ha='center', va='center', fontsize=8)

# Cell: NOR2_23
cell_rect = patches.Rectangle((8.76954, 8.35197), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05454, 9.05197, 'NOR2_23', ha='center', va='center', fontsize=8)

# Cell: NOR2_21
cell_rect = patches.Rectangle((8.71859, 8.68095), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00359, 9.38095, 'NOR2_21', ha='center', va='center', fontsize=8)

# Cell: NOR2_20
cell_rect = patches.Rectangle((8.52743, 8.58974), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81243, 9.28974, 'NOR2_20', ha='center', va='center', fontsize=8)

# Cell: NOR2_18
cell_rect = patches.Rectangle((8.21353, 8.9287), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.49853, 9.6287, 'NOR2_18', ha='center', va='center', fontsize=8)

# Cell: NOR2_17
cell_rect = patches.Rectangle((8.17102, 9.0483), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45602, 9.7483, 'NOR2_17', ha='center', va='center', fontsize=8)

# Cell: NOR2_16
cell_rect = patches.Rectangle((8.15578, 8.88475), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.44078, 9.58475, 'NOR2_16', ha='center', va='center', fontsize=8)

# Cell: NOR2_12
cell_rect = patches.Rectangle((8.99308, 8.84215), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.27808, 9.54215, 'NOR2_12', ha='center', va='center', fontsize=8)

# Cell: NOR2_5
cell_rect = patches.Rectangle((8.0804, 8.36646), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.3654, 9.06646, 'NOR2_5', ha='center', va='center', fontsize=8)

# Cell: NOR2_4
cell_rect = patches.Rectangle((8.19961, 8.44102), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.48461, 9.14102, 'NOR2_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((9.01707, 9.03821), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.30207, 9.73821, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_14
cell_rect = patches.Rectangle((8.92089, 8.3382), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.20589, 9.0382, 'NAND2_14', ha='center', va='center', fontsize=8)

# Cell: NOR2_9
cell_rect = patches.Rectangle((8.39565, 8.98266), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68065, 9.68266, 'NOR2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_12
cell_rect = patches.Rectangle((8.92276, 8.93307), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.20776, 9.63307, 'NAND2_12', ha='center', va='center', fontsize=8)

# Cell: NAND2_11
cell_rect = patches.Rectangle((9.00591, 8.91893), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.29091, 9.61893, 'NAND2_11', ha='center', va='center', fontsize=8)

# Cell: NAND2_9
cell_rect = patches.Rectangle((8.82237, 8.93385), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.10737, 9.63385, 'NAND2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_7
cell_rect = patches.Rectangle((8.96572, 8.96116), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.25072, 9.66116, 'NAND2_7', ha='center', va='center', fontsize=8)

# Cell: NAND2_5
cell_rect = patches.Rectangle((8.92205, 8.99738), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.20705, 9.69738, 'NAND2_5', ha='center', va='center', fontsize=8)

# Cell: NAND2_4
cell_rect = patches.Rectangle((8.95013, 8.98665), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.23513, 9.68665, 'NAND2_4', ha='center', va='center', fontsize=8)

# Cell: NAND2_3
cell_rect = patches.Rectangle((8.43591, 8.55543), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72091, 9.25543, 'NAND2_3', ha='center', va='center', fontsize=8)

# Cell: NAND2_2
cell_rect = patches.Rectangle((8.3672, 8.61995), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6522, 9.31995, 'NAND2_2', ha='center', va='center', fontsize=8)

# Cell: NAND2_1
cell_rect = patches.Rectangle((8.32662, 8.62465), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61162, 9.32465, 'NAND2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((8.29164, 8.52342), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57664, 9.22342, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR3_2
cell_rect = patches.Rectangle((8.81525, 8.42918), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.19525, 9.12918, 'OR3_2', ha='center', va='center', fontsize=8)

# Cell: OR3_1
cell_rect = patches.Rectangle((8.58274, 8.68479), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96274, 9.38479, 'OR3_1', ha='center', va='center', fontsize=8)

# Cell: OR3_0
cell_rect = patches.Rectangle((8.42295, 8.6654), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80295, 9.3654, 'OR3_0', ha='center', va='center', fontsize=8)

# Cell: OR2_5
cell_rect = patches.Rectangle((8.78268, 8.96447), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.16268, 9.66447, 'OR2_5', ha='center', va='center', fontsize=8)

# Cell: OR2_4
cell_rect = patches.Rectangle((8.87072, 8.96116), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.25072, 9.66116, 'OR2_4', ha='center', va='center', fontsize=8)

# Cell: OR2_3
cell_rect = patches.Rectangle((8.79996, 8.42426), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.17996, 9.12426, 'OR2_3', ha='center', va='center', fontsize=8)

# Cell: OR2_2
cell_rect = patches.Rectangle((8.57636, 8.69273), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95636, 9.39273, 'OR2_2', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((8.42951, 8.67977), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80951, 9.37977, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_15
cell_rect = patches.Rectangle((8.5392, 8.70353), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8242, 9.40353, 'NOR2_15', ha='center', va='center', fontsize=8)

# Cell: NOR2_10
cell_rect = patches.Rectangle((8.14622, 9.07995), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.43122, 9.77995, 'NOR2_10', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((8.29675, 8.46744), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67675, 9.16744, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: AND2_40
cell_rect = patches.Rectangle((8.86875, 8.3517), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.24875, 9.0517, 'AND2_40', ha='center', va='center', fontsize=8)

# Cell: AND2_39
cell_rect = patches.Rectangle((8.79928, 8.26818), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.17928, 8.96818, 'AND2_39', ha='center', va='center', fontsize=8)

# Cell: AND2_38
cell_rect = patches.Rectangle((8.64945, 8.44745), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02945, 9.14745, 'AND2_38', ha='center', va='center', fontsize=8)

# Cell: AND2_37
cell_rect = patches.Rectangle((8.83802, 8.36411), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.21802, 9.06411, 'AND2_37', ha='center', va='center', fontsize=8)

# Cell: AND3_2
cell_rect = patches.Rectangle((8.79739, 8.416), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.17739, 9.116, 'AND3_2', ha='center', va='center', fontsize=8)

# Cell: AND2_35
cell_rect = patches.Rectangle((8.51518, 8.34232), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89518, 9.04232, 'AND2_35', ha='center', va='center', fontsize=8)

# Cell: AND2_32
cell_rect = patches.Rectangle((8.48537, 8.69436), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86537, 9.39436, 'AND2_32', ha='center', va='center', fontsize=8)

# Cell: AND2_31
cell_rect = patches.Rectangle((7.96376, 8.96003), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.34376, 9.66003, 'AND2_31', ha='center', va='center', fontsize=8)

# Cell: NOR2_13
cell_rect = patches.Rectangle((8.95456, 8.84615), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.23956, 9.54615, 'NOR2_13', ha='center', va='center', fontsize=8)

# Cell: AND2_30
cell_rect = patches.Rectangle((8.07933, 9.03747), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45933, 9.73747, 'AND2_30', ha='center', va='center', fontsize=8)

# Cell: NOT_20
cell_rect = patches.Rectangle((8.05328, 8.44943), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.24328, 9.14943, 'NOT_20', ha='center', va='center', fontsize=8)

# Cell: NOT_13
cell_rect = patches.Rectangle((9.03694, 8.53823), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.22694, 9.23823, 'NOT_13', ha='center', va='center', fontsize=8)

# Cell: AND2_14
cell_rect = patches.Rectangle((8.80216, 8.42154), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.18216, 9.12154, 'AND2_14', ha='center', va='center', fontsize=8)

# Cell: NOT_12
cell_rect = patches.Rectangle((8.91524, 8.96895), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.10524, 9.66895, 'NOT_12', ha='center', va='center', fontsize=8)

# Cell: NOT_11
cell_rect = patches.Rectangle((8.92209, 8.97343), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11209, 9.67343, 'NOT_11', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((7.34462, 8.84378), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95962, 9.54378, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: NOT_27
cell_rect = patches.Rectangle((8.85154, 8.96225), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04154, 9.66225, 'NOT_27', ha='center', va='center', fontsize=8)

# Cell: NOR2_8
cell_rect = patches.Rectangle((8.14406, 9.01868), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.42906, 9.71868, 'NOR2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_10
cell_rect = patches.Rectangle((8.46361, 8.66438), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65361, 9.36438, 'NOT_10', ha='center', va='center', fontsize=8)

# Cell: P5
cell_rect = patches.Rectangle((8.6543, 8.65973), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8443, 9.35973, 'P5', ha='center', va='center', fontsize=8)

# Cell: AND2_29
cell_rect = patches.Rectangle((8.19709, 8.95867), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57709, 9.65867, 'AND2_29', ha='center', va='center', fontsize=8)

# Cell: NOT_4
cell_rect = patches.Rectangle((8.71738, 8.68906), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90738, 9.38906, 'NOT_4', ha='center', va='center', fontsize=8)

# Cell: DFF_14
cell_rect = patches.Rectangle((7.01896, 8.57651), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63396, 9.27651, 'DFF_14', ha='center', va='center', fontsize=8)

# Cell: AND2_10
cell_rect = patches.Rectangle((8.43408, 8.13737), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81408, 8.83737, 'AND2_10', ha='center', va='center', fontsize=8)

# Cell: NOT_14
cell_rect = patches.Rectangle((9.01298, 9.00639), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.20298, 9.70639, 'NOT_14', ha='center', va='center', fontsize=8)

# Cell: NOT_2
cell_rect = patches.Rectangle((8.96268, 8.97191), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.15268, 9.67191, 'NOT_2', ha='center', va='center', fontsize=8)

# Cell: DFF_11
cell_rect = patches.Rectangle((7.08408, 8.61337), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69908, 9.31337, 'DFF_11', ha='center', va='center', fontsize=8)

# Cell: NOR2_6
cell_rect = patches.Rectangle((7.89152, 8.4926), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.17652, 9.1926, 'NOR2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_18
cell_rect = patches.Rectangle((8.26225, 8.42415), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45225, 9.12415, 'NOT_18', ha='center', va='center', fontsize=8)

# Cell: NOT_28
cell_rect = patches.Rectangle((8.26412, 9.03147), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45412, 9.73147, 'NOT_28', ha='center', va='center', fontsize=8)

# Cell: NOT_5
cell_rect = patches.Rectangle((8.62992, 8.68606), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81992, 9.38606, 'NOT_5', ha='center', va='center', fontsize=8)

# Cell: P4
cell_rect = patches.Rectangle((8.56778, 8.44238), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75778, 9.14238, 'P4', ha='center', va='center', fontsize=8)

# Cell: AND2_11
cell_rect = patches.Rectangle((7.85248, 8.43799), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.23248, 9.13799, 'AND2_11', ha='center', va='center', fontsize=8)

# Cell: DFF_8
cell_rect = patches.Rectangle((7.07934, 8.82118), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69434, 9.52118, 'DFF_8', ha='center', va='center', fontsize=8)

# Cell: AND2_18
cell_rect = patches.Rectangle((7.90267, 9.08628), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.28267, 9.78628, 'AND2_18', ha='center', va='center', fontsize=8)

# Cell: P3
cell_rect = patches.Rectangle((8.33492, 8.96016), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52492, 9.66016, 'P3', ha='center', va='center', fontsize=8)

# Cell: NAND2_16
cell_rect = patches.Rectangle((8.87167, 8.28558), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.15667, 8.98558, 'NAND2_16', ha='center', va='center', fontsize=8)

# Cell: A1
cell_rect = patches.Rectangle((7.88933, 8.46998), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.07933, 9.16998, 'A1', ha='center', va='center', fontsize=8)

# Cell: NOT_43
cell_rect = patches.Rectangle((8.04652, 8.44926), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.23652, 9.14926, 'NOT_43', ha='center', va='center', fontsize=8)

# Cell: AND2_21
cell_rect = patches.Rectangle((8.26821, 9.03649), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64821, 9.73649, 'AND2_21', ha='center', va='center', fontsize=8)

# Cell: NOT_16
cell_rect = patches.Rectangle((8.76458, 8.97544), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95458, 9.67544, 'NOT_16', ha='center', va='center', fontsize=8)

# Cell: START
cell_rect = patches.Rectangle((9.06889, 8.7635), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.25889, 9.4635, 'START', ha='center', va='center', fontsize=8)

# Cell: NOT_49
cell_rect = patches.Rectangle((8.70535, 8.70674), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89535, 9.40674, 'NOT_49', ha='center', va='center', fontsize=8)

# Cell: P6
cell_rect = patches.Rectangle((8.77951, 8.66506), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96951, 9.36506, 'P6', ha='center', va='center', fontsize=8)

# Cell: NOT_9
cell_rect = patches.Rectangle((8.40836, 8.88044), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59836, 9.58044, 'NOT_9', ha='center', va='center', fontsize=8)

# Cell: DFF_7
cell_rect = patches.Rectangle((7.06455, 8.80679), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67955, 9.50679, 'DFF_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_19
cell_rect = patches.Rectangle((8.28767, 8.95784), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57267, 9.65784, 'NOR2_19', ha='center', va='center', fontsize=8)

# Cell: NOR3_0
cell_rect = patches.Rectangle((8.75514, 8.91134), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04014, 9.61134, 'NOR3_0', ha='center', va='center', fontsize=8)

# Cell: CNTVCO2
cell_rect = patches.Rectangle((9.19103, 8.99445), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.38103, 9.69445, 'CNTVCO2', ha='center', va='center', fontsize=8)

# Cell: NOT_52
cell_rect = patches.Rectangle((8.91104, 8.59009), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.10104, 9.29009, 'NOT_52', ha='center', va='center', fontsize=8)

# Cell: A0
cell_rect = patches.Rectangle((8.00982, 8.0143), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.19982, 8.7143, 'A0', ha='center', va='center', fontsize=8)

# Cell: NOT_29
cell_rect = patches.Rectangle((8.44825, 8.99192), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63825, 9.69192, 'NOT_29', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((8.5823, 8.71807), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7723, 9.41807, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_9
cell_rect = patches.Rectangle((7.06038, 8.79124), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67538, 9.49124, 'DFF_9', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((8.97365, 8.9854), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.16365, 9.6854, 'NOT_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_6
cell_rect = patches.Rectangle((8.40812, 8.47644), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69312, 9.17644, 'NAND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_15
cell_rect = patches.Rectangle((8.48611, 8.63872), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67611, 9.33872, 'NOT_15', ha='center', va='center', fontsize=8)

# Cell: AND2_1
cell_rect = patches.Rectangle((8.03396, 8.50478), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.41396, 9.20478, 'AND2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_24
cell_rect = patches.Rectangle((8.9431, 8.38404), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.2281, 9.08404, 'NOR2_24', ha='center', va='center', fontsize=8)

# Cell: NOR2_14
cell_rect = patches.Rectangle((8.94605, 8.81706), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.23105, 9.51706, 'NOR2_14', ha='center', va='center', fontsize=8)

# Cell: B0
cell_rect = patches.Rectangle((7.9472, 8.95858), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.1372, 9.65858, 'B0', ha='center', va='center', fontsize=8)

# Cell: A2
cell_rect = patches.Rectangle((8.12237, 8.31284), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.31237, 9.01284, 'A2', ha='center', va='center', fontsize=8)

# Cell: NAND2_8
cell_rect = patches.Rectangle((8.87768, 8.96447), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.16268, 9.66447, 'NAND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_46
cell_rect = patches.Rectangle((8.20566, 9.19198), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.39566, 9.89198, 'NOT_46', ha='center', va='center', fontsize=8)

# Cell: B2
cell_rect = patches.Rectangle((8.57114, 8.99236), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76114, 9.69236, 'B2', ha='center', va='center', fontsize=8)

# Cell: READY
cell_rect = patches.Rectangle((8.79967, 8.95883), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98967, 9.65883, 'READY', ha='center', va='center', fontsize=8)

# Cell: NOT_7
cell_rect = patches.Rectangle((8.39677, 8.89923), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58677, 9.59923, 'NOT_7', ha='center', va='center', fontsize=8)

# Cell: AND2_25
cell_rect = patches.Rectangle((8.50018, 8.37552), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88018, 9.07552, 'AND2_25', ha='center', va='center', fontsize=8)

# Cell: NAND3_0
cell_rect = patches.Rectangle((8.77344, 8.97018), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05844, 9.67018, 'NAND3_0', ha='center', va='center', fontsize=8)

# Cell: P1
cell_rect = patches.Rectangle((8.35194, 8.93816), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54194, 9.63816, 'P1', ha='center', va='center', fontsize=8)

# Cell: AND2_9
cell_rect = patches.Rectangle((7.78418, 8.44915), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.16418, 9.14915, 'AND2_9', ha='center', va='center', fontsize=8)

# Cell: P0
cell_rect = patches.Rectangle((8.42099, 8.63799), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61099, 9.33799, 'P0', ha='center', va='center', fontsize=8)

# Cell: DFF_6
cell_rect = patches.Rectangle((7.13949, 8.60572), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75449, 9.30572, 'DFF_6', ha='center', va='center', fontsize=8)

# Cell: AND2_17
cell_rect = patches.Rectangle((8.12637, 8.91524), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.50637, 9.61524, 'AND2_17', ha='center', va='center', fontsize=8)

# Cell: NOT_25
cell_rect = patches.Rectangle((8.76609, 8.47003), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95609, 9.17003, 'NOT_25', ha='center', va='center', fontsize=8)

# Cell: NOT_24
cell_rect = patches.Rectangle((8.6119, 8.66951), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8019, 9.36951, 'NOT_24', ha='center', va='center', fontsize=8)

# Cell: NOT_21
cell_rect = patches.Rectangle((8.13732, 8.20827), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.32732, 8.90827, 'NOT_21', ha='center', va='center', fontsize=8)

# Cell: NOT_8
cell_rect = patches.Rectangle((8.44958, 8.89022), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63958, 9.59022, 'NOT_8', ha='center', va='center', fontsize=8)

# Cell: NOT_55
cell_rect = patches.Rectangle((8.98578, 8.31967), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.17578, 9.01967, 'NOT_55', ha='center', va='center', fontsize=8)

# Cell: DFF_5
cell_rect = patches.Rectangle((7.17787, 8.70687), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79287, 9.40687, 'DFF_5', ha='center', va='center', fontsize=8)

# Cell: AND2_26
cell_rect = patches.Rectangle((8.42835, 8.69018), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80835, 9.39018, 'AND2_26', ha='center', va='center', fontsize=8)

# Cell: CNTVCON2
cell_rect = patches.Rectangle((9.02339, 9.0051), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.21339, 9.7051, 'CNTVCON2', ha='center', va='center', fontsize=8)

# Cell: NOT_3
cell_rect = patches.Rectangle((8.86197, 8.51725), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05197, 9.21725, 'NOT_3', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((7.36208, 8.84238), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97708, 9.54238, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: AND2_7
cell_rect = patches.Rectangle((8.21992, 8.87786), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59992, 9.57786, 'AND2_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((8.54141, 8.8758), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82641, 9.5758, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: DFF_13
cell_rect = patches.Rectangle((7.03064, 8.64056), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64564, 9.34056, 'DFF_13', ha='center', va='center', fontsize=8)

# Cell: AND2_19
cell_rect = patches.Rectangle((8.00548, 9.29065), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.38548, 9.99065, 'AND2_19', ha='center', va='center', fontsize=8)

# Cell: DFF_4
cell_rect = patches.Rectangle((7.22708, 8.70915), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84208, 9.40915, 'DFF_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((8.88334, 8.99689), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.16834, 9.69689, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_13
cell_rect = patches.Rectangle((8.906, 8.29817), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.191, 8.99817, 'NAND2_13', ha='center', va='center', fontsize=8)

# Cell: NOT_37
cell_rect = patches.Rectangle((8.89068, 8.46445), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.08068, 9.16445, 'NOT_37', ha='center', va='center', fontsize=8)

# Cell: NOR2_7
cell_rect = patches.Rectangle((7.98151, 8.11174), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.26651, 8.81174, 'NOR2_7', ha='center', va='center', fontsize=8)

# Cell: AND2_22
cell_rect = patches.Rectangle((8.74625, 8.3597), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.12625, 9.0597, 'AND2_22', ha='center', va='center', fontsize=8)

# Cell: P2
cell_rect = patches.Rectangle((8.40367, 8.9459), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59367, 9.6459, 'P2', ha='center', va='center', fontsize=8)

# Cell: B3
cell_rect = patches.Rectangle((8.19559, 9.05389), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.38559, 9.75389, 'B3', ha='center', va='center', fontsize=8)

# Cell: AND2_3
cell_rect = patches.Rectangle((8.04499, 8.48118), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.42499, 9.18118, 'AND2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((9.07579, 8.97045), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.26579, 9.67045, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: A3
cell_rect = patches.Rectangle((8.2591, 8.41674), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.4491, 9.11674, 'A3', ha='center', va='center', fontsize=8)

# Cell: P7
cell_rect = patches.Rectangle((8.97777, 8.42796), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.16777, 9.12796, 'P7', ha='center', va='center', fontsize=8)

# Cell: DFF_12
cell_rect = patches.Rectangle((7.04981, 8.63251), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66481, 9.33251, 'DFF_12', ha='center', va='center', fontsize=8)

# Cell: NOT_48
cell_rect = patches.Rectangle((8.41399, 8.99396), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.60399, 9.69396, 'NOT_48', ha='center', va='center', fontsize=8)

# Cell: DFF_10
cell_rect = patches.Rectangle((7.10324, 8.69473), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71824, 9.39473, 'DFF_10', ha='center', va='center', fontsize=8)

# Cell: NAND2_10
cell_rect = patches.Rectangle((8.37072, 8.53452), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65572, 9.23452, 'NAND2_10', ha='center', va='center', fontsize=8)

# Cell: DFF_3
cell_rect = patches.Rectangle((7.29852, 8.61674), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91352, 9.31674, 'DFF_3', ha='center', va='center', fontsize=8)

# Cell: NOT_31
cell_rect = patches.Rectangle((8.12392, 8.98525), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.31392, 9.68525, 'NOT_31', ha='center', va='center', fontsize=8)

# Cell: NOT_23
cell_rect = patches.Rectangle((8.55048, 8.64271), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74048, 9.34271, 'NOT_23', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((7.31813, 8.83936), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93313, 9.53936, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: AND2_4
cell_rect = patches.Rectangle((8.064, 9.01101), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.444, 9.71101, 'AND2_4', ha='center', va='center', fontsize=8)

# Cell: NOT_26
cell_rect = patches.Rectangle((9.1089, 8.98352), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.2989, 9.68352, 'NOT_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_27
cell_rect = patches.Rectangle((8.87467, 8.35353), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.15967, 9.05353, 'NOR2_27', ha='center', va='center', fontsize=8)

# Cell: NOT_39
cell_rect = patches.Rectangle((8.97941, 8.90083), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.16941, 9.60083, 'NOT_39', ha='center', va='center', fontsize=8)

# Cell: NOT_30
cell_rect = patches.Rectangle((8.26856, 9.07573), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45856, 9.77573, 'NOT_30', ha='center', va='center', fontsize=8)

# Cell: NOT_32
cell_rect = patches.Rectangle((8.62699, 8.6541), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81699, 9.3541, 'NOT_32', ha='center', va='center', fontsize=8)

# Cell: NOT_19
cell_rect = patches.Rectangle((8.19671, 8.35787), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.38671, 9.05787, 'NOT_19', ha='center', va='center', fontsize=8)

# Cell: NOT_34
cell_rect = patches.Rectangle((8.53645, 8.58421), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72645, 9.28421, 'NOT_34', ha='center', va='center', fontsize=8)

# Cell: NOR2_28
cell_rect = patches.Rectangle((8.81168, 8.34078), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09668, 9.04078, 'NOR2_28', ha='center', va='center', fontsize=8)

# Cell: NOR2_3
cell_rect = patches.Rectangle((8.81753, 8.9769), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.10253, 9.6769, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_35
cell_rect = patches.Rectangle((8.90082, 8.40296), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09082, 9.10296, 'NOT_35', ha='center', va='center', fontsize=8)

# Cell: AND2_8
cell_rect = patches.Rectangle((7.86314, 8.09885), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.24314, 8.79885, 'AND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_36
cell_rect = patches.Rectangle((8.87555, 8.36753), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06555, 9.06753, 'NOT_36', ha='center', va='center', fontsize=8)

# Cell: AND2_15
cell_rect = patches.Rectangle((8.20336, 8.91704), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58336, 9.61704, 'AND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_41
cell_rect = patches.Rectangle((7.98144, 8.47473), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.17144, 9.17473, 'NOT_41', ha='center', va='center', fontsize=8)

# Cell: NOT_42
cell_rect = patches.Rectangle((8.6617, 8.14824), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8517, 8.84824, 'NOT_42', ha='center', va='center', fontsize=8)

# Cell: NOT_44
cell_rect = patches.Rectangle((8.37572, 8.65527), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56572, 9.35527, 'NOT_44', ha='center', va='center', fontsize=8)

# Cell: AND2_36
cell_rect = patches.Rectangle((8.8249, 8.43), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.2049, 9.13, 'AND2_36', ha='center', va='center', fontsize=8)

# Cell: NOT_45
cell_rect = patches.Rectangle((8.13892, 9.0198), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.32892, 9.7198, 'NOT_45', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((8.07907, 8.34984), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45907, 9.04984, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: NOR2_11
cell_rect = patches.Rectangle((7.97457, 8.91936), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.25957, 9.61936, 'NOR2_11', ha='center', va='center', fontsize=8)

# Cell: NOT_47
cell_rect = patches.Rectangle((8.22893, 8.92043), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.41893, 9.62043, 'NOT_47', ha='center', va='center', fontsize=8)

# Cell: NOT_50
cell_rect = patches.Rectangle((8.79196, 8.5327), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98196, 9.2327, 'NOT_50', ha='center', va='center', fontsize=8)

# Cell: NOT_33
cell_rect = patches.Rectangle((8.34959, 8.93156), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53959, 9.63156, 'NOT_33', ha='center', va='center', fontsize=8)

# Cell: NOT_51
cell_rect = patches.Rectangle((8.92418, 8.55085), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11418, 9.25085, 'NOT_51', ha='center', va='center', fontsize=8)

# Cell: NOT_53
cell_rect = patches.Rectangle((8.81142, 8.23576), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00142, 8.93576, 'NOT_53', ha='center', va='center', fontsize=8)

# Cell: AND2_33
cell_rect = patches.Rectangle((8.59595, 8.70945), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97595, 9.40945, 'AND2_33', ha='center', va='center', fontsize=8)

# Cell: AND2_2
cell_rect = patches.Rectangle((8.28242, 8.38108), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66242, 9.08108, 'AND2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_22
cell_rect = patches.Rectangle((8.71218, 8.63001), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99718, 9.33001, 'NOR2_22', ha='center', va='center', fontsize=8)

# Cell: NAND2_15
cell_rect = patches.Rectangle((8.90216, 8.31892), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.18716, 9.01892, 'NAND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_56
cell_rect = patches.Rectangle((8.90849, 8.34666), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09849, 9.04666, 'NOT_56', ha='center', va='center', fontsize=8)

# Cell: NOT_57
cell_rect = patches.Rectangle((9.01799, 8.21504), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.20799, 8.91504, 'NOT_57', ha='center', va='center', fontsize=8)

# Cell: AND2_5
cell_rect = patches.Rectangle((8.10521, 9.0798), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.48521, 9.7798, 'AND2_5', ha='center', va='center', fontsize=8)

# Cell: AND2_6
cell_rect = patches.Rectangle((8.10546, 8.94331), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.48546, 9.64331, 'AND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_6
cell_rect = patches.Rectangle((8.56573, 8.51217), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75573, 9.21217, 'NOT_6', ha='center', va='center', fontsize=8)

# Cell: NOT_17
cell_rect = patches.Rectangle((8.556, 9.00175), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.746, 9.70175, 'NOT_17', ha='center', va='center', fontsize=8)

# Cell: AND2_12
cell_rect = patches.Rectangle((8.44041, 8.68812), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82041, 9.38812, 'AND2_12', ha='center', va='center', fontsize=8)

# Cell: NOT_22
cell_rect = patches.Rectangle((8.43314, 8.49795), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62314, 9.19795, 'NOT_22', ha='center', va='center', fontsize=8)

# Cell: NOT_38
cell_rect = patches.Rectangle((8.85506, 8.42443), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04506, 9.12443, 'NOT_38', ha='center', va='center', fontsize=8)

# Cell: AND2_13
cell_rect = patches.Rectangle((8.59073, 8.68719), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97073, 9.38719, 'AND2_13', ha='center', va='center', fontsize=8)

# Cell: B1
cell_rect = patches.Rectangle((8.21336, 9.12722), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.40336, 9.82722, 'B1', ha='center', va='center', fontsize=8)

# Cell: AND2_16
cell_rect = patches.Rectangle((8.19298, 8.95015), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57298, 9.65015, 'AND2_16', ha='center', va='center', fontsize=8)

# Cell: AND2_34
cell_rect = patches.Rectangle((8.65736, 8.6766), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03736, 9.3766, 'AND2_34', ha='center', va='center', fontsize=8)

# Cell: NOT_54
cell_rect = patches.Rectangle((9.02409, 8.37724), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.21409, 9.07724, 'NOT_54', ha='center', va='center', fontsize=8)

# Cell: AND2_20
cell_rect = patches.Rectangle((8.01005, 8.91468), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.39005, 9.61468, 'AND2_20', ha='center', va='center', fontsize=8)

# Cell: NOT_40
cell_rect = patches.Rectangle((8.07253, 8.10105), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.26253, 8.80105, 'NOT_40', ha='center', va='center', fontsize=8)

# Cell: AND2_23
cell_rect = patches.Rectangle((8.69662, 8.49006), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07662, 9.19006, 'AND2_23', ha='center', va='center', fontsize=8)

# Cell: NOT_58
cell_rect = patches.Rectangle((9.07091, 8.31697), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.26091, 9.01697, 'NOT_58', ha='center', va='center', fontsize=8)

# Cell: AND2_24
cell_rect = patches.Rectangle((8.5792, 8.50029), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9592, 9.20029, 'AND2_24', ha='center', va='center', fontsize=8)

# Cell: AND3_1
cell_rect = patches.Rectangle((8.59846, 8.69238), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97846, 9.39238, 'AND3_1', ha='center', va='center', fontsize=8)

# Cell: AND3_0
cell_rect = patches.Rectangle((8.4512, 8.66705), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8312, 9.36705, 'AND3_0', ha='center', va='center', fontsize=8)

# Cell: AND2_27
cell_rect = patches.Rectangle((8.19814, 8.80018), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57814, 9.50018, 'AND2_27', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((12.5396, 2.29842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(12.7296, 2.99842, 'VDD', ha='center', va='center', fontsize=8)

# Cell: AND2_28
cell_rect = patches.Rectangle((8.08888, 8.95769), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.46888, 9.65769, 'AND2_28', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 27.1388)
ax.set_ylim(-10, 28)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251201_104600_360/quadratic_iter_20.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251201_104600_360/quadratic_iter_20.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251201_104600_360/quadratic_iter_20.png")
