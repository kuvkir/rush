# Runtime-Configurable Board Dimensions Implementation

## Overview
This document describes the transformation of the Rush Hour puzzle generator from compile-time fixed board dimensions to runtime-configurable dimensions. The implementation now supports multiple board sizes (5x5, 5x6, 6x6, 6x7, 7x7) without recompilation.

## Key Changes

### 1. BoardConfig Structure (`src/board_config.h`)
Created a new runtime configuration struct that encapsulates all board-related parameters:
- `width`, `height`: Board dimensions
- `primaryRow`: Exit row (2 for height < 7, 3 for height = 7)
- `primarySize`: Always 2 for the primary piece
- `target`: Target position for solving
- `H`, `V`: Horizontal and vertical strides
- Factory method `fromString()` to auto-detect dimensions from board string length

### 2. Board Class Updates (`src/board.h`, `src/board.cpp`)
- Replaced compile-time constants with runtime BoardConfig
- Added dynamic mask generation methods for board edges (TopRow, BottomRow, LeftColumn, RightColumn)
- Constructors now accept BoardConfig or auto-detect from string length
- All bitboard operations updated to use runtime dimensions

### 3. Enumerator Complete Rewrite (`src/enumerator.h`, `src/enumerator.cpp`)
The most significant change - complete reimplementation to support runtime dimensions:
- Dynamic group computation based on actual board dimensions
- Position entries computed at runtime for each board size
- Primary row handling adapted for configurable exit rows
- Row and column enumeration algorithms updated for variable dimensions

### 4. Solver and Cluster Updates
- Modified to accept BoardConfig instead of using compile-time constants
- Target position now retrieved from `board.Config().target`
- All dimension-dependent logic updated

### 5. Main Program Updates (`src/main.cpp`)
- Added command-line argument parsing for board size selection
- MaxID values retrieved from BoardConfig based on board size
- Worker thread count increased from 4 to 10 for better performance
- Progress tracking uses runtime MaxID values

### 6. Tool Updates
- `check_solvable.cpp`: Updated to auto-detect board dimensions from input string

## Difficulties Solved

### 1. Bitboard Size Constraints
**Problem**: Different board sizes require different bitboard representations, but the code assumed fixed 6x6 (36 bits).
**Solution**: Used uint64_t throughout, supporting up to 64 squares (sufficient for 7x7 = 49 squares).

### 2. Enumerator Group Computation
**Problem**: The original enumerator computed groups based on fixed dimensions. With runtime dimensions, groups needed to be computed dynamically.
**Solution**: Rewrote ComputeGroups to use runtime max dimension, ensuring sufficient groups are generated for any supported board size.

### 3. GroupForPieces Exception
**Problem**: Initial implementation failed with "GroupForPieces failed" exception due to mismatched group computation.
**Solution**: Fixed group computation logic to properly handle empty pieces and ensure all piece combinations are valid groups.

### 4. Primary Row Configuration
**Problem**: Exit row was hardcoded to 2, but 6x7 boards require row 3.
**Solution**: Made primaryRow configurable based on board height (row 2 for height < 7, row 3 for height = 7).

### 5. MaxID Runtime Values
**Problem**: MaxID was a compile-time constant used for progress tracking, but different board sizes have vastly different search spaces.
**Solution**: Added getMaxID() method to BoardConfig with known values for each board size.

### 6. Database/Log Separation
**Problem**: Progress logs were mixed with puzzle database entries in output files.
**Solution**: Redirected stderr to separate .log files, keeping database entries clean in stdout.

### 7. Undefined Variable Errors
**Problem**: Refactoring introduced undefined variables (n, Target) in various places.
**Solution**: Carefully updated all references to use runtime values from BoardConfig.

## Performance Considerations

- Runtime configuration adds minimal overhead compared to compile-time constants
- Dynamic mask generation is performed once per board instance
- Bitboard operations remain efficient with uint64_t
- Multi-threading scales well with 10 worker threads

## Usage Examples

```bash
# Run with default 6x6 board
./main

# Run with specific board size
./main 5x5
./main 5x6
./main 6x6
./main 6x7
./main 7x7

# Check solvability (auto-detects size from string length)
./check_solvable "ooooooooooooAAoooooooooooooooooooooo"  # 36 chars = 6x6
./check_solvable "oooooooooAAooooooooooooo"              # 25 chars = 5x5
```

## Testing Results

- 5x5: Successfully generates puzzles with exit row 2
- 5x6: Successfully generates puzzles with exit row 2  
- 6x6: Successfully generates puzzles with exit row 2 (matches original)
- 6x7: Successfully generates puzzles with exit row 3
- 7x7: Supported but not extensively tested due to large search space

## Future Enhancements

1. Support for arbitrary board dimensions (not just predefined sizes)
2. Configurable exit positions (not just rightmost position of exit row)
3. Runtime-configurable piece size ranges
4. Dynamic MaxID estimation for unknown board sizes