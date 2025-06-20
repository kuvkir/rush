#pragma once

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "bb.h"
#include "board_config.h"
#include "move.h"
#include "piece.h"

typedef std::tuple<bb, bb> BoardKey;

class Board {
public:
    Board();  // Uses default 6x6
    explicit Board(const BoardConfig& config);
    explicit Board(const std::string& desc);  // Auto-detects size from string

    bb Mask() const {
        return m_HorzMask | m_VertMask;
    }

    bb HorzMask() const {
        return m_HorzMask;
    }

    bb VertMask() const {
        return m_VertMask;
    }

    BoardKey Key() const {
        return std::make_tuple(m_HorzMask, m_VertMask);
    }

    const std::vector<Piece> &Pieces() const {
        return m_Pieces;
    }
    
    const BoardConfig& Config() const {
        return m_config;
    }

    bool Solved() const {
        return m_Pieces[0].Position() == m_config.target;
    }

    void AddPiece(const Piece &piece);
    void PopPiece();
    void RemovePiece(const int i);

    void DoMove(const int piece, const int steps);
    void DoMove(const Move &move);
    void UndoMove(const Move &move);

    void Moves(std::vector<Move> &moves) const;

    std::string String() const;
    std::string String2D() const;

private:
    BoardConfig m_config;
    bb m_HorzMask;
    bb m_VertMask;
    std::vector<Piece> m_Pieces;
    
    // Dynamic mask getters
    bb TopRow() const;
    bb BottomRow() const;
    bb LeftColumn() const;
    bb RightColumn() const;
};

std::ostream& operator<<(std::ostream &stream, const Board &board);

bool operator<(const Board &b1, const Board &b2);
