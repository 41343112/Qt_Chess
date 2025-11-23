#include "chessboard.h"

ChessBoard::ChessBoard()
    : m_board(8, std::vector<ChessPiece>(8)), m_currentPlayer(PieceColor::White), m_enPassantTarget(-1, -1),
      m_isInReplayMode(false), m_currentMoveIndex(-1)
{
    initializeBoard();
}

void ChessBoard::initializeBoard() {
    // 初始化空棋盤
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            m_board[row][col] = ChessPiece(PieceType::None, PieceColor::None);
        }
    }
    
    // 設置黑色棋子（第 0 和 1 行）
    m_board[0][0] = ChessPiece(PieceType::Rook, PieceColor::Black);
    m_board[0][1] = ChessPiece(PieceType::Knight, PieceColor::Black);
    m_board[0][2] = ChessPiece(PieceType::Bishop, PieceColor::Black);
    m_board[0][3] = ChessPiece(PieceType::Queen, PieceColor::Black);
    m_board[0][4] = ChessPiece(PieceType::King, PieceColor::Black);
    m_board[0][5] = ChessPiece(PieceType::Bishop, PieceColor::Black);
    m_board[0][6] = ChessPiece(PieceType::Knight, PieceColor::Black);
    m_board[0][7] = ChessPiece(PieceType::Rook, PieceColor::Black);
    
    for (int col = 0; col < 8; ++col) {
        m_board[1][col] = ChessPiece(PieceType::Pawn, PieceColor::Black);
    }
    
    // 設置白色棋子（第 6 和 7 行）
    for (int col = 0; col < 8; ++col) {
        m_board[6][col] = ChessPiece(PieceType::Pawn, PieceColor::White);
    }
    
    m_board[7][0] = ChessPiece(PieceType::Rook, PieceColor::White);
    m_board[7][1] = ChessPiece(PieceType::Knight, PieceColor::White);
    m_board[7][2] = ChessPiece(PieceType::Bishop, PieceColor::White);
    m_board[7][3] = ChessPiece(PieceType::Queen, PieceColor::White);
    m_board[7][4] = ChessPiece(PieceType::King, PieceColor::White);
    m_board[7][5] = ChessPiece(PieceType::Bishop, PieceColor::White);
    m_board[7][6] = ChessPiece(PieceType::Knight, PieceColor::White);
    m_board[7][7] = ChessPiece(PieceType::Rook, PieceColor::White);
    
    m_currentPlayer = PieceColor::White;
    m_enPassantTarget = QPoint(-1, -1);
    
    // 清除歷史記錄和回放狀態
    m_moveHistory.clear();
    m_isInReplayMode = false;
    m_currentMoveIndex = -1;
}

const ChessPiece& ChessBoard::getPiece(int row, int col) const {
    return m_board[row][col];
}

ChessPiece& ChessBoard::getPiece(int row, int col) {
    return m_board[row][col];
}

QPoint ChessBoard::findKing(PieceColor color) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece& piece = m_board[row][col];
            if (piece.getType() == PieceType::King && piece.getColor() == color) {
                return QPoint(col, row);
            }
        }
    }
    return QPoint(-1, -1);
}

