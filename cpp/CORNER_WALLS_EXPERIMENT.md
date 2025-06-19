# Corner Walls Experiment

## Overview

This experiment adds fixed walls in all four corners of the Rush Hour board during puzzle generation. The goal was to explore how fixed obstacles affect the puzzle space and create interesting variants with guaranteed corner obstructions.

## Key Findings

Out of 6,329,234 board configurations evaluated, only **11 puzzles** met all criteria (canonical, solvable, and minimal) with 4 corner walls. This severe constraint demonstrates how fixed walls dramatically limit the viable puzzle space.

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

## Results

The 11 puzzles found range from 11 to 20 moves in difficulty. Example puzzle:

```
x . . . . x 
E B B G . H 
E A A G . H 
C C F G . . 
. . F D D . 
x . . . . x 
```

- `x` = fixed corner walls
- `AA` = red car (must reach right edge of row 2)
- Other letters = blocking pieces
- Requires 12 moves to solve

## Lessons Learned

1. **Piece ordering matters**: The codebase makes assumptions about piece indices that must be respected
2. **Fixed obstacles severely constrain puzzles**: 4 corner walls reduced viable puzzles by ~99.9%
3. **Canonical representation is complex**: With pre-placed walls, many configurations become non-canonical because moves can lead to "smaller" board states

## How to Run

```bash
cd cpp
make clean && make
./main > corner_walls_output.txt 2> progress.log
```

The generation takes about 28 minutes on a modern machine and produces a complete database of all valid corner wall puzzles.