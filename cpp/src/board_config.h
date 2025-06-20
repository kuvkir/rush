#pragma once

#include <array>
#include <stdexcept>
#include <string>
#include "bb.h"

// Runtime board configuration
struct BoardConfig {
    int width;
    int height;
    int size2;           // width * height
    int primaryRow;      // Exit row for primary piece
    int primarySize;     // Size of primary piece (always 2)
    int target;          // Target position for primary piece
    int minPieceSize;    // Minimum piece size (always 2)
    int maxPieceSize;    // Maximum piece size (always 3)
    
    // Strides for movement
    int H;  // Horizontal stride (always 1)
    int V;  // Vertical stride (width)
    
    // Constructor from dimensions
    BoardConfig(int w, int h) 
        : width(w)
        , height(h)
        , size2(w * h)
        , primaryRow(h < 7 ? 2 : 3)  // Row 2 for height < 7, row 3 for height = 7
        , primarySize(2)
        , target(primaryRow * width + width - primarySize)
        , minPieceSize(2)
        , maxPieceSize(3)
        , H(1)
        , V(w)
    {
        // Validate board size
        if (size2 > 64) {
            throw std::runtime_error("Board size exceeds 64 squares (bitboard limit)");
        }
        
        // Validate supported sizes
        if (!((w == 5 && h == 5) || (w == 5 && h == 6) || 
              (w == 6 && h == 6) || (w == 6 && h == 7) || 
              (w == 7 && h == 7))) {
            throw std::runtime_error("Unsupported board dimensions");
        }
    }
    
    // Factory method from board string
    static BoardConfig fromString(const std::string& boardStr) {
        int len = boardStr.length();
        switch(len) {
            case 25: return BoardConfig(5, 5);  // 5x5
            case 30: return BoardConfig(5, 6);  // 5x6
            case 36: return BoardConfig(6, 6);  // 6x6
            case 42: return BoardConfig(6, 7);  // 6x7
            case 49: return BoardConfig(7, 7);  // 7x7
            default:
                throw std::runtime_error("Unknown board size: " + std::to_string(len));
        }
    }
    
    // Generate row masks dynamically
    std::vector<bb> generateRowMasks() const {
        std::vector<bb> masks(height);
        for (int y = 0; y < height; y++) {
            bb mask = 0;
            for (int x = 0; x < width; x++) {
                mask |= (bb)1 << (y * width + x);
            }
            masks[y] = mask;
        }
        return masks;
    }
    
    // Generate column masks dynamically
    std::vector<bb> generateColumnMasks() const {
        std::vector<bb> masks(width);
        for (int x = 0; x < width; x++) {
            bb mask = 0;
            for (int y = 0; y < height; y++) {
                mask |= (bb)1 << (y * width + x);
            }
            masks[x] = mask;
        }
        return masks;
    }
    
    // Get MaxID for this board size (approximate values)
    uint64_t getMaxID() const {
        if (width == 5 && height == 5) return 268108;
        if (width == 5 && height == 6) return 6097031;
        if (width == 6 && height == 6) return 243502785;
        if (width == 6 && height == 7) return 10000000000ULL; // ~10 billion estimate
        if (width == 7 && height == 7) return 561276504436ULL;
        return 100000000; // default
    }
};

// Default configuration (6x6)
inline BoardConfig getDefaultConfig() {
    return BoardConfig(6, 6);
}