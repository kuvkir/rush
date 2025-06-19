#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "board.h"
#include "cluster.h"
#include "config.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_database> <output_database>" << std::endl;
        return 1;
    }
    
    std::ifstream input(argv[1]);
    std::ofstream output(argv[2]);
    
    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files" << std::endl;
        return 1;
    }
    
    std::string line;
    int processed = 0;
    int valid = 0;
    
    while (std::getline(input, line)) {
        processed++;
        
        // Parse the line: moves board_string cluster_size distances
        std::istringstream iss(line);
        int moves;
        std::string boardStr;
        int clusterSize;
        
        iss >> moves >> boardStr;
        
        // Check if corners are empty (can place walls)
        if (boardStr[0] != 'o' || boardStr[5] != 'o' || 
            boardStr[30] != 'o' || boardStr[35] != 'o') {
            continue; // Corners not empty, skip
        }
        
        // Replace corners with walls
        boardStr[0] = 'x';
        boardStr[5] = 'x';
        boardStr[30] = 'x';
        boardStr[35] = 'x';
        
        // Replace 'o' with '.' for Board constructor
        for (char& c : boardStr) {
            if (c == 'o') c = '.';
        }
        
        try {
            // Create board and analyze
            Board board(boardStr);
            Cluster cluster(0, board);
            
            // Check if it's still valid
            if (!cluster.Canonical() || !cluster.Solvable() || !cluster.Minimal()) {
                continue;
            }
            
            // Output in same format
            output << cluster.NumMoves() << " " << board.String() << " " 
                   << cluster.NumStates() << " ";
            
            const auto& distances = cluster.DistanceCounts();
            for (int i = 0; i < distances.size(); i++) {
                if (i > 0) output << ",";
                output << distances[i];
            }
            output << std::endl;
            
            valid++;
            
            if (processed % 10000 == 0) {
                std::cerr << "Processed: " << processed 
                          << ", Valid with walls: " << valid << std::endl;
            }
            
        } catch (...) {
            // Invalid board configuration, skip
            continue;
        }
    }
    
    std::cerr << "\nTotal processed: " << processed << std::endl;
    std::cerr << "Valid with corner walls: " << valid << std::endl;
    
    return 0;
}