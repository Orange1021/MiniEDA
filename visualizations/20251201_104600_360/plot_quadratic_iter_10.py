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
cell_rect = patches.Rectangle((9.36055, 8.36964), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.64555, 9.06964, 'NOR2_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_25
cell_rect = patches.Rectangle((9.50587, 8.36881), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.79087, 9.06881, 'NOR2_25', ha='center', va='center', fontsize=8)

# Cell: NOR2_23
cell_rect = patches.Rectangle((9.15547, 7.93098), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.44047, 8.63098, 'NOR2_23', ha='center', va='center', fontsize=8)

# Cell: NOR2_21
cell_rect = patches.Rectangle((8.65519, 8.96285), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94019, 9.66285, 'NOR2_21', ha='center', va='center', fontsize=8)

# Cell: NOR2_20
cell_rect = patches.Rectangle((8.30595, 8.59373), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.59095, 9.29373, 'NOR2_20', ha='center', va='center', fontsize=8)

# Cell: NOR2_18
cell_rect = patches.Rectangle((8.12309, 8.79115), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.40809, 9.49115, 'NOR2_18', ha='center', va='center', fontsize=8)

# Cell: NOR2_17
cell_rect = patches.Rectangle((7.9621, 9.47905), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.2471, 10.179, 'NOR2_17', ha='center', va='center', fontsize=8)

# Cell: NOR2_16
cell_rect = patches.Rectangle((7.70896, 8.85482), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.99396, 9.55482, 'NOR2_16', ha='center', va='center', fontsize=8)

# Cell: NOR2_12
cell_rect = patches.Rectangle((9.12976, 8.93579), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.41476, 9.63579, 'NOR2_12', ha='center', va='center', fontsize=8)

# Cell: NOR2_5
cell_rect = patches.Rectangle((7.80864, 8.20333), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.09364, 8.90333, 'NOR2_5', ha='center', va='center', fontsize=8)

# Cell: NOR2_4
cell_rect = patches.Rectangle((8.2191, 8.4541), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5041, 9.1541, 'NOR2_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((9.3482, 9.43019), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.6332, 10.1302, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_14
cell_rect = patches.Rectangle((9.22828, 7.88453), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.51328, 8.58453, 'NAND2_14', ha='center', va='center', fontsize=8)

# Cell: NOR2_9
cell_rect = patches.Rectangle((9.1565, 8.93005), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.4415, 9.63005, 'NOR2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_12
cell_rect = patches.Rectangle((9.11226, 9.09255), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.39726, 9.79255, 'NAND2_12', ha='center', va='center', fontsize=8)

# Cell: NAND2_11
cell_rect = patches.Rectangle((9.38431, 8.87683), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.66931, 9.57683, 'NAND2_11', ha='center', va='center', fontsize=8)

# Cell: NAND2_9
cell_rect = patches.Rectangle((8.91088, 9.08875), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.19588, 9.78875, 'NAND2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_7
cell_rect = patches.Rectangle((9.31633, 9.07552), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.60133, 9.77552, 'NAND2_7', ha='center', va='center', fontsize=8)

# Cell: NAND2_5
cell_rect = patches.Rectangle((9.42624, 9.12068), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.71124, 9.82068, 'NAND2_5', ha='center', va='center', fontsize=8)

# Cell: NAND2_4
cell_rect = patches.Rectangle((9.45983, 9.06486), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.74483, 9.76486, 'NAND2_4', ha='center', va='center', fontsize=8)

# Cell: NAND2_3
cell_rect = patches.Rectangle((8.39392, 8.55817), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67892, 9.25817, 'NAND2_3', ha='center', va='center', fontsize=8)

# Cell: NAND2_2
cell_rect = patches.Rectangle((8.19232, 8.67658), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.47732, 9.37658, 'NAND2_2', ha='center', va='center', fontsize=8)

# Cell: NAND2_1
cell_rect = patches.Rectangle((8.17453, 8.72505), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45953, 9.42505, 'NAND2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((8.0611, 8.29058), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.3461, 8.99058, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR3_2
cell_rect = patches.Rectangle((9.08975, 8.48103), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.46975, 9.18103, 'OR3_2', ha='center', va='center', fontsize=8)

# Cell: OR3_1
cell_rect = patches.Rectangle((8.50818, 8.91639), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88818, 9.61639, 'OR3_1', ha='center', va='center', fontsize=8)

# Cell: OR3_0
cell_rect = patches.Rectangle((8.33078, 8.91927), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71078, 9.61927, 'OR3_0', ha='center', va='center', fontsize=8)

# Cell: OR2_5
cell_rect = patches.Rectangle((8.91833, 9.21575), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.29833, 9.91575, 'OR2_5', ha='center', va='center', fontsize=8)

# Cell: OR2_4
cell_rect = patches.Rectangle((9.22133, 9.07552), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.60133, 9.77552, 'OR2_4', ha='center', va='center', fontsize=8)

# Cell: OR2_3
cell_rect = patches.Rectangle((9.03525, 8.41249), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.41525, 9.11249, 'OR2_3', ha='center', va='center', fontsize=8)

# Cell: OR2_2
cell_rect = patches.Rectangle((8.48489, 8.95527), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86489, 9.65527, 'OR2_2', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((8.38538, 9.00957), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76538, 9.70957, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_15
cell_rect = patches.Rectangle((8.45644, 9.07949), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74144, 9.77949, 'NOR2_15', ha='center', va='center', fontsize=8)

# Cell: NOR2_10
cell_rect = patches.Rectangle((7.94696, 9.26144), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.23196, 9.96144, 'NOR2_10', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((8.03823, 7.942), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.41823, 8.642, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: AND2_40
cell_rect = patches.Rectangle((9.56744, 8.00673), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.94744, 8.70673, 'AND2_40', ha='center', va='center', fontsize=8)

# Cell: AND2_39
cell_rect = patches.Rectangle((8.82049, 7.6297), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.20049, 8.3297, 'AND2_39', ha='center', va='center', fontsize=8)

# Cell: AND2_38
cell_rect = patches.Rectangle((8.77816, 8.12703), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.15816, 8.82703, 'AND2_38', ha='center', va='center', fontsize=8)

# Cell: AND2_37
cell_rect = patches.Rectangle((9.09715, 8.20116), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.47715, 8.90116, 'AND2_37', ha='center', va='center', fontsize=8)

# Cell: AND3_2
cell_rect = patches.Rectangle((9.04003, 8.36926), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.42003, 9.06926, 'AND3_2', ha='center', va='center', fontsize=8)

# Cell: AND2_35
cell_rect = patches.Rectangle((8.23635, 7.69608), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.61635, 8.39608, 'AND2_35', ha='center', va='center', fontsize=8)

# Cell: AND2_32
cell_rect = patches.Rectangle((8.52965, 9.13367), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90965, 9.83367, 'AND2_32', ha='center', va='center', fontsize=8)

# Cell: AND2_31
cell_rect = patches.Rectangle((7.45075, 8.99013), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.83075, 9.69013, 'AND2_31', ha='center', va='center', fontsize=8)

# Cell: NOR2_13
cell_rect = patches.Rectangle((8.96738, 9.01721), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.25238, 9.71721, 'NOR2_13', ha='center', va='center', fontsize=8)

# Cell: AND2_30
cell_rect = patches.Rectangle((7.80937, 9.28877), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.18937, 9.98877, 'AND2_30', ha='center', va='center', fontsize=8)

# Cell: NOT_20
cell_rect = patches.Rectangle((7.5417, 8.43222), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.7317, 9.13222, 'NOT_20', ha='center', va='center', fontsize=8)

# Cell: NOT_13
cell_rect = patches.Rectangle((9.04713, 8.26132), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.23713, 8.96132, 'NOT_13', ha='center', va='center', fontsize=8)

# Cell: AND2_14
cell_rect = patches.Rectangle((9.07092, 8.46642), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.45092, 9.16642, 'AND2_14', ha='center', va='center', fontsize=8)

# Cell: NOT_12
cell_rect = patches.Rectangle((9.14042, 9.155), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.33042, 9.855, 'NOT_12', ha='center', va='center', fontsize=8)

# Cell: NOT_11
cell_rect = patches.Rectangle((9.11422, 9.21888), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.30422, 9.91888, 'NOT_11', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((7.43685, 8.98025), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.05185, 9.68025, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: NOT_27
cell_rect = patches.Rectangle((8.81303, 9.35088), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.00303, 10.0509, 'NOT_27', ha='center', va='center', fontsize=8)

# Cell: NOR2_8
cell_rect = patches.Rectangle((7.88023, 9.10175), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.16523, 9.80175, 'NOR2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_10
cell_rect = patches.Rectangle((8.31966, 8.7051), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.50966, 9.4051, 'NOT_10', ha='center', va='center', fontsize=8)

# Cell: P5
cell_rect = patches.Rectangle((8.67206, 8.94695), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86206, 9.64695, 'P5', ha='center', va='center', fontsize=8)

# Cell: AND2_29
cell_rect = patches.Rectangle((8.52317, 8.93441), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90317, 9.63441, 'AND2_29', ha='center', va='center', fontsize=8)

# Cell: NOT_4
cell_rect = patches.Rectangle((8.66375, 8.86801), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85375, 9.56801, 'NOT_4', ha='center', va='center', fontsize=8)

# Cell: DFF_14
cell_rect = patches.Rectangle((6.87128, 8.45923), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.48628, 9.15923, 'DFF_14', ha='center', va='center', fontsize=8)

# Cell: AND2_10
cell_rect = patches.Rectangle((9.43002, 7.42784), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.81002, 8.12784, 'AND2_10', ha='center', va='center', fontsize=8)

# Cell: NOT_14
cell_rect = patches.Rectangle((9.19662, 9.28545), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.38662, 9.98545, 'NOT_14', ha='center', va='center', fontsize=8)

# Cell: NOT_2
cell_rect = patches.Rectangle((9.3265, 9.14145), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.5165, 9.84145, 'NOT_2', ha='center', va='center', fontsize=8)

# Cell: DFF_11
cell_rect = patches.Rectangle((7.04889, 8.6354), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66389, 9.3354, 'DFF_11', ha='center', va='center', fontsize=8)

# Cell: NOR2_6
cell_rect = patches.Rectangle((7.28734, 8.59958), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.57234, 9.29958, 'NOR2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_18
cell_rect = patches.Rectangle((8.19678, 8.36051), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.38678, 9.06051, 'NOT_18', ha='center', va='center', fontsize=8)

# Cell: NOT_28
cell_rect = patches.Rectangle((7.98746, 9.1255), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.17746, 9.8255, 'NOT_28', ha='center', va='center', fontsize=8)

# Cell: NOT_5
cell_rect = patches.Rectangle((8.6442, 8.94019), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8342, 9.64019, 'NOT_5', ha='center', va='center', fontsize=8)

# Cell: P4
cell_rect = patches.Rectangle((8.43736, 7.91929), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62736, 8.61929, 'P4', ha='center', va='center', fontsize=8)

# Cell: AND2_11
cell_rect = patches.Rectangle((7.36805, 8.51608), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.74805, 9.21608, 'AND2_11', ha='center', va='center', fontsize=8)

# Cell: DFF_8
cell_rect = patches.Rectangle((6.9985, 8.9735), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6135, 9.6735, 'DFF_8', ha='center', va='center', fontsize=8)

# Cell: AND2_18
cell_rect = patches.Rectangle((7.23414, 9.32229), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.61414, 10.0223, 'AND2_18', ha='center', va='center', fontsize=8)

# Cell: P3
cell_rect = patches.Rectangle((8.18503, 9.15998), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.37503, 9.85998, 'P3', ha='center', va='center', fontsize=8)

# Cell: NAND2_16
cell_rect = patches.Rectangle((8.74888, 7.7533), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03388, 8.4533, 'NAND2_16', ha='center', va='center', fontsize=8)

# Cell: A1
cell_rect = patches.Rectangle((7.16374, 8.58272), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.35374, 9.28272, 'A1', ha='center', va='center', fontsize=8)

# Cell: NOT_43
cell_rect = patches.Rectangle((7.5436, 8.52413), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.7336, 9.22413, 'NOT_43', ha='center', va='center', fontsize=8)

# Cell: AND2_21
cell_rect = patches.Rectangle((8.83612, 9.2352), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.21612, 9.9352, 'AND2_21', ha='center', va='center', fontsize=8)

# Cell: NOT_16
cell_rect = patches.Rectangle((8.84948, 9.18587), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.03948, 9.88587, 'NOT_16', ha='center', va='center', fontsize=8)

# Cell: START
cell_rect = patches.Rectangle((9.07983, 8.80887), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.26983, 9.50887, 'START', ha='center', va='center', fontsize=8)

# Cell: NOT_49
cell_rect = patches.Rectangle((8.6038, 9.04144), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7938, 9.74144, 'NOT_49', ha='center', va='center', fontsize=8)

# Cell: P6
cell_rect = patches.Rectangle((8.73108, 8.85761), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92108, 9.55761, 'P6', ha='center', va='center', fontsize=8)

# Cell: NOT_9
cell_rect = patches.Rectangle((8.27567, 8.97831), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.46567, 9.67831, 'NOT_9', ha='center', va='center', fontsize=8)

# Cell: DFF_7
cell_rect = patches.Rectangle((7.00621, 8.92673), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62121, 9.62673, 'DFF_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_19
cell_rect = patches.Rectangle((8.35401, 9.10076), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63901, 9.80076, 'NOR2_19', ha='center', va='center', fontsize=8)

# Cell: NOR3_0
cell_rect = patches.Rectangle((8.89604, 9.07166), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.18104, 9.77166, 'NOR3_0', ha='center', va='center', fontsize=8)

# Cell: CNTVCO2
cell_rect = patches.Rectangle((10.1722, 8.6844), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(10.3622, 9.3844, 'CNTVCO2', ha='center', va='center', fontsize=8)

# Cell: NOT_52
cell_rect = patches.Rectangle((9.38026, 8.81344), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.57026, 9.51344, 'NOT_52', ha='center', va='center', fontsize=8)

# Cell: A0
cell_rect = patches.Rectangle((7.48204, 7.11663), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.67204, 7.81663, 'A0', ha='center', va='center', fontsize=8)

# Cell: NOT_29
cell_rect = patches.Rectangle((8.98125, 8.98146), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.17125, 9.68146, 'NOT_29', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((8.55607, 8.79074), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74607, 9.49074, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_9
cell_rect = patches.Rectangle((6.94982, 8.86112), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.56482, 9.56112, 'DFF_9', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((9.31925, 9.17383), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.50925, 9.87383, 'NOT_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_6
cell_rect = patches.Rectangle((8.18555, 8.00674), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.47055, 8.70674, 'NAND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_15
cell_rect = patches.Rectangle((8.43786, 8.60367), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62786, 9.30367, 'NOT_15', ha='center', va='center', fontsize=8)

# Cell: AND2_1
cell_rect = patches.Rectangle((7.66816, 8.50961), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.04816, 9.20961, 'AND2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_24
cell_rect = patches.Rectangle((9.17575, 8.28398), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.46075, 8.98398, 'NOR2_24', ha='center', va='center', fontsize=8)

# Cell: NOR2_14
cell_rect = patches.Rectangle((8.98739, 8.82861), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.27239, 9.52861, 'NOR2_14', ha='center', va='center', fontsize=8)

# Cell: B0
cell_rect = patches.Rectangle((6.78298, 8.81226), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.97298, 9.51226, 'B0', ha='center', va='center', fontsize=8)

# Cell: A2
cell_rect = patches.Rectangle((7.84499, 8.10907), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.03499, 8.80907, 'A2', ha='center', va='center', fontsize=8)

# Cell: NAND2_8
cell_rect = patches.Rectangle((9.01333, 9.21575), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.29833, 9.91575, 'NAND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_46
cell_rect = patches.Rectangle((8.01827, 9.92839), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.20827, 10.6284, 'NOT_46', ha='center', va='center', fontsize=8)

# Cell: B2
cell_rect = patches.Rectangle((9.74974, 8.89005), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.93974, 9.59005, 'B2', ha='center', va='center', fontsize=8)

# Cell: READY
cell_rect = patches.Rectangle((8.85575, 9.18825), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.04575, 9.88825, 'READY', ha='center', va='center', fontsize=8)

# Cell: NOT_7
cell_rect = patches.Rectangle((8.2642, 9.09574), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.4542, 9.79574, 'NOT_7', ha='center', va='center', fontsize=8)

# Cell: AND2_25
cell_rect = patches.Rectangle((8.34507, 7.7955), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72507, 8.4955, 'AND2_25', ha='center', va='center', fontsize=8)

# Cell: NAND3_0
cell_rect = patches.Rectangle((8.93512, 9.1732), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.22012, 9.8732, 'NAND3_0', ha='center', va='center', fontsize=8)

# Cell: P1
cell_rect = patches.Rectangle((8.2031, 9.04209), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.3931, 9.74209, 'P1', ha='center', va='center', fontsize=8)

# Cell: AND2_9
cell_rect = patches.Rectangle((7.11126, 8.53079), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.49126, 9.23079, 'AND2_9', ha='center', va='center', fontsize=8)

# Cell: P0
cell_rect = patches.Rectangle((8.25045, 8.6356), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.44045, 9.3356, 'P0', ha='center', va='center', fontsize=8)

# Cell: DFF_6
cell_rect = patches.Rectangle((7.04403, 8.421), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65903, 9.121, 'DFF_6', ha='center', va='center', fontsize=8)

# Cell: AND2_17
cell_rect = patches.Rectangle((7.86138, 8.96567), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.24138, 9.66567, 'AND2_17', ha='center', va='center', fontsize=8)

# Cell: NOT_25
cell_rect = patches.Rectangle((8.89415, 8.44721), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.08415, 9.14721, 'NOT_25', ha='center', va='center', fontsize=8)

# Cell: NOT_24
cell_rect = patches.Rectangle((8.47972, 8.85948), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66972, 9.55948, 'NOT_24', ha='center', va='center', fontsize=8)

# Cell: NOT_21
cell_rect = patches.Rectangle((7.78517, 7.65114), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.97517, 8.35114, 'NOT_21', ha='center', va='center', fontsize=8)

# Cell: NOT_8
cell_rect = patches.Rectangle((8.45716, 8.97502), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64716, 9.67502, 'NOT_8', ha='center', va='center', fontsize=8)

# Cell: NOT_55
cell_rect = patches.Rectangle((9.18846, 7.99189), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.37846, 8.69189, 'NOT_55', ha='center', va='center', fontsize=8)

# Cell: DFF_5
cell_rect = patches.Rectangle((7.16353, 8.88472), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.77853, 9.58472, 'DFF_5', ha='center', va='center', fontsize=8)

# Cell: AND2_26
cell_rect = patches.Rectangle((8.36626, 9.04736), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.74626, 9.74736, 'AND2_26', ha='center', va='center', fontsize=8)

# Cell: CNTVCON2
cell_rect = patches.Rectangle((9.1459, 9.06934), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.3359, 9.76934, 'CNTVCON2', ha='center', va='center', fontsize=8)

# Cell: NOT_3
cell_rect = patches.Rectangle((9.0186, 8.5575), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.2086, 9.2575, 'NOT_3', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((7.49676, 8.93623), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.11176, 9.63623, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: AND2_7
cell_rect = patches.Rectangle((8.32035, 9.0044), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70035, 9.7044, 'AND2_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((8.54611, 8.99358), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83111, 9.69358, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: DFF_13
cell_rect = patches.Rectangle((6.91937, 8.70513), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.53437, 9.40513, 'DFF_13', ha='center', va='center', fontsize=8)

# Cell: AND2_19
cell_rect = patches.Rectangle((7.86669, 10.23), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.24669, 10.93, 'AND2_19', ha='center', va='center', fontsize=8)

# Cell: DFF_4
cell_rect = patches.Rectangle((7.20295, 8.8329), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81795, 9.5329, 'DFF_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((9.012, 9.15892), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.297, 9.85892, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_13
cell_rect = patches.Rectangle((9.18075, 7.73278), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.46575, 8.43278, 'NAND2_13', ha='center', va='center', fontsize=8)

# Cell: NOT_37
cell_rect = patches.Rectangle((9.2922, 8.34832), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.4822, 9.04832, 'NOT_37', ha='center', va='center', fontsize=8)

# Cell: NOR2_7
cell_rect = patches.Rectangle((7.51774, 7.35045), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.80274, 8.05045, 'NOR2_7', ha='center', va='center', fontsize=8)

# Cell: AND2_22
cell_rect = patches.Rectangle((8.85583, 7.98871), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.23583, 8.68871, 'AND2_22', ha='center', va='center', fontsize=8)

# Cell: P2
cell_rect = patches.Rectangle((8.43782, 9.0266), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.62782, 9.7266, 'P2', ha='center', va='center', fontsize=8)

# Cell: B3
cell_rect = patches.Rectangle((7.71636, 9.09973), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.90636, 9.79973, 'B3', ha='center', va='center', fontsize=8)

# Cell: AND2_3
cell_rect = patches.Rectangle((7.74891, 8.41793), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.12891, 9.11793, 'AND2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((9.71223, 8.84971), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.90223, 9.54971, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: A3
cell_rect = patches.Rectangle((8.30022, 8.43197), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.49022, 9.13197, 'A3', ha='center', va='center', fontsize=8)

# Cell: P7
cell_rect = patches.Rectangle((9.20453, 8.43325), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.39453, 9.13325, 'P7', ha='center', va='center', fontsize=8)

# Cell: DFF_12
cell_rect = patches.Rectangle((6.93473, 8.66714), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.54973, 9.36714, 'DFF_12', ha='center', va='center', fontsize=8)

# Cell: NOT_48
cell_rect = patches.Rectangle((8.75262, 9.19486), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94262, 9.89486, 'NOT_48', ha='center', va='center', fontsize=8)

# Cell: DFF_10
cell_rect = patches.Rectangle((7.05143, 8.75684), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66643, 9.45684, 'DFF_10', ha='center', va='center', fontsize=8)

# Cell: NAND2_10
cell_rect = patches.Rectangle((8.16766, 8.08864), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.45266, 8.78864, 'NAND2_10', ha='center', va='center', fontsize=8)

# Cell: DFF_3
cell_rect = patches.Rectangle((7.36588, 8.66293), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98088, 9.36293, 'DFF_3', ha='center', va='center', fontsize=8)

# Cell: NOT_31
cell_rect = patches.Rectangle((7.45028, 8.98454), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.64028, 9.68454, 'NOT_31', ha='center', va='center', fontsize=8)

# Cell: NOT_23
cell_rect = patches.Rectangle((8.52493, 8.8507), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.71493, 9.5507, 'NOT_23', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((7.36277, 9.00606), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97777, 9.70606, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: AND2_4
cell_rect = patches.Rectangle((7.75605, 9.21575), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.13605, 9.91575, 'AND2_4', ha='center', va='center', fontsize=8)

# Cell: NOT_26
cell_rect = patches.Rectangle((9.65708, 8.95976), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.84708, 9.65976, 'NOT_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_27
cell_rect = patches.Rectangle((8.67659, 8.40519), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96159, 9.10519, 'NOR2_27', ha='center', va='center', fontsize=8)

# Cell: NOT_39
cell_rect = patches.Rectangle((8.79751, 9.35874), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98751, 10.0587, 'NOT_39', ha='center', va='center', fontsize=8)

# Cell: NOT_30
cell_rect = patches.Rectangle((8.17659, 9.24154), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.36659, 9.94154, 'NOT_30', ha='center', va='center', fontsize=8)

# Cell: NOT_32
cell_rect = patches.Rectangle((8.63181, 8.53193), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82181, 9.23193, 'NOT_32', ha='center', va='center', fontsize=8)

# Cell: NOT_19
cell_rect = patches.Rectangle((7.97768, 8.14971), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.16768, 8.84971, 'NOT_19', ha='center', va='center', fontsize=8)

# Cell: NOT_34
cell_rect = patches.Rectangle((8.3407, 8.48453), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.5307, 9.18453, 'NOT_34', ha='center', va='center', fontsize=8)

# Cell: NOR2_28
cell_rect = patches.Rectangle((8.79208, 7.98225), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07708, 8.68225, 'NOR2_28', ha='center', va='center', fontsize=8)

# Cell: NOR2_3
cell_rect = patches.Rectangle((9.01806, 9.18608), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.30306, 9.88608, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_35
cell_rect = patches.Rectangle((8.88176, 8.36183), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.07176, 9.06183, 'NOT_35', ha='center', va='center', fontsize=8)

# Cell: AND2_8
cell_rect = patches.Rectangle((7.36855, 7.32336), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.74855, 8.02336, 'AND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_36
cell_rect = patches.Rectangle((8.96647, 8.01227), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.15647, 8.71227, 'NOT_36', ha='center', va='center', fontsize=8)

# Cell: AND2_15
cell_rect = patches.Rectangle((8.22783, 8.97095), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.60783, 9.67095, 'AND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_41
cell_rect = patches.Rectangle((7.27143, 8.602), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.46143, 9.302, 'NOT_41', ha='center', va='center', fontsize=8)

# Cell: NOT_42
cell_rect = patches.Rectangle((9.71945, 7.44272), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.90945, 8.14272, 'NOT_42', ha='center', va='center', fontsize=8)

# Cell: NOT_44
cell_rect = patches.Rectangle((7.99682, 8.32067), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.18682, 9.02067, 'NOT_44', ha='center', va='center', fontsize=8)

# Cell: AND2_36
cell_rect = patches.Rectangle((9.11926, 8.51026), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.49926, 9.21026, 'AND2_36', ha='center', va='center', fontsize=8)

# Cell: NOT_45
cell_rect = patches.Rectangle((7.5553, 9.1842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.7453, 9.8842, 'NOT_45', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((7.82632, 8.00081), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.20632, 8.70081, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: NOR2_11
cell_rect = patches.Rectangle((7.1515, 8.85085), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.4365, 9.55085, 'NOR2_11', ha='center', va='center', fontsize=8)

# Cell: NOT_47
cell_rect = patches.Rectangle((8.02044, 8.58914), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.21044, 9.28914, 'NOT_47', ha='center', va='center', fontsize=8)

# Cell: NOT_50
cell_rect = patches.Rectangle((9.14517, 8.6557), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.33517, 9.3557, 'NOT_50', ha='center', va='center', fontsize=8)

# Cell: NOT_33
cell_rect = patches.Rectangle((8.21181, 9.02617), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.40181, 9.72617, 'NOT_33', ha='center', va='center', fontsize=8)

# Cell: NOT_51
cell_rect = patches.Rectangle((9.05554, 8.67151), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.24554, 9.37151, 'NOT_51', ha='center', va='center', fontsize=8)

# Cell: NOT_53
cell_rect = patches.Rectangle((8.50106, 7.41615), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69106, 8.11615, 'NOT_53', ha='center', va='center', fontsize=8)

# Cell: AND2_33
cell_rect = patches.Rectangle((8.5586, 9.01759), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9386, 9.71759, 'AND2_33', ha='center', va='center', fontsize=8)

# Cell: AND2_2
cell_rect = patches.Rectangle((8.58456, 8.08135), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96456, 8.78135, 'AND2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_22
cell_rect = patches.Rectangle((8.69621, 8.68459), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98121, 9.38459, 'NOR2_22', ha='center', va='center', fontsize=8)

# Cell: NAND2_15
cell_rect = patches.Rectangle((8.70905, 8.05652), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.99405, 8.75652, 'NAND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_56
cell_rect = patches.Rectangle((8.96986, 7.92054), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.15986, 8.62054, 'NOT_56', ha='center', va='center', fontsize=8)

# Cell: NOT_57
cell_rect = patches.Rectangle((8.93829, 7.43711), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.12829, 8.13711, 'NOT_57', ha='center', va='center', fontsize=8)

# Cell: AND2_5
cell_rect = patches.Rectangle((7.96178, 9.48949), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.34178, 10.1895, 'AND2_5', ha='center', va='center', fontsize=8)

# Cell: AND2_6
cell_rect = patches.Rectangle((7.91431, 8.87766), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.29431, 9.57766, 'AND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_6
cell_rect = patches.Rectangle((8.4361, 8.12645), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6261, 8.82645, 'NOT_6', ha='center', va='center', fontsize=8)

# Cell: NOT_17
cell_rect = patches.Rectangle((8.56666, 9.18772), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75666, 9.88772, 'NOT_17', ha='center', va='center', fontsize=8)

# Cell: AND2_12
cell_rect = patches.Rectangle((8.40814, 9.05556), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78814, 9.75556, 'AND2_12', ha='center', va='center', fontsize=8)

# Cell: NOT_22
cell_rect = patches.Rectangle((8.18393, 8.11775), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.37393, 8.81775, 'NOT_22', ha='center', va='center', fontsize=8)

# Cell: NOT_38
cell_rect = patches.Rectangle((9.19328, 8.20182), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.38328, 8.90182, 'NOT_38', ha='center', va='center', fontsize=8)

# Cell: AND2_13
cell_rect = patches.Rectangle((8.55122, 8.93603), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93122, 9.63603, 'AND2_13', ha='center', va='center', fontsize=8)

# Cell: B1
cell_rect = patches.Rectangle((8.12796, 9.2635), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.31796, 9.9635, 'B1', ha='center', va='center', fontsize=8)

# Cell: AND2_16
cell_rect = patches.Rectangle((8.04678, 9.11692), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.42678, 9.81692, 'AND2_16', ha='center', va='center', fontsize=8)

# Cell: AND2_34
cell_rect = patches.Rectangle((8.76453, 9.01239), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.14453, 9.71239, 'AND2_34', ha='center', va='center', fontsize=8)

# Cell: NOT_54
cell_rect = patches.Rectangle((9.10166, 8.50979), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.29166, 9.20979, 'NOT_54', ha='center', va='center', fontsize=8)

# Cell: AND2_20
cell_rect = patches.Rectangle((7.67462, 8.51296), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.05462, 9.21296, 'AND2_20', ha='center', va='center', fontsize=8)

# Cell: NOT_40
cell_rect = patches.Rectangle((7.60076, 7.31617), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.79076, 8.01617, 'NOT_40', ha='center', va='center', fontsize=8)

# Cell: AND2_23
cell_rect = patches.Rectangle((8.96352, 8.30482), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.34352, 9.00482, 'AND2_23', ha='center', va='center', fontsize=8)

# Cell: NOT_58
cell_rect = patches.Rectangle((9.63086, 7.76373), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.82086, 8.46373, 'NOT_58', ha='center', va='center', fontsize=8)

# Cell: AND2_24
cell_rect = patches.Rectangle((8.64811, 8.38162), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.02811, 9.08162, 'AND2_24', ha='center', va='center', fontsize=8)

# Cell: AND3_1
cell_rect = patches.Rectangle((8.6158, 9.00373), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9958, 9.70373, 'AND3_1', ha='center', va='center', fontsize=8)

# Cell: AND3_0
cell_rect = patches.Rectangle((8.47678, 9.00314), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85678, 9.70314, 'AND3_0', ha='center', va='center', fontsize=8)

# Cell: AND2_27
cell_rect = patches.Rectangle((8.13873, 8.67317), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.51873, 9.37317, 'AND2_27', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((12.5396, 2.29842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(12.7296, 2.99842, 'VDD', ha='center', va='center', fontsize=8)

# Cell: AND2_28
cell_rect = patches.Rectangle((8.02341, 9.00859), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.40341, 9.70859, 'AND2_28', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 27.1388)
ax.set_ylim(-10, 28)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251201_104600_360/quadratic_iter_10.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251201_104600_360/quadratic_iter_10.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251201_104600_360/quadratic_iter_10.png")
