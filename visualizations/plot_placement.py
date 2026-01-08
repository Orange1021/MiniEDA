#!/usr/bin/env python3
"""
Plot placement visualization from CSV data
Usage: python3 plot_placement.py <csv_file> [output_file] [--title "Custom Title"]
"""

import sys
import os
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import argparse

def plot_placement(csv_file, output_file=None, title=None):
    """Read placement data from CSV and create visualization"""
    
    cells = []
    
    try:
        with open(csv_file, 'r') as f:
            # Skip header line
            next(f)
            
            for line in f:
                line = line.strip()
                if not line:
                    continue
                    
                parts = line.split(',')
                if len(parts) >= 6:
                    cells.append({
                        'cell_name': parts[0],
                        'x': float(parts[1]),
                        'y': float(parts[2]),
                        'width': float(parts[3]),
                        'height': float(parts[4]),
                        'fixed': parts[5].lower() == 'true'
                    })
                
    except Exception as e:
        print(f"Error reading CSV file: {e}")
        return
    
    # Sort cells: fixed cells first, then by name for consistency
    cells.sort(key=lambda c: (not c['fixed'], c['cell_name']))
    
    # Calculate core area bounds
    if cells:
        x_min = min(cell['x'] for cell in cells)
        y_min = min(cell['y'] for cell in cells)
        x_max = max(cell['x'] + cell['width'] for cell in cells)
        y_max = max(cell['y'] + cell['height'] for cell in cells)
        
        # Add some padding
        padding = max((x_max - x_min), (y_max - y_min)) * 0.05
        x_min -= padding
        x_max += padding
        y_min -= padding
        y_max += padding
    else:
        print("Warning: No cells found in CSV")
        return
    
    # Create figure
    fig, ax = plt.subplots(1, 1, figsize=(12, 10))
    
    # Draw core area boundary
    core_rect = patches.Rectangle((x_min, y_min), x_max - x_min, y_max - y_min,
                                 linewidth=2, edgecolor='black', facecolor='none')
    ax.add_patch(core_rect)
    
    # Draw cells
    movable_cells = 0
    fixed_cells = 0
    
    for cell in cells:
        # Color based on whether cell is fixed
        if cell['fixed']:
            facecolor = 'lightcoral'
            edgecolor = 'red'
            fixed_cells += 1
        else:
            facecolor = 'lightblue'
            edgecolor = 'blue'
            movable_cells += 1
        
        # Draw cell rectangle
        cell_rect = patches.Rectangle(
            (cell['x'], cell['y']),
            cell['width'], cell['height'],
            linewidth=1,
            edgecolor=edgecolor,
            facecolor=facecolor,
            alpha=0.6
        )
        ax.add_patch(cell_rect)
        
        # Add cell name label for all cells
        # Use same font size as plot_routing.py
        ax.text(cell['x'] + cell['width']/2,
               cell['y'] + cell['height']/2,
               cell['cell_name'],
               ha='center', va='center',
               fontsize=6, alpha=0.7)
    
    # Set plot properties
    ax.set_aspect('equal')
    ax.set_xlim(x_min, x_max)
    ax.set_ylim(y_min, y_max)
    ax.set_xlabel('X (micrometers)')
    ax.set_ylabel('Y (micrometers)')
    
    # Set title
    if title:
        ax.set_title(title)
    else:
        # Extract title from filename
        base_name = os.path.basename(csv_file).replace('.csv', '')
        ax.set_title(f'MiniPlacement - {base_name}')
    
    ax.grid(True, alpha=0.3)
    
    # Add legend
    legend_elements = [
        patches.Rectangle((0, 0), 1, 1, facecolor='lightblue', alpha=0.6,
                         edgecolor='blue', label=f'Movable Cells ({movable_cells})'),
        patches.Rectangle((0, 0), 1, 1, facecolor='lightcoral', alpha=0.6,
                         edgecolor='red', label=f'Fixed Cells ({fixed_cells})')
    ]
    ax.legend(handles=legend_elements, loc='upper right')
    
    # Determine output file
    if output_file is None:
        # Default: replace .csv with .png
        output_file = csv_file.replace('.csv', '.png')
    
    # Save plot
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"Visualization saved as: {output_file}")
    print(f"  Total cells: {len(cells)} (movable: {movable_cells}, fixed: {fixed_cells})")

def main():
    parser = argparse.ArgumentParser(description='Plot placement visualization from CSV')
    parser.add_argument('csv_file', help='Input CSV file with placement data')
    parser.add_argument('output_file', nargs='?', help='Output PNG file (optional)')
    parser.add_argument('--title', help='Plot title (optional)')
    
    args = parser.parse_args()
    
    # Check if input file exists
    if not os.path.exists(args.csv_file):
        print(f"Error: Input file '{args.csv_file}' not found")
        sys.exit(1)
    
    plot_placement(args.csv_file, args.output_file, args.title)

if __name__ == "__main__":
    main()