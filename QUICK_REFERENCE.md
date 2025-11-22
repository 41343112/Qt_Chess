# Quick Reference Guide / 快速參考指南

## English

### What Changed?

The chess application now has a new layout:
- **Left Panel**: Shows time controls and game buttons (width adjusts to content, 80-200px)
- **Chessboard**: Always stays square and centered (uses remaining space)

### Layout Comparison

**Before:**
- Board could stretch horizontally or vertically
- No time display
- Button at bottom

**After:**
- Board always square (1:1 ratio)
- Board centered horizontally and vertically
- Left panel with time controls
- Button moved to left panel
- Better use of space on wide screens

### Expected Behavior

#### Wide Window (e.g., 1200×600)
- Left panel: ~120px wide
- Board: 600×600 (square, limited by height)
- Board centered in remaining horizontal space

#### Tall Window (e.g., 600×1000)
- Left panel: ~120px wide
- Board: 480×480 (square, limited by remaining width)
- Board centered in remaining vertical space

#### Square Window (e.g., 800×800)
- Left panel: ~120px wide
- Board: ~680×680 (square, fills available space)
- Board centered both horizontally and vertically

### Files Modified

1. **New Files:**
   - `aspectratiowidget.h` - Widget header
   - `aspectratiowidget.cpp` - Widget implementation

2. **Modified Files:**
   - `Qt_Chess.pro` - Added new files to build
   - `qt_chess.h` - Added member variables
   - `qt_chess.cpp` - Refactored layout from vertical to horizontal

3. **Documentation:**
   - `ASPECT_RATIO_IMPLEMENTATION.md` - Technical details
   - `VISUAL_TESTING_GUIDE.md` - Testing guide with diagrams
   - `QUICK_REFERENCE.md` - This file

### Build and Test

```bash
# Build the project
qmake Qt_Chess.pro
make

# Run the application
./Qt_Chess
```

### Testing Checklist

- [ ] Application starts without errors
- [ ] Board is square at all window sizes
- [ ] Board stays centered when resizing
- [ ] Left panel width is reasonable (80-200px)
- [ ] All game functions work (move, drag, highlight)
- [ ] Settings menus work
- [ ] Board flip feature works

### Troubleshooting

**Board not square?**
→ Verify AspectRatioWidget is wrapping the board

**Board not centered?**
→ Check resizeEvent() implementation in AspectRatioWidget

**Left panel too wide/narrow?**
→ Adjust minimum/maximum width in setupUI()

---

## 中文

### 有什麼改變？

國際象棋應用程式現在有了新的佈局：
- **左側面板**：顯示時間控制和遊戲按鈕（寬度根據內容調整，80-200像素）
- **棋盤**：始終保持正方形並居中（使用剩餘空間）

### 佈局比較

**之前：**
- 棋盤可能會被水平或垂直拉伸
- 沒有時間顯示
- 按鈕在底部

**之後：**
- 棋盤始終為正方形（1:1 比例）
- 棋盤水平和垂直居中
- 左側面板顯示時間控制
- 按鈕移至左側面板
- 在寬螢幕上更好地利用空間

### 預期行為

#### 寬窗口（例如：1200×600）
- 左側面板：約120像素寬
- 棋盤：600×600（正方形，受高度限制）
- 棋盤在剩餘水平空間中居中

#### 高窗口（例如：600×1000）
- 左側面板：約120像素寬
- 棋盤：480×480（正方形，受剩餘寬度限制）
- 棋盤在剩餘垂直空間中居中

#### 正方形窗口（例如：800×800）
- 左側面板：約120像素寬
- 棋盤：約680×680（正方形，填充可用空間）
- 棋盤水平和垂直居中

### 修改的文件

1. **新文件：**
   - `aspectratiowidget.h` - 小部件頭文件
   - `aspectratiowidget.cpp` - 小部件實現

2. **修改的文件：**
   - `Qt_Chess.pro` - 將新文件添加到構建
   - `qt_chess.h` - 添加成員變量
   - `qt_chess.cpp` - 將佈局從垂直重構為水平

