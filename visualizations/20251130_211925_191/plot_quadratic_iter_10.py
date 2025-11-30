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
cell_rect = patches.Rectangle((6.27281, 5.55865), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.55781, 6.25865, 'NOR2_3', ha='center', va='center', fontsize=8)

# Cell: NOR2_2
cell_rect = patches.Rectangle((5.89943, 5.30624), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.18443, 6.00624, 'NOR2_2', ha='center', va='center', fontsize=8)

# Cell: NOR2_1
cell_rect = patches.Rectangle((5.42676, 5.85502), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.71176, 6.55502, 'NOR2_1', ha='center', va='center', fontsize=8)

# Cell: NOR2_0
cell_rect = patches.Rectangle((5.35733, 5.65911), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.64233, 6.35911, 'NOR2_0', ha='center', va='center', fontsize=8)

# Cell: NAND2_0
cell_rect = patches.Rectangle((5.58081, 5.49955), 0.57, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.86581, 6.19955, 'NAND2_0', ha='center', va='center', fontsize=8)

# Cell: OR2_1
cell_rect = patches.Rectangle((5.44546, 5.74495), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.82546, 6.44495, 'OR2_1', ha='center', va='center', fontsize=8)

# Cell: OR2_0
cell_rect = patches.Rectangle((5.61423, 5.65827), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.99423, 6.35827, 'OR2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_0
cell_rect = patches.Rectangle((4.2479, 5.55506), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.8629, 6.25506, 'DFF_0', ha='center', va='center', fontsize=8)

# Cell: NOT_0
cell_rect = patches.Rectangle((5.25042, 5.94613), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.44042, 6.64613, 'NOT_0', ha='center', va='center', fontsize=8)

# Cell: G0
cell_rect = patches.Rectangle((4.1304, 4.45986), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.71154, 6.041, 'G0', ha='center', va='center', fontsize=8)

# Cell: G3
cell_rect = patches.Rectangle((4.24746, 4.61359), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.82859, 6.19473, 'G3', ha='center', va='center', fontsize=8)

# Cell: G2
cell_rect = patches.Rectangle((4.70529, 4.36403), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.28642, 5.94517, 'G2', ha='center', va='center', fontsize=8)

# Cell: G17
cell_rect = patches.Rectangle((3.85362, 5.0749), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.43476, 6.65604, 'G17', ha='center', va='center', fontsize=8)

# Cell: AND2_0
cell_rect = patches.Rectangle((5.45421, 5.54599), 0.76, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.83421, 6.24599, 'AND2_0', ha='center', va='center', fontsize=8)

# Cell: DFF_1
cell_rect = patches.Rectangle((4.23658, 5.73574), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.85158, 6.43574, 'DFF_1', ha='center', va='center', fontsize=8)

# Cell: VDD
cell_rect = patches.Rectangle((7.79123, 5.40599), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(9.37237, 6.98713, 'VDD', ha='center', va='center', fontsize=8)

# Cell: CK
cell_rect = patches.Rectangle((4.38257, 4.74011), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.96371, 6.32125, 'CK', ha='center', va='center', fontsize=8)

# Cell: DFF_2
cell_rect = patches.Rectangle((4.61176, 5.75213), 3.23, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.22676, 6.45213, 'DFF_2', ha='center', va='center', fontsize=8)

# Cell: G1
cell_rect = patches.Rectangle((4.93342, 4.89504), 3.16228, 3.16228, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(6.51455, 6.47618, 'G1', ha='center', va='center', fontsize=8)

# Cell: NOT_1
cell_rect = patches.Rectangle((5.2704, 5.65677), 0.38, 1.4, linewidth=1, edgecolor='blue', facecolor='lightblue', alpha=0.7)
ax.add_patch(cell_rect)
ax.text(5.4604, 6.35677, 'NOT_1', ha='center', va='center', fontsize=8)

# Draw nets as fly-lines
net_colors = []
# Set plot properties
ax.set_aspect('equal')
ax.set_xlim(-10, 21.4525)
ax.set_ylim(-10, 22)
ax.set_xlabel('X (micrometers)')
ax.set_ylabel('Y (micrometers)')
ax.set_title('MiniPlacement - visualizations/20251130_211925_191/quadratic_iter_10.png')
ax.grid(True, alpha=0.3)

# Save the plot
plt.tight_layout()
plt.savefig('visualizations/20251130_211925_191/quadratic_iter_10.png', dpi=150, bbox_inches='tight')
plt.close()

print("Plot saved as: visualizations/20251130_211925_191/quadratic_iter_10.png")
