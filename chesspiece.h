#ifndef CHESSPIECE_H
#define CHESSPIECE_H

#include <QString>
#include <QPoint>
#include <vector>

// 棋子類型枚舉
enum class PieceType {
    None,    // 無
    Pawn,    // 兵
    Rook,    // 車
    Knight,  // 馬
    Bishop,  // 象
    Queen,   // 后
    King     // 王
};

// 棋子顏色枚舉
enum class PieceColor {
    None,   // 無
    White,  // 白方
    Black   // 黑方
};

// 棋子類別 - 表示單個棋子及其移動規則
class ChessPiece {
public:
    ChessPiece(PieceType type = PieceType::None, PieceColor color = PieceColor::None);
    
    PieceType getType() const { return m_type; }
    PieceColor getColor() const { return m_color; }
    bool hasMoved() const { return m_hasMoved; }
    void setMoved(bool moved) { m_hasMoved = moved; }
    
    QString getSymbol() const;
    
    // 檢查移動到目標位置是否有效（基本棋子移動規則）
    bool isValidMove(const QPoint& from, const QPoint& to, 
                     const std::vector<std::vector<ChessPiece>>& board,
                     const QPoint& enPassantTarget = QPoint(-1, -1)) const;
    
private:
    PieceType m_type;
    PieceColor m_color;
    bool m_hasMoved;
    
    bool isValidPawnMove(const QPoint& from, const QPoint& to, 
                         const std::vector<std::vector<ChessPiece>>& board,
                         const QPoint& enPassantTarget) const;
    bool isValidRookMove(const QPoint& from, const QPoint& to, 
                         const std::vector<std::vector<ChessPiece>>& board) const;
    bool isValidKnightMove(const QPoint& from, const QPoint& to) const;
    bool isValidBishopMove(const QPoint& from, const QPoint& to, 
                           const std::vector<std::vector<ChessPiece>>& board) const;
    bool isValidQueenMove(const QPoint& from, const QPoint& to, 
                          const std::vector<std::vector<ChessPiece>>& board) const;
    bool isValidKingMove(const QPoint& from, const QPoint& to) const;
    
    bool isPathClear(const QPoint& from, const QPoint& to, 
                     const std::vector<std::vector<ChessPiece>>& board) const;
};

#endif // CHESSPIECE_H
