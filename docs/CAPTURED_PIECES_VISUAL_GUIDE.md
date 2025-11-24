# 吃子紀錄視覺指南 (Captured Pieces Visual Guide)

## 功能概述 (Feature Overview)

吃子紀錄功能會在遊戲進行時自動顯示被吃掉的棋子，並計算雙方的子力差。此功能整合在時間控制面板中，當計時器啟動後會自動顯示。

The captured pieces record feature automatically displays captured pieces during gameplay and calculates material advantage. This feature is integrated into the time control panel and becomes visible when the timer is started.

---

## UI 佈局 (UI Layout)

### 完整介面佈局 (Complete Interface Layout)

```
+-------------------------------------------------------------------------+
|  選單列 (Menu Bar): [遊戲 ▼] [設定 ▼]                                   |
+-------------------------------------------------------------------------+
|                                                                         |
|  +----------------+  +--------------------------+  +------------------+ |
|  | 棋譜列表       |  |                          |  | 時間控制設定     | |
|  | (Move List)    |  |                          |  |                  | |
|  |                |  |      棋盤 (Chessboard)    |  | 總時間: [10分鐘] | |
|  | 1. e4 e5       |  |      (8 x 8 squares)     |  | 每著加秒: [5秒]  | |
|  | 2. Nf3 Nc6     |  |                          |  |                  | |
|  | 3. Bb5 a6      |  |                          |  | [開始] 按鈕      | |
|  | 4. Bxc6 ...    |  |                          |  |                  | |
|  |                |  |                          |  +------------------+ |
|  | [匯出 PGN]     |  |                          |  | 計時器顯示       | |
|  | [複製棋譜]     |  |                          |  |                  | |
|  |                |  |                          |  | 黑方時間:        | |
|  | [回放控制]     |  |                          |  | ┌──────────────┐ | |
|  | ⏮ ◀ ▶ ⏭      |  |                          |  | │   09:58      │ | |
|  |                |  |                          |  | └──────────────┘ | |
|  +----------------+  |                          |  |                  | |
|                      |                          |  | 吃子紀錄 (黑方):  | |
|  當前回合: 白方       |                          |  | ♞ ♟♟ +2         | |
|  [新遊戲]            |                          |  |    ↑      ↑      | |
|                      +--------------------------+  | 騎士  兵   優勢   | |
|                                                    |                  | |
|                                                    | 白方時間:        | |
|                                                    | ┌──────────────┐ | |
|                                                    | │   09:52      │ | |
|                                                    | └──────────────┘ | |
|                                                    |                  | |
|                                                    | 吃子紀錄 (白方):  | |
|                                                    | ♞ ♟             | |
|                                                    |                  | |
|                                                    | [放棄] 按鈕      | |
|                                                    +------------------+ |
+-------------------------------------------------------------------------+
```

---

## 吃子紀錄顯示細節 (Captured Pieces Display Details)

### 位置說明 (Position Description)

吃子紀錄顯示在時間控制面板中：
- **黑方吃掉的子**：顯示在黑方時間下方
- **白方吃掉的子**：顯示在白方時間上方

The captured pieces are displayed in the time control panel:
- **Pieces captured by Black**: Shown below the Black timer
- **Pieces captured by White**: Shown above the White timer

### 顯示格式 (Display Format)

```
┌─────────────────────────────┐
│ 吃子紀錄範例 (Examples)      │
├─────────────────────────────┤
│                             │
│ 1. 單個棋子 (Single piece)   │
│    ♟                        │
│                             │
│ 2. 相同類型重疊              │
│    (Same type overlap)      │
│    ♟♟♟                      │
│    (3個兵重疊顯示)           │
│                             │
│ 3. 不同類型分開              │
│    (Different types)        │
│    ♟ ♞ ♝                   │
│    (兵 騎士 主教 分開顯示)   │
│                             │
│ 4. 混合顯示 (Mixed)          │
│    ♟♟ ♞♞ ♝                 │
│    (2兵 2騎士 1主教)         │
│                             │
│ 5. 包含子力優勢              │
│    (With advantage)         │
│    ♛ ♜♜ ♟♟♟ +8             │
│    (皇后 2城堡 3兵 領先8分)  │
│                             │
└─────────────────────────────┘
```

