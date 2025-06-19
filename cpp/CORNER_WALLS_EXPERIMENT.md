# Corner Walls Experiment

## Overview

This experiment adds fixed walls in all four corners of the Rush Hour board during puzzle generation. The goal was to explore how fixed obstacles affect the puzzle space and create interesting variants with guaranteed corner obstructions.

## Key Findings

### Initial Incorrect Result: Only 11 Puzzles

Our first implementation found only 11 puzzles out of 6,329,234 configurations. This was incorrect - we weren't exhaustively searching the space.

### Root Cause: Pre-placing Walls

The initial approach pre-placed walls in the `Enumerate()` function before the normal enumeration process:
```cpp
board.AddPiece(Piece(0, 1, H));   // Top-left corner
board.AddPiece(Piece(5, 1, H));   // Top-right corner
board.AddPiece(Piece(30, 1, H));  // Bottom-left corner
board.AddPiece(Piece(35, 1, H));  // Bottom-right corner
```

This approach failed to generate simple configurations like:
- Red car + 4 corner walls only (1-2 move puzzle)
- Red car + 1-2 blocks + 4 corner walls (2-5 move puzzles)

### Correct Implementation: Walls During Enumeration

We modified the enumerator to place walls at corner positions during its normal enumeration process:
- Modified `ComputeRow()` to force wall placement when at corner positions
- Prevented non-corner positions from having walls (skip size 1 pieces)
- Fixed `GroupForPieces()` to handle walls correctly

This exhaustive search found **569 puzzles** that are canonical, solvable, AND minimal.

### The Minimal Check Issue

The Rush Hour generator has a "minimal" requirement: every piece must be necessary for the puzzle. If removing a piece still allows the same solution length, the puzzle isn't minimal.

Corner walls often aren't "necessary" - many puzzles can be solved in the same number of moves without them. This explains why we find relatively few puzzles (569) even with exhaustive search.

## Implementation Details

### Configuration Changes

In `src/config.h`:
```cpp
const int MinWalls = 4;  // Fixed corner walls
const int MaxWalls = 4;  // Only corner walls allowed
```

### The Critical Bug: Piece Ordering

The most challenging issue we encountered was that the code assumes `m_Pieces[0]` is always the primary piece (red car). The `Solved()` check in `board.h` is:

```cpp
bool Solved() const {
    return m_Pieces[0].Position() == Target;
}
```

#### The Problem

Initially, we added walls like this in `enumerator.cpp`:
```cpp
// Add fixed walls first
board.AddPiece(Piece(0, 1, H));      // Top-left
board.AddPiece(Piece(5, 1, H));      // Top-right  
board.AddPiece(Piece(30, 1, H));     // Bottom-left
board.AddPiece(Piece(35, 1, H));     // Bottom-right

// Then enumerate primary row pieces...
```

This caused walls to occupy indices 0-3 in the pieces array, making the red car piece 4. The puzzle could never be "solved" because the code was checking if a wall (piece 0) reached the target position!

#### The Solution

We restructured `PopulatePrimaryRow` to ensure the primary piece is always added first:

```cpp
void Enumerator::PopulatePrimaryRow(
    EnumeratorFunc func, Board &board, uint64_t &id) const
{
    // Save the walls
    Board wallBoard = board;
    
    for (const auto &pe : m_RowEntries[PrimaryRow]) {
        // Start fresh - primary piece must be first
        board = Board();
        
        // Add primary row pieces first (so primary piece is at index 0)
        for (const auto &piece : pe.Pieces()) {
            board.AddPiece(piece);
        }
        
        // Then add the walls
        for (const auto &wallPiece : wallBoard.Pieces()) {
            board.AddPiece(wallPiece);
        }
        
        // Continue with enumeration...
    }
}
```

### Board String Representation

We also fixed the board string functions to properly label walls as 'x' while maintaining correct alphabetic labeling for other pieces:

```cpp
std::string Board::String() const {
    std::string s(BoardSize2, '.');
    int nonWallIndex = 0;
    for (int i = 0; i < m_Pieces.size(); i++) {
        const Piece &piece = m_Pieces[i];
        char c;
        if (piece.Fixed()) {
            c = 'x';  // Walls are always 'x'
        } else {
            c = 'A' + nonWallIndex;  // Other pieces get A, B, C...
            nonWallIndex++;
        }
        // ... rest of function
    }
}
```

## Configuration Flags

We added two configuration flags to control the corner walls experiment:

### CORNER_WALLS Flag

This is the master switch for the corner walls experiment:

```cpp
// In config.h
#define CORNER_WALLS 0  // 0 = standard generation, 1 = corner walls
```

When enabled:
- Sets MinWalls = 4, MaxWalls = 4 (exactly 4 walls)
- Forces walls to be placed at the 4 corner positions during enumeration
- Prevents walls from being placed at non-corner positions

