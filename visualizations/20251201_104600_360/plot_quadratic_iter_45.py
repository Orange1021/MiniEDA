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
cell_rect = patches.Rectangle((8.64557, 8.59026), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93057, 9.29026, 'NOR2_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_25
cell_rect = patches.Rectangle((8.68042, 8.54384), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96542, 9.24384, 'NOR2_25', ha='center', va='center', fontsize=8)

# Cell: NOR2_23
cell_rect = patches.Rectangle((8.62558, 8.59647), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91058, 9.29647, 'NOR2_23', ha='center', va='center', fontsize=8)

# Cell: NOR2_21
cell_rect = patches.Rectangle((8.63091, 8.58368), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91591, 9.28368, 'NOR2_21', ha='center', va='center', fontsize=8)

# Cell: NOR2_20
cell_rect = patches.Rectangle((8.58353, 8.60116), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86853, 9.30116, 'NOR2_20', ha='center', va='center', fontsize=8)

# Cell: NOR2_18
cell_rect = patches.Rectangle((8.39626, 8.82526), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68126, 9.52526, 'NOR2_18', ha='center', va='center', fontsize=8)

# Cell: NOR2_17
cell_rect = patches.Rectangle((8.39799, 8.8232), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68299, 9.5232, 'NOR2_17', ha='center', va='center', fontsize=8)

# Cell: NOR2_16
cell_rect = patches.Rectangle((8.41077, 8.80096), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69577, 9.50096, 'NOR2_16', ha='center', va='center', fontsize=8)

# Cell: NOR2_12
cell_rect = patches.Rectangle((8.67909, 8.76008), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96409, 9.46008, 'NOR2_12', ha='center', va='center', fontsize=8)

# Cell: NOR2_5
cell_rect = patches.Rectangle((8.37454, 8.58896), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65954, 9.28896, 'NOR2_5', ha='center', va='center', fontsize=8)

# Cell: NOR2_4
cell_rect = patches.Rectangle((8.38052, 8.57137), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66552, 9.27137, 'NOR2_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((8.65483, 8.80685), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93983, 9.50685, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_14
cell_rect = patches.Rectangle((8.68442, 8.60231), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96942, 9.30231, 'NAND2_14', ha='center', va='center', fontsize=8)

# Cell: NOR2_9
cell_rect = patches.Rectangle((8.36758, 8.85361), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65258, 9.55361, 'NOR2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_12
cell_rect = patches.Rectangle((8.6506, 8.77984), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9356, 9.47984, 'NAND2_12', ha='center', va='center', fontsize=8)

# Cell: NAND2_11
cell_rect = patches.Rectangle((8.66462, 8.78379), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94962, 9.48379, 'NAND2_11', ha='center', va='center', fontsize=8)

# Cell: NAND2_9
cell_rect = patches.Rectangle((8.61566, 8.78033), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90066, 9.48033, 'NAND2_9', ha='center', va='center', fontsize=8)

# Cell: NAND2_7
cell_rect = patches.Rectangle((8.64744, 8.79039), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93244, 9.49039, 'NAND2_7', ha='center', va='center', fontsize=8)

# Cell: NAND2_5
cell_rect = patches.Rectangle((8.62923, 8.79599), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91423, 9.49599, 'NAND2_5', ha='center', va='center', fontsize=8)

# Cell: NAND2_4
cell_rect = patches.Rectangle((8.63617, 8.79615), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92117, 9.49615, 'NAND2_4', ha='center', va='center', fontsize=8)

# Cell: NAND2_3
cell_rect = patches.Rectangle((8.49988, 8.62687), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78488, 9.32687, 'NAND2_3', ha='center', va='center', fontsize=8)

# Cell: NAND2_2
cell_rect = patches.Rectangle((8.48821, 8.63878), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.77321, 9.33878, 'NAND2_2', ha='center', va='center', fontsize=8)

# Cell: NAND2_1
cell_rect = patches.Rectangle((8.47825, 8.64116), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76325, 9.34116, 'NAND2_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((8.46595, 8.64885), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75095, 9.34885, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR3_2
cell_rect = patches.Rectangle((8.56967, 8.55177), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94967, 9.25177, 'OR3_2', ha='center', va='center', fontsize=8)

# Cell: OR3_1
cell_rect = patches.Rectangle((8.52351, 8.59191), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90351, 9.29191, 'OR3_1', ha='center', va='center', fontsize=8)

# Cell: OR3_0
cell_rect = patches.Rectangle((8.47054, 8.61078), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85054, 9.31078, 'OR3_0', ha='center', va='center', fontsize=8)

# Cell: OR2_5
cell_rect = patches.Rectangle((8.53568, 8.78569), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91568, 9.48569, 'OR2_5', ha='center', va='center', fontsize=8)

# Cell: OR2_4
cell_rect = patches.Rectangle((8.55244, 8.79039), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93244, 9.49039, 'OR2_4', ha='center', va='center', fontsize=8)

# Cell: OR2_3
cell_rect = patches.Rectangle((8.56599, 8.55357), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94599, 9.25357, 'OR2_3', ha='center', va='center', fontsize=8)

# Cell: OR2_2
cell_rect = patches.Rectangle((8.52048, 8.59417), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90048, 9.29417, 'OR2_2', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((8.46891, 8.61131), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84891, 9.31131, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_15
cell_rect = patches.Rectangle((8.57347, 8.60579), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85847, 9.30579, 'NOR2_15', ha='center', va='center', fontsize=8)

# Cell: NOR2_10
cell_rect = patches.Rectangle((8.36934, 8.84659), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65434, 9.54659, 'NOR2_10', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((8.42432, 8.6469), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80432, 9.3469, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: AND2_40
cell_rect = patches.Rectangle((8.57514, 8.56517), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95514, 9.26517, 'AND2_40', ha='center', va='center', fontsize=8)

# Cell: AND2_39
cell_rect = patches.Rectangle((8.58171, 8.55915), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96171, 9.25915, 'AND2_39', ha='center', va='center', fontsize=8)

# Cell: AND2_38
cell_rect = patches.Rectangle((8.5503, 8.58621), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9303, 9.28621, 'AND2_38', ha='center', va='center', fontsize=8)

# Cell: AND2_37
cell_rect = patches.Rectangle((8.58596, 8.54278), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96596, 9.24278, 'AND2_37', ha='center', va='center', fontsize=8)

# Cell: AND3_2
cell_rect = patches.Rectangle((8.56999, 8.55224), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94999, 9.25224, 'AND3_2', ha='center', va='center', fontsize=8)

# Cell: AND2_35
cell_rect = patches.Rectangle((8.52547, 8.59256), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90547, 9.29256, 'AND2_35', ha='center', va='center', fontsize=8)

# Cell: AND2_32
cell_rect = patches.Rectangle((8.48324, 8.60298), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.86324, 9.30298, 'AND2_32', ha='center', va='center', fontsize=8)

# Cell: AND2_31
cell_rect = patches.Rectangle((8.29224, 8.82643), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67224, 9.52643, 'AND2_31', ha='center', va='center', fontsize=8)

# Cell: NOR2_13
cell_rect = patches.Rectangle((8.67257, 8.75826), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95757, 9.45826, 'NOR2_13', ha='center', va='center', fontsize=8)

# Cell: AND2_30
cell_rect = patches.Rectangle((8.28603, 8.83508), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66603, 9.53508, 'AND2_30', ha='center', va='center', fontsize=8)

# Cell: NOT_20
cell_rect = patches.Rectangle((8.48961, 8.58436), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67961, 9.28436, 'NOT_20', ha='center', va='center', fontsize=8)

# Cell: NOT_13
cell_rect = patches.Rectangle((8.78394, 8.67536), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97394, 9.37536, 'NOT_13', ha='center', va='center', fontsize=8)

# Cell: AND2_14
cell_rect = patches.Rectangle((8.5678, 8.5525), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9478, 9.2525, 'AND2_14', ha='center', va='center', fontsize=8)

# Cell: NOT_12
cell_rect = patches.Rectangle((8.69887, 8.78772), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88887, 9.48772, 'NOT_12', ha='center', va='center', fontsize=8)

# Cell: NOT_11
cell_rect = patches.Rectangle((8.7033, 8.78703), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8933, 9.48703, 'NOT_11', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((7.23656, 8.73915), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85156, 9.43915, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: NOT_27
cell_rect = patches.Rectangle((8.68762, 8.78679), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87762, 9.48679, 'NOT_27', ha='center', va='center', fontsize=8)

# Cell: NOR2_8
cell_rect = patches.Rectangle((8.36899, 8.84611), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65399, 9.54611, 'NOR2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_10
cell_rect = patches.Rectangle((8.57608, 8.67313), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76608, 9.37313, 'NOT_10', ha='center', va='center', fontsize=8)

# Cell: P5
cell_rect = patches.Rectangle((8.66671, 8.61181), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85671, 9.31181, 'P5', ha='center', va='center', fontsize=8)

# Cell: AND2_29
cell_rect = patches.Rectangle((8.28465, 8.83859), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66465, 9.53859, 'AND2_29', ha='center', va='center', fontsize=8)

# Cell: NOT_4
cell_rect = patches.Rectangle((8.68188, 8.62558), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87188, 9.32558, 'NOT_4', ha='center', va='center', fontsize=8)

# Cell: DFF_14
cell_rect = patches.Rectangle((7.14542, 8.66136), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76042, 9.36136, 'DFF_14', ha='center', va='center', fontsize=8)

# Cell: AND2_10
cell_rect = patches.Rectangle((8.25906, 8.58394), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63906, 9.28394, 'AND2_10', ha='center', va='center', fontsize=8)

# Cell: NOT_14
cell_rect = patches.Rectangle((8.72647, 8.79632), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91647, 9.49632, 'NOT_14', ha='center', va='center', fontsize=8)

# Cell: NOT_2
cell_rect = patches.Rectangle((8.71095, 8.78842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90095, 9.48842, 'NOT_2', ha='center', va='center', fontsize=8)

# Cell: DFF_11
cell_rect = patches.Rectangle((7.15679, 8.65332), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.77179, 9.35332, 'DFF_11', ha='center', va='center', fontsize=8)

# Cell: NOR2_6
cell_rect = patches.Rectangle((8.39659, 8.57695), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68159, 9.27695, 'NOR2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_18
cell_rect = patches.Rectangle((8.47717, 8.58208), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66717, 9.28208, 'NOT_18', ha='center', va='center', fontsize=8)

# Cell: NOT_28
cell_rect = patches.Rectangle((8.4701, 8.84438), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6601, 9.54438, 'NOT_28', ha='center', va='center', fontsize=8)

# Cell: NOT_5
cell_rect = patches.Rectangle((8.64875, 8.63601), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83875, 9.33601, 'NOT_5', ha='center', va='center', fontsize=8)

# Cell: P4
cell_rect = patches.Rectangle((8.63741, 8.63597), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82741, 9.33597, 'P4', ha='center', va='center', fontsize=8)

# Cell: AND2_11
cell_rect = patches.Rectangle((8.26525, 8.56514), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64525, 9.26514, 'AND2_11', ha='center', va='center', fontsize=8)

# Cell: DFF_8
cell_rect = patches.Rectangle((7.14685, 8.73395), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76185, 9.43395, 'DFF_8', ha='center', va='center', fontsize=8)

# Cell: AND2_18
cell_rect = patches.Rectangle((8.26856, 8.85509), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64856, 9.55509, 'AND2_18', ha='center', va='center', fontsize=8)

# Cell: P3
cell_rect = patches.Rectangle((8.50875, 8.80532), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69875, 9.50532, 'P3', ha='center', va='center', fontsize=8)

# Cell: NAND2_16
cell_rect = patches.Rectangle((8.69288, 8.59805), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97788, 9.29805, 'NAND2_16', ha='center', va='center', fontsize=8)

# Cell: A1
cell_rect = patches.Rectangle((8.47669, 8.5621), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66669, 9.2621, 'A1', ha='center', va='center', fontsize=8)

# Cell: NOT_43
cell_rect = patches.Rectangle((8.45902, 8.56795), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64902, 9.26795, 'NOT_43', ha='center', va='center', fontsize=8)

# Cell: AND2_21
cell_rect = patches.Rectangle((8.27024, 8.82888), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65024, 9.52888, 'AND2_21', ha='center', va='center', fontsize=8)

# Cell: NOT_16
cell_rect = patches.Rectangle((8.64964, 8.79687), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83964, 9.49687, 'NOT_16', ha='center', va='center', fontsize=8)

# Cell: START
cell_rect = patches.Rectangle((8.78009, 8.73843), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97009, 9.43843, 'START', ha='center', va='center', fontsize=8)

# Cell: NOT_49
cell_rect = patches.Rectangle((8.69409, 8.59689), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88409, 9.29689, 'NOT_49', ha='center', va='center', fontsize=8)

# Cell: P6
cell_rect = patches.Rectangle((8.71311, 8.59663), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90311, 9.29663, 'P6', ha='center', va='center', fontsize=8)

# Cell: NOT_9
cell_rect = patches.Rectangle((8.53461, 8.7746), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72461, 9.4746, 'NOT_9', ha='center', va='center', fontsize=8)

# Cell: DFF_7
cell_rect = patches.Rectangle((7.14643, 8.73376), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76143, 9.43376, 'DFF_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_19
cell_rect = patches.Rectangle((8.39573, 8.81759), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68073, 9.51759, 'NOR2_19', ha='center', va='center', fontsize=8)

# Cell: NOR3_0
cell_rect = patches.Rectangle((8.58526, 8.76978), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87026, 9.46978, 'NOR3_0', ha='center', va='center', fontsize=8)

# Cell: CNTVCO2
cell_rect = patches.Rectangle((8.75665, 8.81166), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94665, 9.51166, 'CNTVCO2', ha='center', va='center', fontsize=8)

# Cell: NOT_52
cell_rect = patches.Rectangle((8.73753, 8.58312), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92753, 9.28312, 'NOT_52', ha='center', va='center', fontsize=8)

# Cell: A0
cell_rect = patches.Rectangle((8.4472, 8.5781), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6372, 9.2781, 'A0', ha='center', va='center', fontsize=8)

# Cell: NOT_29
cell_rect = patches.Rectangle((8.46968, 8.84846), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65968, 9.54846, 'NOT_29', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((8.60883, 8.69193), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79883, 9.39193, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_9
cell_rect = patches.Rectangle((7.14662, 8.73345), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76162, 9.43345, 'DFF_9', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((8.71656, 8.79038), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90656, 9.49038, 'NOT_1', ha='center', va='center', fontsize=8)

# Cell: NAND2_6
cell_rect = patches.Rectangle((8.52384, 8.64214), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80884, 9.34214, 'NAND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_15
cell_rect = patches.Rectangle((8.56899, 8.68339), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75899, 9.38339, 'NOT_15', ha='center', va='center', fontsize=8)

# Cell: AND2_1
cell_rect = patches.Rectangle((8.32528, 8.61004), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70528, 9.31004, 'AND2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_24
cell_rect = patches.Rectangle((8.6773, 8.54376), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9623, 9.24376, 'NOR2_24', ha='center', va='center', fontsize=8)

# Cell: NOR2_14
cell_rect = patches.Rectangle((8.67207, 8.75358), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95707, 9.45358, 'NOR2_14', ha='center', va='center', fontsize=8)

# Cell: B0
cell_rect = patches.Rectangle((8.46707, 8.83484), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65707, 9.53484, 'B0', ha='center', va='center', fontsize=8)

# Cell: A2
cell_rect = patches.Rectangle((8.44792, 8.57725), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63792, 9.27725, 'A2', ha='center', va='center', fontsize=8)

# Cell: NAND2_8
cell_rect = patches.Rectangle((8.63068, 8.78569), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91568, 9.48569, 'NAND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_46
cell_rect = patches.Rectangle((8.46941, 8.84631), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65941, 9.54631, 'NOT_46', ha='center', va='center', fontsize=8)

# Cell: B2
cell_rect = patches.Rectangle((8.44812, 8.86943), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63812, 9.56943, 'B2', ha='center', va='center', fontsize=8)

# Cell: READY
cell_rect = patches.Rectangle((8.66559, 8.78923), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85559, 9.48923, 'READY', ha='center', va='center', fontsize=8)

# Cell: NOT_7
cell_rect = patches.Rectangle((8.5347, 8.77541), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7247, 9.47541, 'NOT_7', ha='center', va='center', fontsize=8)

# Cell: AND2_25
cell_rect = patches.Rectangle((8.50062, 8.61123), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88062, 9.31123, 'AND2_25', ha='center', va='center', fontsize=8)

# Cell: NAND3_0
cell_rect = patches.Rectangle((8.58767, 8.79038), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.87267, 9.49038, 'NAND3_0', ha='center', va='center', fontsize=8)

# Cell: P1
cell_rect = patches.Rectangle((8.50859, 8.80424), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69859, 9.50424, 'P1', ha='center', va='center', fontsize=8)

# Cell: AND2_9
cell_rect = patches.Rectangle((8.2742, 8.57606), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6542, 9.27606, 'AND2_9', ha='center', va='center', fontsize=8)

# Cell: P0
cell_rect = patches.Rectangle((8.56379, 8.66515), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.75379, 9.36515, 'P0', ha='center', va='center', fontsize=8)

# Cell: DFF_6
cell_rect = patches.Rectangle((7.19314, 8.67182), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80814, 9.37182, 'DFF_6', ha='center', va='center', fontsize=8)

# Cell: AND2_17
cell_rect = patches.Rectangle((8.32649, 8.80113), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70649, 9.50113, 'AND2_17', ha='center', va='center', fontsize=8)

# Cell: NOT_25
cell_rect = patches.Rectangle((8.6786, 8.58716), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.8686, 9.28716, 'NOT_25', ha='center', va='center', fontsize=8)

# Cell: NOT_24
cell_rect = patches.Rectangle((8.64814, 8.61432), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83814, 9.31432, 'NOT_24', ha='center', va='center', fontsize=8)

# Cell: NOT_21
cell_rect = patches.Rectangle((8.47454, 8.59316), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66454, 9.29316, 'NOT_21', ha='center', va='center', fontsize=8)

# Cell: NOT_8
cell_rect = patches.Rectangle((8.5348, 8.77665), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7248, 9.47665, 'NOT_8', ha='center', va='center', fontsize=8)

# Cell: NOT_55
cell_rect = patches.Rectangle((8.77696, 8.55091), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96696, 9.25091, 'NOT_55', ha='center', va='center', fontsize=8)

# Cell: DFF_5
cell_rect = patches.Rectangle((7.20313, 8.66454), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81813, 9.36454, 'DFF_5', ha='center', va='center', fontsize=8)

# Cell: AND2_26
cell_rect = patches.Rectangle((8.46949, 8.6104), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84949, 9.3104, 'AND2_26', ha='center', va='center', fontsize=8)

# Cell: CNTVCON2
cell_rect = patches.Rectangle((8.7298, 8.80022), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9198, 9.50022, 'CNTVCON2', ha='center', va='center', fontsize=8)

# Cell: NOT_3
cell_rect = patches.Rectangle((8.71103, 8.59879), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90103, 9.29879, 'NOT_3', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((7.23889, 8.74074), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85389, 9.44074, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: AND2_7
cell_rect = patches.Rectangle((8.31882, 8.76605), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69882, 9.46605, 'AND2_7', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((8.51813, 8.76808), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80313, 9.46808, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: DFF_13
cell_rect = patches.Rectangle((7.15133, 8.65829), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76633, 9.35829, 'DFF_13', ha='center', va='center', fontsize=8)

# Cell: AND2_19
cell_rect = patches.Rectangle((8.27289, 8.85604), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65289, 9.55604, 'AND2_19', ha='center', va='center', fontsize=8)

# Cell: DFF_4
cell_rect = patches.Rectangle((7.21978, 8.65955), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.83478, 9.35955, 'DFF_4', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((8.62443, 8.79583), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90943, 9.49583, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_13
cell_rect = patches.Rectangle((8.67796, 8.60353), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96296, 9.30353, 'NAND2_13', ha='center', va='center', fontsize=8)

# Cell: NOT_37
cell_rect = patches.Rectangle((8.73012, 8.59147), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92012, 9.29147, 'NOT_37', ha='center', va='center', fontsize=8)

# Cell: NOR2_7
cell_rect = patches.Rectangle((8.37152, 8.59044), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65652, 9.29044, 'NOR2_7', ha='center', va='center', fontsize=8)

# Cell: AND2_22
cell_rect = patches.Rectangle((8.56006, 8.57356), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94006, 9.27356, 'AND2_22', ha='center', va='center', fontsize=8)

# Cell: P2
cell_rect = patches.Rectangle((8.50875, 8.80704), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69875, 9.50704, 'P2', ha='center', va='center', fontsize=8)

# Cell: B3
cell_rect = patches.Rectangle((8.44856, 8.86128), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.63856, 9.56128, 'B3', ha='center', va='center', fontsize=8)

# Cell: AND2_3
cell_rect = patches.Rectangle((8.31649, 8.60793), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69649, 9.30793, 'AND2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((8.73362, 8.79739), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92362, 9.49739, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: A3
cell_rect = patches.Rectangle((8.45365, 8.5572), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64365, 9.2572, 'A3', ha='center', va='center', fontsize=8)

# Cell: P7
cell_rect = patches.Rectangle((8.75362, 8.5599), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94362, 9.2599, 'P7', ha='center', va='center', fontsize=8)

# Cell: DFF_12
cell_rect = patches.Rectangle((7.15254, 8.65864), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76754, 9.35864, 'DFF_12', ha='center', va='center', fontsize=8)

# Cell: NOT_48
cell_rect = patches.Rectangle((8.4693, 8.8217), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.6593, 9.5217, 'NOT_48', ha='center', va='center', fontsize=8)

# Cell: DFF_10
cell_rect = patches.Rectangle((7.16677, 8.68257), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.78177, 9.38257, 'DFF_10', ha='center', va='center', fontsize=8)

# Cell: NAND2_10
cell_rect = patches.Rectangle((8.50526, 8.66636), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.79026, 9.36636, 'NAND2_10', ha='center', va='center', fontsize=8)

# Cell: DFF_3
cell_rect = patches.Rectangle((7.23474, 8.64562), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84974, 9.34562, 'DFF_3', ha='center', va='center', fontsize=8)

# Cell: NOT_31
cell_rect = patches.Rectangle((8.47941, 8.83199), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66941, 9.53199, 'NOT_31', ha='center', va='center', fontsize=8)

# Cell: NOT_23
cell_rect = patches.Rectangle((8.61395, 8.62522), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.80395, 9.32522, 'NOT_23', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((7.22874, 8.73776), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.84374, 9.43776, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: AND2_4
cell_rect = patches.Rectangle((8.29911, 8.8261), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67911, 9.5261, 'AND2_4', ha='center', va='center', fontsize=8)

# Cell: NOT_26
cell_rect = patches.Rectangle((8.74781, 8.80018), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93781, 9.50018, 'NOT_26', ha='center', va='center', fontsize=8)

# Cell: NOR2_27
cell_rect = patches.Rectangle((8.68048, 8.56461), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96548, 9.26461, 'NOR2_27', ha='center', va='center', fontsize=8)

# Cell: NOT_39
cell_rect = patches.Rectangle((8.74466, 8.76977), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93466, 9.46977, 'NOT_39', ha='center', va='center', fontsize=8)

# Cell: NOT_30
cell_rect = patches.Rectangle((8.47111, 8.84334), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66111, 9.54334, 'NOT_30', ha='center', va='center', fontsize=8)

# Cell: NOT_32
cell_rect = patches.Rectangle((8.63112, 8.69103), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82112, 9.39103, 'NOT_32', ha='center', va='center', fontsize=8)

# Cell: NOT_19
cell_rect = patches.Rectangle((8.47457, 8.59228), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66457, 9.29228, 'NOT_19', ha='center', va='center', fontsize=8)

# Cell: NOT_34
cell_rect = patches.Rectangle((8.63419, 8.63116), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.82419, 9.33116, 'NOT_34', ha='center', va='center', fontsize=8)

# Cell: NOR2_28
cell_rect = patches.Rectangle((8.6701, 8.57663), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9551, 9.27663, 'NOR2_28', ha='center', va='center', fontsize=8)

# Cell: NOR2_3
cell_rect = patches.Rectangle((8.60624, 8.79116), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89124, 9.49116, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOT_35
cell_rect = patches.Rectangle((8.74627, 8.57778), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93627, 9.27778, 'NOT_35', ha='center', va='center', fontsize=8)

# Cell: AND2_8
cell_rect = patches.Rectangle((8.28138, 8.57893), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66138, 9.27893, 'AND2_8', ha='center', va='center', fontsize=8)

# Cell: NOT_36
cell_rect = patches.Rectangle((8.74397, 8.58158), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93397, 9.28158, 'NOT_36', ha='center', va='center', fontsize=8)

# Cell: AND2_15
cell_rect = patches.Rectangle((8.32424, 8.80411), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70424, 9.50411, 'AND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_41
cell_rect = patches.Rectangle((8.47008, 8.57896), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66008, 9.27896, 'NOT_41', ha='center', va='center', fontsize=8)

# Cell: NOT_42
cell_rect = patches.Rectangle((8.45372, 8.58719), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64372, 9.28719, 'NOT_42', ha='center', va='center', fontsize=8)

# Cell: NOT_44
cell_rect = patches.Rectangle((8.5665, 8.71819), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7565, 9.41819, 'NOT_44', ha='center', va='center', fontsize=8)

# Cell: AND2_36
cell_rect = patches.Rectangle((8.57655, 8.54703), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95655, 9.24703, 'AND2_36', ha='center', va='center', fontsize=8)

# Cell: NOT_45
cell_rect = patches.Rectangle((8.46941, 8.84081), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.65941, 9.54081, 'NOT_45', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((8.3308, 8.60864), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.7108, 9.30864, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: NOR2_11
cell_rect = patches.Rectangle((8.38804, 8.8102), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67304, 9.5102, 'NOR2_11', ha='center', va='center', fontsize=8)

# Cell: NOT_47
cell_rect = patches.Rectangle((8.465, 8.84944), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.655, 9.54944, 'NOT_47', ha='center', va='center', fontsize=8)

# Cell: NOT_50
cell_rect = patches.Rectangle((8.69685, 8.59382), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.88685, 9.29382, 'NOT_50', ha='center', va='center', fontsize=8)

# Cell: NOT_33
cell_rect = patches.Rectangle((8.51271, 8.80566), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70271, 9.50566, 'NOT_33', ha='center', va='center', fontsize=8)

# Cell: NOT_51
cell_rect = patches.Rectangle((8.74894, 8.56395), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.93894, 9.26395, 'NOT_51', ha='center', va='center', fontsize=8)

# Cell: NOT_53
cell_rect = patches.Rectangle((8.75223, 8.59712), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.94223, 9.29712, 'NOT_53', ha='center', va='center', fontsize=8)

# Cell: AND2_33
cell_rect = patches.Rectangle((8.52671, 8.58949), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90671, 9.28949, 'AND2_33', ha='center', va='center', fontsize=8)

# Cell: AND2_2
cell_rect = patches.Rectangle((8.31631, 8.61396), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.69631, 9.31396, 'AND2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_22
cell_rect = patches.Rectangle((8.63521, 8.58466), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92021, 9.28466, 'NOR2_22', ha='center', va='center', fontsize=8)

# Cell: NAND2_15
cell_rect = patches.Rectangle((8.69602, 8.59368), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.98102, 9.29368, 'NAND2_15', ha='center', va='center', fontsize=8)

# Cell: NOT_56
cell_rect = patches.Rectangle((8.76532, 8.59353), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.95532, 9.29353, 'NOT_56', ha='center', va='center', fontsize=8)

# Cell: NOT_57
cell_rect = patches.Rectangle((8.7889, 8.5755), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.9789, 9.2755, 'NOT_57', ha='center', va='center', fontsize=8)

# Cell: AND2_5
cell_rect = patches.Rectangle((8.30037, 8.827), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.68037, 9.527, 'AND2_5', ha='center', va='center', fontsize=8)

# Cell: AND2_6
cell_rect = patches.Rectangle((8.29814, 8.82774), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67814, 9.52774, 'AND2_6', ha='center', va='center', fontsize=8)

# Cell: NOT_6
cell_rect = patches.Rectangle((8.62856, 8.65154), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.81856, 9.35154, 'NOT_6', ha='center', va='center', fontsize=8)

# Cell: NOT_17
cell_rect = patches.Rectangle((8.57377, 8.81437), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.76377, 9.51437, 'NOT_17', ha='center', va='center', fontsize=8)

# Cell: AND2_12
cell_rect = patches.Rectangle((8.47154, 8.60995), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85154, 9.30995, 'AND2_12', ha='center', va='center', fontsize=8)

# Cell: NOT_22
cell_rect = patches.Rectangle((8.58631, 8.64413), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.77631, 9.34413, 'NOT_22', ha='center', va='center', fontsize=8)

# Cell: NOT_38
cell_rect = patches.Rectangle((8.71752, 8.59402), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90752, 9.29402, 'NOT_38', ha='center', va='center', fontsize=8)

# Cell: AND2_13
cell_rect = patches.Rectangle((8.52323, 8.59236), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90323, 9.29236, 'AND2_13', ha='center', va='center', fontsize=8)

# Cell: B1
cell_rect = patches.Rectangle((8.45043, 8.85957), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64043, 9.55957, 'B1', ha='center', va='center', fontsize=8)

# Cell: AND2_16
cell_rect = patches.Rectangle((8.32492, 8.80357), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.70492, 9.50357, 'AND2_16', ha='center', va='center', fontsize=8)

# Cell: AND2_34
cell_rect = patches.Rectangle((8.53965, 8.58192), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.91965, 9.28192, 'AND2_34', ha='center', va='center', fontsize=8)

# Cell: NOT_54
cell_rect = patches.Rectangle((8.77819, 8.54626), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.96819, 9.24626, 'NOT_54', ha='center', va='center', fontsize=8)

# Cell: AND2_20
cell_rect = patches.Rectangle((8.26659, 8.86005), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.64659, 9.56005, 'AND2_20', ha='center', va='center', fontsize=8)

# Cell: NOT_40
cell_rect = patches.Rectangle((8.47863, 8.58075), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.66863, 9.28075, 'NOT_40', ha='center', va='center', fontsize=8)

# Cell: AND2_23
cell_rect = patches.Rectangle((8.54173, 8.59001), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.92173, 9.29001, 'AND2_23', ha='center', va='center', fontsize=8)

# Cell: NOT_58
cell_rect = patches.Rectangle((8.78026, 8.58302), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.97026, 9.28302, 'NOT_58', ha='center', va='center', fontsize=8)

# Cell: AND2_24
cell_rect = patches.Rectangle((8.51841, 8.60017), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.89841, 9.30017, 'AND2_24', ha='center', va='center', fontsize=8)

# Cell: AND3_1
cell_rect = patches.Rectangle((8.52335, 8.59236), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.90335, 9.29236, 'AND3_1', ha='center', va='center', fontsize=8)

# Cell: AND3_0
cell_rect = patches.Rectangle((8.47122, 8.61009), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.85122, 9.31009, 'AND3_0', ha='center', va='center', fontsize=8)

# Cell: AND2_27
cell_rect = patches.Rectangle((8.34673, 8.76836), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.72673, 9.46836, 'AND2_27', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((12.5396, 2.29842), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(12.7296, 2.99842, 'VDD', ha='center', va='center', fontsize=8)

# Cell: AND2_28
cell_rect = patches.Rectangle((8.29044, 8.82703), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(8.67044, 9.52703, 'AND2_28', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 27.1388)
ax.set_ylim(-10, 28)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251201_104600_360/quadratic_iter_45.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251201_104600_360/quadratic_iter_45.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251201_104600_360/quadratic_iter_45.png")
