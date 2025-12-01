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
cell_rect = patches.Rectangle((8.68331, 8.55403), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96831, 9.25403, 'NOR2_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_25
cell_rect = patches.Rectangle((8.74716, 8.48948), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03216, 9.18948, 'NOR2_25', ha='center', va='center', fontsize=8)

# Cell: NOR2_23
cell_rect = patches.Rectangle((8.65075, 8.54679), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93575, 9.24679, 'NOR2_23', ha='center', va='center', fontsize=8)

# Cell: NOR2_21
cell_rect = patches.Rectangle((8.66322, 8.58122), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94822, 9.28122, 'NOR2_21', ha='center', va='center', fontsize=8)

# Cell: NOR2_20
cell_rect = patches.Rectangle((8.58806, 8.58907), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87306, 9.28907, 'NOR2_20', ha='center', va='center', fontsize=8)

# Cell: NOR2_18
cell_rect = patches.Rectangle((8.3422, 8.87663), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6272, 9.57663, 'NOR2_18', ha='center', va='center', fontsize=8)

# Cell: NOR2_17
cell_rect = patches.Rectangle((8.3495, 8.86864), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6345, 9.56864, 'NOR2_17', ha='center', va='center', fontsize=8)

# Cell: NOR2_16
cell_rect = patches.Rectangle((8.35821, 8.83738), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64321, 9.53738, 'NOR2_16', ha='center', va='center', fontsize=8)

# Cell: NOR2_12
cell_rect = patches.Rectangle((8.76917, 8.78857), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05417, 9.48857, 'NOR2_12', ha='center', va='center', fontsize=8)

# Cell: NOR2_5
cell_rect = patches.Rectangle((8.27503, 8.54155), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56003, 9.24155, 'NOR2_5', ha='center', va='center', fontsize=8)

# Cell: NOR2_4
cell_rect = patches.Rectangle((8.29026, 8.48627), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57526, 9.18627, 'NOR2_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((8.74367, 8.86471), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02867, 9.56471, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_14
cell_rect = patches.Rectangle((8.74745, 8.55398), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03245, 9.25398, 'NAND2_14', ha='center', va='center', fontsize=8)

# Cell: NOR2_9
cell_rect = patches.Rectangle((8.29773, 8.92743), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58273, 9.62743, 'NOR2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_12
cell_rect = patches.Rectangle((8.72828, 8.82083), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01328, 9.52083, 'NAND2_12', ha='center', va='center', fontsize=8)

# Cell: NAND2_11
cell_rect = patches.Rectangle((8.75231, 8.82691), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03731, 9.52691, 'NAND2_11', ha='center', va='center', fontsize=8)

# Cell: NAND2_9
cell_rect = patches.Rectangle((8.67481, 8.82109), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95981, 9.52109, 'NAND2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_7
cell_rect = patches.Rectangle((8.72782, 8.8382), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01282, 9.5382, 'NAND2_7', ha='center', va='center', fontsize=8)

# Cell: NAND2_5
cell_rect = patches.Rectangle((8.70039, 8.84819), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98539, 9.54819, 'NAND2_5', ha='center', va='center', fontsize=8)

# Cell: NAND2_4
cell_rect = patches.Rectangle((8.71146, 8.84869), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99646, 9.54869, 'NAND2_4', ha='center', va='center', fontsize=8)

# Cell: NAND2_3
cell_rect = patches.Rectangle((8.47992, 8.59901), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76492, 9.29901, 'NAND2_3', ha='center', va='center', fontsize=8)

# Cell: NAND2_2
cell_rect = patches.Rectangle((8.45818, 8.62641), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74318, 9.32641, 'NAND2_2', ha='center', va='center', fontsize=8)

# Cell: NAND2_1
cell_rect = patches.Rectangle((8.44758, 8.62391), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73258, 9.32391, 'NAND2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((8.42546, 8.62794), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71046, 9.32794, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR3_2
cell_rect = patches.Rectangle((8.63055, 8.50474), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01055, 9.20474, 'OR3_2', ha='center', va='center', fontsize=8)

# Cell: OR3_1
cell_rect = patches.Rectangle((8.54906, 8.59124), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92906, 9.29124, 'OR3_1', ha='center', va='center', fontsize=8)

# Cell: OR3_0
cell_rect = patches.Rectangle((8.46605, 8.60432), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84605, 9.30432, 'OR3_0', ha='center', va='center', fontsize=8)

# Cell: OR2_5
cell_rect = patches.Rectangle((8.60438, 8.83088), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98438, 9.53088, 'OR2_5', ha='center', va='center', fontsize=8)

# Cell: OR2_4
cell_rect = patches.Rectangle((8.63282, 8.8382), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01282, 9.5382, 'OR2_4', ha='center', va='center', fontsize=8)

# Cell: OR2_3
cell_rect = patches.Rectangle((8.62593, 8.50695), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00593, 9.20695, 'OR2_3', ha='center', va='center', fontsize=8)

# Cell: OR2_2
cell_rect = patches.Rectangle((8.54457, 8.59429), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92457, 9.29429, 'OR2_2', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((8.46357, 8.60517), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84357, 9.30517, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_15
cell_rect = patches.Rectangle((8.57131, 8.6036), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85631, 9.3036, 'NOR2_15', ha='center', va='center', fontsize=8)

# Cell: NOR2_10
cell_rect = patches.Rectangle((8.30288, 8.8965), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58788, 9.5965, 'NOR2_10', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((8.40259, 8.62271), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78259, 9.32271, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: AND2_40
cell_rect = patches.Rectangle((8.62994, 8.51265), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00994, 9.21265, 'AND2_40', ha='center', va='center', fontsize=8)

# Cell: AND2_39
cell_rect = patches.Rectangle((8.64473, 8.50386), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02473, 9.20386, 'AND2_39', ha='center', va='center', fontsize=8)

# Cell: AND2_38
cell_rect = patches.Rectangle((8.59181, 8.55787), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97181, 9.25787, 'AND2_38', ha='center', va='center', fontsize=8)

# Cell: AND2_37
cell_rect = patches.Rectangle((8.65655, 8.49033), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03655, 9.19033, 'AND2_37', ha='center', va='center', fontsize=8)

# Cell: AND3_2
cell_rect = patches.Rectangle((8.63205, 8.50583), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01205, 9.20583, 'AND3_2', ha='center', va='center', fontsize=8)

# Cell: AND2_35
cell_rect = patches.Rectangle((8.55372, 8.55384), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93372, 9.25384, 'AND2_35', ha='center', va='center', fontsize=8)

# Cell: AND2_32
cell_rect = patches.Rectangle((8.48169, 8.59672), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86169, 9.29672, 'AND2_32', ha='center', va='center', fontsize=8)

# Cell: AND2_31
cell_rect = patches.Rectangle((8.23995, 8.87585), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61995, 9.57585, 'AND2_31', ha='center', va='center', fontsize=8)

# Cell: NOR2_13
cell_rect = patches.Rectangle((8.75783, 8.7859), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04283, 9.4859, 'NOR2_13', ha='center', va='center', fontsize=8)

# Cell: AND2_30
cell_rect = patches.Rectangle((8.22396, 8.88269), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.60396, 9.58269, 'AND2_30', ha='center', va='center', fontsize=8)

# Cell: NOT_20
cell_rect = patches.Rectangle((8.43892, 8.5127), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62892, 9.2127, 'NOT_20', ha='center', va='center', fontsize=8)

# Cell: NOT_13
cell_rect = patches.Rectangle((8.86402, 8.65953), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05402, 9.35953, 'NOT_13', ha='center', va='center', fontsize=8)

# Cell: AND2_14
cell_rect = patches.Rectangle((8.62893, 8.50417), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00893, 9.20417, 'AND2_14', ha='center', va='center', fontsize=8)

# Cell: NOT_12
cell_rect = patches.Rectangle((8.75436, 8.8344), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94436, 9.5344, 'NOT_12', ha='center', va='center', fontsize=8)

# Cell: NOT_11
cell_rect = patches.Rectangle((8.76086, 8.83353), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95086, 9.53353, 'NOT_11', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((7.26456, 8.76456), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87956, 9.46456, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: NOT_27
cell_rect = patches.Rectangle((8.73552, 8.83009), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92552, 9.53009, 'NOT_27', ha='center', va='center', fontsize=8)

# Cell: NOR2_8
cell_rect = patches.Rectangle((8.29479, 8.90519), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57979, 9.60519, 'NOR2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_10
cell_rect = patches.Rectangle((8.54895, 8.66829), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73895, 9.36829, 'NOT_10', ha='center', va='center', fontsize=8)

# Cell: P5
cell_rect = patches.Rectangle((8.66606, 8.6033), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85606, 9.3033, 'P5', ha='center', va='center', fontsize=8)

# Cell: AND2_29
cell_rect = patches.Rectangle((8.21866, 8.89864), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59866, 9.59864, 'AND2_29', ha='center', va='center', fontsize=8)

# Cell: NOT_4
cell_rect = patches.Rectangle((8.69816, 8.62558), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88816, 9.32558, 'NOT_4', ha='center', va='center', fontsize=8)

# Cell: DFF_14
cell_rect = patches.Rectangle((7.11348, 8.64729), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72848, 9.34729, 'DFF_14', ha='center', va='center', fontsize=8)

# Cell: AND2_10
cell_rect = patches.Rectangle((8.14316, 8.53453), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52316, 9.23453, 'AND2_10', ha='center', va='center', fontsize=8)

# Cell: NOT_14
cell_rect = patches.Rectangle((8.79926, 8.84875), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98926, 9.54875, 'NOT_14', ha='center', va='center', fontsize=8)

# Cell: NOT_2
cell_rect = patches.Rectangle((8.77346, 8.83651), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96346, 9.53651, 'NOT_2', ha='center', va='center', fontsize=8)

# Cell: DFF_11
cell_rect = patches.Rectangle((7.132, 8.63484), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.747, 9.33484, 'DFF_11', ha='center', va='center', fontsize=8)

# Cell: NOR2_6
cell_rect = patches.Rectangle((8.36039, 8.49781), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64539, 9.19781, 'NOR2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_18
cell_rect = patches.Rectangle((8.39032, 8.50775), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58032, 9.20775, 'NOT_18', ha='center', va='center', fontsize=8)

# Cell: NOT_28
cell_rect = patches.Rectangle((8.40213, 8.90407), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59213, 9.60407, 'NOT_28', ha='center', va='center', fontsize=8)

# Cell: NOT_5
cell_rect = patches.Rectangle((8.64561, 8.63213), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83561, 9.33213, 'NOT_5', ha='center', va='center', fontsize=8)

# Cell: P4
cell_rect = patches.Rectangle((8.6266, 8.60679), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8166, 9.30679, 'P4', ha='center', va='center', fontsize=8)

# Cell: AND2_11
cell_rect = patches.Rectangle((8.16444, 8.46844), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54444, 9.16844, 'AND2_11', ha='center', va='center', fontsize=8)

# Cell: DFF_8
cell_rect = patches.Rectangle((7.12604, 8.75276), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74104, 9.45276, 'DFF_8', ha='center', va='center', fontsize=8)

# Cell: AND2_18
cell_rect = patches.Rectangle((8.20123, 8.91604), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58123, 9.61604, 'AND2_18', ha='center', va='center', fontsize=8)

# Cell: P3
cell_rect = patches.Rectangle((8.46027, 8.8487), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65027, 9.5487, 'P3', ha='center', va='center', fontsize=8)

# Cell: NAND2_16
cell_rect = patches.Rectangle((8.76553, 8.54745), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05053, 9.24745, 'NAND2_16', ha='center', va='center', fontsize=8)

# Cell: A1
cell_rect = patches.Rectangle((8.44141, 8.46939), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63141, 9.16939, 'A1', ha='center', va='center', fontsize=8)

# Cell: NOT_43
cell_rect = patches.Rectangle((8.35926, 8.47214), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54926, 9.17214, 'NOT_43', ha='center', va='center', fontsize=8)

# Cell: AND2_21
cell_rect = patches.Rectangle((8.18352, 8.87891), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56352, 9.57891, 'AND2_21', ha='center', va='center', fontsize=8)

# Cell: NOT_16
cell_rect = patches.Rectangle((8.67894, 8.84418), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86894, 9.54418, 'NOT_16', ha='center', va='center', fontsize=8)

# Cell: START
cell_rect = patches.Rectangle((8.86847, 8.7543), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05847, 9.4543, 'START', ha='center', va='center', fontsize=8)

# Cell: NOT_49
cell_rect = patches.Rectangle((8.70641, 8.59724), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89641, 9.29724, 'NOT_49', ha='center', va='center', fontsize=8)

# Cell: P6
cell_rect = patches.Rectangle((8.73968, 8.59319), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92968, 9.29319, 'P6', ha='center', va='center', fontsize=8)

# Cell: NOT_9
cell_rect = patches.Rectangle((8.49633, 8.80602), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68633, 9.50602, 'NOT_9', ha='center', va='center', fontsize=8)

# Cell: DFF_7
cell_rect = patches.Rectangle((7.12333, 8.75287), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73833, 9.45287, 'DFF_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_19
cell_rect = patches.Rectangle((8.33266, 8.86262), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61766, 9.56262, 'NOR2_19', ha='center', va='center', fontsize=8)

# Cell: NOR3_0
cell_rect = patches.Rectangle((8.62879, 8.80667), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91379, 9.50667, 'NOR3_0', ha='center', va='center', fontsize=8)

# Cell: CNTVCO2
cell_rect = patches.Rectangle((8.84966, 8.87328), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03966, 9.57328, 'CNTVCO2', ha='center', va='center', fontsize=8)

# Cell: NOT_52
cell_rect = patches.Rectangle((8.77204, 8.56454), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96204, 9.26454, 'NOT_52', ha='center', va='center', fontsize=8)

# Cell: A0
cell_rect = patches.Rectangle((8.34096, 8.5218), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53096, 9.2218, 'A0', ha='center', va='center', fontsize=8)

# Cell: NOT_29
cell_rect = patches.Rectangle((8.40402, 8.91834), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59402, 9.61834, 'NOT_29', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((8.60119, 8.69616), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79119, 9.39616, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_9
cell_rect = patches.Rectangle((7.12419, 8.75197), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73919, 9.45197, 'DFF_9', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((8.78208, 8.83946), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97208, 9.53946, 'NOT_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_6
cell_rect = patches.Rectangle((8.50347, 8.61824), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78847, 9.31824, 'NAND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_15
cell_rect = patches.Rectangle((8.54398, 8.67331), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73398, 9.37331, 'NOT_15', ha='center', va='center', fontsize=8)

# Cell: AND2_1
cell_rect = patches.Rectangle((8.26349, 8.56759), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64349, 9.26759, 'AND2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_24
cell_rect = patches.Rectangle((8.74511, 8.49243), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03011, 9.19243, 'NOR2_24', ha='center', va='center', fontsize=8)

# Cell: NOR2_14
cell_rect = patches.Rectangle((8.75581, 8.77773), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04081, 9.47773, 'NOR2_14', ha='center', va='center', fontsize=8)

# Cell: B0
cell_rect = patches.Rectangle((8.41962, 8.89261), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.60962, 9.59261, 'B0', ha='center', va='center', fontsize=8)

# Cell: A2
cell_rect = patches.Rectangle((8.33397, 8.52323), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52397, 9.22323, 'A2', ha='center', va='center', fontsize=8)

# Cell: NAND2_8
cell_rect = patches.Rectangle((8.69938, 8.83088), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98438, 9.53088, 'NAND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_46
cell_rect = patches.Rectangle((8.42002, 8.89883), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61002, 9.59883, 'NOT_46', ha='center', va='center', fontsize=8)

# Cell: B2
cell_rect = patches.Rectangle((8.3715, 8.95561), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5615, 9.65561, 'B2', ha='center', va='center', fontsize=8)

# Cell: READY
cell_rect = patches.Rectangle((8.7022, 8.83329), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8922, 9.53329, 'READY', ha='center', va='center', fontsize=8)

# Cell: NOT_7
cell_rect = patches.Rectangle((8.49676, 8.80862), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68676, 9.50862, 'NOT_7', ha='center', va='center', fontsize=8)

# Cell: AND2_25
cell_rect = patches.Rectangle((8.51835, 8.57426), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89835, 9.27426, 'AND2_25', ha='center', va='center', fontsize=8)

# Cell: NAND3_0
cell_rect = patches.Rectangle((8.63477, 8.83725), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91977, 9.53725, 'NAND3_0', ha='center', va='center', fontsize=8)

# Cell: P1
cell_rect = patches.Rectangle((8.45952, 8.84531), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64952, 9.54531, 'P1', ha='center', va='center', fontsize=8)

# Cell: AND2_9
cell_rect = patches.Rectangle((8.18581, 8.50993), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56581, 9.20993, 'AND2_9', ha='center', va='center', fontsize=8)

# Cell: P0
cell_rect = patches.Rectangle((8.52914, 8.65618), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71914, 9.35618, 'P0', ha='center', va='center', fontsize=8)

# Cell: DFF_6
cell_rect = patches.Rectangle((7.18391, 8.66345), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79891, 9.36345, 'DFF_6', ha='center', va='center', fontsize=8)

# Cell: AND2_17
cell_rect = patches.Rectangle((8.28428, 8.841), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66428, 9.541, 'AND2_17', ha='center', va='center', fontsize=8)

# Cell: NOT_25
cell_rect = patches.Rectangle((8.70076, 8.55012), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89076, 9.25012, 'NOT_25', ha='center', va='center', fontsize=8)

# Cell: NOT_24
cell_rect = patches.Rectangle((8.64552, 8.60838), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83552, 9.30838, 'NOT_24', ha='center', va='center', fontsize=8)

# Cell: NOT_21
cell_rect = patches.Rectangle((8.3876, 8.54169), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5776, 9.24169, 'NOT_21', ha='center', va='center', fontsize=8)

# Cell: NOT_8
cell_rect = patches.Rectangle((8.49885, 8.81223), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68885, 9.51223, 'NOT_8', ha='center', va='center', fontsize=8)

# Cell: NOT_55
cell_rect = patches.Rectangle((8.84733, 8.49777), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03733, 9.19777, 'NOT_55', ha='center', va='center', fontsize=8)

# Cell: DFF_5
cell_rect = patches.Rectangle((7.19751, 8.66538), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81251, 9.36538, 'DFF_5', ha='center', va='center', fontsize=8)

# Cell: AND2_26
cell_rect = patches.Rectangle((8.46275, 8.60587), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84275, 9.30587, 'AND2_26', ha='center', va='center', fontsize=8)

# Cell: CNTVCON2
cell_rect = patches.Rectangle((8.80482, 8.85513), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99482, 9.55513, 'CNTVCON2', ha='center', va='center', fontsize=8)

# Cell: NOT_3
cell_rect = patches.Rectangle((8.75049, 8.56785), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94049, 9.26785, 'NOT_3', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((7.2689, 8.76689), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8839, 9.46689, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: AND2_7
cell_rect = patches.Rectangle((8.26136, 8.7942), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64136, 9.4942, 'AND2_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((8.52358, 8.79914), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80858, 9.49914, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: DFF_13
cell_rect = patches.Rectangle((7.12762, 8.64377), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74262, 9.34377, 'DFF_13', ha='center', va='center', fontsize=8)

# Cell: AND2_19
cell_rect = patches.Rectangle((8.22813, 8.91258), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.60813, 9.61258, 'AND2_19', ha='center', va='center', fontsize=8)

# Cell: DFF_4
cell_rect = patches.Rectangle((7.22358, 8.66245), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83858, 9.36245, 'DFF_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((8.6924, 8.84771), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9774, 9.54771, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_13
cell_rect = patches.Rectangle((8.73609, 8.5495), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02109, 9.2495, 'NAND2_13', ha='center', va='center', fontsize=8)

# Cell: NOT_37
cell_rect = patches.Rectangle((8.76502, 8.55749), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95502, 9.25749, 'NOT_37', ha='center', va='center', fontsize=8)

# Cell: NOR2_7
cell_rect = patches.Rectangle((8.27699, 8.53974), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56199, 9.23974, 'NOR2_7', ha='center', va='center', fontsize=8)

# Cell: AND2_22
cell_rect = patches.Rectangle((8.61245, 8.52734), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99245, 9.22734, 'AND2_22', ha='center', va='center', fontsize=8)

# Cell: P2
cell_rect = patches.Rectangle((8.46248, 8.85373), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65248, 9.55373, 'P2', ha='center', va='center', fontsize=8)

# Cell: B3
cell_rect = patches.Rectangle((8.36539, 8.92757), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.55539, 9.62757, 'B3', ha='center', va='center', fontsize=8)

# Cell: AND2_3
cell_rect = patches.Rectangle((8.24712, 8.55122), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62712, 9.25122, 'AND2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((8.81106, 8.8507), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00106, 9.5507, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: A3
cell_rect = patches.Rectangle((8.34961, 8.4587), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53961, 9.1587, 'A3', ha='center', va='center', fontsize=8)

# Cell: P7
cell_rect = patches.Rectangle((8.81221, 8.51406), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00221, 9.21406, 'P7', ha='center', va='center', fontsize=8)

# Cell: DFF_12
cell_rect = patches.Rectangle((7.12355, 8.64832), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.73855, 9.34832, 'DFF_12', ha='center', va='center', fontsize=8)

# Cell: NOT_48
cell_rect = patches.Rectangle((8.3898, 8.86804), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5798, 9.56804, 'NOT_48', ha='center', va='center', fontsize=8)

# Cell: DFF_10
cell_rect = patches.Rectangle((7.14852, 8.68225), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76352, 9.38225, 'DFF_10', ha='center', va='center', fontsize=8)

# Cell: NAND2_10
cell_rect = patches.Rectangle((8.4769, 8.65174), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7619, 9.35174, 'NAND2_10', ha='center', va='center', fontsize=8)

# Cell: DFF_3
cell_rect = patches.Rectangle((7.25073, 8.63257), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86573, 9.33257, 'DFF_3', ha='center', va='center', fontsize=8)

# Cell: NOT_31
cell_rect = patches.Rectangle((8.43021, 8.88778), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62021, 9.58778, 'NOT_31', ha='center', va='center', fontsize=8)

# Cell: NOT_23
cell_rect = patches.Rectangle((8.59132, 8.61547), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78132, 9.31547, 'NOT_23', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((7.25223, 8.76207), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86723, 9.46207, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: AND2_4
cell_rect = patches.Rectangle((8.24432, 8.87695), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62432, 9.57695, 'AND2_4', ha='center', va='center', fontsize=8)

# Cell: NOT_26
cell_rect = patches.Rectangle((8.83319, 8.85427), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02319, 9.55427, 'NOT_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_27
cell_rect = patches.Rectangle((8.7456, 8.50409), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.0306, 9.20409, 'NOR2_27', ha='center', va='center', fontsize=8)

# Cell: NOT_39
cell_rect = patches.Rectangle((8.81944, 8.80317), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00944, 9.50317, 'NOT_39', ha='center', va='center', fontsize=8)

# Cell: NOT_30
cell_rect = patches.Rectangle((8.40858, 8.89536), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59858, 9.59536, 'NOT_30', ha='center', va='center', fontsize=8)

# Cell: NOT_32
cell_rect = patches.Rectangle((8.63114, 8.68989), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82114, 9.38989, 'NOT_32', ha='center', va='center', fontsize=8)

# Cell: NOT_19
cell_rect = patches.Rectangle((8.38327, 8.54073), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57327, 9.24073, 'NOT_19', ha='center', va='center', fontsize=8)

# Cell: NOT_34
cell_rect = patches.Rectangle((8.61827, 8.61803), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80827, 9.31803, 'NOT_34', ha='center', va='center', fontsize=8)

# Cell: NOR2_28
cell_rect = patches.Rectangle((8.73001, 8.53146), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01501, 9.23146, 'NOR2_28', ha='center', va='center', fontsize=8)

# Cell: NOR2_3
cell_rect = patches.Rectangle((8.66272, 8.83849), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94772, 9.53849, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_35
cell_rect = patches.Rectangle((8.79493, 8.52936), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98493, 9.22936, 'NOT_35', ha='center', va='center', fontsize=8)

# Cell: AND2_8
cell_rect = patches.Rectangle((8.20654, 8.52318), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58654, 9.22318, 'AND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_36
cell_rect = patches.Rectangle((8.79289, 8.53845), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98289, 9.23845, 'NOT_36', ha='center', va='center', fontsize=8)

# Cell: AND2_15
cell_rect = patches.Rectangle((8.27663, 8.84802), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65663, 9.54802, 'AND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_41
cell_rect = patches.Rectangle((8.38412, 8.51639), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57412, 9.21639, 'NOT_41', ha='center', va='center', fontsize=8)

# Cell: NOT_42
cell_rect = patches.Rectangle((8.33968, 8.54139), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.52968, 9.24139, 'NOT_42', ha='center', va='center', fontsize=8)

# Cell: NOT_44
cell_rect = patches.Rectangle((8.53056, 8.72229), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72056, 9.42229, 'NOT_44', ha='center', va='center', fontsize=8)

# Cell: AND2_36
cell_rect = patches.Rectangle((8.64148, 8.4999), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02148, 9.1999, 'AND2_36', ha='center', va='center', fontsize=8)

# Cell: NOT_45
cell_rect = patches.Rectangle((8.40482, 8.89419), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59482, 9.59419, 'NOT_45', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((8.27653, 8.56741), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65653, 9.26741, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: NOR2_11
cell_rect = patches.Rectangle((8.34025, 8.85698), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62525, 9.55698, 'NOR2_11', ha='center', va='center', fontsize=8)

# Cell: NOT_47
cell_rect = patches.Rectangle((8.39941, 8.91188), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.58941, 9.61188, 'NOT_47', ha='center', va='center', fontsize=8)

# Cell: NOT_50
cell_rect = patches.Rectangle((8.70558, 8.56377), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89558, 9.26377, 'NOT_50', ha='center', va='center', fontsize=8)

# Cell: NOT_33
cell_rect = patches.Rectangle((8.46622, 8.84648), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65622, 9.54648, 'NOT_33', ha='center', va='center', fontsize=8)

# Cell: NOT_51
cell_rect = patches.Rectangle((8.79876, 8.53948), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98876, 9.23948, 'NOT_51', ha='center', va='center', fontsize=8)

# Cell: NOT_53
cell_rect = patches.Rectangle((8.80238, 8.54409), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99238, 9.24409, 'NOT_53', ha='center', va='center', fontsize=8)

# Cell: AND2_33
cell_rect = patches.Rectangle((8.55262, 8.59083), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93262, 9.29083, 'AND2_33', ha='center', va='center', fontsize=8)

# Cell: AND2_2
cell_rect = patches.Rectangle((8.24182, 8.57771), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62182, 9.27771, 'AND2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_22
cell_rect = patches.Rectangle((8.66946, 8.57941), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95446, 9.27941, 'NOR2_22', ha='center', va='center', fontsize=8)

# Cell: NAND2_15
cell_rect = patches.Rectangle((8.76903, 8.53658), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05403, 9.23658, 'NAND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_56
cell_rect = patches.Rectangle((8.82163, 8.54997), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.01163, 9.24997, 'NOT_56', ha='center', va='center', fontsize=8)

# Cell: NOT_57
cell_rect = patches.Rectangle((8.85951, 8.51624), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04951, 9.21624, 'NOT_57', ha='center', va='center', fontsize=8)

# Cell: AND2_5
cell_rect = patches.Rectangle((8.25361, 8.87673), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63361, 9.57673, 'AND2_5', ha='center', va='center', fontsize=8)

# Cell: AND2_6
cell_rect = patches.Rectangle((8.2424, 8.88129), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6224, 9.58129, 'AND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_6
cell_rect = patches.Rectangle((8.61801, 8.63103), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80801, 9.33103, 'NOT_6', ha='center', va='center', fontsize=8)

# Cell: NOT_17
cell_rect = patches.Rectangle((8.56422, 8.86585), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75422, 9.56585, 'NOT_17', ha='center', va='center', fontsize=8)

# Cell: AND2_12
cell_rect = patches.Rectangle((8.46719, 8.60445), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84719, 9.30445, 'AND2_12', ha='center', va='center', fontsize=8)

# Cell: NOT_22
cell_rect = patches.Rectangle((8.55305, 8.61778), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74305, 9.31778, 'NOT_22', ha='center', va='center', fontsize=8)

# Cell: NOT_38
cell_rect = patches.Rectangle((8.7438, 8.55352), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9338, 9.25352, 'NOT_38', ha='center', va='center', fontsize=8)

# Cell: AND2_13
cell_rect = patches.Rectangle((8.54897, 8.59115), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92897, 9.29115, 'AND2_13', ha='center', va='center', fontsize=8)

# Cell: B1
cell_rect = patches.Rectangle((8.37805, 8.91126), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56805, 9.61126, 'B1', ha='center', va='center', fontsize=8)

# Cell: AND2_16
cell_rect = patches.Rectangle((8.2804, 8.84437), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6604, 9.54437, 'AND2_16', ha='center', va='center', fontsize=8)

# Cell: AND2_34
cell_rect = patches.Rectangle((8.57274, 8.57665), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95274, 9.27665, 'AND2_34', ha='center', va='center', fontsize=8)

# Cell: NOT_54
cell_rect = patches.Rectangle((8.84696, 8.48748), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03696, 9.18748, 'NOT_54', ha='center', va='center', fontsize=8)

# Cell: AND2_20
cell_rect = patches.Rectangle((8.19839, 8.92912), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.57839, 9.62912, 'AND2_20', ha='center', va='center', fontsize=8)

# Cell: NOT_40
cell_rect = patches.Rectangle((8.4079, 8.52733), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5979, 9.22733, 'NOT_40', ha='center', va='center', fontsize=8)

# Cell: AND2_23
cell_rect = patches.Rectangle((8.57851, 8.5612), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95851, 9.2612, 'AND2_23', ha='center', va='center', fontsize=8)

# Cell: NOT_58
cell_rect = patches.Rectangle((8.84164, 8.5287), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03164, 9.2287, 'NOT_58', ha='center', va='center', fontsize=8)

# Cell: AND2_24
cell_rect = patches.Rectangle((8.54313, 8.57435), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92313, 9.27435, 'AND2_24', ha='center', va='center', fontsize=8)

# Cell: AND3_1
cell_rect = patches.Rectangle((8.54776, 8.59087), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92776, 9.29087, 'AND3_1', ha='center', va='center', fontsize=8)

# Cell: AND3_0
cell_rect = patches.Rectangle((8.4657, 8.60103), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8457, 9.30103, 'AND3_0', ha='center', va='center', fontsize=8)

# Cell: AND2_27
cell_rect = patches.Rectangle((8.30263, 8.79284), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68263, 9.49284, 'AND2_27', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((12.5396, 2.29842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(12.7296, 2.99842, 'VDD', ha='center', va='center', fontsize=8)

# Cell: AND2_28
cell_rect = patches.Rectangle((8.22432, 8.87504), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.60432, 9.57504, 'AND2_28', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 27.1388)
ax.set_ylim(-10, 28)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251201_104600_360/quadratic_iter_35.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251201_104600_360/quadratic_iter_35.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251201_104600_360/quadratic_iter_35.png")
