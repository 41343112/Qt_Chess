# ChessBoard 棋盤功能

## 概述
`ChessBoard` 類別管理整個西洋棋遊戲的棋盤狀態、遊戲規則、移動歷史記錄，以及將軍、將死、僵局等遊戲結束條件的判定。

## 檔案位置
- **標頭檔**: `src/chessboard.h`
- **實作檔**: `src/chessboard.cpp`

## 主要資料結構

### GameResult - 遊戲結果
```cpp
enum class GameResult {
    InProgress,      // 遊戲進行中
    WhiteWins,       // 白方獲勝
    BlackWins,       // 黑方獲勝
    Draw,            // 和局
    WhiteResigns,    // 白方認輸
    BlackResigns     // 黑方認輸
};
```

### MoveRecord - 移動記錄
```cpp
struct MoveRecord {
    QPoint from;                    // 起始位置
    QPoint to;                      // 目標位置
    PieceType pieceType;            // 棋子類型
    PieceColor pieceColor;          // 棋子顏色
    bool isCapture;                 // 是否為吃子
    bool isCastling;                // 是否為王車易位
    bool isEnPassant;               // 是否為吃過路兵
    bool isPromotion;               // 是否為兵升變
    PieceType promotionType;        // 升變後的棋子類型
    bool isCheck;                   // 是否造成將軍
    bool isCheckmate;               // 是否造成將死
    QString algebraicNotation;      // 代數記譜法表示
};
```

記錄每一步移動的詳細資訊，用於棋譜顯示和 PGN 匯出。

## 類別成員

### 私有成員變數
```cpp
std::vector<std::vector<ChessPiece>> m_board;  // 8x8 棋盤
PieceColor m_currentPlayer;                     // 當前玩家
QPoint m_enPassantTarget;                       // 吃過路兵的目標位置
std::vector<MoveRecord> m_moveHistory;          // 移動歷史
GameResult m_gameResult;                        // 遊戲結果
```

## 主要功能

### 1. 棋盤初始化

#### initializeBoard()
```cpp
void initializeBoard()
```
設置標準西洋棋開局棋盤配置：
- **第0列（黑方底線）**: 城堡、騎士、主教、皇后、國王、主教、騎士、城堡
- **第1列**: 黑色兵
- **第2-5列**: 空格
- **第6列**: 白色兵
- **第7列（白方底線）**: 城堡、騎士、主教、皇后、國王、主教、騎士、城堡

**實作重點**:
- 清空棋盤（設為 None 棋子）
- 設置雙方的主要棋子和兵
- 將當前玩家設為白方
- 清空移動歷史
- 重設遊戲結果為進行中

### 2. 棋子存取

#### getPiece()
```cpp
const ChessPiece& getPiece(int row, int col) const
ChessPiece& getPiece(int row, int col)
```
取得指定位置的棋子（提供 const 和非 const 版本）。

#### setPiece()
```cpp
void setPiece(int row, int col, const ChessPiece& piece)
```
安全地設置指定位置的棋子，包含邊界檢查。

### 3. 移動處理

#### movePiece()
```cpp
bool movePiece(const QPoint& from, const QPoint& to)
```
執行棋子移動的主要方法。

**處理流程**:
1. 驗證移動是否有效（調用 `isValidMove()`）
2. 取得移動的棋子
3. 檢查特殊走法：
   - **王車易位**: 同時移動國王和城堡
   - **吃過路兵**: 移除被吃的兵
   - **兵升變**: 彈出對話框讓玩家選擇升變類型
4. 執行移動
5. 更新棋子的 `hasMoved` 狀態
6. 更新吃過路兵目標位置
7. 記錄移動到歷史
8. 檢查是否將軍、將死
9. 切換當前玩家
10. 返回移動是否成功

**特殊處理**:

**王車易位**:
```cpp
// 檢測是否為王車易位（國王移動兩格）
if (piece.getType() == PieceType::King && abs(dx) == 2) {
    // 執行王車易位，同時移動城堡
    int rookFromX = (dx > 0) ? 7 : 0;
    int rookToX = (dx > 0) ? to.x() - 1 : to.x() + 1;
    // 移動城堡...
}
```

