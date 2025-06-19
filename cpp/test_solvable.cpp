#include <iostream>
#include "board.h"
#include "enumerator.h"
#include "cluster.h"
#include "config.h"

int main() {
    std::cout << "Testing which boards are solvable with corner walls..." << std::endl;
    
    Enumerator enumerator;
    int totalCount = 0;
    int solvableCount = 0;
    int canonicalCount = 0;
    int minimalCount = 0;
    int validCount = 0;
    
    enumerator.Enumerate([&](uint64_t id, const Board &board) {
        totalCount++;
        
        if (totalCount > 1000) {
            return; // Check first 1000 boards
        }
        
        Cluster cluster(id, board);
        
        bool canonical = cluster.Canonical();
        bool solvable = cluster.Solvable();
        bool minimal = cluster.Minimal();
        
        if (canonical) canonicalCount++;
        if (solvable) solvableCount++;
        if (minimal) minimalCount++;
        if (canonical && solvable && minimal) {
            validCount++;
            if (validCount <= 5) {
                std::cout << "\nValid puzzle found! ID: " << id << std::endl;
                std::cout << "Moves needed: " << cluster.NumMoves() << std::endl;
                std::cout << "Board:\n" << board.String2D() << std::endl;
            }
        }
    });
    
    std::cout << "\nResults from first " << totalCount << " boards:\n";
    std::cout << "Total boards: " << totalCount << std::endl;
    std::cout << "Canonical: " << canonicalCount << std::endl;
    std::cout << "Solvable: " << solvableCount << std::endl;
    std::cout << "Minimal: " << minimalCount << std::endl;
    std::cout << "Valid (all 3): " << validCount << std::endl;
    
    return 0;
}