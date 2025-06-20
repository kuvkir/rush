#include "enumerator.h"

#include <algorithm>
#include <cmath>

PositionEntry::PositionEntry(const int group, const std::vector<Piece> &pieces, const BoardConfig& config) :
    m_Group(group),
    m_Pieces(pieces),
    m_Mask(0),
    m_Require(0)
{
    bb movableMask = 0;
    for (const auto &piece : pieces) {
        m_Mask |= piece.Mask();
        if (!piece.Fixed()) {
            movableMask |= piece.Mask();
        }
    }
    if (!pieces.empty()) {
        const int stride = pieces[0].Stride();
        if (stride == config.H) {
            // Generate right column mask dynamically
            bb rightColumn = 0;
            for (int y = 0; y < config.height; y++) {
                rightColumn |= (bb)1 << (y * config.width + config.width - 1);
            }
            m_Require = (movableMask >> stride) & ~m_Mask & ~rightColumn;
        } else {
            m_Require = (movableMask >> stride) & ~m_Mask;
        }
    }
}

Enumerator::Enumerator(const BoardConfig& config) : m_config(config) {
    m_RowEntries.resize(m_config.height);
    m_ColumnEntries.resize(m_config.width);
    
    std::vector<int> sizes;
    ComputeGroups(sizes, 0);
    
    ComputePositionEntries();
}

void Enumerator::Enumerate(EnumeratorFunc func) {
    Board board(m_config);
    uint64_t id = 0;
    PopulatePrimaryRow(func, board, id);
}

void Enumerator::PopulatePrimaryRow(
    EnumeratorFunc func, Board &board, uint64_t &id) const
{
    for (const auto &pe : m_RowEntries[m_config.primaryRow]) {
        for (const auto &piece : pe.Pieces()) {
            board.AddPiece(piece);
        }
        PopulateRow(func, board, id, 0, pe.Mask(), pe.Require());
        for (int i = 0; i < pe.Pieces().size(); i++) {
            board.PopPiece();
        }
    }
}

void Enumerator::PopulateRow(
    EnumeratorFunc func, Board &board, uint64_t &id, int y,
    bb mask, bb require) const
{
    // Skip wall handling for now (DoWalls is always false in current config)
    if (y >= m_config.height) {
        PopulateColumn(func, board, id, 0, mask, require);
        return;
    }
    if (y == m_config.primaryRow) {
        PopulateRow(func, board, id, y + 1, mask, require);
        return;
    }
    
    // Try placing nothing in this row if allowed
    if (m_RowEntries[y].empty()) {
        PopulateRow(func, board, id, y + 1, mask, require);
        return;
    }
    
    for (const auto &pe : m_RowEntries[y]) {
        if ((mask & pe.Mask()) != 0) {
            continue;
        }
        for (const auto &piece : pe.Pieces()) {
            board.AddPiece(piece);
        }
        PopulateRow(
            func, board, id, y + 1,
            mask | pe.Mask(), require | pe.Require());
        for (int i = 0; i < pe.Pieces().size(); i++) {
            board.PopPiece();
        }
    }
}

void Enumerator::PopulateColumn(
    EnumeratorFunc func, Board &board, uint64_t &id, int x,
    bb mask, bb require) const
{
    if (x >= m_config.width) {
        func(id, board);
        id++;
        return;
    }
    for (const auto &pe : m_ColumnEntries[x]) {
        if ((mask & pe.Mask()) != 0) {
            continue;
        }
        if ((mask & pe.Require()) != pe.Require()) {
            continue;
        }
        // Generate column mask dynamically
        bb columnMask = 0;
        for (int y = 0; y < m_config.height; y++) {
            columnMask |= (bb)1 << (y * m_config.width + x);
        }
        const bb columnRequire = require & columnMask;
        if ((pe.Mask() & columnRequire) != columnRequire) {
            continue;
        }
        for (const auto &piece : pe.Pieces()) {
            board.AddPiece(piece);
        }
        PopulateColumn(
            func, board, id, x + 1,
            mask | pe.Mask(), require | pe.Require());
        for (int i = 0; i < pe.Pieces().size(); i++) {
            board.PopPiece();
        }
    }
}

