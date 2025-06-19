#pragma once

#include <array>

#include "bb.h"

// Board dimensions
#define BOARD_WIDTH 5
#define BOARD_HEIGHT 5
#define BOARD_SIZE 5  // For now, must equal WIDTH and HEIGHT (square boards only)

const int BoardSize = BOARD_SIZE;
const int PrimaryRow = 2;
const int PrimarySize = 2;
const int MinPieceSize = 2;
const int MaxPieceSize = 3;

// Master switch for corner walls experiment
// When enabled, generates puzzles with fixed walls in all 4 corners
#define CORNER_WALLS 0

#if CORNER_WALLS
const int MinWalls = 4;  // Fixed corner walls
const int MaxWalls = 4;  // Only corner walls allowed
#else
const int MinWalls = 0;  // Standard generation
const int MaxWalls = 0;  // No walls
#endif

const int NumWorkers = 4;

// Configuration for corner walls treatment
// When enabled, corner walls are treated as inherent board geometry
// and are exempt from the minimal puzzle check
// Only relevant when CORNER_WALLS is enabled
#define CORNER_WALLS_AS_GEOMETRY 0

// MaxID values for different board sizes:
// 4x4: 1348 (no walls), 9803 (0-1 walls), 33952 (0-2 walls), 76837 (0-3 walls)
// 5x5: 268108 (no walls), 2988669 (0-1 walls), 16330429 (0-2 walls)
// 6x6: 243502785 (no walls), 3670622351 (0-1 walls), 27403231254 (0-2 walls)
// 7x7: 561276504436 (5h42m for no walls)

#if BOARD_SIZE == 5
    #if CORNER_WALLS
    const uint64_t MaxID = 268108; // 5x5 - Will stop early for corner walls
    #else
    const uint64_t MaxID = 268108; // 5x5 no walls
    #endif
#elif BOARD_SIZE == 6
    #if CORNER_WALLS
    const uint64_t MaxID = 243502785; // 6x6 - Will stop early for corner walls
    #else
    const uint64_t MaxID = 243502785; // 6x6 no walls
    #endif
#elif BOARD_SIZE == 4
    const uint64_t MaxID = 1348; // 4x4
#elif BOARD_SIZE == 7
    const uint64_t MaxID = 561276504436; // 7x7 - 5h42m
#else
    #error "Unsupported board size"
#endif

const int BoardSize2 = BoardSize * BoardSize;
const int Target = PrimaryRow * BoardSize + BoardSize - PrimarySize;
const int H = 1; // horizontal stride
const int V = BoardSize; // vertical stride
const bool DoWalls = MinPieceSize == 1;

const std::array<bb, BoardSize> RowMasks = []() {
    std::array<bb, BoardSize> rowMasks;
    for (int y = 0; y < BoardSize; y++) {
        bb mask = 0;
        for (int x = 0; x < BoardSize; x++) {
            const int i = y * BoardSize + x;
            mask |= (bb)1 << i;
        }
        rowMasks[y] = mask;
    }
    return rowMasks;
}();

const std::array<bb, BoardSize> ColumnMasks = []() {
    std::array<bb, BoardSize> columnMasks;
    for (int x = 0; x < BoardSize; x++) {
        bb mask = 0;
        for (int y = 0; y < BoardSize; y++) {
            const int i = y * BoardSize + x;
            mask |= (bb)1 << i;
        }
        columnMasks[x] = mask;
    }
    return columnMasks;
}();

const bb TopRow = RowMasks.front();
const bb BottomRow = RowMasks.back();
const bb LeftColumn = ColumnMasks.front();
const bb RightColumn = ColumnMasks.back();
