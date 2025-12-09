# ChessPiece 棋子功能

## 概述
`ChessPiece` 類別定義了西洋棋中的棋子，包括棋子類型、顏色、移動規則驗證等核心功能。

## 檔案位置
- **標頭檔**: `src/chesspiece.h`
- **實作檔**: `src/chesspiece.cpp`

## 主要資料結構

### 列舉型別（Enums）

#### PieceType - 棋子類型
```cpp
enum class PieceType {
    None,    // 空格（無棋子）
    Pawn,    // 兵
    Rook,    // 城堡
    Knight,  // 騎士
    Bishop,  // 主教
    Queen,   // 皇后
    King     // 國王
};
```

#### PieceColor - 棋子顏色
```cpp
enum class PieceColor {
    None,   // 無顏色
    White,  // 白色
    Black   // 黑色
};
```

## 類別成員

### 私有成員變數
- `PieceType m_type` - 棋子的類型
- `PieceColor m_color` - 棋子的顏色
- `bool m_hasMoved` - 記錄棋子是否已經移動過（用於王車易位和兵的特殊走法）

### 公開方法

#### 建構函式
```cpp
ChessPiece(PieceType type = PieceType::None, PieceColor color = PieceColor::None)
```
建立一個新的棋子物件。

**參數**:
- `type` - 棋子類型（預設為 None）
- `color` - 棋子顏色（預設為 None）

#### Getter 方法
- `PieceType getType() const` - 取得棋子類型
- `PieceColor getColor() const` - 取得棋子顏色
- `bool hasMoved() const` - 檢查棋子是否已經移動過

#### Setter 方法
- `void setMoved(bool moved)` - 設定棋子的移動狀態

#### getSymbol()
```cpp
QString getSymbol() const
```
返回棋子的 Unicode 符號表示。

**返回值**:
- 白棋: ♔(國王), ♕(皇后), ♖(城堡), ♗(主教), ♘(騎士), ♙(兵)
- 黑棋: ♚(國王), ♛(皇后), ♜(城堡), ♝(主教), ♞(騎士), ♟(兵)

#### isValidMove()
```cpp
bool isValidMove(const QPoint& from, const QPoint& to, 
                 const std::vector<std::vector<ChessPiece>>& board,
                 const QPoint& enPassantTarget = QPoint(-1, -1)) const
```
驗證從起始位置到目標位置的移動是否符合該棋子的移動規則。

**參數**:
- `from` - 起始位置
- `to` - 目標位置
- `board` - 當前棋盤狀態（8x8 二維陣列）
- `enPassantTarget` - 吃過路兵的目標位置（可選）

**返回值**: `true` 如果移動有效，否則 `false`

**基本驗證**:
1. 檢查起始位置和目標位置是否相同
2. 檢查目標位置是否在棋盤範圍內（0-7）
3. 確認不會吃掉自己的棋子
4. 根據棋子類型調用相應的移動驗證方法

## 移動規則驗證方法

### isValidPawnMove() - 兵的移動規則
```cpp
bool isValidPawnMove(const QPoint& from, const QPoint& to, 
                     const std::vector<std::vector<ChessPiece>>& board,
                     const QPoint& enPassantTarget) const
```

**兵的移動規則**:
- **前進一格**: 只能向前移動，目標格必須為空
- **前進兩格**: 只能在起始位置（白兵第6列，黑兵第1列）且中間無阻擋
- **斜向吃子**: 斜前方一格有對方棋子時可吃掉
- **吃過路兵**: 當對手的兵從起始位置前進兩格停在旁邊時，可以斜向吃掉

**實作重點**:
- 白兵向上移動（direction = -1），黑兵向下移動（direction = 1）
- 檢查是否為首次移動（`!m_hasMoved`）
- 驗證吃過路兵的目標位置

### isValidRookMove() - 城堡的移動規則
```cpp
bool isValidRookMove(const QPoint& from, const QPoint& to, 
                     const std::vector<std::vector<ChessPiece>>& board) const
```

**城堡的移動規則**:
- 只能水平或垂直移動（同一行或同一列）
- 路徑上不能有其他棋子阻擋

