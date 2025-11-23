#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspiece.h"
#include <QPoint>
#include <vector>

// 棋步記錄結構
struct Move {
    QPoint from;
    QPoint to;
    ChessPiece movedPiece;
    ChessPiece capturedPiece;
    PieceType promotionType;  // 如果是兵升變，記錄升變類型
    bool wasCastling;         // 是否為王車易位
    bool wasEnPassant;        // 是否為吃過路兵
    QPoint prevEnPassantTarget; // 移動前的吃過路兵目標
    
    Move() : from(-1, -1), to(-1, -1), 
             movedPiece(PieceType::None, PieceColor::None),
             capturedPiece(PieceType::None, PieceColor::None),
             promotionType(PieceType::None),
             wasCastling(false), wasEnPassant(false),
             prevEnPassantTarget(-1, -1) {}
};

class ChessBoard {
public:
    ChessBoard();
    
    void initializeBoard();
    const ChessPiece& getPiece(int row, int col) const;
    ChessPiece& getPiece(int row, int col);
    
    bool movePiece(const QPoint& from, const QPoint& to);
    bool isValidMove(const QPoint& from, const QPoint& to) const;
    
    PieceColor getCurrentPlayer() const { return m_currentPlayer; }
    bool isInCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;
    bool isInsufficientMaterial() const;
    
    QPoint findKing(PieceColor color) const;
    QPoint getEnPassantTarget() const { return m_enPassantTarget; }
    
    // 升變 - 如果需要兵升變則返回 true
    bool needsPromotion(const QPoint& to) const;
    void promotePawn(const QPoint& pos, PieceType newType);
    
    // 回放功能
    bool isInReplayMode() const { return m_isInReplayMode; }
    bool canGoToPreviousMove() const;
    bool canGoToNextMove() const;
    bool goToPreviousMove();
    bool goToNextMove();
    int getCurrentMoveIndex() const { return m_currentMoveIndex; }
    int getTotalMoves() const { return static_cast<int>(m_moveHistory.size()); }
    void exitReplayMode();  // 退出回放模式
    
private:
    std::vector<std::vector<ChessPiece>> m_board;
    PieceColor m_currentPlayer;
    QPoint m_enPassantTarget; // 可以進行吃過路兵的位置（如果沒有則為 -1, -1）
    
    // 回放功能相關
    std::vector<Move> m_moveHistory;  // 棋步歷史記錄
    bool m_isInReplayMode;            // 是否處於回放模式
    int m_currentMoveIndex;           // 當前回放位置（-1 表示初始狀態）
    
    void switchPlayer();
    bool wouldBeInCheck(const QPoint& from, const QPoint& to, PieceColor color) const;
    bool hasAnyValidMoves(PieceColor color) const;
    bool canPieceMove(const QPoint& pos) const;
    bool canCastle(const QPoint& from, const QPoint& to) const;
    
    void recordMove(const Move& move);  // 記錄棋步
    void applyMove(const Move& move);   // 應用棋步
    void undoMove(const Move& move);    // 撤銷棋步
};

#endif // CHESSBOARD_H
