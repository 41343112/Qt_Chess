# 吃子紀錄容器寬度修復 (Captured Pieces Container Width Fix)

## 問題描述 (Problem Description)
吃子紀錄顯示無法重疊。Captured pieces display cannot overlap properly due to collapsed container.

## 根本原因 (Root Cause)
使用 `position:absolute` 的子元素會脫離文件流，不會對父容器的尺寸產生貢獻。當容器 div 沒有明確設定寬度時，它會收縮到接近零寬度，導致吃子棋子的顯示被截斷或無法正確顯示。

When using `position:absolute` for child elements, they are removed from the document flow and don't contribute to the parent container's dimensions. Without an explicit width, the container div collapses to near-zero width, causing captured pieces to be cut off or displayed incorrectly.

## 技術細節 (Technical Details)

### 問題來源 (Origin)
在之前的修復中 (PR #162)，為了解決 QTextDocument 不支援負邊距的問題，採用了絕對定位的方式。但是忽略了需要為容器設定明確寬度的問題。

In the previous fix (PR #162), absolute positioning was used to solve the issue of QTextDocument not supporting negative margins. However, the need to set an explicit width for the container was overlooked.

### 修改前的問題 (Before - The Issue)
```cpp
QString html = "<html><body style='margin:0; padding:0;'>";
html += QString("<div style='position:relative; white-space:nowrap; height:%1px;'>")
            .arg(CONTAINER_HEIGHT);

// ... 在迴圈中建立絕對定位的子元素
html += QString("<span style='position:absolute; left:%1px; ...'>...</span>")
            .arg(currentLeft);

html += "</div></body></html>";
```

**問題 (Problems)**:
- ❌ 容器 div 沒有明確寬度
- ❌ 絕對定位的子元素不影響容器寬度
- ❌ 容器寬度收縮，可能截斷棋子顯示

### 修改後的解決方案 (After - The Solution)
```cpp
// 用於追蹤相同棋子以實現重疊效果
PieceType lastPieceType = PieceType::None;
int sameTypeCount = 0;
int currentLeft = 0;  // 追蹤當前的水平位置

// 建立內容字串
QString content;

for (size_t i = 0; i < sortedPieces.size(); ++i) {
    // ... 計算位置並建立內容
    content += QString("<span style='position:absolute; left:%1px; ...'>...</span>")
                .arg(currentLeft);
}

// 計算容器所需的總寬度
// 寬度應該是最後一個棋子的位置加上棋子寬度
int containerWidth = sortedPieces.empty() ? 0 : (currentLeft + PIECE_WIDTH);

// 組裝最終的 HTML，包含正確的容器寬度
QString html = "<html><body style='margin:0; padding:0;'>";
html += QString("<div style='position:relative; white-space:nowrap; height:%1px; width:%2px;'>")
            .arg(CONTAINER_HEIGHT)
            .arg(containerWidth);
html += content;
html += "</div></body></html>";
return html;
```

**優點 (Advantages)**:
- ✅ 明確計算並設定容器寬度
- ✅ 確保所有棋子都能正確顯示
- ✅ 支援任意數量的棋子和重疊模式
- ✅ 程式碼結構更清晰

## 寬度計算 (Width Calculation)

### 公式 (Formula)
```
containerWidth = currentLeft + PIECE_WIDTH
```

其中：
- `currentLeft`: 最後一個棋子的左側位置（像素）
- `PIECE_WIDTH`: 單個棋子的寬度（18像素）

### 範例 (Examples)

#### 範例 1: 單個棋子 (Single Piece)
輸入: [Pawn]
- Pawn at left=0px
- containerWidth = 0 + 18 = **18px**

#### 範例 2: 兩個相同棋子 (Two Same Pieces)
輸入: [Pawn, Pawn]
- Pawn 1 at left=0px
- Pawn 2 at left=10px (重疊 8px)
- containerWidth = 10 + 18 = **28px**

#### 範例 3: 四個棋子 (Four Pieces)
輸入: [Pawn, Pawn, Knight, Knight]
- Pawn 1 at left=0px
- Pawn 2 at left=10px (與 Pawn 1 重疊 8px)
- Knight 1 at left=28px (與 Pawn 2 不重疊，10 + 18)
- Knight 2 at left=38px (與 Knight 1 重疊 8px，28 + 10)
- containerWidth = 38 + 18 = **56px**

#### 範例 4: 空列表 (Empty List)
輸入: []
- containerWidth = **0px**

## 修改的檔案 (Modified Files)
- `src/qt_chess.cpp` (renderCapturedPieces 函數)
  - 第 2050-2135 行

## 測試驗證 (Testing & Verification)

### 編譯測試 (Compilation Test)
```bash
qmake Qt_Chess.pro
make
```
結果: ✅ 編譯成功無錯誤

### 邏輯測試 (Logic Test)
使用獨立的 C++ 程式測試寬度計算邏輯：
- ✅ 空列表: containerWidth = 0px
- ✅ 單個棋子: containerWidth = 18px
- ✅ 兩個相同棋子: containerWidth = 28px
- ✅ 多個不同棋子: containerWidth 正確計算

### 下一步測試 (Next Steps)
- [ ] UI 測試：驗證吃子棋子在實際遊戲中正確顯示
- [ ] 視覺回歸測試：確保沒有破壞現有功能

## CSS 屬性說明 (CSS Properties Explained)

### position:relative (容器)
- 建立定位上下文
- 子元素的絕對定位相對於此容器

### position:absolute (子元素)
- 脫離文檔流
- 不影響父容器尺寸
- 相對於最近的 positioned 祖先定位

### 為什麼需要明確寬度 (Why Explicit Width is Needed)
因為絕對定位的元素不參與正常的文件流佈局，父容器無法自動計算其所需的寬度。必須手動計算並設定。

Because absolutely positioned elements don't participate in normal document flow layout, the parent container cannot automatically calculate the required width. It must be manually calculated and set.

## 相關文件 (Related Documentation)
- [CAPTURED_PIECES_OVERLAP_FIX.md](CAPTURED_PIECES_OVERLAP_FIX.md) - 原始的重疊問題修復
- Qt Documentation: [QTextDocument Supported HTML Subset](https://doc.qt.io/qt-5/richtext-html-subset.html)
- MDN: [CSS position](https://developer.mozilla.org/en-US/docs/Web/CSS/position)

## 問題追蹤 (Issue Tracking)
- Issue: 找出吃子紀錄顯示無法重疊的問題
- PR: copilot/fix-eat-record-overlap-issue

## 總結 (Summary)
此修復確保了吃子紀錄的容器有正確的寬度，使得使用絕對定位的棋子圖示能夠完整顯示，並正確實現重疊效果。

This fix ensures that the captured pieces container has the correct width, allowing absolutely positioned piece icons to be fully displayed and properly overlap.
