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
cell_rect = patches.Rectangle((5.65083, 5.60767), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.93583, 6.30767, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((5.65715, 5.62983), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.94215, 6.32983, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((5.596, 5.63031), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.881, 6.33031, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((5.57884, 5.64259), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.86384, 6.34259, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((5.60997, 5.63748), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89497, 6.33748, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((5.51867, 5.6251), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89867, 6.3251, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((5.53233, 5.62415), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.91233, 6.32415, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((4.27764, 5.63842), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89264, 6.33842, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((5.67334, 5.63389), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.86334, 6.33389, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: G0
cell_rect = patches.Rectangle((4.2658, 4.77389), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.84694, 6.35503, 'G0', ha='center', va='center', fontsize=8)

# Cell: G3
cell_rect = patches.Rectangle((4.31441, 4.75695), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89555, 6.33809, 'G3', ha='center', va='center', fontsize=8)

# Cell: G2
cell_rect = patches.Rectangle((4.38284, 4.74286), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.96398, 6.324, 'G2', ha='center', va='center', fontsize=8)

# Cell: G17
cell_rect = patches.Rectangle((4.26404, 4.7593), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.84518, 6.34044, 'G17', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((5.50361, 5.63899), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.88361, 6.33899, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((4.28213, 5.62916), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89713, 6.32916, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((7.79123, 5.40599), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.37237, 6.98713, 'VDD', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((4.3234, 4.75024), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.90454, 6.33138, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((4.29954, 5.61741), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.91454, 6.31741, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: G1
cell_rect = patches.Rectangle((4.35204, 4.724), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.93318, 6.30514, 'G1', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((5.66471, 5.65083), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.85471, 6.35083, 'NOT_1', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 21.4525)
ax.set_ylim(-10, 22)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251130_211925_191/quadratic_iter_25.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251130_211925_191/quadratic_iter_25.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251130_211925_191/quadratic_iter_25.png")
