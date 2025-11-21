#include "chesspiece.h"
#include <cmath>

// Initialize static member
PieceAppearanceStyle ChessPiece::s_appearanceStyle = PieceAppearanceStyle::UnicodeSymbols;

ChessPiece::ChessPiece(PieceType type, PieceColor color)
    : m_type(type), m_color(color), m_hasMoved(false)
{
}

QString ChessPiece::getSymbol() const {
    return getSymbolForStyle(m_type, m_color, s_appearanceStyle);
}

void ChessPiece::setAppearanceStyle(PieceAppearanceStyle style) {
    s_appearanceStyle = style;
}

PieceAppearanceStyle ChessPiece::getAppearanceStyle() {
    return s_appearanceStyle;
}

QString ChessPiece::getSymbolForStyle(PieceType type, PieceColor color, PieceAppearanceStyle style) {
    if (type == PieceType::None) return " ";
    
    switch (style) {
        case PieceAppearanceStyle::UnicodeSymbols:
            if (color == PieceColor::White) {
                switch (type) {
                    case PieceType::King:   return "♔";
                    case PieceType::Queen:  return "♕";
                    case PieceType::Rook:   return "♖";
                    case PieceType::Bishop: return "♗";
                    case PieceType::Knight: return "♘";
                    case PieceType::Pawn:   return "♙";
                    default: return " ";
                }
            } else {
                switch (type) {
                    case PieceType::King:   return "♚";
                    case PieceType::Queen:  return "♛";
                    case PieceType::Rook:   return "♜";
                    case PieceType::Bishop: return "♝";
                    case PieceType::Knight: return "♞";
                    case PieceType::Pawn:   return "♟";
                    default: return " ";
                }
            }
            
        case PieceAppearanceStyle::UnicodeAlternate:
            // Use filled symbols for white, outlined for black (reversed)
            if (color == PieceColor::White) {
                switch (type) {
                    case PieceType::King:   return "♚";
                    case PieceType::Queen:  return "♛";
                    case PieceType::Rook:   return "♜";
                    case PieceType::Bishop: return "♝";
                    case PieceType::Knight: return "♞";
                    case PieceType::Pawn:   return "♟";
                    default: return " ";
                }
            } else {
                switch (type) {
                    case PieceType::King:   return "♔";
                    case PieceType::Queen:  return "♕";
                    case PieceType::Rook:   return "♖";
                    case PieceType::Bishop: return "♗";
                    case PieceType::Knight: return "♘";
                    case PieceType::Pawn:   return "♙";
                    default: return " ";
                }
            }
            
        case PieceAppearanceStyle::TextBased:
            switch (type) {
                case PieceType::King:   return "K";
                case PieceType::Queen:  return "Q";
                case PieceType::Rook:   return "R";
                case PieceType::Bishop: return "B";
                case PieceType::Knight: return "N";
                case PieceType::Pawn:   return "P";
                default: return " ";
            }
    }
    
    return " ";
}

bool ChessPiece::isValidMove(const QPoint& from, const QPoint& to, 
                              const std::vector<std::vector<ChessPiece>>& board,
                              const QPoint& enPassantTarget) const {
    if (from == to) return false;
    
    // Check bounds
    if (to.x() < 0 || to.x() >= 8 || to.y() < 0 || to.y() >= 8) return false;
    
    // Can't capture own piece
    const ChessPiece& target = board[to.y()][to.x()];
    if (target.getColor() == m_color && target.getType() != PieceType::None) return false;
    
    switch (m_type) {
        case PieceType::Pawn:   return isValidPawnMove(from, to, board, enPassantTarget);
        case PieceType::Rook:   return isValidRookMove(from, to, board);
        case PieceType::Knight: return isValidKnightMove(from, to);
        case PieceType::Bishop: return isValidBishopMove(from, to, board);
        case PieceType::Queen:  return isValidQueenMove(from, to, board);
        case PieceType::King:   return isValidKingMove(from, to);
        default: return false;
    }
}

bool ChessPiece::isValidPawnMove(const QPoint& from, const QPoint& to, 
                                  const std::vector<std::vector<ChessPiece>>& board,
                                  const QPoint& enPassantTarget) const {
    int direction = (m_color == PieceColor::White) ? -1 : 1;
    int startRow = (m_color == PieceColor::White) ? 6 : 1;
    
    int dx = to.x() - from.x();
    int dy = to.y() - from.y();
    
    // Forward move
    if (dx == 0) {
        const ChessPiece& target = board[to.y()][to.x()];
        if (target.getType() != PieceType::None) return false;
        
        // One square forward
        if (dy == direction) return true;
        
        // Two squares forward from starting position
        if (!m_hasMoved && from.y() == startRow && dy == 2 * direction) {
            const ChessPiece& middle = board[from.y() + direction][from.x()];
            return middle.getType() == PieceType::None;
        }
    }
    // Diagonal capture
    else if (abs(dx) == 1 && dy == direction) {
        const ChessPiece& target = board[to.y()][to.x()];
        // Normal diagonal capture
        if (target.getType() != PieceType::None && target.getColor() != m_color) {
            return true;
        }
        // En passant capture
        if (enPassantTarget.x() >= 0 && to == enPassantTarget) {
            return true;
        }
    }
    
    return false;
}

bool ChessPiece::isValidRookMove(const QPoint& from, const QPoint& to, 
                                  const std::vector<std::vector<ChessPiece>>& board) const {
    if (from.x() != to.x() && from.y() != to.y()) return false;
    return isPathClear(from, to, board);
}

bool ChessPiece::isValidKnightMove(const QPoint& from, const QPoint& to) const {
    int dx = abs(to.x() - from.x());
    int dy = abs(to.y() - from.y());
    return (dx == 2 && dy == 1) || (dx == 1 && dy == 2);
}

bool ChessPiece::isValidBishopMove(const QPoint& from, const QPoint& to, 
                                    const std::vector<std::vector<ChessPiece>>& board) const {
    if (abs(to.x() - from.x()) != abs(to.y() - from.y())) return false;
    return isPathClear(from, to, board);
}

bool ChessPiece::isValidQueenMove(const QPoint& from, const QPoint& to, 
                                   const std::vector<std::vector<ChessPiece>>& board) const {
    return isValidRookMove(from, to, board) || isValidBishopMove(from, to, board);
}

bool ChessPiece::isValidKingMove(const QPoint& from, const QPoint& to) const {
    int dx = abs(to.x() - from.x());
    int dy = abs(to.y() - from.y());
    
    // Normal king move (one square in any direction)
    if (dx <= 1 && dy <= 1) return true;
    
    // Castling (2 squares horizontally)
    // Note: Additional validation (e.g., not in check, path clear) is done in ChessBoard::canCastle
    if (dx == 2 && dy == 0 && !m_hasMoved) return true;
    
    return false;
}

bool ChessPiece::isPathClear(const QPoint& from, const QPoint& to, 
                              const std::vector<std::vector<ChessPiece>>& board) const {
    int dx = (to.x() > from.x()) ? 1 : (to.x() < from.x()) ? -1 : 0;
    int dy = (to.y() > from.y()) ? 1 : (to.y() < from.y()) ? -1 : 0;
    
    int x = from.x() + dx;
    int y = from.y() + dy;
    
    while (x != to.x() || y != to.y()) {
        if (board[y][x].getType() != PieceType::None) return false;
        x += dx;
        y += dy;
    }
    
    return true;
}
