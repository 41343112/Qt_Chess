# Testing Guide for Replay Navigation Buttons

## Test Environment Setup

### Prerequisites
- Qt5 development environment
- C++17 compatible compiler
- Qt Chess application built and ready to run

### Build Instructions
```bash
qmake Qt_Chess.pro
make
./Qt_Chess
```

## Test Cases

### Test Case 1: Initial Application State
**Objective**: Verify buttons are visible but disabled at startup

**Steps**:
1. Launch the Qt Chess application
2. Observe the left panel

**Expected Results**:
- ✓ "回放控制" (Replay Control) title is visible
- ✓ Four buttons (⏮, ◀, ▶, ⏭) are visible
- ✓ All four buttons are disabled (grayed out)
- ✓ Buttons have tooltips: "第一步", "上一步", "下一步", "最後一步"

**Reason**: No game has been played yet, so no moves exist to replay

---

### Test Case 2: During Active Gameplay
**Objective**: Verify buttons remain disabled during game

**Steps**:
1. Click "開始" (Start) button to begin a game
2. Make several moves (e.g., e2-e4, e7-e5, Nf3, Nc6)
3. Observe the replay buttons

**Expected Results**:
- ✓ All four replay buttons remain disabled during gameplay
- ✓ Buttons are visible but cannot be clicked
- ✓ Game continues normally

**Reason**: Replay is not allowed during active gameplay to prevent state confusion

---

### Test Case 3: After Game Ends (Checkmate/Stalemate)
**Objective**: Verify buttons become enabled after game conclusion

**Steps**:
1. Play a game to completion (checkmate, stalemate, or resign)
2. Close the game-end dialog
3. Observe the replay buttons

**Expected Results**:
- ✓ All four replay buttons become enabled (clickable)
- ✓ Buttons appear in normal color (not grayed out)
- ✓ Move list shows all played moves
- ✓ "匯出 PGN" and "複製棋譜" buttons are visible

**Reason**: Game has ended with move history, replay is now available

---

### Test Case 4: First Step Button (⏮)
**Objective**: Test navigation to initial board state

**Steps**:
1. Complete a game with at least 5 moves
2. Click the "第一步" (⏮) button
3. Observe the board and UI state

**Expected Results**:
- ✓ Application enters replay mode automatically
- ✓ Board shows initial position (all pieces in starting squares)
- ✓ "退出回放" (Exit Replay) button appears
- ✓ "第一步" (⏮) and "上一步" (◀) buttons become disabled
- ✓ "下一步" (▶) and "最後一步" (⏭) buttons remain enabled
- ✓ No move is highlighted in move list

---

### Test Case 5: Previous Step Button (◀)
**Objective**: Test backward navigation through moves

**Steps**:
1. Enter replay mode at a middle position (not at start)
2. Click the "上一步" (◀) button multiple times
3. Observe board changes

**Expected Results**:
- ✓ Board updates to show previous position with each click
- ✓ Move list highlights the current move
- ✓ Button becomes disabled when reaching the initial position
- ✓ "下一步" and "最後一步" buttons are enabled

---

### Test Case 6: Next Step Button (▶)
**Objective**: Test forward navigation through moves

**Steps**:
1. Enter replay mode at initial position
2. Click the "下一步" (▶) button multiple times
3. Observe board changes

**Expected Results**:
- ✓ Board updates to show next position with each click
- ✓ Move list highlights the current move
- ✓ "第一步" and "上一步" buttons become enabled (after first click)
- ✓ Button becomes disabled when reaching the last move

---

### Test Case 7: Last Step Button (⏭)
**Objective**: Test navigation to final board state

**Steps**:
1. Enter replay mode at initial position
2. Click the "最後一步" (⏭) button
3. Observe the board and UI state

**Expected Results**:
- ✓ Board shows final position (last move played)
- ✓ Last move is highlighted in move list
- ✓ "第一步" (⏮) and "上一步" (◀) buttons are enabled
- ✓ "下一步" (▶) and "最後一步" (⏭) buttons become disabled

---

### Test Case 8: Exit Replay Mode
**Objective**: Test returning to normal mode

**Steps**:
1. Enter replay mode and navigate to any position
2. Click "退出回放" (Exit Replay) button
3. Observe the UI changes

**Expected Results**:
- ✓ Board returns to the final game position
- ✓ "退出回放" button disappears
- ✓ "匯出 PGN" and "複製棋譜" buttons reappear
- ✓ All four replay buttons remain enabled (game is still ended)
- ✓ Move list selection is cleared

