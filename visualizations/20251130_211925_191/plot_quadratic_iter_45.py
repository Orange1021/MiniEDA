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
cell_rect = patches.Rectangle((5.61371, 5.63046), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89871, 6.33046, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((5.61359, 5.631), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89859, 6.331, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((5.61087, 5.6315), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89587, 6.3315, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((5.6101, 5.63183), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.8951, 6.33183, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((5.61161, 5.63144), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89661, 6.33144, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((5.51671, 5.63126), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89671, 6.33126, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((5.51735, 5.63111), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89735, 6.33111, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((4.28144, 5.6315), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89644, 6.3315, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((5.70485, 5.63183), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89485, 6.33183, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: G0
cell_rect = patches.Rectangle((4.31327, 4.75099), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89441, 6.33213, 'G0', ha='center', va='center', fontsize=8)

# Cell: G3
cell_rect = patches.Rectangle((4.31554, 4.75029), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89668, 6.33143, 'G3', ha='center', va='center', fontsize=8)

# Cell: G2
cell_rect = patches.Rectangle((4.31838, 4.74963), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89952, 6.33077, 'G2', ha='center', va='center', fontsize=8)

# Cell: G17
cell_rect = patches.Rectangle((4.31308, 4.75084), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89422, 6.33197, 'G17', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((5.51606, 5.63157), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89606, 6.33157, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((4.28158, 5.63135), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89658, 6.33135, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((7.79123, 5.40599), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.37237, 6.98713, 'VDD', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((4.31582, 4.75016), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89696, 6.3313, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((4.28258, 5.63089), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89758, 6.33089, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: G1
cell_rect = patches.Rectangle((4.31741, 4.74932), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89855, 6.33046, 'G1', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((5.70459, 5.63209), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.89459, 6.33209, 'NOT_1', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 21.4525)
ax.set_ylim(-10, 22)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251130_211925_191/quadratic_iter_45.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251130_211925_191/quadratic_iter_45.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251130_211925_191/quadratic_iter_45.png")
