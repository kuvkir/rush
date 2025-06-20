#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include "src/board.h"
#include "src/solver.h"
#include "src/cluster.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <board_string>" << std::endl;
        std::cerr << "Example: " << argv[0] << " \"BB.C...D.CEE.DAAFGH.IIFGH.JKK.LLJ...\"" << std::endl;
        return 1;
    }
    
    std::string boardString = argv[1];
    
    try {
        Board board(boardString);
        
        std::cout << "Board:\n" << board.String2D() << std::endl;
        
        // Check if already solved
        if (board.Solved()) {
            std::cout << "Board is already in solved position!" << std::endl;
            return 0;
        }
        
        // Use solver to check solvability
        Solver solver;
        
        // Time the solving process
        auto start = std::chrono::high_resolution_clock::now();
        int numMoves = solver.CountMoves(board);
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double milliseconds = duration.count() / 1000.0;
        
        if (numMoves >= 0) {
            std::cout << "Board is SOLVABLE!" << std::endl;
            std::cout << "Minimum moves to solve: " << numMoves << std::endl;
            std::cout << "Time to solve: " << std::fixed << std::setprecision(3) 
                      << milliseconds << " ms" << std::endl;
            
            // Get the actual solution with moves
            auto solution = solver.Solve(board);
            if (solution.NumMoves() > 0) {
                std::cout << "Move sequence:" << std::endl;
                
                Board workingBoard = board;
                int moveNum = 1;
                for (const auto& move : solution.Moves()) {
                    std::cout << moveNum++ << ". Move piece " << (char)('A' + move.Piece()) 
                             << " by " << move.Steps() << " steps" << std::endl;
                    workingBoard.DoMove(move);
                }
                
                std::cout << "\nFinal board:\n" << workingBoard.String2D() << std::endl;
            }
        } else {
            std::cout << "Board is NOT SOLVABLE!" << std::endl;
        }
        
        // Also show cluster information
        Cluster cluster(0, board);
        std::cout << "\nCluster analysis:" << std::endl;
        std::cout << "Canonical: " << (cluster.Canonical() ? "Yes" : "No") << std::endl;
        std::cout << "Minimal: " << (cluster.Minimal() ? "Yes" : "No") << std::endl;
        std::cout << "Total states in cluster: " << cluster.NumStates() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (const char* msg) {
        std::cerr << "Error: " << msg << std::endl;
        return 1;
    }
    
    return 0;
}