#include <iostream>
#include "board.h"
#include "enumerator.h"
#include "config.h"

int main() {
    std::cout << "Testing corner walls generation..." << std::endl;
    std::cout << "Board size: " << BoardSize << "x" << BoardSize << std::endl;
    std::cout << "Primary row: " << PrimaryRow << std::endl;
    std::cout << "Target position: " << Target << std::endl;
    std::cout << "MinWalls: " << MinWalls << ", MaxWalls: " << MaxWalls << std::endl;
    
    Enumerator enumerator;
    int count = 0;
    bool foundAny = false;
    
    enumerator.Enumerate([&](uint64_t id, const Board &board) {
        count++;
        foundAny = true;
        
        if (count <= 3) {
            std::cout << "\nBoard " << id << ":\n";
            std::cout << board.String2D() << std::endl;
            std::cout << "Linear: " << board.String() << std::endl;
        }
        
        if (count >= 10) {
            std::cout << "Found at least 10 boards, stopping test...\n";
            exit(0);
        }
    });
    
    if (!foundAny) {
        std::cout << "ERROR: No boards were generated!\n";
        std::cout << "This suggests the corner walls are preventing all valid configurations.\n";
    } else {
        std::cout << "Total boards found: " << count << std::endl;
    }
    
    return 0;
}