**吃過路兵**:
```cpp
if (piece.getType() == PieceType::Pawn && to == m_enPassantTarget) {
    // 移除被吃過路的兵
    int capturedPawnRow = from.y();
    m_board[capturedPawnRow][to.x()] = ChessPiece();
}
```

**兵升變**:
```cpp
if (piece.getType() == PieceType::Pawn) {
    if ((color == PieceColor::White && to.y() == 0) ||
        (color == PieceColor::Black && to.y() == 7)) {
        // 彈出對話框讓玩家選擇升變類型
        // 將兵替換為選擇的棋子
    }
}
```

#### isValidMove()
```cpp
bool isValidMove(const QPoint& from, const QPoint& to) const
```
完整驗證移動是否合法。

**驗證步驟**:
1. 檢查起始位置是否有棋子
2. 檢查是否為當前玩家的棋子
3. 調用 `ChessPiece::isValidMove()` 檢查基本移動規則
4. 特殊走法驗證：
   - **王車易位**: 檢查國王和城堡都未移動、中間無棋子、不經過被將軍位置
   - **吃過路兵**: 驗證目標是否為吃過路兵目標位置
5. 模擬移動後檢查是否會讓自己的國王陷入將軍
6. 所有條件都滿足才返回 `true`

### 4. 遊戲狀態檢查

#### isInCheck()
```cpp
bool isInCheck(PieceColor color) const
```
檢查指定顏色的國王是否被將軍。

**實作方式**:
1. 找到指定顏色的國王位置
2. 遍歷所有對方棋子
3. 檢查是否有對方棋子可以攻擊到國王

#### isCheckmate()
```cpp
bool isCheckmate(PieceColor color) const
```
檢查指定顏色是否被將死。

**將死條件**:
1. 當前處於將軍狀態
2. 沒有任何合法移動可以解除將軍

**實作方式**:
```cpp
if (!isInCheck(color)) return false;  // 必須先被將軍

// 嘗試所有可能的移動
for (每個己方棋子) {
    for (每個可能的目標位置) {
        if (isValidMove(from, to)) {
            return false;  // 找到一個可以解除將軍的移動
        }
    }
}
return true;  // 沒有任何移動可以解除將軍
```

#### isStalemate()
```cpp
bool isStalemate(PieceColor color) const
```
檢查是否為僵局（當前玩家沒有合法移動但未被將軍）。

**僵局條件**:
1. 未處於將軍狀態
2. 沒有任何合法移動

### 5. 王車易位驗證

#### canCastle()
```cpp
bool canCastle(const QPoint& kingPos, const QPoint& rookPos) const
```
檢查是否可以進行王車易位。

**必要條件**:
1. 國王未移動過（`!king.hasMoved()`）
2. 城堡未移動過（`!rook.hasMoved()`）
3. 國王和城堡之間無棋子阻擋
4. 國王當前未被將軍
5. 國王移動路徑上不會被將軍
6. 國王移動後的位置不會被將軍

**兩種易位方式**:
- **王翼易位（短易位）**: 國王向右移動兩格
- **后翼易位（長易位）**: 國王向左移動兩格

### 6. 移動記錄與棋譜

#### getMoveHistory()
```cpp
const std::vector<MoveRecord>& getMoveHistory() const
```
取得所有移動的歷史記錄。

#### getLastMove()
```cpp
const MoveRecord* getLastMove() const
```
取得最後一步移動。

#### generateAlgebraicNotation()
```cpp
QString generateAlgebraicNotation(const MoveRecord& move) const
```
生成標準代數記譜法（Algebraic Notation）。

**記譜格式**:
- **普通移動**: `Nf3`（騎士移動到 f3）
- **吃子**: `Nxf3`（騎士吃掉 f3 的棋子）
- **王車易位**: `O-O`（王翼）或 `O-O-O`（后翼）
- **兵升變**: `e8=Q`（兵升變為皇后）
- **將軍**: 加上 `+`（如 `Nf3+`）
- **將死**: 加上 `#`（如 `Qh7#`）

**實作重點**:
- 兵的移動只寫目標格（如 `e4`）
- 其他棋子加上棋子符號（K=國王、Q=皇后、R=城堡、B=主教、N=騎士）
- 需要消歧義時加上起始位置（如 `Nbd7`）