void Enumerator::ComputeGroups(std::vector<int> &sizes, int sum) {
    m_Groups.push_back(sizes);
    // Use the maximum dimension to ensure we compute enough groups
    const int maxDimension = std::max(m_config.width, m_config.height);
    if (sum >= maxDimension) {
        return;
    }
    for (int s = m_config.minPieceSize; s <= m_config.maxPieceSize; s++) {
        sizes.push_back(s);
        ComputeGroups(sizes, sum + s);
        sizes.pop_back();
    }
}

int Enumerator::GroupForPieces(const std::vector<Piece> &pieces) {
    // Extract non-wall piece sizes
    std::vector<int> sizes;
    for (const auto &piece : pieces) {
        if (!piece.Fixed()) {
            sizes.push_back(piece.Size());
        }
    }
    std::sort(sizes.begin(), sizes.end());
    
    // Find group index
    for (int i = 0; i < m_Groups.size(); i++) {
        if (m_Groups[i] == sizes) {
            return i;
        }
    }
    
    
    throw "GroupForPieces failed";
}

void Enumerator::ComputeRow(int y, int x, std::vector<Piece> &pieces) {
    if (x >= m_config.width) {
        const int group = pieces.empty() ? 0 : GroupForPieces(pieces);
        m_RowEntries[y].emplace_back(PositionEntry(group, pieces, m_config));
        return;
    }
    
    // Try placing a wall (removed for simplicity)
    
    // Try not placing a piece
    ComputeRow(y, x + 1, pieces);
    
    // Skip primary row - it's handled separately
    if (y == m_config.primaryRow) {
        return;
    }
    
    // Try placing a piece
    const int n = m_config.width - x;
    for (int s = m_config.minPieceSize; s <= m_config.maxPieceSize; s++) {
        if (s > n) {
            break;
        }
        if (x + s > m_config.width) {
            break;
        }
        const int p = y * m_config.width + x;
        pieces.emplace_back(Piece(p, s, m_config.H));
        ComputeRow(y, x + s, pieces);
        pieces.pop_back();
    }
}

void Enumerator::ComputeColumn(int x, int y, std::vector<Piece> &pieces) {
    if (y >= m_config.height) {
        const int group = pieces.empty() ? 0 : GroupForPieces(pieces);
        m_ColumnEntries[x].emplace_back(PositionEntry(group, pieces, m_config));
        return;
    }
    
    // Try not placing a piece
    ComputeColumn(x, y + 1, pieces);
    
    // Try placing a piece
    const int n = m_config.height - y;
    for (int s = m_config.minPieceSize; s <= m_config.maxPieceSize; s++) {
        if (s > n) {
            break;
        }
        if (y + s > m_config.height) {
            break;
        }
        const int p = y * m_config.width + x;
        pieces.emplace_back(Piece(p, s, m_config.V));
        ComputeColumn(x, y + s, pieces);
        pieces.pop_back();
    }
}

void Enumerator::ComputePositionEntries() {
    // Ensure arrays are already sized from constructor
    if (m_RowEntries.size() != m_config.height) {
        m_RowEntries.resize(m_config.height);
    }
    if (m_ColumnEntries.size() != m_config.width) {
        m_ColumnEntries.resize(m_config.width);
    }
    
    // Compute entries for all rows except primary row
    for (int i = 0; i < m_config.height; i++) {
        if (i != m_config.primaryRow) {
            std::vector<Piece> pieces;
            ComputeRow(i, 0, pieces);
        }
    }
    
    // Compute entries for all columns
    for (int i = 0; i < m_config.width; i++) {
        std::vector<Piece> pieces;
        ComputeColumn(i, 0, pieces);
    }
    
    // Add special handling for primary row
    // The primary piece can be placed at any position where it fits
    for (int x = 0; x + m_config.primarySize <= m_config.width; x++) {
        std::vector<Piece> pieces;
        const int p = m_config.primaryRow * m_config.width + x;
        pieces.emplace_back(Piece(p, m_config.primarySize, m_config.H));
        // Get the group for this single primary piece
        const int group = GroupForPieces(pieces);
        m_RowEntries[m_config.primaryRow].emplace_back(PositionEntry(group, pieces, m_config));
    }
}