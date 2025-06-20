#pragma once

#include <array>

#include "bb.h"

// Runtime configurable values
extern int BoardSize;
extern int PrimaryRow;
extern uint64_t MaxID;

const int PrimarySize = 2;
const int MinPieceSize = 2;
const int MaxPieceSize = 3;
const int MinWalls = 0;
const int MaxWalls = 0;
extern int NumWorkers;

// const uint64_t MaxID = 1348; // 4x4
// const uint64_t MaxID = 9803; // 4x4, 0-1 walls
// const uint64_t MaxID = 33952; // 4x4, 0-2 walls
// const uint64_t MaxID = 76837; // 4x4, 0-3 walls

// const uint64_t MaxID = 268108; // 5x5
// const uint64_t MaxID = 2988669; // 5x5, 0-1 walls
// const uint64_t MaxID = 16330429; // 5x5, 0-2 walls

// const uint64_t MaxID = 243502785; // 6x6
// const uint64_t MaxID = 3670622351; // 6x6, 0-1 walls
// const uint64_t MaxID = 27403231254; // 6x6, 0-2 walls

// const uint64_t MaxID = 561276504436; // 7x7 - 5h42m

extern int BoardSize2;
extern int Target;
const int H = 1; // horizontal stride
extern int V; // vertical stride
const bool DoWalls = MinPieceSize == 1;

extern std::array<bb, 7> RowMasks;
extern std::array<bb, 7> ColumnMasks;

extern bb TopRow;
extern bb BottomRow;
extern bb LeftColumn;
extern bb RightColumn;