---

## 棋子價值系統 (Piece Value System)

用於計算子力優勢的標準棋子價值：

```
┌──────────┬────────┬─────────┐
│ 棋子類型 │ 符號   │ 價值    │
│ Piece    │ Symbol │ Value   │
├──────────┼────────┼─────────┤
│ 兵 Pawn  │ ♟ ♙    │   1     │
│ 騎士     │ ♞ ♘    │   3     │
│ Knight   │        │         │
│ 主教     │ ♝ ♗    │   3     │
│ Bishop   │        │         │
│ 城堡     │ ♜ ♖    │   5     │
│ Rook     │        │         │
│ 皇后     │ ♛ ♕    │   9     │
│ Queen    │        │         │
│ 國王     │ ♚ ♔    │   -     │
│ King     │        │ (不計分) │
└──────────┴────────┴─────────┘
```

---

## 實際遊戲範例 (Real Game Examples)

### 範例 1: 早期遊戲 (Early Game)

```
情況：白方吃了一個黑兵，黑方吃了一個白兵
Scenario: White captured one black pawn, Black captured one white pawn

右側面板顯示：
┌──────────────────┐
│ 黑方時間:        │
│ ┌──────────────┐ │
│ │   09:45      │ │
│ └──────────────┘ │
│                  │
│ 吃子紀錄:        │
│ ♟               │  ← 黑方吃了白方的兵
│                  │
│ 白方時間:        │
│ ┌──────────────┐ │
│ │   09:40      │ │
│ └──────────────┘ │
│                  │
│ 吃子紀錄:        │
│ ♟               │  ← 白方吃了黑方的兵
│                  │
└──────────────────┘

子力狀況：均勢 (Material: Equal)
```

### 範例 2: 中局交換 (Middle Game - Material Exchange)

```
情況：白方吃了黑方的皇后和兵，黑方吃了白方的城堡和兩個兵
Scenario: White captured Queen + Pawn, Black captured Rook + 2 Pawns

右側面板顯示：
┌──────────────────┐
│ 黑方時間:        │
│ ┌──────────────┐ │
│ │   07:23      │ │
│ └──────────────┘ │
│                  │
│ 吃子紀錄:        │
│ ♟♟ ♜            │  ← 黑方吃了：2兵、1城堡 (= 7分)
│                  │
│ 白方時間:        │
│ ┌──────────────┐ │
│ │   07:15      │ │
│ └──────────────┘ │
│                  │
│ 吃子紀錄:        │
│ ♟ ♛ +3          │  ← 白方吃了：1兵、1皇后 (= 10分)
│                  │     白方領先 3 分
└──────────────────┘

子力計算：
- 黑方吃掉：2×1 + 5 = 7分
- 白方吃掉：1×1 + 9 = 10分
- 白方領先：+3
```

### 範例 3: 複雜局面 (Complex Position)

```
情況：激烈交換後的局面
Scenario: After intense material exchange

右側面板顯示：
┌──────────────────┐
│ 黑方時間:        │
│ ┌──────────────┐ │
│ │   05:12      │ │
│ └──────────────┘ │
│                  │
│ 吃子紀錄:        │
│ ♟♟♟ ♞ ♝ ♜      │  ← 黑方吃了：3兵、1騎士、1主教、1城堡
│                  │     (3×1 + 3 + 3 + 5 = 14分)
│                  │
│ 白方時間:        │
│ ┌──────────────┐ │
│ │   04:58      │ │
│ └──────────────┘ │
│                  │
│ 吃子紀錄:        │
│ ♟♟ ♞♞ ♝ ♜♜ +5  │  ← 白方吃了：2兵、2騎士、1主教、2城堡
│                  │     (2×1 + 2×3 + 3 + 2×5 = 19分)
│                  │     白方領先 5 分
└──────────────────┘

子力計算：
- 黑方吃掉：3 + 3 + 3 + 5 = 14分
- 白方吃掉：2 + 6 + 3 + 10 = 19分
- 白方領先：+5
```

