#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "chesspiece.h"
#include <QPoint>
#include <vector>

enum class BoardPreset {
    Standard,       // 標準開局 - Standard starting position
    MidGame,        // 中局練習 - Mid-game tactical position
    EndGame         // 殘局練習 - Endgame position
};

class ChessBoard {
public:
    ChessBoard();
    
    void initializeBoard();
    void initializeBoard(BoardPreset preset);
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
    
    // Promotion - returns true if pawn promotion is needed
    bool needsPromotion(const QPoint& to) const;
    void promotePawn(const QPoint& pos, PieceType newType);
    
private:
    std::vector<std::vector<ChessPiece>> m_board;
    PieceColor m_currentPlayer;
    QPoint m_enPassantTarget; // Position where en passant capture is possible (-1, -1 if none)
    
    void switchPlayer();
    bool wouldBeInCheck(const QPoint& from, const QPoint& to, PieceColor color) const;
    bool hasAnyValidMoves(PieceColor color) const;
    bool canPieceMove(const QPoint& pos) const;
    bool canCastle(const QPoint& from, const QPoint& to) const;
    
    // Preset board initialization methods
    void initializeStandardBoard();
    void initializeMidGameBoard();
    void initializeEndGameBoard();
    void clearBoard();
    void markAllPiecesAsMoved();
};

#endif // CHESSBOARD_H
