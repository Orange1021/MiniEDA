#!/usr/bin/env python3
"""
Plot routing visualization from exported data
Usage: python3 plot_routing.py <data_file.txt>
"""

import sys
import matplotlib.pyplot as plt
import matplotlib.patches as patches

def plot_routing(data_file):
    """Read routing data and create visualization"""
    
    # Read routing and cell data
    segments = []
    cells = []
    nets = set()
    
    with open(data_file, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('#') or not line:
                continue
            
            parts = line.split()
            
            # Parse cell position data: CELL x y width height cell_name
            if parts[0] == 'CELL' and len(parts) >= 6:
                x, y, width, height = map(float, parts[1:5])
                cell_name = parts[5] if len(parts) > 5 else f"Cell_{len(cells)}"
                cells.append({
                    'x': x, 'y': y, 
                    'width': width, 'height': height,
                    'name': cell_name
                })
            
            # Parse routing data: x1 y1 z1 x2 y2 z2 net_id
            elif len(parts) >= 7:
                x1, y1, z1, x2, y2, z2, net_id = map(float, parts[:7])
                net_id = int(net_id)
                segments.append({
                    'x1': x1, 'y1': y1, 'z1': z1,
                    'x2': x2, 'y2': y2, 'z2': z2,
                    'net_id': net_id
                })
                nets.add(net_id)
    
    print(f"Loaded {len(cells)} cells, {len(segments)} segments from {len(nets)} networks")
    
    # Create figure
    fig, ax = plt.subplots(1, 1, figsize=(12, 10))
    
    # Draw cells first (as background)
    for cell in cells:
        cell_rect = patches.Rectangle(
            (cell['x'], cell['y']), 
            cell['width'], cell['height'],
            linewidth=0.5, 
            edgecolor='gray', 
            facecolor='lightgray', 
            alpha=0.6
        )
        ax.add_patch(cell_rect)
        
        # Add cell name label (optional, for small number of cells)
        if len(cells) <= 50:  # Only show labels for small designs
            ax.text(cell['x'] + cell['width']/2, 
                   cell['y'] + cell['height']/2,
                   cell['name'], 
                   ha='center', va='center',
                   fontsize=6, alpha=0.7)
    
    # Define colors for different layers
    layer_colors = {
        0: 'blue',   # M1 - horizontal
        1: 'red',    # M2 - vertical
        2: 'green'   # M3 - optional
    }
    
    # Plot segments
    for seg in segments:
        color = layer_colors.get(int(seg['z1']), 'black')
        ax.plot([seg['x1'], seg['x2']], [seg['y1'], seg['y2']], 
                color=color, linewidth=1.0, alpha=0.7)
        
        # Add via markers for layer changes
        if seg['z1'] != seg['z2']:
            ax.plot(seg['x1'], seg['y1'], 'ko', markersize=3, zorder=5)
    
    # Set plot properties
    ax.set_aspect('equal')
    ax.set_xlabel('X (grid units)')
    ax.set_ylabel('Y (grid units)')
    ax.set_title(f'Routing & Placement Visualization - {len(cells)} Cells, {len(nets)} Networks, {len(segments)} Segments')
    ax.grid(True, alpha=0.3)
    
    # Add legend
    legend_elements = [
        patches.Rectangle((0, 0), 1, 1, facecolor='lightgray', alpha=0.6, 
                         edgecolor='gray', label='Cells'),
        plt.Line2D([0], [0], color='blue', linewidth=2, label='Metal 1'),
        plt.Line2D([0], [0], color='red', linewidth=2, label='Metal 2'),
        plt.Line2D([0], [0], color='green', linewidth=2, label='Metal 3'),
        plt.Line2D([0], [0], marker='o', color='black', markersize=6, 
                  linestyle='None', label='Via')
    ]
    ax.legend(handles=legend_elements, loc='upper right')
    
    # Save plot
    output_file = data_file.replace('.txt', '.png')
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Visualization saved as: {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 plot_routing.py <data_file.txt>")
        sys.exit(1)
    
    plot_routing(sys.argv[1])