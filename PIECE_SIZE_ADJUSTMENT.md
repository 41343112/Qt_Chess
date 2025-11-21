# Piece Size Adjustment Feature

## Overview
The piece size adjustment feature allows users to customize the size of chess pieces displayed on the board by adjusting a scale factor from 60% to 100% of the square size.

## Implementation Details

### User Interface
- **Location**: Settings menu > 棋子圖標設定 (Piece Icon Settings)
- **Control**: Horizontal slider labeled "棋子大小調整" (Piece Size Adjustment)
- **Range**: 60% to 100% (default: 80%)
- **Display**: Real-time percentage value shown next to the slider

### Technical Details

#### Data Structure
The `PieceIconSettings` struct now includes:
```cpp
struct PieceIconSettings {
    // ... existing fields ...
    int pieceScale;  // Scale factor in percentage (60-100), default 80
};
```

#### Key Functions
1. **calculateIconSize()**: Calculates icon size based on square size and user-configured scale
   ```cpp
   return static_cast<int>(squareWidth * m_pieceIconSettings.pieceScale / 100.0);
   ```

2. **resizeEvent()**: Updates icon sizes when window is resized, applying the scale factor

3. **Settings Persistence**: Scale factor is saved and loaded via QSettings
   - Save key: `"PieceIcons/pieceScale"`
   - Default value: 80 (representing 80%)

### User Experience
1. Open Settings menu from the menu bar
2. Click "棋子圖標設定" (Piece Icon Settings)
3. Locate the "棋子大小調整" (Piece Size Adjustment) section
4. Drag the slider to adjust piece size (60% to 100%)
5. See the percentage value update in real-time
6. Click "確定" (OK) to apply changes
7. All pieces on the board immediately resize according to the new scale

### Benefits
- **Flexibility**: Users can make pieces larger for better visibility or smaller for a more compact board
- **Accessibility**: Larger pieces benefit users with visual impairments
- **Aesthetics**: Users can choose the size that looks best to them
- **Persistence**: Settings are saved between sessions

### Affected Components
- Chess board piece display
- Drag-and-drop piece rendering
- Window resize handling
- Settings dialog UI
- Settings persistence (QSettings)

## Testing Recommendations
1. Test slider at various values (60%, 70%, 80%, 90%, 100%)
2. Verify pieces resize correctly on the board
3. Test drag-and-drop with different scale values
4. Verify window resize maintains correct piece sizes
5. Confirm settings persist after application restart
6. Test with both Unicode symbols and custom icons
