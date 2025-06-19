#include <iostream>
#include "board.h"
#include "solver.h"
#include "cluster.h"

int main() {
    std::cout << "Testing if simple corner wall puzzle is solvable..." << std::endl;
    
    // Create board with AA starting at the right edge (should be immediately solvable)
    Board board;
    
    // Add corner walls
    board.AddPiece(Piece(0, 1, H));   // Top-left
    board.AddPiece(Piece(5, 1, H));   // Top-right  
    board.AddPiece(Piece(30, 1, H));  // Bottom-left
    board.AddPiece(Piece(35, 1, H));  // Bottom-right
    
    // Add primary piece at target position (should be solved!)
    board.AddPiece(Piece(16, 2, H));  // AA at target
    
    std::cout << "Board at target position:\n" << board.String2D() << std::endl;
    std::cout << "Is solved: " << board.Solved() << std::endl;
    
    // Now test with AA one step away
    Board board2;
    board2.AddPiece(Piece(0, 1, H));   
    board2.AddPiece(Piece(5, 1, H));   
    board2.AddPiece(Piece(30, 1, H));  
    board2.AddPiece(Piece(35, 1, H));  
    board2.AddPiece(Piece(15, 2, H));  // AA one step from target
    
    std::cout << "\nBoard one step from solution:\n" << board2.String2D() << std::endl;
    
    Solver solver;
    auto solution = solver.Solve(board2);
    std::cout << "Moves to solve: " << solution.Moves().size() << std::endl;
    
    // Test cluster analysis
    std::cout << "\nCluster analysis:" << std::endl;
    Cluster cluster(0, board2);
    std::cout << "Canonical: " << cluster.Canonical() << std::endl;
    std::cout << "Solvable: " << cluster.Solvable() << std::endl;
    
    // Let's trace why it's not canonical
    // The issue is that when we explore from board2, we might reach states
    // that are "less than" board2
    
    // Test a board that should definitely be canonical
    Board board3;
    board3.AddPiece(Piece(12, 2, H));  // AA at leftmost position in row 2
    board3.AddPiece(Piece(0, 1, H));   
    board3.AddPiece(Piece(5, 1, H));   
    board3.AddPiece(Piece(30, 1, H));  
    board3.AddPiece(Piece(35, 1, H));  
    
    std::cout << "\nBoard with AA at leftmost:\n" << board3.String2D() << std::endl;
    Cluster cluster3(0, board3);
    std::cout << "Canonical: " << cluster3.Canonical() << std::endl;
    std::cout << "Solvable: " << cluster3.Solvable() << std::endl;
    
    return 0;
}