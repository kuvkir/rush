#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

# Parse the database file
moves_dist = defaultdict(int)
non_empty_cells_dist = defaultdict(int)
num_blocks_dist = defaultdict(int)

line_count = 0
with open('cpp/rush_database_no_walls.txt', 'r') as f:
    for line in f:
        line_count += 1
        parts = line.strip().split()
        if len(parts) < 3:
            continue
            
        try:
            moves = int(parts[0])
            board = parts[1]
            
            # Count non-empty cells (anything that's not '.')
            non_empty = sum(1 for c in board if c != '.')
            
            # Count unique blocks (letters)
            unique_blocks = len(set(c for c in board if c != '.'))
            
            # Update distributions
            moves_dist[moves] += 1
            non_empty_cells_dist[non_empty] += 1
            num_blocks_dist[unique_blocks] += 1
        except:
            print(f"Error parsing line {line_count}: {line.strip()}")
            continue

print(f"Total lines processed: {line_count}")
print(f"Total valid puzzles: {sum(moves_dist.values())}")

# Create figure with 3 subplots
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12))

# Plot 1: Distribution of moves required to solve
moves_keys = sorted(moves_dist.keys())
moves_values = [moves_dist[k] for k in moves_keys]
ax1.bar(moves_keys, moves_values, color='steelblue', edgecolor='black', linewidth=0.5)
ax1.set_xlabel('Number of Moves Required', fontsize=12)
ax1.set_ylabel('Number of Puzzles', fontsize=12)
ax1.set_title('Distribution of Moves Required to Solve', fontsize=14, fontweight='bold')
ax1.grid(True, alpha=0.3)

# Add statistics
total_puzzles = sum(moves_values)
avg_moves = sum(k * v for k, v in moves_dist.items()) / total_puzzles if total_puzzles > 0 else 0
ax1.text(0.02, 0.95, f'Total puzzles: {total_puzzles:,}\nAverage moves: {avg_moves:.1f}', 
         transform=ax1.transAxes, verticalalignment='top',
         bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

# Plot 2: Distribution of non-empty cells
cells_keys = sorted(non_empty_cells_dist.keys()) if non_empty_cells_dist else [0]
cells_values = [non_empty_cells_dist[k] for k in cells_keys]
ax2.bar(cells_keys, cells_values, color='forestgreen', edgecolor='black', linewidth=0.5)
ax2.set_xlabel('Number of Non-Empty Cells', fontsize=12)
ax2.set_ylabel('Number of Puzzles', fontsize=12)
ax2.set_title('Distribution of Board Density (Non-Empty Cells)', fontsize=14, fontweight='bold')
ax2.grid(True, alpha=0.3)

# Add percentage info
if non_empty_cells_dist:
    ax2.text(0.02, 0.95, f'Board size: 6x6 (36 cells)\nRange: {min(cells_keys)}-{max(cells_keys)} cells occupied', 
         transform=ax2.transAxes, verticalalignment='top',
         bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

# Plot 3: Distribution of number of blocks
blocks_keys = sorted(num_blocks_dist.keys()) if num_blocks_dist else [0]
blocks_values = [num_blocks_dist[k] for k in blocks_keys]
ax3.bar(blocks_keys, blocks_values, color='coral', edgecolor='black', linewidth=0.5)
ax3.set_xlabel('Number of Blocks/Pieces', fontsize=12)
ax3.set_ylabel('Number of Puzzles', fontsize=12)
ax3.set_title('Distribution of Number of Blocks on Board', fontsize=14, fontweight='bold')
ax3.grid(True, alpha=0.3)

# Add statistics
avg_blocks = sum(k * v for k, v in num_blocks_dist.items()) / total_puzzles if total_puzzles > 0 else 0
if num_blocks_dist:
    ax3.text(0.02, 0.95, f'Average blocks per puzzle: {avg_blocks:.1f}\nRange: {min(blocks_keys)}-{max(blocks_keys)} blocks', 
         transform=ax3.transAxes, verticalalignment='top',
         bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

plt.tight_layout()
plt.savefig('rush_database_distributions.png', dpi=300, bbox_inches='tight')
print(f"Graph saved as 'rush_database_distributions.png'")

# Print summary statistics
print("\nSummary Statistics:")
print(f"Total puzzles analyzed: {total_puzzles:,}")
print(f"\nMoves distribution:")
for moves in sorted(moves_dist.keys())[:20]:  # Show first 20
    print(f"  {moves} moves: {moves_dist[moves]:,} puzzles ({moves_dist[moves]/total_puzzles*100:.1f}%)")
if len(moves_dist) > 20:
    print(f"  ... and {len(moves_dist)-20} more move counts")

print(f"\nBoard density (non-empty cells):")
for cells in sorted(non_empty_cells_dist.keys()):
    pct_filled = cells / 36 * 100
    print(f"  {cells} cells ({pct_filled:.0f}%): {non_empty_cells_dist[cells]:,} puzzles")

print(f"\nNumber of blocks:")
for blocks in sorted(num_blocks_dist.keys()):
    print(f"  {blocks} blocks: {num_blocks_dist[blocks]:,} puzzles")