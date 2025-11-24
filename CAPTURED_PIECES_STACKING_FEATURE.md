# 被吃棋子堆疊顯示功能 (Captured Pieces Stacking Feature)

## 概述 (Overview)

此功能增強了被吃棋子的顯示方式，實現了三個主要改進：
1. 棋子按照價值從小到大排序
2. 棋子部分重疊顯示（堆疊效果）
3. 顯示分數差異（物質優勢）

This feature enhances the display of captured pieces with three main improvements:
1. Pieces are sorted by value from small to large
2. Pieces are displayed with partial overlap (stacking effect)
3. Score difference (material advantage) is displayed

## 功能說明 (Feature Description)

### 1. 棋子排序 (Piece Sorting)

被吃的棋子現在按照國際象棋標準價值從小到大排列：

Captured pieces are now arranged by standard chess values from small to large:

- **兵 (Pawn)**: 價值 1 / Value 1
- **騎士 (Knight)**: 價值 3 / Value 3
- **主教 (Bishop)**: 價值 3 / Value 3
- **城堡 (Rook)**: 價值 5 / Value 5
- **皇后 (Queen)**: 價值 9 / Value 9

**範例 (Example):**
- 原來 (Before): ♛♜♞♟♟♜ (按照吃子順序)
- 現在 (After): ♟♟♞♜♜♛ (按照價值排序)

### 2. 堆疊顯示 (Stacking Display)

棋子符號現在使用部分重疊的方式顯示，而不是完全分開：

Piece symbols are now displayed with partial overlap instead of being completely separate:

**技術實現 (Technical Implementation):**
- 使用 HTML/CSS 格式化
- 每個棋子（除了第一個）使用負邊距 `-8px` 創建重疊效果
- 第一個棋子完整顯示，後續棋子向左偏移

**視覺效果 (Visual Effect):**
```
原來 (Before): ♟ ♞ ♜ ♛ (完全分開)
現在 (After):  ♟♞♜♛    (部分重疊)
```

### 3. 分數差異顯示 (Score Difference Display)

分數標籤現在顯示物質優勢：

Score labels now show material advantage:

**顯示格式 (Display Format):**
- 當白方領先時 (When white is ahead): `白方: 12 (+3)` / `黑方: 9`
- 當黑方領先時 (When black is ahead): `白方: 5` / `黑方: 8 (+3)`
- 當分數相等時 (When scores are equal): `白方: 10` / `黑方: 10`

優勢方的分數後面會顯示 `(+N)`，其中 N 是分數差異。

The leading player's score shows `(+N)` where N is the score difference.

## 技術實現細節 (Technical Implementation Details)

### 修改的檔案 (Modified Files)

#### qt_chess.h
新增了輔助函數宣告：
```cpp
int getPieceValue(PieceType type) const;  // 取得棋子價值的輔助函數
```

#### qt_chess.cpp

**新增包含檔案 (New Include):**
```cpp
#include <algorithm>  // For std::sort
```

**新增函數 (New Function):**
```cpp
int Qt_Chess::getPieceValue(PieceType type) const
```
返回棋子的標準價值。

Returns the standard value of a piece.

**修改函數 (Modified Functions):**

1. `updateScoreDisplay()`
   - 計算分數差異
   - 在領先方的分數後顯示優勢

2. `updateCapturedPiecesDisplay()`
   - 複製被吃棋子向量以進行排序
   - 使用 `std::sort` 和自定義 lambda 比較器按價值排序
   - 使用 HTML 格式和負邊距創建堆疊效果
   - 設置標籤為 `Qt::RichText` 格式以支持 HTML

### 排序邏輯 (Sorting Logic)

```cpp
std::sort(captured.begin(), captured.end(), 
    [this](PieceType a, PieceType b) {
        int valueA = getPieceValue(a);
        int valueB = getPieceValue(b);
        if (valueA != valueB) {
            return valueA < valueB;  // 按價值排序
        }
        // 如果價值相同，按類型排序以保持一致性
        return static_cast<int>(a) < static_cast<int>(b);
    });
```

### HTML 堆疊效果 (HTML Stacking Effect)

```cpp
whiteCapturedText = "<div style='display: inline;'>";
for (size_t i = 0; i < whiteCaptured.size(); ++i) {
    ChessPiece piece(whiteCaptured[i], PieceColor::Black);
    if (i > 0) {
        // 使用負邊距創建重疊效果
        whiteCapturedText += QString("<span style='margin-left: -8px;'>%1</span>")
                              .arg(piece.getSymbol());
    } else {
        whiteCapturedText += QString("<span>%1</span>").arg(piece.getSymbol());
    }
}
whiteCapturedText += "</div>";
```

## 使用範例 (Usage Examples)

### 範例 1: 白方吃掉多個棋子 (Example 1: White Captures Multiple Pieces)

**被吃棋子 (Captured in order):**
1. 黑方兵 (Black pawn) ♟
2. 黑方騎士 (Black knight) ♞
3. 黑方主教 (Black bishop) ♝
4. 黑方兵 (Black pawn) ♟

