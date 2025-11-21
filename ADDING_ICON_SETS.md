# Adding Chess Piece Icon Sets

This guide explains how to add chess piece images to the three preset icon sets.

## Quick Start

1. Prepare 12 PNG images for each icon set (36 images total)
2. Place images in the appropriate directories (set1, set2, set3)
3. Uncomment the corresponding lines in resources.qrc
4. Rebuild the application

## Detailed Steps

### 1. Prepare Your Images

Create or obtain chess piece images with the following specifications:

- **Format**: PNG (recommended for transparent backgrounds)
- **Size**: At least 128x128 pixels (256x256 recommended)
- **Background**: Transparent preferred
- **File names**: Must match exactly as shown below

### 2. Required Files for Each Set

Each of the three sets (set1, set2, set3) needs these 12 files:

#### White Pieces:
- `white_king.png`
- `white_queen.png`
- `white_rook.png`
- `white_bishop.png`
- `white_knight.png`
- `white_pawn.png`

#### Black Pieces:
- `black_king.png`
- `black_queen.png`
- `black_rook.png`
- `black_bishop.png`
- `black_knight.png`
- `black_pawn.png`

### 3. Place Images in Directories

Copy your images to the appropriate directories:

```
resources/icons/set1/
├── white_king.png
├── white_queen.png
├── white_rook.png
├── white_bishop.png
├── white_knight.png
├── white_pawn.png
├── black_king.png
├── black_queen.png
├── black_rook.png
├── black_bishop.png
├── black_knight.png
└── black_pawn.png

resources/icons/set2/
├── (same 12 files)
└── ...

resources/icons/set3/
├── (same 12 files)
└── ...
```

### 4. Update resources.qrc

Open `resources.qrc` and uncomment the sections for the icon sets you've added:

**Before:**
```xml
<!-- Set 1 -->
<!--
<file>resources/icons/set1/white_king.png</file>
...
-->
```

**After:**
```xml
<!-- Set 1 -->
<file>resources/icons/set1/white_king.png</file>
<file>resources/icons/set1/white_queen.png</file>
<file>resources/icons/set1/white_rook.png</file>
<file>resources/icons/set1/white_bishop.png</file>
<file>resources/icons/set1/white_knight.png</file>
<file>resources/icons/set1/white_pawn.png</file>
<file>resources/icons/set1/black_king.png</file>
<file>resources/icons/set1/black_queen.png</file>
<file>resources/icons/set1/black_rook.png</file>
<file>resources/icons/set1/black_bishop.png</file>
<file>resources/icons/set1/black_knight.png</file>
<file>resources/icons/set1/black_pawn.png</file>
```

Repeat for Set 2 and Set 3 as needed.

### 5. Rebuild the Application

```bash
qmake Qt_Chess.pro
make clean
make
```

### 6. Test the Icon Sets

1. Run the application: `./Qt_Chess`
2. Go to menu: 設定 → 棋子圖標設定
3. Select a preset icon set from the dropdown
4. Click 確定 (OK)
5. Verify the icons appear correctly on the board

## Icon Set Suggestions

### Set 1: Classic Traditional
Traditional chess piece designs with realistic details.

### Set 2: Modern Minimalist
Simple, flat design with clean lines and solid colors.

### Set 3: Artistic Decorative
Creative, artistic interpretations of chess pieces.

## Troubleshooting

### Icons not showing?
1. Verify file names match exactly (case-sensitive)
2. Check that files are actual PNG images
3. Ensure resources.qrc was updated correctly
4. Rebuild the project completely (`make clean && make`)

### Icons appear distorted?
- Use square images (same width and height)
- Recommended size: 256x256 pixels
- Use PNG format with transparency

### Missing files error during build?
- Make sure all 12 PNG files exist in each set directory
- Only uncomment sections in resources.qrc for sets you've completed

## Where to Find Chess Piece Icons

Free chess piece icons can be found at:
- OpenGameArt.org
- WikiMedia Commons
- FlatIcon.com (attribution required)
- Custom creation using vector graphics software

## Notes

- You can add icons for just one set at a time
- Placeholder files (*.placeholder) are ignored by git
- Icons are embedded in the application binary for easy distribution
- Users can still choose custom icons by selecting "自訂圖標" (Custom Icons) option