bool ChessBoard::isInCheck(PieceColor color) const {
    QPoint kingPos = findKing(color);
    if (kingPos.x() < 0) return false;
    
    PieceColor opponentColor = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
    
    // 檢查是否有任何對手的棋子可以吃掉國王
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece& piece = m_board[row][col];
            if (piece.getColor() == opponentColor && piece.getType() != PieceType::None) {
                if (piece.isValidMove(QPoint(col, row), kingPos, m_board, QPoint(-1, -1))) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool ChessBoard::wouldBeInCheck(const QPoint& from, const QPoint& to, PieceColor color) const {
    // 複製棋盤並模擬移動
    std::vector<std::vector<ChessPiece>> tempBoard = m_board;
    
    // 在臨時棋盤上執行移動
    // 注意：QPoint(x, y) 對應到 board[y][x]，因為 x=列，y=行
    tempBoard[to.y()][to.x()] = tempBoard[from.y()][from.x()];
    tempBoard[from.y()][from.x()] = ChessPiece(PieceType::None, PieceColor::None);
    
    // 在移動後找到國王的位置
    QPoint kingPos(-1, -1);
    if (tempBoard[to.y()][to.x()].getType() == PieceType::King) {
        kingPos = to;
    } else {
        // 尋找國王
        bool found = false;
        for (int row = 0; row < 8 && !found; ++row) {
            for (int col = 0; col < 8; ++col) {
                const ChessPiece& piece = tempBoard[row][col];
                if (piece.getType() == PieceType::King && piece.getColor() == color) {
                    kingPos = QPoint(col, row);
                    found = true;
                    break;
                }
            }
        }
    }
    
    // 如果找不到國王，認為它被將軍（防禦性程式設計）
    if (kingPos.x() < 0) return true;
    
    PieceColor opponentColor = (color == PieceColor::White) ? PieceColor::Black : PieceColor::White;
    
    // 檢查是否有任何對手的棋子可以吃掉國王
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece& piece = tempBoard[row][col];
            if (piece.getColor() == opponentColor && piece.getType() != PieceType::None) {
                if (piece.isValidMove(QPoint(col, row), kingPos, tempBoard, QPoint(-1, -1))) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool ChessBoard::isValidMove(const QPoint& from, const QPoint& to) const {
    const ChessPiece& piece = m_board[from.y()][from.x()];
    
    // 檢查起始位置是否有棋子
    if (piece.getType() == PieceType::None) return false;
    
    // 檢查是否為當前玩家的棋子
    if (piece.getColor() != m_currentPlayer) return false;
    
    // 特殊處理王車易位
    if (piece.getType() == PieceType::King && abs(to.x() - from.x()) == 2) {
        return canCastle(from, to);
    }
    
    // 檢查該棋子類型的移動是否有效
    if (!piece.isValidMove(from, to, m_board, m_enPassantTarget)) return false;
    
    // 檢查移動是否會使自己的國王被將軍
    if (wouldBeInCheck(from, to, m_currentPlayer)) return false;
    
    return true;
}

bool ChessBoard::movePiece(const QPoint& from, const QPoint& to) {
    // 如果在回放模式中，不允許移動
    if (m_isInReplayMode) {
        return false;
    }
    
    if (!isValidMove(from, to)) return false;
    
    ChessPiece& piece = m_board[from.y()][from.x()];
    PieceType pieceType = piece.getType();
    PieceColor pieceColor = piece.getColor();
    
    // 創建棋步記錄
    Move move;
    move.from = from;
    move.to = to;
    move.movedPiece = piece;
    move.capturedPiece = m_board[to.y()][to.x()];
    move.prevEnPassantTarget = m_enPassantTarget;
    move.wasCastling = false;
    move.wasEnPassant = false;
    move.promotionType = PieceType::None;
    
    // 處理王車易位
    if (pieceType == PieceType::King && abs(to.x() - from.x()) == 2) {
        move.wasCastling = true;
        // 王翼易位
        if (to.x() > from.x()) {
            // 將車從 h 列移到 f 列
            ChessPiece& rook = m_board[from.y()][7];
            m_board[from.y()][5] = rook;
            m_board[from.y()][5].setMoved(true);
            m_board[from.y()][7] = ChessPiece(PieceType::None, PieceColor::None);
        }
        // 后翼易位
        else {
            // 將車從 a 列移到 d 列
            ChessPiece& rook = m_board[from.y()][0];
            m_board[from.y()][3] = rook;
            m_board[from.y()][3].setMoved(true);
            m_board[from.y()][0] = ChessPiece(PieceType::None, PieceColor::None);
        }
    }
    
    // 處理吃過路兵
    if (pieceType == PieceType::Pawn && to == m_enPassantTarget && m_enPassantTarget.x() >= 0) {
        move.wasEnPassant = true;
        // 移除被吃掉的兵
        int capturedPawnRow = (pieceColor == PieceColor::White) ? to.y() + 1 : to.y() - 1;
        move.capturedPiece = m_board[capturedPawnRow][to.x()];  // 記錄被吃的兵
        m_board[capturedPawnRow][to.x()] = ChessPiece(PieceType::None, PieceColor::None);
    }
    
    // 在設置新的吃過路兵目標之前清除舊的
    m_enPassantTarget = QPoint(-1, -1);
    
    // 追蹤兵的雙格移動以便吃過路兵
    if (pieceType == PieceType::Pawn && abs(to.y() - from.y()) == 2) {
        // 將吃過路兵目標設置為兵跳過的格子（中間格子）
        // 例如，如果白兵從第 6 行移到第 4 行，目標為第 5 行
        int targetRow = (from.y() + to.y()) / 2;
        m_enPassantTarget = QPoint(from.x(), targetRow);
    }
    
    // 執行移動
    m_board[to.y()][to.x()] = piece;
    m_board[to.y()][to.x()].setMoved(true);
    m_board[from.y()][from.x()] = ChessPiece(PieceType::None, PieceColor::None);
    
    // 記錄棋步
    recordMove(move);
    
    switchPlayer();
    return true;
}

void ChessBoard::switchPlayer() {
    m_currentPlayer = (m_currentPlayer == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool ChessBoard::canPieceMove(const QPoint& pos) const {
    const ChessPiece& piece = m_board[pos.y()][pos.x()];
    if (piece.getType() == PieceType::None) return false;
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QPoint to(col, row);
            if (piece.isValidMove(pos, to, m_board, m_enPassantTarget)) {
                if (!wouldBeInCheck(pos, to, piece.getColor())) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ChessBoard::hasAnyValidMoves(PieceColor color) const {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece& piece = m_board[row][col];
            if (piece.getColor() == color && piece.getType() != PieceType::None) {
                if (canPieceMove(QPoint(col, row))) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ChessBoard::isCheckmate(PieceColor color) const {
    return isInCheck(color) && !hasAnyValidMoves(color);
}

bool ChessBoard::isStalemate(PieceColor color) const {
    return !isInCheck(color) && !hasAnyValidMoves(color);
}

bool ChessBoard::canCastle(const QPoint& from, const QPoint& to) const {
    const ChessPiece& king = m_board[from.y()][from.x()];
    
    // 必須是未移動過的國王
    if (king.getType() != PieceType::King || king.hasMoved()) return false;
    
    // 必須正好水平移動 2 格
    if (abs(to.x() - from.x()) != 2 || to.y() != from.y()) return false;
    
    // 國王不能處於被將軍狀態
    if (isInCheck(king.getColor())) return false;
    
    // 確定車的位置並檢查路徑
    int rookCol = (to.x() > from.x()) ? 7 : 0; // 王翼或后翼
    const ChessPiece& rook = m_board[from.y()][rookCol];
    
    // 車必須存在且未移動過
    if (rook.getType() != PieceType::Rook || rook.hasMoved()) return false;
    
    // 檢查國王和車之間的路徑是否暢通
    // 驗證國王當前位置和車之間的所有格子都是空的
    int direction = (to.x() > from.x()) ? 1 : -1;
    for (int col = from.x() + direction; col != rookCol; col += direction) {
        if (m_board[from.y()][col].getType() != PieceType::None) return false;
    }
    
    // 檢查國王不會經過被將軍的格子
    // 國王移動 2 格，所以檢查中間的格子
    QPoint intermediate(from.x() + direction, from.y());
    if (wouldBeInCheck(from, intermediate, king.getColor())) return false;
    
    // 檢查國王不會在目標位置被將軍
    if (wouldBeInCheck(from, to, king.getColor())) return false;
    
    return true;
}

bool ChessBoard::needsPromotion(const QPoint& to) const {
    const ChessPiece& piece = m_board[to.y()][to.x()];
    if (piece.getType() != PieceType::Pawn) return false;
    
    // 白兵到達第 0 行，黑兵到達第 7 行
    if (piece.getColor() == PieceColor::White && to.y() == 0) return true;
    if (piece.getColor() == PieceColor::Black && to.y() == 7) return true;
    
    return false;
}

void ChessBoard::promotePawn(const QPoint& pos, PieceType newType) {
    ChessPiece& piece = m_board[pos.y()][pos.x()];
    if (piece.getType() != PieceType::Pawn) return;
    
    PieceColor color = piece.getColor();
    m_board[pos.y()][pos.x()] = ChessPiece(newType, color);
    m_board[pos.y()][pos.x()].setMoved(true);
    
    // 更新最後一步的升變類型
    if (!m_moveHistory.empty() && m_currentMoveIndex >= 0 && 
        m_currentMoveIndex < static_cast<int>(m_moveHistory.size())) {
        m_moveHistory[m_currentMoveIndex].promotionType = newType;
    }
}

bool ChessBoard::isInsufficientMaterial() const {
    // 計算棋盤上的棋子
    int whiteKnights = 0, blackKnights = 0;
    int whiteBishops = 0, blackBishops = 0;
    int whiteBishopsOnEvenSquares = 0, whiteBishopsOnOddSquares = 0;
    int blackBishopsOnEvenSquares = 0, blackBishopsOnOddSquares = 0;
    bool hasWhiteKing = false, hasBlackKing = false;
    bool hasOtherPieces = false;
    
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            const ChessPiece& piece = m_board[row][col];
            PieceType type = piece.getType();
            PieceColor color = piece.getColor();
            
            if (type == PieceType::None) continue;
            
            if (type == PieceType::King) {
                if (color == PieceColor::White) hasWhiteKing = true;
                else hasBlackKing = true;
            } else if (type == PieceType::Knight) {
                if (color == PieceColor::White) whiteKnights++;
                else blackKnights++;
            } else if (type == PieceType::Bishop) {
                bool isEvenSquare = (row + col) % 2 == 0;
                if (color == PieceColor::White) {
                    whiteBishops++;
                    if (isEvenSquare) whiteBishopsOnEvenSquares++;
                    else whiteBishopsOnOddSquares++;
                } else {
                    blackBishops++;
                    if (isEvenSquare) blackBishopsOnEvenSquares++;
                    else blackBishopsOnOddSquares++;
                }
            } else {
                // 兵、車或后 - 材料足夠
                hasOtherPieces = true;
            }
        }
    }
    
    // 安全檢查：雙方國王必須存在
    if (!hasWhiteKing || !hasBlackKing) return false;
    
    // 如果有兵、車或后，材料足夠
    if (hasOtherPieces) return false;
    
    // 王對王
    if (whiteKnights == 0 && blackKnights == 0 && 
        whiteBishops == 0 && blackBishops == 0) {
        return true;
    }
    
    // 王和馬對王
    if ((whiteKnights == 1 && blackKnights == 0 && whiteBishops == 0 && blackBishops == 0) ||
        (blackKnights == 1 && whiteKnights == 0 && whiteBishops == 0 && blackBishops == 0)) {
        return true;
    }
    
    // 王和象對王
    if ((whiteBishops == 1 && blackBishops == 0 && whiteKnights == 0 && blackKnights == 0) ||
        (blackBishops == 1 && whiteBishops == 0 && whiteKnights == 0 && blackKnights == 0)) {
        return true;
    }
    
    // 王和象對王和象，且雙方象在同色格上
    if (whiteBishops == 1 && blackBishops == 1 && whiteKnights == 0 && blackKnights == 0) {
        // 兩個象必須在同色格上（都在偶數格或都在奇數格）
        if ((whiteBishopsOnEvenSquares > 0 && blackBishopsOnEvenSquares > 0) ||
            (whiteBishopsOnOddSquares > 0 && blackBishopsOnOddSquares > 0)) {
            return true;
        }
    }
    
    return false;
}

// 回放功能實現

void ChessBoard::recordMove(const Move& move) {
    // 如果我們不在最新的位置，刪除之後的所有移動
    if (m_currentMoveIndex < static_cast<int>(m_moveHistory.size()) - 1) {
        m_moveHistory.erase(m_moveHistory.begin() + m_currentMoveIndex + 1, m_moveHistory.end());
    }
    
    m_moveHistory.push_back(move);
    m_currentMoveIndex = static_cast<int>(m_moveHistory.size()) - 1;
    
    // 確保不在回放模式
    m_isInReplayMode = false;
}

bool ChessBoard::canGoToPreviousMove() const {
    return m_currentMoveIndex >= 0;
}

bool ChessBoard::canGoToNextMove() const {
    return m_currentMoveIndex < static_cast<int>(m_moveHistory.size()) - 1;
}

bool ChessBoard::goToPreviousMove() {
    if (!canGoToPreviousMove()) {
        return false;
    }
    
    // 進入回放模式
    m_isInReplayMode = true;
    
    // 撤銷當前移動
    const Move& move = m_moveHistory[m_currentMoveIndex];
    undoMove(move);
    m_currentMoveIndex--;
    
    return true;
}

bool ChessBoard::goToNextMove() {
    if (!canGoToNextMove()) {
        return false;
    }
    
    // 應用下一個移動
    m_currentMoveIndex++;
    const Move& move = m_moveHistory[m_currentMoveIndex];
    applyMove(move);
    
    // 檢查是否到達最新狀態，如果是則自動退出回放模式
    if (m_currentMoveIndex == static_cast<int>(m_moveHistory.size()) - 1) {
        exitReplayMode();
    }
    
    return true;
}

void ChessBoard::exitReplayMode() {
    m_isInReplayMode = false;
}

void ChessBoard::applyMove(const Move& move) {
    // 執行基本移動
    m_board[move.to.y()][move.to.x()] = move.movedPiece;
    m_board[move.to.y()][move.to.x()].setMoved(true);
    m_board[move.from.y()][move.from.x()] = ChessPiece(PieceType::None, PieceColor::None);
    
    // 處理王車易位
    if (move.wasCastling) {
        int row = move.from.y();
        if (move.to.x() > move.from.x()) {
            // 王翼易位 - 移動車
            ChessPiece rook(PieceType::Rook, move.movedPiece.getColor());
            rook.setMoved(true);
            m_board[row][5] = rook;
            m_board[row][7] = ChessPiece(PieceType::None, PieceColor::None);
        } else {
            // 后翼易位 - 移動車
            ChessPiece rook(PieceType::Rook, move.movedPiece.getColor());
            rook.setMoved(true);
            m_board[row][3] = rook;
            m_board[row][0] = ChessPiece(PieceType::None, PieceColor::None);
        }
    }
    
    // 處理吃過路兵
    if (move.wasEnPassant) {
        int capturedPawnRow = (move.movedPiece.getColor() == PieceColor::White) ? move.to.y() + 1 : move.to.y() - 1;
        m_board[capturedPawnRow][move.to.x()] = ChessPiece(PieceType::None, PieceColor::None);
    }
    
    // 處理升變
    if (move.promotionType != PieceType::None) {
        m_board[move.to.y()][move.to.x()] = ChessPiece(move.promotionType, move.movedPiece.getColor());
        m_board[move.to.y()][move.to.x()].setMoved(true);
    }
    
    // 更新吃過路兵目標
    m_enPassantTarget = move.prevEnPassantTarget;
    if (move.movedPiece.getType() == PieceType::Pawn && abs(move.to.y() - move.from.y()) == 2) {
        int targetRow = (move.from.y() + move.to.y()) / 2;
        m_enPassantTarget = QPoint(move.from.x(), targetRow);
    }
    
    switchPlayer();
}

void ChessBoard::undoMove(const Move& move) {
    // 恢復移動的棋子
    m_board[move.from.y()][move.from.x()] = move.movedPiece;
    
    // 恢復被吃的棋子（如果有）
    if (move.wasEnPassant) {
        // 吃過路兵 - 恢復被吃的兵
        int capturedPawnRow = (move.movedPiece.getColor() == PieceColor::White) ? move.to.y() + 1 : move.to.y() - 1;
        m_board[capturedPawnRow][move.to.x()] = move.capturedPiece;
        m_board[move.to.y()][move.to.x()] = ChessPiece(PieceType::None, PieceColor::None);
    } else {
        m_board[move.to.y()][move.to.x()] = move.capturedPiece;
    }
    
    // 撤銷王車易位
    if (move.wasCastling) {
        int row = move.from.y();
        if (move.to.x() > move.from.x()) {
            // 王翼易位 - 恢復車
            ChessPiece rook(PieceType::Rook, move.movedPiece.getColor());
            m_board[row][7] = rook;
            m_board[row][5] = ChessPiece(PieceType::None, PieceColor::None);
        } else {
            // 后翼易位 - 恢復車
            ChessPiece rook(PieceType::Rook, move.movedPiece.getColor());
            m_board[row][0] = rook;
            m_board[row][3] = ChessPiece(PieceType::None, PieceColor::None);
        }
    }
    
    // 恢復吃過路兵目標
    m_enPassantTarget = move.prevEnPassantTarget;
    
    switchPlayer();
}
