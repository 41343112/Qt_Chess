# 被吃棋子顯示功能 (Captured Pieces Display Feature)

## 概述 (Overview)

這個功能在遊戲中新增了被吃棋子的視覺化顯示，讓玩家可以在對局中即時看到雙方吃掉的棋子。

This feature adds a visual display of captured pieces during the game, allowing players to see the pieces captured by both sides in real-time during gameplay.

## 功能說明 (Feature Description)

### 顯示位置 (Display Position)

根據需求：
- **白方吃掉的棋子**：顯示在白方時間標籤**上面**
- **黑方吃掉的棋子**：顯示在黑方時間標籤**下面**

According to the requirements:
- **Pieces captured by White**: Displayed **above** the white player's time label
- **Pieces captured by Black**: Displayed **below** the black player's time label

### 視覺呈現 (Visual Presentation)

- 被吃的棋子使用 Unicode 棋子符號顯示（♔♕♖♗♘♙）
- 標籤背景為半透明白色，文字為黑色
- 標籤會自動換行以適應不同數量的棋子
- 當沒有被吃的棋子時，標籤自動隱藏
- 當有棋子被吃時，標籤自動顯示

Visual representation:
- Captured pieces are shown using Unicode chess symbols (♔♕♖♗♘♙)
- Label background is semi-transparent white with black text
- Labels automatically wrap to accommodate different numbers of pieces
- Labels are hidden when there are no captured pieces
- Labels are shown automatically when pieces are captured

### 佈局結構 (Layout Structure)

```
┌─────────────┐                    ┌─────────────┐
│ Black Time  │                    │ White Capt  │
│   (黑方)    │                    │  (白方吃)   │
├─────────────┤    ┌────────┐     ├─────────────┤
│ Black Capt  │    │        │     │ White Time  │
│  (黑方吃)   │ ◄─►│ Board  │◄─►  │   (白方)    │
└─────────────┘    │        │     └─────────────┘
   Left Panel      └────────┘        Right Panel
```

## 技術實現 (Technical Implementation)

### 後端修改 (Backend Changes)

**ChessBoard 類別 (chessboard.h/cpp):**

1. **新增成員變數**：
   - `std::vector<PieceType> m_whiteCapturedPieces` - 儲存白方吃掉的棋子
   - `std::vector<PieceType> m_blackCapturedPieces` - 儲存黑方吃掉的棋子

2. **建構函式更新**：
   - 在初始化列表中初始化被吃棋子向量

3. **新增公開方法**：
   - `getWhiteCapturedPieces()` - 取得白方吃掉的棋子列表
   - `getBlackCapturedPieces()` - 取得黑方吃掉的棋子列表

4. **修改 `movePiece()` 方法**：
   - 在吃子時將被吃的棋子加入對應的列表

5. **更新 `initializeBoard()` 和 `resetScores()` 方法**：
   - 清空被吃棋子列表

**ChessBoard class (chessboard.h/cpp):**

1. **New member variables**:
   - `std::vector<PieceType> m_whiteCapturedPieces` - Stores pieces captured by white
   - `std::vector<PieceType> m_blackCapturedPieces` - Stores pieces captured by black

2. **Constructor update**:
   - Initialize captured pieces vectors in the initializer list

3. **New public methods**:
   - `getWhiteCapturedPieces()` - Get list of pieces captured by white
   - `getBlackCapturedPieces()` - Get list of pieces captured by black

4. **Modified `movePiece()` method**:
   - Add captured pieces to the appropriate list when a capture occurs

5. **Updated `initializeBoard()` and `resetScores()` methods**:
   - Clear the captured pieces lists

### 前端修改 (Frontend Changes)

**Qt_Chess 類別 (qt_chess.h/cpp):**

1. **新增成員變數**：
   - `QLabel* m_whiteCapturedPiecesLabel` - 顯示白方吃掉的棋子
   - `QLabel* m_blackCapturedPiecesLabel` - 顯示黑方吃掉的棋子

2. **佈局重構 (setupUI())**：
   - 創建左側面板（`leftPanel`）：
     - 黑方時間標籤（頂部）
     - 黑方被吃棋子標籤（在時間下方）
   - 創建右側面板（`rightPanel`）：
     - 白方被吃棋子標籤（在時間上方）
     - 白方時間標籤（底部）
   - 使用 `QVBoxLayout` 垂直排列每側的元素