---

## 技術特性 (Technical Features)

### 1. 重疊顯示效果 (Overlapping Display)

相同類型的棋子會重疊顯示以節省空間：

```
┌────────────────────────────────────┐
│ 重疊機制 (Overlap Mechanism)        │
├────────────────────────────────────┤
│                                    │
│ 非重疊 (Non-overlapping):          │
│ ♟  ♞  ♝  ♜                        │
│ ├─┤├─┤├─┤                         │
│ 18px 間距                          │
│                                    │
│ 重疊 (Overlapping):                │
│ ♟♟♟♟                               │
│ ├┤├┤├┤                             │
│ 10px 間距 (18px - 8px overlap)     │
│                                    │
│ 混合 (Mixed):                      │
│ ♟♟  ♞♞♞  ♝                        │
│ ├┤├─┤├┤├┤├─┤                       │
│ 重疊  間隔  重疊  間隔              │
│                                    │
└────────────────────────────────────┘
```

**技術實現**：
- 使用 HTML 絕對定位 (`position: absolute`)
- 棋子寬度：18px
- 重疊偏移：8px
- 相同類型間距：10px (18 - 8)
- 不同類型間距：18px

**Technical Implementation**:
- Uses HTML absolute positioning (`position: absolute`)
- Piece width: 18px
- Overlap offset: 8px
- Same-type spacing: 10px (18 - 8)
- Different-type spacing: 18px

### 2. 自動排序 (Automatic Sorting)

被吃掉的棋子按價值排序顯示：

```
輸入順序 (Input order): 皇后, 兵, 騎士, 兵, 城堡, 兵
Input order: Queen, Pawn, Knight, Pawn, Rook, Pawn

顯示順序 (Display order): ♟♟♟ ♞ ♜ ♛
Display order: Pawns → Knight → Rook → Queen
               (1)     (3)      (5)    (9)
```

**排序規則**：
1. 按價值從小到大排序
2. 相同價值按類型排序
3. 確保相同類型的棋子相鄰顯示

**Sorting Rules**:
1. Sort by value (ascending)
2. Same value sorted by piece type
3. Ensures pieces of same type are adjacent

### 3. 子力優勢指示器 (Material Advantage Indicator)

```
┌──────────────────────────────────┐
│ 優勢顯示規則 (Advantage Display) │
├──────────────────────────────────┤
│                                  │
│ 均勢 (Equal material):           │
│ ♟♟ ♞                            │
│ (不顯示 +0)                      │
│                                  │
│ 微弱優勢 (Small advantage):      │
│ ♟♟♟ ♞ +1                        │
│                                  │
│ 明顯優勢 (Clear advantage):      │
│ ♟♟ ♜ ♛ +9                       │
│                                  │
│ 壓倒性優勢 (Overwhelming):       │
│ ♟♟♟ ♞♞ ♝ ♜♜ ♛ +15              │
│                                  │
└──────────────────────────────────┘
```

**顯示格式**：
- 綠色粗體 (Green, bold)
- 顯示在領先方的吃子記錄後
- 格式：`+N` (N 為領先分數)

**Display Format**:
- Green color, bold font
- Shown after the leading side's captures
- Format: `+N` (N is the advantage in points)

---

## 顯示時機 (Display Timing)

### 何時顯示 (When to Display)

```
遊戲流程 (Game Flow):

1. 應用程式啟動
   └─> 吃子紀錄：隱藏 ❌

2. 設定時間控制，點擊"開始"
   └─> 吃子紀錄：隱藏 ❌
   
3. 計時器啟動（第一步棋後）
   └─> 吃子紀錄：顯示 ✅
   
4. 遊戲進行中
   └─> 吃子紀錄：實時更新 ✅
   
5. 遊戲結束
   └─> 吃子紀錄：隱藏 ❌
   
6. 開始新遊戲
   └─> 返回步驟 2
```

### 自動更新 (Auto-Update)

吃子紀錄在以下情況自動更新：
- 每次吃子後
- 兵升變後（如果升變前吃子）
- 回放模式下查看歷史局面