3. **文檔：**
   - `ASPECT_RATIO_IMPLEMENTATION.md` - 技術細節
   - `VISUAL_TESTING_GUIDE.md` - 測試指南（含示意圖）
   - `QUICK_REFERENCE.md` - 本文件

### 構建和測試

```bash
# 構建專案
qmake Qt_Chess.pro
make

# 運行應用程式
./Qt_Chess
```

### 測試清單

- [ ] 應用程式正常啟動，無錯誤
- [ ] 所有窗口大小下棋盤都是正方形
- [ ] 調整大小時棋盤保持居中
- [ ] 左側面板寬度合理（80-200像素）
- [ ] 所有遊戲功能正常（移動、拖放、高亮）
- [ ] 設置菜單正常工作
- [ ] 棋盤翻轉功能正常

### 故障排除

**棋盤不是正方形？**
→ 驗證 AspectRatioWidget 是否正確包裹棋盤

**棋盤沒有居中？**
→ 檢查 AspectRatioWidget 中的 resizeEvent() 實現

**左側面板太寬/太窄？**
→ 在 setupUI() 中調整最小/最大寬度

---

## Key Features / 主要特點

### AspectRatioWidget
A reusable Qt widget that maintains any child widget in a 1:1 aspect ratio (square) and centers it in the available space.

一個可重用的 Qt 小部件，使任何子小部件保持 1:1 縱橫比（正方形）並在可用空間中居中。

**Key Methods / 關鍵方法:**
- `hasHeightForWidth()` - Returns true / 返回 true
- `heightForWidth(int w)` - Returns w / 返回 w
- `resizeEvent()` - Centers content / 使內容居中

### Left Panel Configuration / 左側面板配置

```cpp
// Size policy / 大小策略
leftPanel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

// Width constraints / 寬度約束
leftPanel->setMinimumWidth(80);   // 最小寬度
leftPanel->setMaximumWidth(200);  // 最大寬度
```

### Layout Structure / 佈局結構

```cpp
QHBoxLayout
├── leftPanel (QSizePolicy::Preferred)
│   ├── 白方時間 (White time)
│   ├── 黑方時間 (Black time)
│   └── 新遊戲按鈕 (New game button)
└── boardContainer (AspectRatioWidget, stretch=1)
    └── chessBoard (8×8 grid)
```

---

## Technical Notes / 技術說明

### Why heightForWidth? / 為什麼使用 heightForWidth？

Qt's layout system uses `heightForWidth()` to handle widgets that need to maintain aspect ratios. This is the proper, built-in mechanism for this purpose.

Qt 的佈局系統使用 `heightForWidth()` 來處理需要保持縱橫比的小部件。這是實現此目的的正確內建機制。

### Why Manual Positioning? / 為什麼手動定位？

While we have a layout, manual positioning in `resizeEvent()` is needed to center the square board within potentially rectangular space that AspectRatioWidget occupies.

雖然我們有佈局，但需要在 `resizeEvent()` 中手動定位，以便在 AspectRatioWidget 佔據的可能是矩形的空間內使正方形棋盤居中。

### Performance / 性能

- Minimal overhead: one extra widget wrapper / 最小開銷：一個額外的小部件包裝器
- Simple arithmetic in resize events / 調整大小事件中的簡單算術
- No impact on game logic / 不影響遊戲邏輯

---

## Contact / 聯繫

For issues or questions about this implementation:
關於此實現的問題或疑問：

- Check ASPECT_RATIO_IMPLEMENTATION.md for technical details
  查看 ASPECT_RATIO_IMPLEMENTATION.md 獲取技術細節
- Check VISUAL_TESTING_GUIDE.md for testing procedures
  查看 VISUAL_TESTING_GUIDE.md 獲取測試程序
- Review the code comments in aspectratiowidget.cpp
  查看 aspectratiowidget.cpp 中的代碼註釋

---

**Last Updated / 最後更新:** 2025-11-22
**Version / 版本:** 1.0
