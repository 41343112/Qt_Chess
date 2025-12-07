#ifndef THEME_H
#define THEME_H

#include <QString>

// ===== chess.jpg 實際顏色方案 =====
// 共用的主題配色常數，直接從 chess.jpg 提取

namespace Theme {
    // 背景顏色（來自 chess.jpg 主要顏色）
    const QString BG_DARK = "#060C10";           // 主要色 #2 (17.31%)
    const QString BG_MEDIUM = "#0C2E3E";         // 主要色 #1 (22.37%)
    const QString BG_PANEL = "#0F5975";          // 主要色 #4 (11.95%)
    
    // 強調色（來自 chess.jpg 主要顏色）
    const QString ACCENT_PRIMARY = "#39D0EE";    // 主要色 #6 (8.87%) - 明亮青色
    const QString ACCENT_SECONDARY = "#6F6648";  // 主要色 #7 (8.24%) - 棕褐色
    const QString ACCENT_SUCCESS = "#1F8FB1";    // 主要色 #5 (9.33%) - 青藍色
    const QString ACCENT_WARNING = "#B7AB92";    // 主要色 #8 (7.38%) - 淺褐色
    
    // 文字顏色（來自 chess.jpg）
    const QString TEXT_PRIMARY = "#B9E1EC";      // 主要色 #3 (14.57%) - 淺青色
    
    // 邊框顏色（來自 chess.jpg）
    const QString BORDER = "#48919D";            // 中間區域平均色
    
    // 懸停效果顏色
    const QString HOVER_BORDER = "#39D0EE";
    const QString HOVER_BG = "rgba(57, 208, 238, 0.2)";
    const QString PRESSED_BG = "rgba(57, 208, 238, 0.4)";
}

#endif // THEME_H