3. **新增方法**：
   - `updateCapturedPiecesDisplay()` - 更新被吃棋子顯示
     - 從 ChessBoard 獲取被吃棋子列表
     - 將 PieceType 轉換為 Unicode 符號
     - 更新標籤文字
     - 控制標籤的可見性

4. **連接更新**：
   - 在 `updateBoard()` 方法中調用 `updateCapturedPiecesDisplay()`

**Qt_Chess class (qt_chess.h/cpp):**

1. **New member variables**:
   - `QLabel* m_whiteCapturedPiecesLabel` - Display pieces captured by white
   - `QLabel* m_blackCapturedPiecesLabel` - Display pieces captured by black

2. **Layout restructuring (setupUI())**:
   - Created left panel (`leftPanel`):
     - Black time label (top)
     - Black captured pieces label (below time)
   - Created right panel (`rightPanel`):
     - White captured pieces label (above time)
     - White time label (bottom)
   - Used `QVBoxLayout` to vertically arrange elements on each side

3. **New method**:
   - `updateCapturedPiecesDisplay()` - Update captured pieces display
     - Get captured pieces lists from ChessBoard
     - Convert PieceType to Unicode symbols
     - Update label text
     - Control label visibility

4. **Update connection**:
   - Call `updateCapturedPiecesDisplay()` in the `updateBoard()` method

## 使用說明 (Usage)

1. **遊戲進行中**：
   - 當任何一方吃掉對方的棋子時，被吃的棋子會立即顯示在相應的位置
   - 白方吃掉的黑方棋子顯示在白方時間上面
   - 黑方吃掉的白方棋子顯示在黑方時間下面

2. **棋子順序**：
   - 棋子按照被吃的順序從左到右顯示
   - 每個棋子使用 Unicode 符號表示

3. **新遊戲**：
   - 點擊「新遊戲」按鈕時，被吃棋子顯示會自動清空

**How to use:**

1. **During gameplay**:
   - When either player captures an opponent's piece, it immediately appears in the appropriate location
   - Black pieces captured by white are displayed above white's time
   - White pieces captured by black are displayed below black's time

2. **Piece order**:
   - Pieces are displayed left-to-right in the order they were captured
   - Each piece is represented by its Unicode symbol

3. **New game**:
   - When clicking the "New Game" button, the captured pieces display is automatically cleared

## 樣式設定 (Styling)

### 被吃棋子標籤樣式 (Captured Pieces Label Style)

```css
QLabel {
    background-color: rgba(255, 255, 255, 180);  /* 半透明白色背景 */
    color: #000;                                  /* 黑色文字 */
    padding: 5px;                                 /* 內邊距 */
    border-radius: 3px;                           /* 圓角 */
}
```

- 字體大小：16 點
- 對齊方式：居中
- 自動換行：啟用
- 最小尺寸：100x20 像素

Font size: 16 points  
Alignment: Center  
Word wrap: Enabled  
Minimum size: 100x20 pixels

## 特殊情況處理 (Special Cases)

### 吃過路兵 (En Passant)
當兵通過吃過路兵規則吃掉對方的兵時，被吃的兵會正確顯示在被吃棋子列表中。

When a pawn captures via en passant, the captured pawn is correctly displayed in the captured pieces list.

### 兵升變 (Pawn Promotion)
兵升變後被吃掉時，顯示的是升變後的棋子類型（如皇后、城堡等）。

When a promoted pawn is captured, the displayed piece type is the promoted piece (e.g., queen, rook).

## 相關檔案 (Related Files)

- `chessboard.h` - ChessBoard 類別標頭檔
- `chessboard.cpp` - ChessBoard 類別實作檔
- `qt_chess.h` - Qt_Chess 主視窗標頭檔
- `qt_chess.cpp` - Qt_Chess 主視窗實作檔

## 與其他功能的整合 (Integration with Other Features)

### 時間控制功能 (Time Control Feature)
被吃棋子顯示與時間控制功能完美整合，當時間標籤顯示時，被吃棋子會顯示在適當的位置。

The captured pieces display integrates seamlessly with the time control feature, appearing in the appropriate positions when time labels are visible.

### 吃子分數功能 (Scoring Feature)
被吃棋子顯示是對現有吃子分數功能的補充，提供視覺化的棋子資訊，而不僅僅是數字分數。

The captured pieces display complements the existing scoring feature by providing visual piece information rather than just numerical scores.

### 回放功能 (Replay Feature)
在回放模式下，被吃棋子顯示會根據當前回放的棋步正確更新。

In replay mode, the captured pieces display correctly updates based on the current replay position.