### isValidKnightMove() - 騎士的移動規則
```cpp
bool isValidKnightMove(const QPoint& from, const QPoint& to) const
```

**騎士的移動規則**:
- 走「L」形：一個方向移動2格，垂直方向移動1格
- 可以跳過其他棋子（不需要檢查路徑）
- 有效移動組合: (±1, ±2) 或 (±2, ±1)

### isValidBishopMove() - 主教的移動規則
```cpp
bool isValidBishopMove(const QPoint& from, const QPoint& to, 
                       const std::vector<std::vector<ChessPiece>>& board) const
```

**主教的移動規則**:
- 只能斜向移動（對角線）
- 路徑上不能有其他棋子阻擋
- 檢查條件: `|dx| == |dy|` （x 和 y 的移動距離相等）

### isValidQueenMove() - 皇后的移動規則
```cpp
bool isValidQueenMove(const QPoint& from, const QPoint& to, 
                      const std::vector<std::vector<ChessPiece>>& board) const
```

**皇后的移動規則**:
- 結合城堡和主教的移動方式
- 可以水平、垂直或斜向移動任意格數
- 路徑上不能有其他棋子阻擋

### isValidKingMove() - 國王的移動規則
```cpp
bool isValidKingMove(const QPoint& from, const QPoint& to) const
```

**國王的移動規則**:
- 向任何方向移動一格
- 檢查條件: `|dx| <= 1 && |dy| <= 1`
- 注意：王車易位的規則在 ChessBoard 類別中處理

### isPathClear() - 路徑檢查
```cpp
bool isPathClear(const QPoint& from, const QPoint& to, 
                 const std::vector<std::vector<ChessPiece>>& board) const
```

檢查兩個位置之間的路徑是否暢通（用於城堡、主教、皇后的移動）。

**實作方式**:
1. 計算移動的方向（dx 和 dy 的符號）
2. 沿著路徑逐格檢查
3. 如果遇到任何棋子則返回 `false`
4. 不檢查終點位置（可能有對方棋子可以吃）

## 使用範例

### 建立棋子
```cpp
// 建立白色國王
ChessPiece whiteKing(PieceType::King, PieceColor::White);

// 建立黑色兵
ChessPiece blackPawn(PieceType::Pawn, PieceColor::Black);
```

### 檢查移動是否有效
```cpp
ChessPiece piece(PieceType::Knight, PieceColor::White);
QPoint from(0, 7);  // 起始位置
QPoint to(2, 6);    // 目標位置

if (piece.isValidMove(from, to, board)) {
    // 移動有效，可以執行移動
}
```

### 取得棋子符號
```cpp
ChessPiece queen(PieceType::Queen, PieceColor::White);
QString symbol = queen.getSymbol();  // 返回 "♕"
```

## 設計考量

### 為什麼需要 hasMoved 標記？
- **王車易位**: 國王和城堡都必須沒有移動過才能進行易位
- **兵的雙格移動**: 兵只能在首次移動時前進兩格
- **吃過路兵**: 需要追蹤對手兵是否剛剛進行了雙格移動

### 為什麼移動驗證需要整個棋盤？
- 檢查路徑是否有其他棋子阻擋（城堡、主教、皇后）
- 驗證目標位置是否有可吃的對方棋子
- 處理特殊走法（如吃過路兵）

### 為什麼不在這裡檢查「將軍」狀態？
- 將軍檢查需要考慮整個棋盤的狀態和所有對方棋子
- 這是由 `ChessBoard` 類別負責的更高層次邏輯
- `ChessPiece` 只負責單一棋子的基本移動規則

## 相關類別
- `ChessBoard` - 使用 `ChessPiece` 來管理整個棋盤狀態
- `Qt_Chess` - 主視窗類別，處理棋子的 UI 顯示

## 測試要點
1. 每種棋子的基本移動規則
2. 邊界條件（棋盤邊緣）
3. 吃子規則（不能吃自己的棋子）
4. 路徑阻擋檢查
5. 特殊走法（兵的雙格移動、吃過路兵）
