# 被吃棋子顯示視覺指南 (Captured Pieces Display Visual Guide)

## 功能概覽 (Feature Overview)

此功能在對局中顯示被吃的棋子，讓玩家可以一目了然地看到雙方吃掉的所有棋子。

This feature displays captured pieces during gameplay, allowing players to see at a glance all the pieces captured by both sides.

## 顯示位置 (Display Location)

### 原始需求 (Original Requirement)
> "將對方吃的子顯示在時間下面，我吃的子顯示在時間上面，在對局顯示"

### 實現方式 (Implementation)

從白方（玩家）視角：
From White's (player's) perspective:

```
╔════════════════════════════════════════════════════════╗
║                                                        ║
║  ┌──────────┐                        ┌──────────┐     ║
║  │ 05:00    │◄── 黑方時間            │ ♟♞♝      │     ║
║  │ (Black)  │    (對手)              │ (White   │     ║
║  └──────────┘    Opponent's time     │ Captured)│     ║
║  ┌──────────┐                        └──────────┘     ║
║  │ ♘♙♙      │◄── 黑方吃掉的子                         ║
║  │ (Black   │    (對手吃的)                           ║
║  │ Captured)│    Opponent's captures                  ║
║  └──────────┘    (時間下面)                           ║
║                  Below time                           ║
║       ┌────────────────────┐                          ║
║       │                    │                          ║
║       │    Chess Board     │                          ║
║       │     (8x8 grid)     │                          ║
║       │                    │                          ║
║       └────────────────────┘                          ║
║                                      ┌──────────┐     ║
║                                      │ ♟♜♛      │     ║
║                                      │ (White   │     ║
║                                      │ Captured)│     ║
║                  我吃掉的子 ─────────►└──────────┘     ║
║                  My captures                          ║
║                  (時間上面)          ┌──────────┐     ║
║                  Above time          │ 05:00    │     ║
║                                      │ (White)  │     ║
║                  白方時間 ─────────►└──────────┘     ║
║                  My time                              ║
║                                                        ║
╚════════════════════════════════════════════════════════╝
```

## 遊戲場景示例 (Game Scenario Examples)

### 場景 1: 遊戲開始 (Scenario 1: Game Start)

```
初始狀態 - 沒有被吃的棋子
Initial state - No captured pieces

Left Side                Board                Right Side
┌────────┐                                   
│ 05:00  │              ♜♞♝♛♚♝♞♜           
│ Black  │              ♟♟♟♟♟♟♟♟           
└────────┘              · · · · · · · ·           
                        · · · · · · · ·           
                        · · · · · · · ·           
                        · · · · · · · ·           
                        ♙♙♙♙♙♙♙♙            ┌────────┐
                        ♖♘♗♕♔♗♘♖            │ 05:00  │
                                            │ White  │
                                            └────────┘
```

### 場景 2: 遊戲進行中 (Scenario 2: Mid Game)

```
白方吃掉了黑方的一個兵和一個馬
White has captured a black pawn and knight

Left Side                Board                Right Side
┌────────┐                                   
│ 04:30  │              ♜ · ♝♛♚♝♞♜           
│ Black  │              ♟♟ · ♟♟♟♟♟           
└────────┘              · · · · · · · ·           
┌────────┐              · · · · · · · ·           ┌────────┐
│ ♘♙     │              · · · · · · · ·           │ ♟♞     │
│ Black  │              · · · · · · · ·           │ White  │
│ Capt.  │              ♙♙♙♙♙ · ♙♙            │ Capt.  │
└────────┘              ♖♘♗♕♔♗ · ♖            └────────┘
                                            ┌────────┐
                                            │ 04:45  │
                                            │ White  │
                                            └────────┘

說明 (Explanation):
- 白方吃掉: ♟ (黑兵), ♞ (黑馬) - 顯示在右上方
  White captured: ♟ (black pawn), ♞ (black knight) - shown top right
- 黑方吃掉: ♘ (白馬), ♙ (白兵) - 顯示在左下方
  Black captured: ♘ (white knight), ♙ (white pawn) - shown bottom left
```

### 場景 3: 激烈對局 (Scenario 3: Intense Game)

```
雙方都吃掉了多個棋子
Both sides have captured multiple pieces

Left Side                Board                Right Side
┌────────┐                                   
│ 02:15  │              ♜ · · ♛♚ · · ♜           
│ Black  │              · · ♟ · · ♟♟ ·           
└────────┘              · · · · · · · ·           
┌────────┐              · · · · ♟ · · ·           ┌────────┐
│ ♘♗♙♙♙  │              · · · · · · · ·           │ ♟♞♝♜♛  │
│ Black  │              · · · ♙ · ♙ · ·           │ White  │
│ Capt.  │              · · ♙ · · · ♙♙            │ Capt.  │
└────────┘              ♖ · · ♕♔♗ · ♖            └────────┘
                                            ┌────────┐
                                            │ 02:30  │
                                            │ White  │
                                            └────────┘

分數 (Scores):
- 白方分數: 1+3+3+5+9 = 21 分
  White's score: 1+3+3+5+9 = 21 points
- 黑方分數: 3+3+1+1+1 = 9 分
  Black's score: 3+3+1+1+1 = 9 points
```

