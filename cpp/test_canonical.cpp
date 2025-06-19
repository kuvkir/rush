#include <iostream>
#include "board.h"
#include "enumerator.h"
#include "cluster.h"
#include "config.h"

int main() {
    std::cout << "Testing canonical check with corner walls..." << std::endl;
    
    // Create a simple board with just AA and corner walls
    Board board;
    
    // Add corner walls first
    board.AddPiece(Piece(BoardSize * (BoardSize - 1), 1, H)); // Bottom-left
    board.AddPiece(Piece(BoardSize * BoardSize - 1, 1, H));   // Bottom-right
    
    // Add primary piece in the middle of row 2
    board.AddPiece(Piece(2 * BoardSize + 2, 2, H)); // AA at row 2, col 2-3
    
    std::cout << "Initial board:\n" << board.String2D() << std::endl;
    std::cout << "Linear: " << board.String() << std::endl;
    
    // Test if this board is canonical
    Cluster cluster(0, board);
    
    std::cout << "\nCluster analysis:" << std::endl;
    std::cout << "Canonical: " << cluster.Canonical() << std::endl;
    std::cout << "Solvable: " << cluster.Solvable() << std::endl;
    std::cout << "Minimal: " << cluster.Minimal() << std::endl;
    std::cout << "Num states: " << cluster.NumStates() << std::endl;
    
    if (cluster.Solvable()) {
        std::cout << "Moves to solve: " << cluster.NumMoves() << std::endl;
    }
    
    // Now let's try adding pieces in different order
    std::cout << "\n\nTrying different piece order..." << std::endl;
    Board board2;
    
    // Add primary piece first
    board2.AddPiece(Piece(2 * BoardSize + 2, 2, H)); // AA at row 2, col 2-3
    
    // Then add walls
    board2.AddPiece(Piece(BoardSize * (BoardSize - 1), 1, H)); // Bottom-left
    board2.AddPiece(Piece(BoardSize * BoardSize - 1, 1, H));   // Bottom-right
    
    std::cout << "Board with different order:\n" << board2.String2D() << std::endl;
    
    // Compare masks
    std::cout << "\nMask comparison:" << std::endl;
    std::cout << "Board1 HorzMask: " << board.HorzMask() << std::endl;
    std::cout << "Board2 HorzMask: " << board2.HorzMask() << std::endl;
    std::cout << "Board1 < Board2: " << (board < board2) << std::endl;
    std::cout << "Board2 < Board1: " << (board2 < board) << std::endl;
    
    return 0;
}