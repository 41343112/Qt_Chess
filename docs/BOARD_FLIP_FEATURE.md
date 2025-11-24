# 棋盤反轉功能 (Board Flip Feature)

## 功能說明 (Feature Description)

新增了棋盤反轉功能，允許玩家從不同視角查看棋盤。此功能在雙人對弈時特別有用，讓坐在棋盤另一側的玩家能夠從自己的視角觀看棋局。

The board flip feature allows players to view the chess board from different perspectives. This is especially useful in player-vs-player games, enabling players sitting on opposite sides to view the board from their own perspective.

## 使用方法 (How to Use)

1. 點擊頂部選單欄的「設定」(Settings) 選單
2. 選擇「反轉棋盤」(Flip Board) 選項
3. 棋盤將會立即旋轉 180 度
4. 再次點擊相同選項即可恢復原始視角

1. Click the "設定" (Settings) menu in the menu bar
2. Select "反轉棋盤" (Flip Board) option
3. The board will immediately rotate 180 degrees
4. Click the same option again to restore the original view

## 技術實現 (Technical Implementation)

### 座標轉換 (Coordinate Transformation)

當棋盤被反轉時，顯示座標需要與邏輯座標進行轉換：

When the board is flipped, display coordinates need to be converted to/from logical coordinates:

- **顯示座標 (Display Coordinates)**: m_squares[row][col] 陣列中按鈕的位置
- **邏輯座標 (Logical Coordinates)**: 棋盤邏輯中棋子的實際位置

轉換公式 (Conversion formulas):
```cpp
displayRow = isBoardFlipped ? (7 - logicalRow) : logicalRow
displayCol = isBoardFlipped ? (7 - logicalCol) : logicalCol
```

### 持久化設定 (Persistent Settings)

棋盤反轉狀態會自動儲存到使用者設定中，並在下次啟動應用程式時恢復。

The board flip state is automatically saved to user settings and restored when the application starts.

## 影響範圍 (Affected Components)

1. **updateBoard()**: 更新棋盤顯示時使用座標轉換
2. **onSquareClicked()**: 點擊處理時轉換顯示座標為邏輯座標
3. **highlightValidMoves()**: 高亮顯示時使用座標轉換
4. **applyCheckHighlight()**: 將軍高亮時使用座標轉換
5. **mousePressEvent()**: 拖放開始時轉換座標
6. **mouseReleaseEvent()**: 拖放結束時轉換座標
7. **restorePieceToSquare()**: 恢復棋子時使用座標轉換

All mouse interactions and board updates properly handle coordinate transformation to ensure correct functionality when the board is flipped.
