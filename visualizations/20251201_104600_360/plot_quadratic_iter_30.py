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
cell_rect = patches.Rectangle((8.71835, 8.53188), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00335, 9.23188, 'NOR2_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_25
cell_rect = patches.Rectangle((8.80546, 8.45891), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09046, 9.15891, 'NOR2_25', ha='center', va='center', fontsize=8)

# Cell: NOR2_23
cell_rect = patches.Rectangle((8.68305, 8.51108), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96805, 9.21108, 'NOR2_23', ha='center', va='center', fontsize=8)

# Cell: NOR2_21
cell_rect = patches.Rectangle((8.68161, 8.593), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96661, 9.293, 'NOR2_21', ha='center', va='center', fontsize=8)

# Cell: NOR2_20
cell_rect = patches.Rectangle((8.57568, 8.58175), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86068, 9.28175, 'NOR2_20', ha='center', va='center', fontsize=8)

# Cell: NOR2_18
cell_rect = patches.Rectangle((8.30494, 8.89273), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58994, 9.59273, 'NOR2_18', ha='center', va='center', fontsize=8)

# Cell: NOR2_17
cell_rect = patches.Rectangle((8.29432, 8.91756), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57932, 9.61756, 'NOR2_17', ha='center', va='center', fontsize=8)

# Cell: NOR2_16
cell_rect = patches.Rectangle((8.30826, 8.8571), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59326, 9.5571, 'NOR2_16', ha='center', va='center', fontsize=8)

# Cell: NOR2_12
cell_rect = patches.Rectangle((8.83119, 8.80522), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11619, 9.50522, 'NOR2_12', ha='center', va='center', fontsize=8)

# Cell: NOR2_5
cell_rect = patches.Rectangle((8.24254, 8.4822), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52754, 9.1822, 'NOR2_5', ha='center', va='center', fontsize=8)

# Cell: NOR2_4
cell_rect = patches.Rectangle((8.27955, 8.5019), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56455, 9.2019, 'NOR2_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((8.80877, 8.90773), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09377, 9.60773, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_14
cell_rect = patches.Rectangle((8.79477, 8.51198), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07977, 9.21198, 'NAND2_14', ha='center', va='center', fontsize=8)

# Cell: NOR2_9
cell_rect = patches.Rectangle((8.31372, 8.93201), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59872, 9.63201, 'NOR2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_12
cell_rect = patches.Rectangle((8.78207, 8.84861), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06707, 9.54861, 'NAND2_12', ha='center', va='center', fontsize=8)

# Cell: NAND2_11
cell_rect = patches.Rectangle((8.81568, 8.85449), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.10068, 9.55449, 'NAND2_11', ha='center', va='center', fontsize=8)

# Cell: NAND2_9
cell_rect = patches.Rectangle((8.71615, 8.84897), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00115, 9.54897, 'NAND2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_7
cell_rect = patches.Rectangle((8.78633, 8.8712), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07133, 9.5712, 'NAND2_7', ha='center', va='center', fontsize=8)

# Cell: NAND2_5
cell_rect = patches.Rectangle((8.75294, 8.88589), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03794, 9.58589, 'NAND2_5', ha='center', va='center', fontsize=8)

# Cell: NAND2_4
cell_rect = patches.Rectangle((8.7677, 8.88513), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.0527, 9.58513, 'NAND2_4', ha='center', va='center', fontsize=8)

# Cell: NAND2_3
cell_rect = patches.Rectangle((8.46922, 8.58755), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75422, 9.28755, 'NAND2_3', ha='center', va='center', fontsize=8)

# Cell: NAND2_2
cell_rect = patches.Rectangle((8.43788, 8.61912), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72288, 9.31912, 'NAND2_2', ha='center', va='center', fontsize=8)

# Cell: NAND2_1
cell_rect = patches.Rectangle((8.41064, 8.6217), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69564, 9.3217, 'NAND2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((8.39366, 8.6027), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67866, 9.3027, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR3_2
cell_rect = patches.Rectangle((8.67653, 8.47859), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05653, 9.17859, 'OR3_2', ha='center', va='center', fontsize=8)

# Cell: OR3_1
cell_rect = patches.Rectangle((8.56189, 8.60272), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94189, 9.30272, 'OR3_1', ha='center', va='center', fontsize=8)

# Cell: OR3_0
cell_rect = patches.Rectangle((8.45668, 8.60802), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83668, 9.30802, 'OR3_0', ha='center', va='center', fontsize=8)

# Cell: OR2_5
cell_rect = patches.Rectangle((8.65268, 8.86288), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03268, 9.56288, 'OR2_5', ha='center', va='center', fontsize=8)

# Cell: OR2_4
cell_rect = patches.Rectangle((8.69133, 8.8712), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07133, 9.5712, 'OR2_4', ha='center', va='center', fontsize=8)

# Cell: OR2_3
cell_rect = patches.Rectangle((8.66927, 8.47935), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04927, 9.17935, 'OR2_3', ha='center', va='center', fontsize=8)

# Cell: OR2_2
cell_rect = patches.Rectangle((8.55726, 8.60688), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93726, 9.30688, 'OR2_2', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((8.45583, 8.61103), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83583, 9.31103, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_15
cell_rect = patches.Rectangle((8.56523, 8.61352), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85023, 9.31352, 'NOR2_15', ha='center', va='center', fontsize=8)

# Cell: NOR2_10
cell_rect = patches.Rectangle((8.26151, 8.95806), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54651, 9.65806, 'NOR2_10', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((8.38165, 8.59718), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76165, 9.29718, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: AND2_40
cell_rect = patches.Rectangle((8.68657, 8.47939), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06657, 9.17939, 'AND2_40', ha='center', va='center', fontsize=8)

# Cell: AND2_39
cell_rect = patches.Rectangle((8.68788, 8.45219), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06788, 9.15219, 'AND2_39', ha='center', va='center', fontsize=8)

# Cell: AND2_38
cell_rect = patches.Rectangle((8.60627, 8.53282), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98627, 9.23282, 'AND2_38', ha='center', va='center', fontsize=8)

# Cell: AND2_37
cell_rect = patches.Rectangle((8.70131, 8.45359), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.08131, 9.15359, 'AND2_37', ha='center', va='center', fontsize=8)

# Cell: AND3_2
cell_rect = patches.Rectangle((8.67278, 8.47669), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05278, 9.17669, 'AND3_2', ha='center', va='center', fontsize=8)

# Cell: AND2_35
cell_rect = patches.Rectangle((8.5488, 8.51144), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9288, 9.21144, 'AND2_35', ha='center', va='center', fontsize=8)

# Cell: AND2_32
cell_rect = patches.Rectangle((8.48397, 8.60678), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86397, 9.30678, 'AND2_32', ha='center', va='center', fontsize=8)

# Cell: AND2_31
cell_rect = patches.Rectangle((8.16123, 8.90206), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54123, 9.60206, 'AND2_31', ha='center', va='center', fontsize=8)

# Cell: NOR2_13
cell_rect = patches.Rectangle((8.8155, 8.80234), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.1005, 9.50234, 'NOR2_13', ha='center', va='center', fontsize=8)

# Cell: AND2_30
cell_rect = patches.Rectangle((8.18537, 8.93087), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56537, 9.63087, 'AND2_30', ha='center', va='center', fontsize=8)

# Cell: NOT_20
cell_rect = patches.Rectangle((8.2982, 8.51386), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.4882, 9.21386, 'NOT_20', ha='center', va='center', fontsize=8)

# Cell: NOT_13
cell_rect = patches.Rectangle((8.91584, 8.63871), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.10584, 9.33871, 'NOT_13', ha='center', va='center', fontsize=8)

# Cell: AND2_14
cell_rect = patches.Rectangle((8.67218, 8.47616), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05218, 9.17616, 'AND2_14', ha='center', va='center', fontsize=8)

# Cell: NOT_12
cell_rect = patches.Rectangle((8.79484, 8.8676), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98484, 9.5676, 'NOT_12', ha='center', va='center', fontsize=8)

# Cell: NOT_11
cell_rect = patches.Rectangle((8.80233, 8.86701), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99233, 9.56701, 'NOT_11', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((7.28517, 8.78312), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90017, 9.48312, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: NOT_27
cell_rect = patches.Rectangle((8.76903, 8.86046), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95903, 9.56046, 'NOT_27', ha='center', va='center', fontsize=8)

# Cell: NOR2_8
cell_rect = patches.Rectangle((8.26524, 8.93682), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.55024, 9.63682, 'NOR2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_10
cell_rect = patches.Rectangle((8.52738, 8.66547), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71738, 9.36547, 'NOT_10', ha='center', va='center', fontsize=8)

# Cell: P5
cell_rect = patches.Rectangle((8.66389, 8.60604), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85389, 9.30604, 'P5', ha='center', va='center', fontsize=8)

# Cell: AND2_29
cell_rect = patches.Rectangle((8.20974, 8.9115), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58974, 9.6115, 'AND2_29', ha='center', va='center', fontsize=8)

# Cell: NOT_4
cell_rect = patches.Rectangle((8.70607, 8.63378), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89607, 9.33378, 'NOT_4', ha='center', va='center', fontsize=8)

# Cell: DFF_14
cell_rect = patches.Rectangle((7.09075, 8.62752), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70575, 9.32752, 'DFF_14', ha='center', va='center', fontsize=8)

# Cell: AND2_10
cell_rect = patches.Rectangle((8.25515, 8.40954), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63515, 9.10954, 'AND2_10', ha='center', va='center', fontsize=8)

# Cell: NOT_14
cell_rect = patches.Rectangle((8.85228, 8.88708), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04228, 9.58708, 'NOT_14', ha='center', va='center', fontsize=8)

# Cell: NOT_2
cell_rect = patches.Rectangle((8.8196, 8.87033), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.0096, 9.57033, 'NOT_2', ha='center', va='center', fontsize=8)

# Cell: DFF_11
cell_rect = patches.Rectangle((7.12079, 8.63055), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73579, 9.33055, 'DFF_11', ha='center', va='center', fontsize=8)

# Cell: NOR2_6
cell_rect = patches.Rectangle((8.1755, 8.52183), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.4605, 9.22183, 'NOR2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_18
cell_rect = patches.Rectangle((8.3657, 8.50454), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5557, 9.20454, 'NOT_18', ha='center', va='center', fontsize=8)

# Cell: NOT_28
cell_rect = patches.Rectangle((8.37249, 8.93877), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56249, 9.63877, 'NOT_28', ha='center', va='center', fontsize=8)

# Cell: NOT_5
cell_rect = patches.Rectangle((8.6421, 8.63644), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8321, 9.33644, 'NOT_5', ha='center', va='center', fontsize=8)

# Cell: P4
cell_rect = patches.Rectangle((8.61678, 8.57961), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80678, 9.27961, 'P4', ha='center', va='center', fontsize=8)

# Cell: AND2_11
cell_rect = patches.Rectangle((8.08973, 8.49523), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.46973, 9.19523, 'AND2_11', ha='center', va='center', fontsize=8)

# Cell: DFF_8
cell_rect = patches.Rectangle((7.11127, 8.76914), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72627, 9.46914, 'DFF_8', ha='center', va='center', fontsize=8)

# Cell: AND2_18
cell_rect = patches.Rectangle((8.12592, 8.96223), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.50592, 9.66223, 'AND2_18', ha='center', va='center', fontsize=8)

# Cell: P3
cell_rect = patches.Rectangle((8.42765, 8.87819), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61765, 9.57819, 'P3', ha='center', va='center', fontsize=8)

# Cell: NAND2_16
cell_rect = patches.Rectangle((8.79876, 8.49487), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.08376, 9.19487, 'NAND2_16', ha='center', va='center', fontsize=8)

# Cell: A1
cell_rect = patches.Rectangle((8.22055, 8.50162), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.41055, 9.20162, 'A1', ha='center', va='center', fontsize=8)

# Cell: NOT_43
cell_rect = patches.Rectangle((8.28549, 8.5024), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.47549, 9.2024, 'NOT_43', ha='center', va='center', fontsize=8)

# Cell: AND2_21
cell_rect = patches.Rectangle((8.21653, 8.92352), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59653, 9.62352, 'AND2_21', ha='center', va='center', fontsize=8)

# Cell: NOT_16
cell_rect = patches.Rectangle((8.70101, 8.87673), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89101, 9.57673, 'NOT_16', ha='center', va='center', fontsize=8)

# Cell: START
cell_rect = patches.Rectangle((8.92778, 8.76046), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11778, 9.46046, 'START', ha='center', va='center', fontsize=8)

# Cell: NOT_49
cell_rect = patches.Rectangle((8.70961, 8.61016), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89961, 9.31016, 'NOT_49', ha='center', va='center', fontsize=8)

# Cell: P6
cell_rect = patches.Rectangle((8.75377, 8.60135), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94377, 9.30135, 'P6', ha='center', va='center', fontsize=8)

# Cell: NOT_9
cell_rect = patches.Rectangle((8.4739, 8.82822), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6639, 9.52822, 'NOT_9', ha='center', va='center', fontsize=8)

# Cell: DFF_7
cell_rect = patches.Rectangle((7.1083, 8.7653), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7233, 9.4653, 'DFF_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_19
cell_rect = patches.Rectangle((8.32519, 8.89111), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61019, 9.59111, 'NOR2_19', ha='center', va='center', fontsize=8)

# Cell: NOR3_0
cell_rect = patches.Rectangle((8.66059, 8.83265), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94559, 9.53265, 'NOR3_0', ha='center', va='center', fontsize=8)

# Cell: CNTVCO2
cell_rect = patches.Rectangle((8.92293, 8.91284), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11293, 9.61284, 'CNTVCO2', ha='center', va='center', fontsize=8)

# Cell: NOT_52
cell_rect = patches.Rectangle((8.80519, 8.56411), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99519, 9.26411, 'NOT_52', ha='center', va='center', fontsize=8)

# Cell: A0
cell_rect = patches.Rectangle((8.26242, 8.3642), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45242, 9.0642, 'A0', ha='center', va='center', fontsize=8)

# Cell: NOT_29
cell_rect = patches.Rectangle((8.40847, 8.93057), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59847, 9.63057, 'NOT_29', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((8.59656, 8.7), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78656, 9.4, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_9
cell_rect = patches.Rectangle((7.10764, 8.76221), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72264, 9.46221, 'DFF_9', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((8.82965, 8.875), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01965, 9.575, 'NOT_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_6
cell_rect = patches.Rectangle((8.48475, 8.59434), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76975, 9.29434, 'NAND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_15
cell_rect = patches.Rectangle((8.52839, 8.66468), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71839, 9.36468, 'NOT_15', ha='center', va='center', fontsize=8)

# Cell: AND2_1
cell_rect = patches.Rectangle((8.20177, 8.55204), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58177, 9.25204, 'AND2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_24
cell_rect = patches.Rectangle((8.79478, 8.45941), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07978, 9.15941, 'NOR2_24', ha='center', va='center', fontsize=8)

# Cell: NOR2_14
cell_rect = patches.Rectangle((8.81225, 8.79064), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09725, 9.49064, 'NOR2_14', ha='center', va='center', fontsize=8)

# Cell: B0
cell_rect = patches.Rectangle((8.2831, 8.91392), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.4731, 9.61392, 'B0', ha='center', va='center', fontsize=8)

# Cell: A2
cell_rect = patches.Rectangle((8.30105, 8.45218), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.49105, 9.15218, 'A2', ha='center', va='center', fontsize=8)

# Cell: NAND2_8
cell_rect = patches.Rectangle((8.74768, 8.86288), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03268, 9.56288, 'NAND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_46
cell_rect = patches.Rectangle((8.34365, 8.97617), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53365, 9.67617, 'NOT_46', ha='center', va='center', fontsize=8)

# Cell: B2
cell_rect = patches.Rectangle((8.4099, 8.94981), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5999, 9.64981, 'B2', ha='center', va='center', fontsize=8)

# Cell: READY
cell_rect = patches.Rectangle((8.72873, 8.86383), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91873, 9.56383, 'READY', ha='center', va='center', fontsize=8)

# Cell: NOT_7
cell_rect = patches.Rectangle((8.47089, 8.8317), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66089, 9.5317, 'NOT_7', ha='center', va='center', fontsize=8)

# Cell: AND2_25
cell_rect = patches.Rectangle((8.51839, 8.53708), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89839, 9.23708, 'AND2_25', ha='center', va='center', fontsize=8)

# Cell: NAND3_0
cell_rect = patches.Rectangle((8.66949, 8.87021), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95449, 9.57021, 'NAND3_0', ha='center', va='center', fontsize=8)

# Cell: P1
cell_rect = patches.Rectangle((8.43191, 8.8741), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62191, 9.5741, 'P1', ha='center', va='center', fontsize=8)

# Cell: AND2_9
cell_rect = patches.Rectangle((8.07487, 8.49845), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45487, 9.19845, 'AND2_9', ha='center', va='center', fontsize=8)

# Cell: P0
cell_rect = patches.Rectangle((8.50166, 8.64996), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69166, 9.34996, 'P0', ha='center', va='center', fontsize=8)

# Cell: DFF_6
cell_rect = patches.Rectangle((7.17509, 8.65406), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79009, 9.35406, 'DFF_6', ha='center', va='center', fontsize=8)

# Cell: AND2_17
cell_rect = patches.Rectangle((8.24068, 8.8647), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62068, 9.5647, 'AND2_17', ha='center', va='center', fontsize=8)

# Cell: NOT_25
cell_rect = patches.Rectangle((8.71559, 8.52373), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90559, 9.22373, 'NOT_25', ha='center', va='center', fontsize=8)

# Cell: NOT_24
cell_rect = patches.Rectangle((8.63996, 8.61518), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82996, 9.31518, 'NOT_24', ha='center', va='center', fontsize=8)

# Cell: NOT_21
cell_rect = patches.Rectangle((8.32591, 8.44135), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.51591, 9.14135, 'NOT_21', ha='center', va='center', fontsize=8)

# Cell: NOT_8
cell_rect = patches.Rectangle((8.48164, 8.83094), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67164, 9.53094, 'NOT_8', ha='center', va='center', fontsize=8)

# Cell: NOT_55
cell_rect = patches.Rectangle((8.87966, 8.4533), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06966, 9.1533, 'NOT_55', ha='center', va='center', fontsize=8)

# Cell: DFF_5
cell_rect = patches.Rectangle((7.19279, 8.66998), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80779, 9.36998, 'DFF_5', ha='center', va='center', fontsize=8)

# Cell: AND2_26
cell_rect = patches.Rectangle((8.45588, 8.6134), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83588, 9.3134, 'AND2_26', ha='center', va='center', fontsize=8)

# Cell: CNTVCON2
cell_rect = patches.Rectangle((8.85971, 8.89392), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04971, 9.59392, 'CNTVCON2', ha='center', va='center', fontsize=8)

# Cell: NOT_3
cell_rect = patches.Rectangle((8.77843, 8.55), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96843, 9.25, 'NOT_3', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((7.29151, 8.78566), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90651, 9.48566, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: AND2_7
cell_rect = patches.Rectangle((8.25393, 8.81687), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63393, 9.51687, 'AND2_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((8.52805, 8.81946), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81305, 9.51946, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: DFF_13
cell_rect = patches.Rectangle((7.09567, 8.6432), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71067, 9.3432, 'DFF_13', ha='center', va='center', fontsize=8)

# Cell: AND2_19
cell_rect = patches.Rectangle((8.14082, 9.01074), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52082, 9.71074, 'AND2_19', ha='center', va='center', fontsize=8)

# Cell: DFF_4
cell_rect = patches.Rectangle((7.22616, 8.66939), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84116, 9.36939, 'DFF_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((8.74157, 8.88467), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02657, 9.58467, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_13
cell_rect = patches.Rectangle((8.78472, 8.50309), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06972, 9.20309, 'NAND2_13', ha='center', va='center', fontsize=8)

# Cell: NOT_37
cell_rect = patches.Rectangle((8.79503, 8.53621), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98503, 9.23621, 'NOT_37', ha='center', va='center', fontsize=8)

# Cell: NOR2_7
cell_rect = patches.Rectangle((8.20656, 8.40836), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.49156, 9.10836, 'NOR2_7', ha='center', va='center', fontsize=8)

# Cell: AND2_22
cell_rect = patches.Rectangle((8.64849, 8.48887), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02849, 9.18887, 'AND2_22', ha='center', va='center', fontsize=8)

# Cell: P2
cell_rect = patches.Rectangle((8.44158, 8.87651), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63158, 9.57651, 'P2', ha='center', va='center', fontsize=8)

# Cell: B3
cell_rect = patches.Rectangle((8.33692, 8.96211), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52692, 9.66211, 'B3', ha='center', va='center', fontsize=8)

# Cell: AND2_3
cell_rect = patches.Rectangle((8.1978, 8.54791), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5778, 9.24791, 'AND2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((8.87054, 8.88609), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06054, 9.58609, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: A3
cell_rect = patches.Rectangle((8.34224, 8.48194), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53224, 9.18194, 'A3', ha='center', va='center', fontsize=8)

# Cell: P7
cell_rect = patches.Rectangle((8.85381, 8.48636), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04381, 9.18636, 'P7', ha='center', va='center', fontsize=8)

# Cell: DFF_12
cell_rect = patches.Rectangle((7.10721, 8.64037), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72221, 9.34037, 'DFF_12', ha='center', va='center', fontsize=8)

# Cell: NOT_48
cell_rect = patches.Rectangle((8.40456, 8.90419), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59456, 9.60419, 'NOT_48', ha='center', va='center', fontsize=8)

# Cell: DFF_10
cell_rect = patches.Rectangle((7.13752, 8.6836), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75252, 9.3836, 'DFF_10', ha='center', va='center', fontsize=8)

# Cell: NAND2_10
cell_rect = patches.Rectangle((8.45548, 8.63503), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74048, 9.33503, 'NAND2_10', ha='center', va='center', fontsize=8)

# Cell: DFF_3
cell_rect = patches.Rectangle((7.26272, 8.62526), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87772, 9.32526, 'DFF_3', ha='center', va='center', fontsize=8)

# Cell: NOT_31
cell_rect = patches.Rectangle((8.34171, 8.91585), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53171, 9.61585, 'NOT_31', ha='center', va='center', fontsize=8)

# Cell: NOT_23
cell_rect = patches.Rectangle((8.5834, 8.61159), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7734, 9.31159, 'NOT_23', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((7.26959, 8.77985), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88459, 9.47985, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: AND2_4
cell_rect = patches.Rectangle((8.19669, 8.9131), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57669, 9.6131, 'AND2_4', ha='center', va='center', fontsize=8)

# Cell: NOT_26
cell_rect = patches.Rectangle((8.8973, 8.8909), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.0873, 9.5909, 'NOT_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_27
cell_rect = patches.Rectangle((8.78651, 8.46665), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07151, 9.16665, 'NOR2_27', ha='center', va='center', fontsize=8)

# Cell: NOT_39
cell_rect = patches.Rectangle((8.86939, 8.825), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05939, 9.525, 'NOT_39', ha='center', va='center', fontsize=8)

# Cell: NOT_30
cell_rect = patches.Rectangle((8.37018, 8.95278), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56018, 9.65278, 'NOT_30', ha='center', va='center', fontsize=8)

# Cell: NOT_32
cell_rect = patches.Rectangle((8.63008, 8.68553), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82008, 9.38553, 'NOT_32', ha='center', va='center', fontsize=8)

# Cell: NOT_19
cell_rect = patches.Rectangle((8.34596, 8.48543), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53596, 9.18543, 'NOT_19', ha='center', va='center', fontsize=8)

# Cell: NOT_34
cell_rect = patches.Rectangle((8.60167, 8.60994), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79167, 9.30994, 'NOT_34', ha='center', va='center', fontsize=8)

# Cell: NOR2_28
cell_rect = patches.Rectangle((8.75202, 8.48904), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03702, 9.18904, 'NOR2_28', ha='center', va='center', fontsize=8)

# Cell: NOR2_3
cell_rect = patches.Rectangle((8.70323, 8.872), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98823, 9.572, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_35
cell_rect = patches.Rectangle((8.82599, 8.49864), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01599, 9.19864, 'NOT_35', ha='center', va='center', fontsize=8)

# Cell: AND2_8
cell_rect = patches.Rectangle((8.09921, 8.39443), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.47921, 9.09443, 'AND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_36
cell_rect = patches.Rectangle((8.8135, 8.50031), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.0035, 9.20031, 'NOT_36', ha='center', va='center', fontsize=8)

# Cell: AND2_15
cell_rect = patches.Rectangle((8.25726, 8.86629), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63726, 9.56629, 'AND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_41
cell_rect = patches.Rectangle((8.27356, 8.50901), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.46356, 9.20901, 'NOT_41', ha='center', va='center', fontsize=8)

# Cell: NOT_42
cell_rect = patches.Rectangle((8.46081, 8.41587), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65081, 9.11587, 'NOT_42', ha='center', va='center', fontsize=8)

# Cell: NOT_44
cell_rect = patches.Rectangle((8.49607, 8.71731), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68607, 9.41731, 'NOT_44', ha='center', va='center', fontsize=8)

# Cell: AND2_36
cell_rect = patches.Rectangle((8.68718, 8.47356), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.06718, 9.17356, 'AND2_36', ha='center', va='center', fontsize=8)

# Cell: NOT_45
cell_rect = patches.Rectangle((8.33782, 8.93067), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52782, 9.63067, 'NOT_45', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((8.21806, 8.50289), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59806, 9.20289, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: NOR2_11
cell_rect = patches.Rectangle((8.23527, 8.87642), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52027, 9.57642, 'NOR2_11', ha='center', va='center', fontsize=8)

# Cell: NOT_47
cell_rect = patches.Rectangle((8.35372, 8.91785), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54372, 9.61785, 'NOT_47', ha='center', va='center', fontsize=8)

# Cell: NOT_50
cell_rect = patches.Rectangle((8.72744, 8.55372), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91744, 9.25372, 'NOT_50', ha='center', va='center', fontsize=8)

# Cell: NOT_33
cell_rect = patches.Rectangle((8.43459, 8.8723), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62459, 9.5723, 'NOT_33', ha='center', va='center', fontsize=8)

# Cell: NOT_51
cell_rect = patches.Rectangle((8.83201, 8.53176), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02201, 9.23176, 'NOT_51', ha='center', va='center', fontsize=8)

# Cell: NOT_53
cell_rect = patches.Rectangle((8.81308, 8.48569), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00308, 9.18569, 'NOT_53', ha='center', va='center', fontsize=8)

# Cell: AND2_33
cell_rect = patches.Rectangle((8.56812, 8.60672), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94812, 9.30672, 'AND2_33', ha='center', va='center', fontsize=8)

# Cell: AND2_2
cell_rect = patches.Rectangle((8.26717, 8.51676), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64717, 9.21676, 'AND2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_22
cell_rect = patches.Rectangle((8.68319, 8.58328), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96819, 9.28328, 'NOR2_22', ha='center', va='center', fontsize=8)

# Cell: NAND2_15
cell_rect = patches.Rectangle((8.81297, 8.49164), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09797, 9.19164, 'NAND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_56
cell_rect = patches.Rectangle((8.84653, 8.50816), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03653, 9.20816, 'NOT_56', ha='center', va='center', fontsize=8)

# Cell: NOT_57
cell_rect = patches.Rectangle((8.90771, 8.45538), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09771, 9.15538, 'NOT_57', ha='center', va='center', fontsize=8)

# Cell: AND2_5
cell_rect = patches.Rectangle((8.20295, 8.9303), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58295, 9.6303, 'AND2_5', ha='center', va='center', fontsize=8)

# Cell: AND2_6
cell_rect = patches.Rectangle((8.20598, 8.90011), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58598, 9.60011, 'AND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_6
cell_rect = patches.Rectangle((8.60866, 8.61177), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79866, 9.31177, 'NOT_6', ha='center', va='center', fontsize=8)

# Cell: NOT_17
cell_rect = patches.Rectangle((8.56012, 8.90108), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75012, 9.60108, 'NOT_17', ha='center', va='center', fontsize=8)

# Cell: AND2_12
cell_rect = patches.Rectangle((8.46131, 8.61176), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84131, 9.31176, 'AND2_12', ha='center', va='center', fontsize=8)

# Cell: NOT_22
cell_rect = patches.Rectangle((8.52732, 8.59903), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71732, 9.29903, 'NOT_22', ha='center', va='center', fontsize=8)

# Cell: NOT_38
cell_rect = patches.Rectangle((8.77231, 8.52711), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96231, 9.22711, 'NOT_38', ha='center', va='center', fontsize=8)

# Cell: AND2_13
cell_rect = patches.Rectangle((8.56389, 8.60314), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94389, 9.30314, 'AND2_13', ha='center', va='center', fontsize=8)

# Cell: B1
cell_rect = patches.Rectangle((8.33436, 8.9871), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52436, 9.6871, 'B1', ha='center', va='center', fontsize=8)

# Cell: AND2_16
cell_rect = patches.Rectangle((8.2551, 8.87466), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6351, 9.57466, 'AND2_16', ha='center', va='center', fontsize=8)

# Cell: AND2_34
cell_rect = patches.Rectangle((8.59684, 8.58844), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97684, 9.28844, 'AND2_34', ha='center', va='center', fontsize=8)

# Cell: NOT_54
cell_rect = patches.Rectangle((8.89519, 8.45455), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.08519, 9.15455, 'NOT_54', ha='center', va='center', fontsize=8)

# Cell: AND2_20
cell_rect = patches.Rectangle((8.14987, 8.92922), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52987, 9.62922, 'AND2_20', ha='center', va='center', fontsize=8)

# Cell: NOT_40
cell_rect = patches.Rectangle((8.30227, 8.39696), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.49227, 9.09696, 'NOT_40', ha='center', va='center', fontsize=8)

# Cell: AND2_23
cell_rect = patches.Rectangle((8.60936, 8.54491), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98936, 9.24491, 'AND2_23', ha='center', va='center', fontsize=8)

# Cell: NOT_58
cell_rect = patches.Rectangle((8.90046, 8.48906), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.09046, 9.18906, 'NOT_58', ha='center', va='center', fontsize=8)

# Cell: AND2_24
cell_rect = patches.Rectangle((8.55305, 8.5551), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93305, 9.2551, 'AND2_24', ha='center', va='center', fontsize=8)

# Cell: AND3_1
cell_rect = patches.Rectangle((8.56406, 8.60388), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94406, 9.30388, 'AND3_1', ha='center', va='center', fontsize=8)

# Cell: AND3_0
cell_rect = patches.Rectangle((8.46282, 8.60597), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84282, 9.30597, 'AND3_0', ha='center', va='center', fontsize=8)

# Cell: AND2_27
cell_rect = patches.Rectangle((8.27831, 8.80327), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65831, 9.50327, 'AND2_27', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((12.5396, 2.29842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(12.7296, 2.99842, 'VDD', ha='center', va='center', fontsize=8)

# Cell: AND2_28
cell_rect = patches.Rectangle((8.19274, 8.90161), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57274, 9.60161, 'AND2_28', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 27.1388)
ax.set_ylim(-10, 28)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251201_104600_360/quadratic_iter_30.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251201_104600_360/quadratic_iter_30.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251201_104600_360/quadratic_iter_30.png")