Auto-updates occur:
- After each capture
- After pawn promotion (if capture occurred)
- When viewing historical positions in replay mode

---

## 支援的棋子圖示 (Supported Piece Icons)

### Unicode 符號模式 (Unicode Symbol Mode)

```
白方棋子 (White pieces):
♔ = 國王 (King)
♕ = 皇后 (Queen)
♖ = 城堡 (Rook)
♗ = 主教 (Bishop)
♘ = 騎士 (Knight)
♙ = 兵 (Pawn)

黑方棋子 (Black pieces):
♚ = 國王 (King)
♛ = 皇后 (Queen)
♜ = 城堡 (Rook)
♝ = 主教 (Bishop)
♞ = 騎士 (Knight)
♟ = 兵 (Pawn)
```

### 自訂圖示模式 (Custom Icon Mode)

當使用自訂棋子圖示時，吃子紀錄也會顯示相同的圖示：

```
┌─────────────────────────────────┐
│ Unicode 模式:                    │
│ ♟♟ ♞ +2                         │
│                                 │
│ 自訂圖示模式:                    │
│ [🎲][🎲] [🎪] +2                │
│ (顯示自訂的棋子圖片)             │
└─────────────────────────────────┘
```

支援的圖片格式：PNG、JPG、SVG、BMP

---

## 使用案例 (Use Cases)

### 案例 1: 學習棋局評估

```
情境：初學者學習如何評估局面

步驟：
1. 開始新遊戲
2. 進行多次交換
3. 查看吃子紀錄的 +N 指示器
4. 理解哪方在子力上佔優

範例局面：
白方吃子：♟♟ ♞ ♝ (= 8分)
黑方吃子：♟ ♜ (= 6分)
結果：白方領先 +2

學習要點：
- 白方雖然少吃一個棋子
- 但吃掉的價值更高
- 培養棋子價值概念
```

### 案例 2: 快棋比賽

```
情境：快速時間控制下的對局

優點：
1. 快速看到子力狀況
2. 無需計算就知道優劣
3. 幫助決策是否進行交換
4. 時間緊張時的視覺輔助

範例：
時間剩餘：30秒
看到：對方 +5 領先
決策：避免交換，尋求戰術機會
```

### 案例 3: 遊戲回放分析

```
情境：回放並分析已完成的對局

功能：
1. 使用回放按鈕 (⏮ ◀ ▶ ⏭)
2. 逐步查看每個局面
3. 吃子紀錄隨局面更新
4. 找出關鍵的子力交換點

分析要點：
- 何時發生重大子力交換
- 是否是等價交換
- 交換後誰獲得優勢
- 學習改進決策
```

---

## 限制與注意事項 (Limitations & Notes)

### 1. 顯示限制

```
限制情況：
- 僅在啟用時間控制時顯示
- 需要點擊"開始"按鈕後才可見
- 無限時模式下不顯示

原因：
吃子紀錄整合在時間控制面板中，
作為時間控制功能的一部分。
```

### 2. 不計算國王

```
注意：
國王被將死時，遊戲已結束，
因此國王不會出現在吃子紀錄中，
也不計入子力計算。

正確理解：
吃子紀錄 = 已被吃掉的子
不包括：仍在棋盤上的子（包括國王）
```

### 3. 特殊規則不影響顯示

```
吃過路兵 (En Passant):
- 顯示為普通兵 ♟
- 無特殊標記

兵升變 (Pawn Promotion):
- 升變後的棋子按新類型計算
- 原本的兵不再計入

王車易位 (Castling):
- 不涉及吃子，無顯示
```

---

## 測試驗證 (Testing Verification)

### 功能測試清單

