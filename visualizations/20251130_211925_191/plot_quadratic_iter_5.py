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
cell_rect = patches.Rectangle((6.33531, 4.80556), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.62031, 5.50556, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((6.81271, 6.08349), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.09771, 6.78349, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((5.2425, 5.38729), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.5275, 6.08729, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((4.91774, 5.98358), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.20274, 6.68358, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((5.38194, 6.04265), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.66694, 6.74265, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((5.17352, 5.44468), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.55352, 6.14468, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((5.79319, 5.19401), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.17319, 5.89401, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((4.22891, 6.3115), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.84391, 7.0115, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((5.62427, 4.70491), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.81427, 5.40491, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: G0
cell_rect = patches.Rectangle((3.10689, 5.71181), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(4.68803, 7.29295, 'G0', ha='center', va='center', fontsize=8)

# Cell: G3
cell_rect = patches.Rectangle((4.00513, 4.91643), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.58627, 6.49757, 'G3', ha='center', va='center', fontsize=8)

# Cell: G2
cell_rect = patches.Rectangle((6.22268, 4.20516), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(7.80382, 5.7863, 'G2', ha='center', va='center', fontsize=8)

# Cell: G17
cell_rect = patches.Rectangle((3.29441, 4.65333), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(4.87555, 6.23447, 'G17', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((5.14968, 6.02431), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.52968, 6.72431, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((4.45273, 5.61142), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.06773, 6.31142, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((7.79123, 5.40599), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.37237, 6.98713, 'VDD', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((4.58013, 5.1195), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.16127, 6.70064, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((4.66223, 5.20603), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.27723, 5.90603, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: G1
cell_rect = patches.Rectangle((5.02886, 3.69267), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.61, 5.27381, 'G1', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((4.83437, 6.31531), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.02437, 7.01531, 'NOT_1', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 21.4525)
ax.set_ylim(-10, 22)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251130_211925_191/quadratic_iter_5.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251130_211925_191/quadratic_iter_5.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251130_211925_191/quadratic_iter_5.png")
