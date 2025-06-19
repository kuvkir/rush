# Non-Square Board Support

## Overview

Added support for non-square board dimensions in the Rush Hour puzzle generator. The codebase now supports independent width and height configurations, allowing for boards like 5x6, 6x5, etc.

## Key Changes

### 1. Configuration Updates (`config.h`)

- Added separate `BOARD_WIDTH` and `BOARD_HEIGHT` defines
- Maintained `BoardSize` for backward compatibility (will be phased out)
- Updated all board-related constants to use appropriate dimensions:
  - `BoardSize2 = BoardWidth * BoardHeight`
  - `V = BoardWidth` (vertical stride)
  - Row and column masks now properly sized

### 2. Core Algorithm Updates

#### Board Class (`board.cpp`)
- Updated position calculations to use `BoardWidth` for modulo/division operations
- Fixed `String2D()` method to handle non-square boards correctly

#### Enumerator Class (`enumerator.cpp`)
- **Critical fix**: Modified `ComputeGroups()` to use `max(BoardWidth, BoardHeight)`
- This ensures sufficient groups are precomputed for both row and column processing
- Fixed "GroupForPieces failed" exception that occurred with non-square boards

#### Cluster Class (`cluster.cpp`)
- Updated position calculations for proper width/height handling

### 3. Problem Solved

The original implementation assumed square boards throughout. The main issue was in the group computation:
- Groups were precomputed based only on `BoardWidth`
- For 5x6 boards, column processing could need groups up to size 6
- This exceeded the precomputed groups (only up to size 5), causing exceptions

## Test Results

### 5x5 Board (Square - Baseline)
- Input configurations: 268,108
- Canonical puzzles: 130,299
- Solvable puzzles: 62,106
- Minimal puzzles: 1,730
- Generation time: ~0.89 seconds

### 5x6 Board (Non-Square)
- Input configurations: 6,097,031
- Canonical puzzles: 2,519,046
- Solvable puzzles: 1,237,218
- Minimal puzzles: 21,749
- Generation time: ~41.4 seconds

### 6x5 Board (Non-Square - Transposed)
- Successfully tested without errors
- Demonstrates bidirectional width/height independence

## Implementation Details

### Key Code Changes

1. **Position Calculations**:
   ```cpp
   // Old (assumed square):
   const int y = piece.Position() / BoardSize;
   const int x = piece.Position() % BoardSize;
   
   // New (supports non-square):
   const int y = piece.Position() / BoardWidth;
   const int x = piece.Position() % BoardWidth;
   ```

2. **Group Computation Fix**:
   ```cpp
   // Old (limited by width only):
   if (sum >= BoardWidth) {
       return;
   }
   
   // New (handles both dimensions):
   const int maxDimension = std::max(BoardWidth, BoardHeight);
   if (sum >= maxDimension) {
       return;
   }
   ```

3. **Board Constants**:
   ```cpp
   const int BoardSize2 = BoardWidth * BoardHeight;
   const int V = BoardWidth; // vertical stride
   const int Target = PrimaryRow * BoardWidth + BoardWidth - PrimarySize;
   ```

## Usage

To generate puzzles for a specific board size:

1. Edit `src/config.h`:
   ```cpp
   #define BOARD_WIDTH 5
   #define BOARD_HEIGHT 6
   ```

2. Rebuild and run:
   ```bash
   make clean && make
   ./main > rush_database_5x6.txt
   ```

## Future Work

- Add more non-square configurations to the MaxID table
- Consider removing `BoardSize` completely after thorough testing
- Optimize group computation for specific width/height combinations