```
✅ 基本顯示
  ├─ 單一棋子顯示正確
  ├─ 多個棋子顯示正確
  ├─ Unicode 符號正確渲染
  └─ 自訂圖示正確載入

✅ 重疊效果
  ├─ 相同類型棋子重疊 (8px)
  ├─ 不同類型棋子分開 (18px)
  ├─ 混合情況正確處理
  └─ 大量棋子不溢出

✅ 子力計算
  ├─ 價值計算正確
  ├─ 優勢指示器正確 (+N)
  ├─ 均勢時不顯示 +0
  └─ 顏色標示正確 (綠色)

✅ 自動更新
  ├─ 吃子後立即更新
  ├─ 回放時正確更新
  ├─ 新遊戲後正確重置
  └─ 多次吃子累積正確

✅ 顯示時機
  ├─ 計時器啟動時顯示
  ├─ 遊戲結束時隱藏
  ├─ 回放模式正常顯示
  └─ 無限時模式正確隱藏
```

---

## 實現文件參考 (Implementation References)

### 相關原始碼檔案

```
src/qt_chess.h (第 124-126 行)
├─ m_whiteCapturedLabel (QLabel*)
├─ m_blackCapturedLabel (QLabel*)
└─ 吃子顯示相關方法聲明

src/qt_chess.cpp (第 2028-2190 行)
├─ renderCapturedPieces() - 渲染吃子顯示
├─ updateCapturedPiecesDisplay() - 更新顯示
├─ getPieceValue() - 取得棋子價值
└─ UI 初始化和佈局代碼

src/chessboard.h (第 74 行)
└─ getCapturedPiecesByColor() - 取得被吃棋子

src/chessboard.cpp (第 625-636 行)
└─ getCapturedPiecesByColor() 實現
```

### 相關文件

```
docs/CAPTURED_PIECES_OVERLAP_FIX.md
└─ 重疊顯示修復說明

docs/CAPTURED_PIECES_OVERLAP_ANALYSIS.md
└─ 技術分析報告

docs/TIME_CONTROL_FEATURE.md
└─ 時間控制功能（包含吃子顯示）

README.md
└─ 功能特色列表
```

---

## 常見問題 (FAQ)

### Q1: 為什麼看不到吃子紀錄？

```
A: 確保滿足以下條件：
   1. ✓ 已設定時間控制（非"無限制"）
   2. ✓ 已點擊"開始"按鈕
   3. ✓ 計時器已啟動（至少走了一步棋）
   4. ✓ 遊戲尚未結束
```

### Q2: 子力優勢 +N 是如何計算的？

```
A: 計算公式：
   優勢 = 場上白方總子力 - 場上黑方總子力
   
   正數 (+N): 白方領先 N 分
   負數 (-N): 黑方領先 N 分
   零 (0):   均勢（不顯示）
   
   僅在領先方的吃子紀錄後顯示 +N
```

### Q3: 為什麼相同的棋子會重疊？

```
A: 這是特意設計的視覺效果：
   - 節省顯示空間
   - 清楚表達數量
   - 避免過長的橫向排列
   - 模擬"堆疊"效果
```

### Q4: 能否顯示更詳細的統計？

```
A: 當前設計專注於簡潔：
   - 顯示被吃棋子
   - 顯示總子力差
   
   未來可能添加：
   - 各類棋子的得失統計
   - 交換評分
   - 歷史子力曲線圖
```

### Q5: 回放時吃子紀錄如何工作？

```
A: 回放模式會正確顯示歷史局面：
   - 使用 ⏮ ◀ ▶ ⏭ 按鈕導航
   - 吃子紀錄隨當前步數更新
   - 顯示該步之前的所有吃子
   - 退出回放返回當前局面
```

---

## 總結 (Summary)

吃子紀錄功能提供了直觀的視覺反饋，幫助玩家：
1. ✅ 即時了解場上子力狀況
2. ✅ 快速評估局面優劣
3. ✅ 做出更好的戰術決策
4. ✅ 學習棋子價值概念

The captured pieces feature provides intuitive visual feedback, helping players:
1. ✅ Understand material status in real-time
2. ✅ Quickly evaluate position advantages
3. ✅ Make better tactical decisions
4. ✅ Learn piece value concepts

---

**文件版本 (Document Version)**: 1.0  
**最後更新 (Last Updated)**: 2025-11-24  
**作者 (Author)**: Copilot SWE Agent  
**狀態 (Status)**: 已完成 (Complete)
