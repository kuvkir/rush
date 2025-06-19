#pragma once

#include <array>

#include "bb.h"

// Board dimensions
#define BOARD_WIDTH 5
#define BOARD_HEIGHT 6

const int BoardWidth = BOARD_WIDTH;
const int BoardHeight = BOARD_HEIGHT;
const int BoardSize = BoardWidth;  // Keep for compatibility, will phase out
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
// 5x6: 6097031 (no walls, 41.4 seconds)
// 6x6: 243502785 (no walls), 3670622351 (0-1 walls), 27403231254 (0-2 walls)
// 7x7: 561276504436 (5h42m for no walls)

// MaxID selection based on board dimensions
#if BOARD_WIDTH == 5 && BOARD_HEIGHT == 5
    const uint64_t MaxID = 268108; // 5x5 no walls
#elif BOARD_WIDTH == 6 && BOARD_HEIGHT == 6
    const uint64_t MaxID = 243502785; // 6x6 no walls
#elif BOARD_WIDTH == 4 && BOARD_HEIGHT == 4
    const uint64_t MaxID = 1348; // 4x4
#elif BOARD_WIDTH == 7 && BOARD_HEIGHT == 7
    const uint64_t MaxID = 561276504436; // 7x7 - 5h42m
#elif BOARD_WIDTH == 5 && BOARD_HEIGHT == 6
    const uint64_t MaxID = 6097031; // 5x6 no walls (confirmed)
#else
    #warning "Using default MaxID for non-standard board size"
    const uint64_t MaxID = 100000000; // Generic large value
#endif

const int BoardSize2 = BoardWidth * BoardHeight;
const int Target = PrimaryRow * BoardWidth + BoardWidth - PrimarySize;
const int H = 1; // horizontal stride
const int V = BoardWidth; // vertical stride
const bool DoWalls = MinPieceSize == 1;

const std::array<bb, BoardHeight> RowMasks = []() {
    std::array<bb, BoardHeight> rowMasks;
    for (int y = 0; y < BoardHeight; y++) {
        bb mask = 0;
        for (int x = 0; x < BoardWidth; x++) {
            const int i = y * BoardWidth + x;
            mask |= (bb)1 << i;
        }
        rowMasks[y] = mask;
    }
    return rowMasks;
}();

const std::array<bb, BoardWidth> ColumnMasks = []() {
    std::array<bb, BoardWidth> columnMasks;
    for (int x = 0; x < BoardWidth; x++) {
        bb mask = 0;
        for (int y = 0; y < BoardHeight; y++) {
            const int i = y * BoardWidth + x;
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
