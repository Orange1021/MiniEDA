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
    
    print(f"Reading data file: {data_file}")
    line_count = 0
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
            
            line_count += 1
            if line_count % 100000 == 0:
                print(f"  Processed {line_count} lines...")
    
    print(f"Loaded {len(cells)} cells, {len(segments)} segments from {len(nets)} networks")
    
    # Create figure
    print("Creating visualization...")
    fig, ax = plt.subplots(1, 1, figsize=(12, 10))
    
    # Draw cells first (as background)
    if len(cells) > 0:
        print(f"Drawing {len(cells)} cells...")
        cell_patches = []
        for cell in cells:
            cell_rect = patches.Rectangle(
                (cell['x'], cell['y']), 
                cell['width'], cell['height'],
                linewidth=0.5, 
                edgecolor='gray', 
                facecolor='lightgray', 
                alpha=0.6
            )
            cell_patches.append(cell_rect)
        
        # Add all cell patches at once (more efficient)
        for patch in cell_patches:
            ax.add_patch(patch)
        
        # Add cell name label (optional, for small number of cells)
        if len(cells) <= 50:  # Only show labels for small designs
            for cell in cells:
                ax.text(cell['x'] + cell['width']/2, 
                       cell['y'] + cell['height']/2,
                       cell['name'], 
                       ha='center', va='center',
                       fontsize=6, alpha=0.7)
    
    # Define colors for different layers (supports up to 12 layers)
    # Color cycle optimized for visual distinction
    color_cycle = [
        'blue',      # M1
        'red',       # M2
        'green',     # M3
        'orange',    # M4
        'purple',    # M5
        'cyan',      # M6
        'magenta',   # M7
        'brown',     # M8
        'pink',      # M9
        'gray',      # M10
        'olive',     # M11
        'teal'       # M12
    ]
    
    # Plot segments by layer (more efficient)
    print(f"Drawing {len(segments)} segments...")
    layer_segments = {}  # Will be populated dynamically
    via_positions = []
    
    for seg in segments:
        z1 = int(seg['z1'])
        z2 = int(seg['z2'])
        
        # Initialize layer if not exists
        if z1 not in layer_segments:
            layer_segments[z1] = []
        
        layer_segments[z1].append((seg['x1'], seg['y1'], seg['x2'], seg['y2']))
        
        # Collect via positions
        if z1 != z2:
            via_positions.append((seg['x1'], seg['y1']))
    
    # Draw segments by layer
    max_layer = max(layer_segments.keys()) if layer_segments else 0
    for layer, segs in layer_segments.items():
        if segs:
            # Cycle through colors based on layer number
            color = color_cycle[layer % len(color_cycle)]
            # Extract x and y coordinates
            x_coords = []
            y_coords = []
            for seg in segs:
                x_coords.extend([seg[0], seg[2], None])  # None breaks the line
                y_coords.extend([seg[1], seg[3], None])
            ax.plot(x_coords, y_coords, color=color, linewidth=0.5, alpha=0.7)
    
    # Draw vias
    if via_positions:
        print(f"Drawing {len(via_positions)} vias...")
        via_x = [v[0] for v in via_positions]
        via_y = [v[1] for v in via_positions]
        ax.plot(via_x, via_y, 'ko', markersize=2, zorder=5)
    
    # Set plot properties
    ax.set_aspect('equal')
    ax.set_xlabel('X (grid units)')
    ax.set_ylabel('Y (grid units)')
    ax.set_title(f'Routing & Placement Visualization - {len(cells)} Cells, {len(nets)} Networks, {len(segments)} Segments')
    ax.grid(True, alpha=0.3)
    
    # Add legend (dynamic based on actual layers used)
    legend_elements = [
        patches.Rectangle((0, 0), 1, 1, facecolor='lightgray', alpha=0.6, 
                         edgecolor='gray', label='Cells'),
        plt.Line2D([0], [0], marker='o', color='black', markersize=6, 
                  linestyle='None', label='Via')
    ]
    
    # Add legend entries for each layer that has segments
    for layer in sorted(layer_segments.keys()):
        color = color_cycle[layer % len(color_cycle)]
        legend_elements.append(
            plt.Line2D([0], [0], color=color, linewidth=2, label=f'Metal {layer+1}')
        )
    
    ax.legend(handles=legend_elements, loc='upper right')
    
    # Save plot
    output_file = data_file.replace('.txt', '.png')
    print(f"Saving visualization to: {output_file}")
    plt.savefig(output_file, dpi=100, bbox_inches='tight')
    plt.close()
    
    print(f"Visualization saved successfully!")
    print(f"File size: {output_file}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 plot_routing.py <data_file.txt>")
        sys.exit(1)
    
    plot_routing(sys.argv[1])