## 棋子符號對照表 (Chess Piece Symbol Reference)

### 白方棋子 (White Pieces)
| 棋子類型 | Unicode 符號 | 分數值 |
|---------|-------------|--------|
| 國王 King | ♔ | 0 |
| 皇后 Queen | ♕ | 9 |
| 城堡 Rook | ♖ | 5 |
| 主教 Bishop | ♗ | 3 |
| 騎士 Knight | ♘ | 3 |
| 兵 Pawn | ♙ | 1 |

### 黑方棋子 (Black Pieces)
| 棋子類型 | Unicode 符號 | 分數值 |
|---------|-------------|--------|
| 國王 King | ♚ | 0 |
| 皇后 Queen | ♛ | 9 |
| 城堡 Rook | ♜ | 5 |
| 主教 Bishop | ♝ | 3 |
| 騎士 Knight | ♞ | 3 |
| 兵 Pawn | ♟ | 1 |

## 顯示規則 (Display Rules)

### 1. 棋子順序 (Piece Order)
- 棋子按照被吃的時間順序排列
- 從左到右顯示
- 不會按類型排序

Pieces are arranged in the order they were captured  
Displayed left to right  
Not sorted by piece type

示例 (Example):
```
第一個被吃: ♟
第二個被吃: ♞
第三個被吃: ♝
顯示為: ♟♞♝

First captured: ♟
Second captured: ♞
Third captured: ♝
Displayed as: ♟♞♝
```

### 2. 自動換行 (Automatic Wrapping)
- 當棋子太多時，標籤會自動換行
- 保持可讀性

When there are too many pieces, the label wraps automatically  
Maintains readability

示例 (Example):
```
♟♞♝♜♛♟♞
♝♜
```

### 3. 可見性控制 (Visibility Control)
- 沒有被吃棋子時：標籤隱藏
- 有被吃棋子時：標籤自動顯示

When no pieces captured: Label hidden  
When pieces captured: Label automatically shown

## 特殊情況 (Special Cases)

### 吃過路兵 (En Passant Capture)
```
Before:              After:
♙ ♟ ·               ♙ · ·
                    
被吃棋子顯示: ♟
Captured pieces display: ♟
```

### 兵升變後被吃 (Promoted Pawn Captured)
```
升變為皇后: ♙ → ♕
被吃時顯示: ♕ (不是 ♙)

Promoted to queen: ♙ → ♕
When captured, shows: ♕ (not ♙)
```

### 新遊戲重置 (New Game Reset)
```
遊戲中:                新遊戲後:
White Capt: ♟♞♝       White Capt: (empty/hidden)
Black Capt: ♘♙♙       Black Capt: (empty/hidden)

During game:           After new game:
White Capt: ♟♞♝       White Capt: (empty/hidden)
Black Capt: ♘♙♙       Black Capt: (empty/hidden)
```

## 與其他功能的配合 (Integration with Other Features)

### 1. 時間控制功能 (Time Control Feature)
- 被吃棋子顯示只在啟用時間控制時顯示
- 與時間標籤完美對齊

Captured pieces display only shown when time control is enabled  
Perfectly aligned with time labels

### 2. 分數系統 (Scoring System)
- 數字分數顯示在右側面板
- 被吃棋子提供視覺化補充

Numerical scores displayed in right panel  
Captured pieces provide visual supplement

### 3. 棋盤翻轉 (Board Flip)
- 棋盤翻轉時，標籤位置保持一致
- 對應關係不變

When board is flipped, label positions remain consistent  
Correspondence unchanged

## 視覺化流程 (Visual Flow)

```
遊戲開始
Game Start
    ↓
[No Captures]
標籤隱藏
Labels Hidden
    ↓
第一次吃子
First Capture
    ↓
[White captures ♟]
白方標籤顯示: ♟
White label shows: ♟
    ↓
黑方反擊吃子
Black counter-captures
    ↓
[Black captures ♙]
黑方標籤顯示: ♙
Black label shows: ♙
    ↓
繼續對局...
Game continues...
    ↓
[Multiple captures]
雙方標籤顯示多個棋子
Both labels show multiple pieces
    ↓
新遊戲
New Game
    ↓
[Reset]
所有標籤清空並隱藏
All labels cleared and hidden
```

## 總結 (Summary)

這個功能完整實現了需求：
This feature fully implements the requirement:

✅ **對方吃的子顯示在時間下面**
   Opponent's captured pieces below their time
   
✅ **我吃的子顯示在時間上面**
   My captured pieces above my time
   
✅ **在對局顯示**
   Shown during gameplay

提供直觀的視覺化資訊，幫助玩家：
Provides intuitive visual information to help players:
- 快速評估局勢
- 追蹤物質得失
- 規劃下一步策略

- Quickly assess the position
- Track material gains and losses
- Plan next moves strategically
