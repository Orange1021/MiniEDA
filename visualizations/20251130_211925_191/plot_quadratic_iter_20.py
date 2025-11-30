#!/usr/bin/env python3
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Create figure and axis
fig, ax = plt.subplots(1, 1, figsize=(12, 10))

# Draw core area boundary
core_rect = patches.Rectangle((0, 0), 11.4525, 12, linewidth=2, edgecolor='black', facecolor='none')
ax.add_patch(core_rect)

# Draw cells
cell_colors = []
# Cell: NOR2_3
cell_rect = patches.Rectangle((5.7395, 5.61978), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.0245, 6.31978, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((5.67709, 5.57851), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.96209, 6.27851, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((5.57488, 5.66112), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.85988, 6.36112, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((5.54889, 5.64681), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.83389, 6.34681, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((5.61274, 5.6143), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89774, 6.3143, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((5.51242, 5.64726), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89242, 6.34726, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((5.54434, 5.62907), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.92434, 6.32907, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((4.27338, 5.61972), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.88838, 6.31972, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((5.61331, 5.68028), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.80331, 6.38028, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: G0
cell_rect = patches.Rectangle((4.24086, 4.74182), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.822, 6.32296, 'G0', ha='center', va='center', fontsize=8)

# Cell: G3
cell_rect = patches.Rectangle((4.31873, 4.72877), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89987, 6.30991, 'G3', ha='center', va='center', fontsize=8)

# Cell: G2
cell_rect = patches.Rectangle((4.40824, 4.68476), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.98938, 6.2659, 'G2', ha='center', va='center', fontsize=8)

# Cell: G17
cell_rect = patches.Rectangle((4.21251, 4.80464), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.79365, 6.38578, 'G17', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((5.4986, 5.62505), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.8786, 6.32505, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((4.27341, 5.64138), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.88841, 6.34138, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((7.79123, 5.40599), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.37237, 6.98713, 'VDD', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((4.32757, 4.7424), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.90871, 6.32354, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((4.34362, 5.63694), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.95862, 6.33694, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: G1
cell_rect = patches.Rectangle((4.43463, 4.75494), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.01577, 6.33608, 'G1', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((5.61141, 5.64585), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.80141, 6.34585, 'NOT_1', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 21.4525)
ax.set_ylim(-10, 22)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251130_211925_191/quadratic_iter_20.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251130_211925_191/quadratic_iter_20.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251130_211925_191/quadratic_iter_20.png")
