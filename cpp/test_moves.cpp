#include <iostream>
#include <vector>
#include "board.h"
#include "piece.h"
#include "move.h"

int main() {
    std::cout << "Testing simple board with corner walls and red block..." << std::endl;
    
    Board board;
    
    // Add all 4 corner walls
    board.AddPiece(Piece(0, 1, H));                           // Top-left
    board.AddPiece(Piece(5, 1, H));                          // Top-right  
    board.AddPiece(Piece(30, 1, H));                         // Bottom-left
    board.AddPiece(Piece(35, 1, H));                         // Bottom-right
    
    // Add primary piece (AA) at row 2, starting at column 1
    board.AddPiece(Piece(2 * 6 + 1, 2, H));  // Position 13-14
    
    std::cout << "Initial board:\n" << board.String2D() << std::endl;
    std::cout << "Board string: " << board.String() << std::endl;
    std::cout << "HorzMask: " << board.HorzMask() << std::endl;
    std::cout << "VertMask: " << board.VertMask() << std::endl;
    
    // Get possible moves
    std::vector<Move> moves;
    board.Moves(moves);
    
    std::cout << "\nPossible moves: " << moves.size() << std::endl;
    
    // Try each move and check ordering
    for (int i = 0; i < moves.size(); i++) {
        const Move& move = moves[i];
        std::cout << "\nMove " << i << ": piece " << move.Piece() 
                  << " by " << move.Steps() << " steps" << std::endl;
        
        board.DoMove(move);
        std::cout << "After move:\n" << board.String2D() << std::endl;
        std::cout << "HorzMask: " << board.HorzMask() << std::endl;
        
        // Check if this board is "less than" the original
        Board original;
        original.AddPiece(Piece(0, 1, H));
        original.AddPiece(Piece(5, 1, H));
        original.AddPiece(Piece(30, 1, H));
        original.AddPiece(Piece(35, 1, H));
        original.AddPiece(Piece(13, 2, H));
        
        bool less = board < original;
        bool greater = original < board;
        std::cout << "board < original: " << less << std::endl;
        std::cout << "original < board: " << greater << std::endl;
        
        board.UndoMove(move);
    }
    
    // Check if board is solved
    std::cout << "\nIs solved: " << board.Solved() << std::endl;
    std::cout << "Target position: " << Target << std::endl;
    
    return 0;
}