---

### Test Case 9: New Game Reset
**Objective**: Verify buttons reset correctly for new game

**Steps**:
1. Complete a game and enter replay mode
2. Click "遊戲" → "新遊戲" in menu
3. Observe button states

**Expected Results**:
- ✓ All four replay buttons become disabled
- ✓ Buttons remain visible
- ✓ Move list is cleared
- ✓ Board resets to initial position
- ✓ Time control panel becomes visible

---

### Test Case 10: Replay from Non-Replay Mode
**Objective**: Test automatic replay mode entry

**Steps**:
1. Complete a game (do not double-click move list to enter replay)
2. Directly click any replay button (⏮, ◀, ▶, or ⏭)
3. Observe behavior

**Expected Results**:
- ✓ Application automatically enters replay mode
- ✓ Board navigates to the requested position
- ✓ "退出回放" button appears
- ✓ "匯出 PGN" and "複製棋譜" buttons hide

---

### Test Case 11: Button State During Replay Navigation
**Objective**: Verify buttons enable/disable correctly based on position

**Steps**:
1. Enter replay mode
2. Navigate through moves using all four buttons
3. Observe button states at each position

**Expected Results**:
At initial position (-1):
- ✓ ⏮ and ◀ are disabled
- ✓ ▶ and ⏭ are enabled

At middle positions:
- ✓ All four buttons are enabled

At final position:
- ✓ ⏮ and ◀ are enabled
- ✓ ▶ and ⏭ are disabled

---

### Test Case 12: Tooltip Verification
**Objective**: Verify all buttons have correct tooltips

**Steps**:
1. Hover mouse over each replay button
2. Read the tooltip text

**Expected Results**:
- ✓ ⏮ button shows "第一步" (First Step)
- ✓ ◀ button shows "上一步" (Previous Step)
- ✓ ▶ button shows "下一步" (Next Step)
- ✓ ⏭ button shows "最後一步" (Last Step)

---

## Regression Testing

### Verify Existing Functionality Still Works

1. **Double-click Move List**: Verify it still enters replay mode
2. **Time Control**: Verify game timer works correctly
3. **Sound Effects**: Verify move sounds still play
4. **Board Flip**: Verify board can still be flipped
5. **PGN Export**: Verify PGN export still works
6. **Copy PGN**: Verify copy to clipboard still works

---

## UI Visual Verification

### Layout Check
- ✓ Buttons are aligned in a single row
- ✓ Button spacing is consistent (5px)
- ✓ Buttons fit within left panel width (200px max)
- ✓ Title "回放控制" is centered above buttons
- ✓ No overlap with move list above

### Visual Appearance
- ✓ Enabled buttons: Normal appearance, responds to hover
- ✓ Disabled buttons: Grayed out, no hover effect
- ✓ Button symbols (⏮◀▶⏭) are clear and readable
- ✓ Tooltips appear correctly on hover

---

## Performance Testing

### Memory Check
- ✓ No memory leaks when repeatedly entering/exiting replay mode
- ✓ Move list updates efficiently

### Responsiveness
- ✓ Button clicks respond immediately
- ✓ Board updates smoothly during navigation
- ✓ No lag when navigating through large move lists (50+ moves)

---

## Edge Cases

### Test Case 13: Single Move Game
**Steps**:
1. Play a game with only one move (e.g., white moves, then black gives up)
2. Test all replay buttons

**Expected Results**:
- ✓ At start: Only ▶ and ⏭ enabled
- ✓ At move 1: Only ⏮ and ◀ enabled

### Test Case 14: Rapid Button Clicking
**Steps**:
1. Enter replay mode
2. Rapidly click navigation buttons

**Expected Results**:
- ✓ No crashes or errors
- ✓ Board state remains consistent
- ✓ Button states update correctly

### Test Case 15: Window Resize
**Steps**:
1. Resize application window to minimum size
2. Resize to maximum size
3. Verify buttons remain functional

**Expected Results**:
- ✓ Buttons remain visible at all window sizes
- ✓ Button layout doesn't break
- ✓ Tooltips still work

---

## Success Criteria

All test cases should pass with ✓ checkmarks for the implementation to be considered complete and correct.

## Bug Reporting Template

If any test fails, report using this template:

```
Test Case: [Number and Name]
Step: [Which step failed]
Expected: [What should happen]
Actual: [What actually happened]
Environment: [OS, Qt version, compiler]
Reproducibility: [Always/Sometimes/Rare]
Screenshot: [If applicable]
```
