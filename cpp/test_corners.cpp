#include <iostream>
#include "board.h"
#include "enumerator.h"

int main() {
    std::cout << "Testing corner walls generation..." << std::endl;
    
    Enumerator enumerator;
    int count = 0;
    
    enumerator.Enumerate([&](uint64_t id, const Board &board) {
        count++;
        if (count <= 5) {
            std::cout << "Board " << id << ": " << board.String() << std::endl;
            std::cout << "2D view:\n" << board.String2D() << std::endl;
        }
        
        // Skip wall check - just observe the output
    });
    
    std::cout << "Total boards generated: " << count << std::endl;
    return 0;
}