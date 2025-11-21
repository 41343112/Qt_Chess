# User Interface Preview - Preset Icon Sets

This document describes the new UI changes for selecting preset chess piece icon sets.

## Settings Dialog Changes

When you open "設定" → "棋子圖標設定" (Settings → Piece Icon Settings), you'll now see:

### Top Section: Icon Set Selection
```
┌─────────────────────────────────────────────────────────┐
│ 圖標集選擇 (Icon Set Selection)                          │
├─────────────────────────────────────────────────────────┤
│ 選擇圖標集：[v Unicode 符號 (預設)            ▼]        │
│                                                           │
│ Options in dropdown:                                      │
│   • Unicode 符號 (預設) - Default Unicode symbols        │
│   • 預設圖標集 1 - Preset Icon Set 1                     │
│   • 預設圖標集 2 - Preset Icon Set 2                     │
│   • 預設圖標集 3 - Preset Icon Set 3                     │
│   • 自訂圖標 - Custom Icons                              │
└─────────────────────────────────────────────────────────┘
```

### Middle Section: White Pieces
```
┌─────────────────────────────────────────────────────────┐
│ 白方棋子 (White Pieces)                                  │
├─────────────────────────────────────────────────────────┤
│ 國王 (♔)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 皇后 (♕)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 城堡 (♖)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 主教 (♗)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 騎士 (♘)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 兵 (♙)    [icon path display]  [瀏覽] [預覽] [重設]    │
└─────────────────────────────────────────────────────────┘
```

### Bottom Section: Black Pieces
```
┌─────────────────────────────────────────────────────────┐
│ 黑方棋子 (Black Pieces)                                  │
├─────────────────────────────────────────────────────────┤
│ 國王 (♚)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 皇后 (♛)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 城堡 (♜)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 主教 (♝)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 騎士 (♞)  [icon path display]  [瀏覽] [預覽] [重設]    │
│ 兵 (♟)    [icon path display]  [瀏覽] [預覽] [重設]    │
└─────────────────────────────────────────────────────────┘
```

### Action Buttons
```
[重設為預設值]              [確定]  [取消]
```

## Behavior

### When selecting "Unicode 符號 (預設)"
- Icon path fields show: (empty)
- 瀏覽 (Browse), 重設 (Reset) buttons: **DISABLED**
- 預覽 (Preview) buttons: **ENABLED** (shows Unicode symbols)
- Board displays: Unicode chess symbols (♔, ♕, ♖, etc.)

### When selecting "預設圖標集 1/2/3"
- Icon path fields show: `:/resources/icons/set1/white_king.png` etc.
- 瀏覽 (Browse), 重設 (Reset) buttons: **DISABLED**
- 預覽 (Preview) buttons: **ENABLED** (shows preset images)
- Board displays: Preset icon images

### When selecting "自訂圖標" (Custom Icons)
- Icon path fields show: User's custom paths or (empty)
- 瀏覽 (Browse), 重設 (Reset) buttons: **ENABLED**
- 預覽 (Preview) buttons: **ENABLED**
- Board displays: User's custom icon images

## Workflow Examples

### Scenario 1: Using a Preset Icon Set
1. Open Settings → Piece Icon Settings
2. Click dropdown at top
3. Select "預設圖標集 1"
4. All icon paths automatically populate with preset paths
5. Click "預覽" to see any icon before applying
6. Click "確定" to apply
7. Board immediately shows new icons

### Scenario 2: Switching Between Sets
1. Currently using Unicode symbols
2. Select "預設圖標集 2" from dropdown
3. Click "確定"
4. Board switches to Preset Set 2
5. Later, select "預設圖標集 3"
6. Click "確定"
7. Board switches to Preset Set 3
8. No need to browse for files!

### Scenario 3: Custom Icons (Existing Functionality)
1. Select "自訂圖標" from dropdown
2. Browse buttons become enabled
3. Click "瀏覽" next to each piece
4. Select your custom image files
5. Click "確定"
6. Board shows your custom icons

### Scenario 4: Return to Default
1. Select "Unicode 符號 (預設)" from dropdown
2. Click "確定"
3. Board returns to Unicode symbols
4. Or click "重設為預設值" button anytime

## Key Improvements

1. **Easier Icon Management**: Select entire sets with one click
2. **No File Browsing**: Preset sets don't require browsing
3. **Quick Switching**: Change appearance instantly
4. **Safe Exploration**: Preview before applying
5. **Backward Compatible**: Custom icons still work as before

## Technical Notes

- Settings persist between application restarts
- Icon set choice is saved in application settings
- Preset icon paths use Qt resource system (:/resources/...)
- Custom icon paths use absolute file paths
- Preview shows actual icon or Unicode symbol if icon unavailable
