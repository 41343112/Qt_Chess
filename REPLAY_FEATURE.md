# 回放功能說明 (Replay Feature Documentation)

## 功能概述 (Feature Overview)

本次實作新增了棋局回放功能，允許玩家查看之前的棋步，並且**當棋盤回到目前狀態時會自動退出回放模式**。

This implementation adds game replay functionality that allows players to review previous moves, and **automatically exits replay mode when the board returns to the current state**.

## 主要功能 (Key Features)

### 1. 棋步歷史記錄 (Move History Tracking)
- 每一步棋都會被記錄，包括：
  - 起始位置和目標位置
  - 移動的棋子
  - 被吃掉的棋子（如有）
  - 特殊移動（王車易位、吃過路兵）
  - 兵升變類型（如有）

### 2. 回放導航 (Replay Navigation)
- **上一步按鈕 (Previous Move)**: 查看上一步棋的棋盤狀態
- **下一步按鈕 (Next Move)**: 前進到下一步棋的棋盤狀態
- **回放模式指示 (Replay Mode Indicator)**: 顯示當前位於第幾步（例如：回放模式 3/10）

### 3. 自動退出回放模式 (Automatic Exit from Replay Mode)
**核心功能：當使用「下一步」按鈕前進到最新的棋局狀態時，系統會自動退出回放模式，恢復正常對弈。**

Core Feature: When using the "Next Move" button to advance to the latest game state, the system automatically exits replay mode and restores normal gameplay.

## 使用方法 (Usage)

1. **開始遊戲** (Start Game)
   - 點擊「開始」按鈕開始新遊戲
   - 進行若干步棋

2. **進入回放模式** (Enter Replay Mode)
   - 點擊「◄ 上一步」按鈕查看上一步棋
   - 此時會顯示紅色的回放模式指示標籤
   - 在回放模式中無法移動棋子

3. **導航棋步** (Navigate Moves)
   - 使用「◄ 上一步」按鈕向後查看
   - 使用「下一步 ►」按鈕向前查看
   - 回放模式指示會顯示當前位置（例如：回放模式 3/10）

4. **自動退出回放模式** (Auto-Exit Replay Mode)
   - 當你點擊「下一步 ►」按鈕直到到達最新狀態時
   - 系統會自動退出回放模式
   - 回放模式指示標籤會自動隱藏
   - 恢復正常對弈，可以繼續移動棋子

5. **繼續遊戲** (Continue Game)
   - 退出回放模式後，可以繼續正常下棋
   - 新的棋步會被記錄到歷史中

## 技術實現 (Technical Implementation)

### ChessBoard 類別更新 (ChessBoard Class Updates)
- `Move` 結構體：記錄每一步棋的完整資訊
- `m_moveHistory`: 儲存所有棋步的向量
- `m_isInReplayMode`: 回放模式標誌
- `m_currentMoveIndex`: 當前回放位置索引

### 關鍵方法 (Key Methods)
- `goToPreviousMove()`: 回到上一步，進入回放模式
- `goToNextMove()`: 前進到下一步，**當到達最新狀態時自動退出回放模式**
- `exitReplayMode()`: 退出回放模式
- `applyMove()`: 應用一個棋步到棋盤
- `undoMove()`: 撤銷一個棋步

### UI 更新 (UI Updates)
- 添加了「◄ 上一步」和「下一步 ►」按鈕
- 添加了回放模式指示標籤
- 在回放模式中禁用棋子移動
- 按鈕狀態會根據可用性自動更新

## 核心邏輯：自動退出 (Core Logic: Auto-Exit)

```cpp
bool ChessBoard::goToNextMove() {
    if (!canGoToNextMove()) {
        return false;
    }
    
    // 應用下一個移動
    m_currentMoveIndex++;
    const Move& move = m_moveHistory[m_currentMoveIndex];
    applyMove(move);
    
    // 檢查是否到達最新狀態，如果是則自動退出回放模式
    if (m_currentMoveIndex == static_cast<int>(m_moveHistory.size()) - 1) {
        exitReplayMode();  // 自動退出！
    }
    
    return true;
}
```

當 `m_currentMoveIndex` 等於 `m_moveHistory.size() - 1` 時，表示已經到達最新的棋局狀態，此時會自動調用 `exitReplayMode()` 退出回放模式。

## 問題解決 (Problem Solved)

**原始需求**: 當棋盤已經在目前的狀態就要退出回放功能

**Original Requirement**: When the chessboard is already in the current state, exit the replay function.

**解決方案**: 在 `goToNextMove()` 方法中，當檢測到已經前進到最新狀態（`m_currentMoveIndex == m_moveHistory.size() - 1`）時，自動調用 `exitReplayMode()` 退出回放模式，恢復正常對弈狀態。

**Solution**: In the `goToNextMove()` method, when detecting that we've advanced to the latest state (`m_currentMoveIndex == m_moveHistory.size() - 1`), automatically call `exitReplayMode()` to exit replay mode and restore normal gameplay.

## 測試建議 (Testing Suggestions)

1. 開始新遊戲並下幾步棋（至少 5 步）
2. 點擊「◄ 上一步」按鈕數次，確認能正確回到之前的狀態
3. 在回放模式中嘗試移動棋子，確認無法移動
4. 點擊「下一步 ►」按鈕逐步前進
5. **重點測試**：持續點擊「下一步 ►」直到到達最新狀態
6. **驗證**：回放模式指示標籤應該消失，可以正常移動棋子
7. 移動一步新棋子，確認歷史記錄正確更新
