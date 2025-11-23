# Test Plan for Give Up Button and New Game Fixes

## Issue Description
1. 放棄按鈕不見 (Give up button disappearing) - The give up button was hidden after game ends
2. 遊戲結束後無法開以新遊戲 (Unable to start new game after game ends) - The start button was disabled after game ends

## Changes Made
1. Added "Game" menu (遊戲) to menu bar with:
   - "New Game" (新遊戲) option - calls `onNewGameClicked()`
   - "Give Up" (放棄) option - calls `onGiveUpClicked()`

2. Fixed `handleGameEnd()` function to:
   - Re-enable the start button (instead of disabling it)
   - Change button text back to "開始" (Start)
   - Hide time displays for consistency

## Test Cases

### Test Case 1: Normal Game Flow → Checkmate → New Game
1. Launch the application
2. Click "開始" (Start) button to begin the game
3. Play until checkmate occurs
4. Verify: Checkmate message is displayed
5. Verify: Time control panel is shown again
6. Verify: Start button shows "開始" and is enabled
7. Click "開始" button again
8. Verify: A new game starts successfully

### Test Case 2: Game Flow → Give Up (Button) → New Game  
1. Launch the application
2. Click "開始" (Start) button to begin the game
3. Click the "放棄" button in the time control panel
4. Click "Yes" on the confirmation dialog
5. Verify: Give up message is displayed with winner
6. Verify: Give up button is hidden
7. Verify: Time control panel is shown again
8. Verify: Start button shows "開始" and is enabled
9. Click "開始" button again
10. Verify: A new game starts successfully

### Test Case 3: Game Flow → Give Up (Menu) → New Game
1. Launch the application
2. Click "開始" (Start) button to begin the game
3. Click "遊戲" → "放棄" in the menu bar
4. Click "Yes" on the confirmation dialog
5. Verify: Give up message is displayed with winner
6. Verify: Give up button is hidden
7. Verify: Time control panel is shown again
8. Verify: Start button shows "開始" and is enabled
9. Click "開始" button again
10. Verify: A new game starts successfully

### Test Case 4: New Game from Menu During Active Game
1. Launch the application
2. Click "開始" (Start) button to begin the game
3. Make a few moves
4. Click "遊戲" → "新遊戲" in the menu bar
5. Verify: Board is reset to initial position
6. Verify: Time control panel is shown
7. Verify: Start button shows "開始" and is enabled
8. Click "開始" button again
9. Verify: A new game starts successfully

### Test Case 5: Time Control → Time Runs Out → New Game
1. Launch the application
2. Set white or black time to 30 seconds
3. Click "開始" (Start) button to begin the game
4. Wait for time to run out
5. Verify: Timeout message is displayed with winner
6. Verify: Time control panel is shown again
7. Verify: Start button shows "開始" and is enabled
8. Click "開始" button again
9. Verify: A new game starts successfully

### Test Case 6: Stalemate → New Game
1. Launch the application
2. Click "開始" (Start) button to begin the game
3. Play until stalemate occurs
4. Verify: Stalemate message is displayed
5. Verify: Time control panel is shown again
6. Verify: Start button shows "開始" and is enabled
7. Click "開始" button again
8. Verify: A new game starts successfully

## Expected Results
- All game end scenarios should allow starting a new game via:
  - The "開始" (Start) button in the time control panel
  - The "遊戲" → "新遊戲" (Game → New Game) menu option
- The "放棄" (Give Up) button in time control panel should work during active games
- The "遊戲" → "放棄" (Game → Give Up) menu option should work during active games
- No UI elements should be permanently stuck or disabled after any game end scenario
