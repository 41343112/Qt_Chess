# 吃子分數功能 (Captured Piece Scoring Feature)

## 概述 (Overview)

這個功能在遊戲中新增了吃子分數追蹤系統，讓玩家可以即時看到雙方吃子的分數累計。

This feature adds a captured piece scoring system to the game, allowing players to see the accumulated score from captured pieces in real-time.

## 功能說明 (Feature Description)

### 分數計算規則 (Scoring Rules)

遊戲使用標準的國際象棋棋子價值計算分數：

The game uses standard chess piece values to calculate scores:

- **兵 (Pawn)**: 1 分
- **騎士 (Knight)**: 3 分
- **主教 (Bishop)**: 3 分
- **城堡 (Rook)**: 5 分
- **皇后 (Queen)**: 9 分
- **國王 (King)**: 0 分（國王被吃代表遊戲結束，不計分）

### 分數顯示 (Score Display)

- 分數顯示在右側面板的「吃子分數」群組框中
- 白方和黑方的分數分別顯示
- 分數會在每次吃子後自動更新
- 新遊戲開始時分數會自動重置為 0

The scores are displayed in the "吃子分數" (Captured Pieces Score) group box in the right panel:
- White and black scores are displayed separately
- Scores are automatically updated after each capture
- Scores are automatically reset to 0 when a new game starts

### 特殊情況處理 (Special Cases)

#### 吃過路兵 (En Passant)
當兵通過吃過路兵規則吃掉對方的兵時，分數會正確增加 1 分。

When a pawn captures another pawn via en passant, the score correctly increases by 1 point.

#### 兵升變 (Pawn Promotion)
兵升變本身不會影響分數，但升變後的棋子如果被吃掉，會根據升變後的棋子類型計分。

Pawn promotion itself doesn't affect the score, but if the promoted piece is captured, it will be scored according to its promoted piece type.

## 技術實現 (Technical Implementation)

### 後端 (Backend)

**ChessBoard 類別修改：**
- 新增 `m_whiteScore` 和 `m_blackScore` 成員變數
- 新增 `getPieceValue()` 私有方法來計算棋子價值
- 修改 `movePiece()` 方法以追蹤被吃的棋子並更新分數
- 新增公開方法：
  - `getWhiteScore()` - 取得白方分數
  - `getBlackScore()` - 取得黑方分數
  - `resetScores()` - 重置分數（在 `initializeBoard()` 中自動調用）

**ChessBoard class modifications:**
- Added `m_whiteScore` and `m_blackScore` member variables
- Added `getPieceValue()` private method to calculate piece values
- Modified `movePiece()` method to track captured pieces and update scores
- Added public methods:
  - `getWhiteScore()` - Get white's score
  - `getBlackScore()` - Get black's score
  - `resetScores()` - Reset scores (automatically called in `initializeBoard()`)

### 前端 (Frontend)

**Qt_Chess 類別修改：**
- 新增 `m_whiteScoreLabel` 和 `m_blackScoreLabel` 標籤元件
- 在 `setupTimeControlUI()` 中新增分數顯示群組框
- 新增 `updateScoreDisplay()` 方法以更新 UI 顯示
- 在 `updateBoard()` 中調用 `updateScoreDisplay()` 以保持分數同步

**Qt_Chess class modifications:**
- Added `m_whiteScoreLabel` and `m_blackScoreLabel` label components
- Added score display group box in `setupTimeControlUI()`
- Added `updateScoreDisplay()` method to update UI display
- Call `updateScoreDisplay()` in `updateBoard()` to keep scores synchronized

## 使用說明 (Usage)

1. **查看分數**：遊戲進行中，右側面板會即時顯示雙方的吃子分數
2. **分數累計**：每次成功吃掉對方棋子時，分數會自動增加
3. **開始新遊戲**：點擊「新遊戲」按鈕時，分數會自動重置為 0

**How to use:**
1. **View scores**: During the game, the right panel displays real-time scores for both players
2. **Score accumulation**: Scores automatically increase each time you capture an opponent's piece
3. **Start new game**: When you click the "New Game" button, scores automatically reset to 0

## 相關檔案 (Related Files)

- `chessboard.h` - ChessBoard 類別標頭檔
- `chessboard.cpp` - ChessBoard 類別實作檔
- `qt_chess.h` - Qt_Chess 主視窗標頭檔
- `qt_chess.cpp` - Qt_Chess 主視窗實作檔
