#!/usr/bin/env python3
"""
Density Map Visualization Script
Usage: python visualize_density.py <density_csv_file> [output_image]
"""

import matplotlib.pyplot as plt
import numpy as np
import sys
import os

def create_heatmap(csv_file, output_image=None):
    """
    Create a heatmap from density CSV data
    """
    # Read CSV data manually
    try:
        with open(csv_file, 'r') as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"Error: Could not find file {csv_file}")
        return False
    
    # Skip header line
    if len(lines) < 2:
        print("Error: CSV file is empty")
        return False
    
    # Parse data
    data = []
    for line in lines[1:]:
        parts = line.strip().split(',')
        if len(parts) >= 3:
            try:
                x = float(parts[0])
                y = float(parts[1])
                density = float(parts[2])
                data.append((x, y, density))
            except ValueError:
                continue
    
    if not data:
        print("Error: No valid data found in CSV")
        return False
    
    # Get unique x and y coordinates to determine grid size
    x_coords = sorted(set(d[0] for d in data))
    y_coords = sorted(set(d[1] for d in data))
    
    # Create density matrix
    density_matrix = np.zeros((len(y_coords), len(x_coords)))
    
    # Fill matrix with density values
    for x, y, density in data:
        x_idx = x_coords.index(x)
        y_idx = y_coords.index(y)
        density_matrix[y_idx, x_idx] = density
    
    # Create the plot
    plt.figure(figsize=(12, 8))
    
    # Create heatmap
    im = plt.imshow(density_matrix, 
                   extent=[x_coords[0], x_coords[-1], y_coords[0], y_coords[-1]],
                   origin='lower', 
                   cmap='hot', 
                   interpolation='nearest',
                   aspect='auto')
    
    # Add colorbar
    cbar = plt.colorbar(im)
    cbar.set_label('Cell Density', rotation=270, labelpad=20)
    
    # Labels and title
    plt.xlabel('X Coordinate (μm)')
    plt.ylabel('Y Coordinate (μm)')
    plt.title('Placement Density Map')
    
    # Add grid
    plt.grid(True, alpha=0.3)
    
    # Add density statistics as text
    densities = [d[2] for d in data]
    max_density = max(densities)
    avg_density = sum(densities) / len(densities)
    overcrowded = sum(1 for d in densities if d > 0.7)
    total_bins = len(densities)
    
    stats_text = f'Max Density: {max_density:.3f}\nAvg Density: {avg_density:.3f}\nOvercrowded Bins: {overcrowded}/{total_bins} ({100*overcrowded/total_bins:.1f}%)'
    plt.text(0.02, 0.98, stats_text, transform=plt.gca().transAxes, 
             verticalalignment='top', bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
    
    # Save or show
    if output_image:
        plt.savefig(output_image, dpi=150, bbox_inches='tight')
        print(f"Density heatmap saved to {output_image}")
    else:
        plt.show()
    
    return True

def main():
    if len(sys.argv) < 2:
        print("Usage: python visualize_density.py <density_csv_file> [output_image]")
        print("Example: python visualize_density.py test_density_map.csv density_heatmap.png")
        sys.exit(1)
    
    csv_file = sys.argv[1]
    output_image = sys.argv[2] if len(sys.argv) > 2 else None
    
    # Check if file exists
    if not os.path.exists(csv_file):
        print(f"Error: File {csv_file} does not exist")
        sys.exit(1)
    
    # Create visualization
    success = create_heatmap(csv_file, output_image)
    
    if not success:
        sys.exit(1)

if __name__ == "__main__":
    main()