**顯示結果 (Display result):**
- 排序後 (After sorting): ♟♟♞♝ (價值: 1, 1, 3, 3)
- 堆疊顯示 (With stacking): ♟♟♞♝ (部分重疊)
- 分數顯示 (Score display): 白方: 8 (+8) / 黑方: 0

### 範例 2: 雙方都有吃子且黑方領先 (Example 2: Both Sides Capture, Black Ahead)

**白方吃掉 (White captures):**
- 黑方兵 ♟ (1)
- 黑方騎士 ♞ (3)
- 總分: 4

**黑方吃掉 (Black captures):**
- 白方城堡 ♖ (5)
- 白方主教 ♗ (3)
- 總分: 8

**顯示結果 (Display result):**
- 白方被吃棋子標籤: ♟♞ (1, 3)
- 黑方被吃棋子標籤: ♗♖ (3, 5)
- 分數顯示: 白方: 4 / 黑方: 8 (+4)

## 可調整參數 (Adjustable Parameters)

### 重疊量 (Overlap Amount)

當前設定: `-8px`

Current setting: `-8px`

如果需要調整重疊程度，可以修改 `updateCapturedPiecesDisplay()` 中的值：

To adjust the overlap amount, modify the value in `updateCapturedPiecesDisplay()`:

```cpp
whiteCapturedText += QString("<span style='margin-left: -8px;'>%1</span>")
                                                      // ^^^
                                                      // 調整此值
```

**建議範圍 (Suggested range):**
- `-4px`: 輕微重疊 (Light overlap)
- `-8px`: 中等重疊 (Medium overlap) - 當前設定
- `-12px`: 重度重疊 (Heavy overlap)

## 相容性 (Compatibility)

### 與其他功能的整合 (Integration with Other Features)

- ✅ **時間控制功能**: 完全相容
- ✅ **回放功能**: 回放時被吃棋子顯示會正確更新
- ✅ **棋盤翻轉**: 不受影響
- ✅ **自定義棋子圖示**: 使用 Unicode 符號，不依賴圖示設定

- ✅ **Time Control Feature**: Fully compatible
- ✅ **Replay Feature**: Captured pieces display correctly updates during replay
- ✅ **Board Flip**: Not affected
- ✅ **Custom Piece Icons**: Uses Unicode symbols, independent of icon settings

## 測試場景 (Test Scenarios)

### 基本測試 (Basic Tests)

1. **空列表測試 (Empty List Test)**
   - 新遊戲開始時，兩個標籤都應該隱藏

2. **單個棋子測試 (Single Piece Test)**
   - 吃掉一個棋子後，應該顯示該棋子且無重疊效果

3. **多個相同價值棋子測試 (Multiple Same-Value Pieces Test)**
   - 吃掉多個兵：♟♟♟（應該按類型排序）

4. **混合價值棋子測試 (Mixed Value Pieces Test)**
   - 吃掉順序：皇后、兵、城堡、騎士
   - 顯示順序：♟♞♜♛ (1, 3, 5, 9)

### 分數差異測試 (Score Difference Tests)

1. **白方領先測試 (White Ahead Test)**
   - 白方: 10 分
   - 黑方: 5 分
   - 顯示: `白方: 10 (+5)` / `黑方: 5`

2. **黑方領先測試 (Black Ahead Test)**
   - 白方: 3 分
   - 黑方: 8 分
   - 顯示: `白方: 3` / `黑方: 8 (+5)`

3. **平等測試 (Equal Test)**
   - 白方: 12 分
   - 黑方: 12 分
   - 顯示: `白方: 12` / `黑方: 12`

## 已知限制 (Known Limitations)

1. **HTML 渲染**: 重疊效果依賴 Qt 的 HTML/CSS 支持，在某些平台上可能略有不同

2. **Unicode 字體**: 需要系統支持 Unicode 棋子符號的字體

3. **固定重疊量**: 重疊量是固定的 `-8px`，不會根據字體大小自動調整

HTML rendering: Overlap effect depends on Qt's HTML/CSS support, which may vary slightly across platforms.

Unicode fonts: System must support fonts with Unicode chess piece symbols.

Fixed overlap: Overlap amount is fixed at `-8px`, doesn't automatically adjust based on font size.

## 未來改進建議 (Future Improvement Suggestions)

1. **響應式重疊**: 根據字體大小和視窗大小動態調整重疊量

2. **顏色編碼**: 為不同價值的棋子添加顏色提示

3. **動畫效果**: 當新棋子被吃掉時添加過渡動畫

4. **工具提示**: 滑鼠懸停時顯示每個棋子的詳細信息

Responsive overlap: Dynamically adjust overlap based on font size and window size.

Color coding: Add color hints for pieces of different values.

Animation: Add transition animation when new pieces are captured.

Tooltips: Show detailed information for each piece on hover.

## 相關檔案 (Related Files)

- `qt_chess.h` - 標頭檔 (Header file)
- `qt_chess.cpp` - 實作檔 (Implementation file)
- `chessboard.h` - ChessBoard 類別標頭檔
- `chessboard.cpp` - ChessBoard 類別實作檔
- `chesspiece.h` - ChessPiece 類別標頭檔
- `chesspiece.cpp` - ChessPiece 類別實作檔