### 7. 遊戲狀態管理

#### getGameResult()
```cpp
GameResult getGameResult() const
```
取得當前遊戲結果。

#### setGameResult()
```cpp
void setGameResult(GameResult result)
```
設定遊戲結果（用於處理認輸）。

#### resign()
```cpp
void resign(PieceColor color)
```
處理玩家認輸。

**實作**:
```cpp
void ChessBoard::resign(PieceColor color) {
    if (color == PieceColor::White) {
        m_gameResult = GameResult::BlackWins;
    } else {
        m_gameResult = GameResult::WhiteWins;
    }
}
```

### 8. 棋盤狀態轉換

#### getBoardState()
```cpp
QString getBoardState() const
```
將棋盤狀態轉換為 FEN（Forsyth-Edwards Notation）格式，用於：
- 儲存棋局
- 載入棋局
- 與 AI 引擎通訊

**FEN 格式範例**:
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

## 使用範例

### 初始化並開始新遊戲
```cpp
ChessBoard board;
board.initializeBoard();
```

### 移動棋子
```cpp
QPoint from(6, 4);  // e2
QPoint to(4, 4);    // e4
if (board.isValidMove(from, to)) {
    board.movePiece(from, to);
}
```

### 檢查遊戲狀態
```cpp
if (board.isInCheck(PieceColor::White)) {
    qDebug() << "白方被將軍！";
}

if (board.isCheckmate(PieceColor::White)) {
    qDebug() << "白方被將死！黑方獲勝！";
}

if (board.isStalemate(PieceColor::White)) {
    qDebug() << "僵局！平手！";
}
```

### 取得移動歷史
```cpp
const auto& history = board.getMoveHistory();
for (const auto& move : history) {
    qDebug() << move.algebraicNotation;
}
```

## 設計模式與架構

### 責任分離
- **ChessPiece**: 負責單一棋子的移動規則
- **ChessBoard**: 負責整體棋局邏輯和規則
- **Qt_Chess**: 負責 UI 顯示和使用者互動

### 狀態管理
- 使用 `m_currentPlayer` 追蹤回合
- 使用 `m_enPassantTarget` 追蹤吃過路兵機會
- 使用 `m_moveHistory` 記錄所有移動
- 使用 `m_gameResult` 追蹤遊戲結果

### 驗證與執行分離
- `isValidMove()`: 只驗證，不改變狀態
- `movePiece()`: 執行移動並更新狀態

## 重要演算法

### 將軍檢測算法
```cpp
// 找到國王位置
QPoint kingPos = findKing(color);

// 檢查所有對方棋子是否能攻擊國王
for (對方每個棋子 at position) {
    if (piece.isValidMove(position, kingPos, board)) {
        return true;  // 國王被將軍
    }
}
return false;
```

### 將死檢測算法
```cpp
// 1. 必須處於將軍狀態
if (!isInCheck(color)) return false;

// 2. 嘗試所有可能的移動
for (己方每個棋子 at from) {
    for (所有可能的目標位置 to) {
        if (isValidMove(from, to)) {
            return false;  // 找到解圍方法
        }
    }
}
return true;  // 無法解圍，將死
```

## 效能考量

### 移動驗證的優化
- 先進行快速檢查（邊界、回合、基本規則）
- 最後才進行昂貴的將軍模擬檢查
- 王車易位的多個條件按照計算成本從低到高檢查

### 記憶體管理
- 使用 `std::vector` 管理棋盤，提供高效的隨機存取
- 移動歷史使用動態陣列，避免不必要的記憶體分配

## 相關類別
- `ChessPiece` - 被 ChessBoard 使用來表示棋盤上的棋子
- `ChessEngine` - 使用 ChessBoard 的狀態來與 AI 引擎通訊
- `Qt_Chess` - 使用 ChessBoard 作為遊戲邏輯的核心

## 測試要點
1. 基本移動和吃子
2. 特殊走法（王車易位、吃過路兵、兵升變）
3. 將軍、將死、僵局判定
4. 移動歷史記錄
5. 代數記譜法生成
6. 認輸處理
7. 邊界條件和錯誤輸入