When disabled:
- Sets MinWalls = 0, MaxWalls = 0 (no walls)
- Standard Rush Hour puzzle generation

### CORNER_WALLS_AS_GEOMETRY Flag

This secondary flag controls how corner walls are treated (only relevant when CORNER_WALLS = 1):

```cpp
// In config.h  
#define CORNER_WALLS_AS_GEOMETRY 0  // 0 = strict minimal, 1 = walls as geometry
```

### When CORNER_WALLS_AS_GEOMETRY = 0 (Default)

This is the "puzzle purist" approach:
- Corner walls must be **necessary** for the puzzle
- If removing a corner wall doesn't change the solution length, the puzzle isn't minimal
- Results: **569 puzzles** found
- Philosophically correct: every piece must contribute meaningfully to the puzzle

### When CORNER_WALLS_AS_GEOMETRY = 1

This treats corner walls as inherent board geometry:
- Corner walls are exempt from the minimal check
- They're considered part of the board structure, not removable pieces
- Results: **35,646 puzzles** found
- Includes trivial puzzles like red car + walls only (1 move solution)

### Implementation in cluster.cpp

```cpp
for (int i = 1; i < pieceMoved.size(); i++) {
    if (pieceMoved[i]) {
        continue;
    }
#if CORNER_WALLS_AS_GEOMETRY
    // Skip minimal check for corner walls (treat as board geometry)
    const auto &piece = input.Pieces()[i];
    if (piece.Fixed()) {
        const int pos = piece.Position();
        const int x = pos % BoardSize;
        const int y = pos / BoardSize;
        if ((x == 0 || x == BoardSize - 1) && (y == 0 || y == BoardSize - 1)) {
            // This is a corner wall, skip minimal check
            continue;
        }
    }
#endif
    // ... rest of minimal check
}
```

### Which is More Correct?

From a **puzzle purist perspective**, `CORNER_WALLS_AS_GEOMETRY = 0` is more correct:
- It maintains the principle that every piece must be necessary
- Puzzles are truly minimal - removing ANY piece would make them easier
- This is consistent with how the standard Rush Hour puzzle database was generated

However, `CORNER_WALLS_AS_GEOMETRY = 1` is useful for:
- Game variants where corners are always blocked
- Finding simple tutorial puzzles
- Exploring the full mathematical space of corner wall configurations

## Results

### With Strict Minimal Check (CORNER_WALLS_AS_GEOMETRY = 0)
- **569 puzzles** found
- Simplest puzzle requires 8 moves
- All corner walls are necessary for maintaining difficulty

### With Walls as Geometry (CORNER_WALLS_AS_GEOMETRY = 1)  
- **35,646 puzzles** found
- Simplest puzzle requires just 1 move
- Includes many trivial configurations

Example of simplest puzzle (1 move, only with CORNER_WALLS_AS_GEOMETRY = 1):

```
x . . . . x
. . . . . .
. . A A . .
. . . . . .
. . . . . .
x . . . . x
```

- Just the red car and 4 corner walls
- Red car slides right once to win

Example of a typical minimal puzzle (with CORNER_WALLS_AS_GEOMETRY = 0):

```
x . B B . x 
. . . F . .
. . A A F .
. C C C F .
. . . . E .
x . E D D x
```

- Requires 11 moves to solve
- Every piece (including corner walls) is necessary

## Lessons Learned

1. **Piece ordering matters**: The codebase assumes piece[0] is the primary piece for `Solved()` checks
2. **Pre-placing vs. enumeration**: Pre-placing walls breaks the exhaustive enumeration - walls must be placed during the normal enumeration process
3. **Minimal requirement is strict**: The standard minimal check ensures every piece contributes meaningfully to the puzzle
4. **Board geometry vs. pieces**: There's a philosophical difference between treating obstacles as removable pieces vs. fixed board geometry
5. **Exhaustive search is crucial**: Initial approaches that seemed logical (pre-placing walls) can miss large portions of the search space

## How to Run

### For standard generation (no walls):
```bash
cd cpp
# Set CORNER_WALLS to 0 in config.h
make clean && make
./main > standard_puzzles.txt 2> progress.log
```

### For strict minimal puzzles with corner walls:
```bash
cd cpp
# Set CORNER_WALLS to 1 and CORNER_WALLS_AS_GEOMETRY to 0 in config.h
make clean && make
./main > corner_walls_minimal.txt 2> progress.log
```
- Generates ~569 puzzles in ~9.5 hours

### For all puzzles with corner walls as geometry:
```bash
cd cpp
# Set CORNER_WALLS to 1 and CORNER_WALLS_AS_GEOMETRY to 1 in config.h
make clean && make
./main > corner_walls_geometry.txt 2> progress.log  
```
- Generates ~35,646 puzzles in ~9.5 hours

Both runs evaluate the same 30+ million configurations but apply different minimal criteria.