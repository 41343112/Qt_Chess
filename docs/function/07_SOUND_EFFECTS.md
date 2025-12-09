# 音效系統

## 功能概述

Qt_Chess 提供豐富的音效系統，為不同的遊戲事件播放相應的音效，增強遊戲體驗和視覺回饋。

## 主要功能

### 1. 音效類型

#### 移動音效
- **觸發時機**：普通移動棋子
- **用途**：確認移動完成
- **預設音效**：輕柔的移動聲

#### 吃子音效
- **觸發時機**：吃掉對方棋子
- **用途**：強調戰術成功
- **預設音效**：較重的碰撞聲

#### 王車易位音效
- **觸發時機**：執行王車易位
- **用途**：標示特殊走法
- **預設音效**：特殊的滑動聲

#### 將軍音效
- **觸發時機**：國王被將軍
- **用途**：警告玩家危險
- **預設音效**：警示音

#### 將死音效
- **觸發時機**：遊戲結束（將死）
- **用途**：標示遊戲結束
- **預設音效**：終局音效

### 2. 音效播放

#### 自動播放
- 遊戲事件發生時自動播放
- 適當的音量和時機
- 不影響遊戲進行

#### 播放優先級
1. 將死（最高優先級）
2. 將軍
3. 吃子
4. 王車易位
5. 普通移動（最低優先級）

#### 音效覆蓋
- 高優先級音效會覆蓋低優先級
- 例如：吃子並將軍 → 播放將軍音效

### 3. 預設音效

#### 內建資源
Qt_Chess 包含一套完整的預設音效：
- `:/sounds/move.wav` - 移動音效
- `:/sounds/capture.wav` - 吃子音效
- `:/sounds/castling.wav` - 王車易位音效
- `:/sounds/check.wav` - 將軍音效
- `:/sounds/checkmate.wav` - 將死音效

#### 音訊格式
- **主要格式**：WAV
- **備用格式**：MP3、OGG
- **建議**：使用 WAV 獲得最佳相容性

#### 品質特點
- 清晰不刺耳
- 音量適中
- 長度適當（0.5-2 秒）

### 4. 音效控制

#### 全域開關
- 啟用/停用所有音效
- 快速靜音
- 設定會保存

#### 個別控制
- 每種音效可獨立開關
- 在音效設定中調整
- 詳見 [08_SOUND_SETTINGS.md](08_SOUND_SETTINGS.md)

## 使用方法

### 基本使用

1. **正常遊戲**
   - 音效預設啟用
   - 自動播放相應音效
   - 無需手動操作

2. **辨識音效**
   - **移動**：輕柔的聲音
   - **吃子**：較重的聲音
   - **將軍**：警示音
   - **將死**：終局音

3. **享受音效**
   - 音效幫助確認操作
   - 注意對手的移動音效
   - 將軍音效提醒危險

### 快速靜音

**方法一：音效設定**
```
設定選單 → 音效設定 → 關閉「啟用所有音效」
```

**方法二：個別關閉**
```
設定選單 → 音效設定 → 取消勾選特定音效
```

### 自訂音效

如需更換音效檔案，請參閱 [08_SOUND_SETTINGS.md](08_SOUND_SETTINGS.md)

## 不同模式下的音效

### 雙人模式
- 每次移動播放音效
- 幫助雙方確認操作
- 增加對局氛圍

### AI 模式
- 玩家移動播放音效
- 電腦移動也播放音效
- 幫助追蹤電腦動作

### 線上模式
- 自己移動播放音效
- 對手移動播放音效
- 即時回饋網路同步

### 回放模式
- 回放時播放音效（可選）
- 重現對局過程
- 增強回放體驗

## 技術實現

### 音效播放器
```cpp
QSoundEffect m_moveSound;
QSoundEffect m_captureSound;
QSoundEffect m_castlingSound;
QSoundEffect m_checkSound;
QSoundEffect m_checkmateSound;
```

### 音效載入
```cpp
void loadSounds() {
    m_moveSound.setSource(QUrl("qrc:/sounds/move.wav"));
    m_captureSound.setSource(QUrl("qrc:/sounds/capture.wav"));
    m_castlingSound.setSource(QUrl("qrc:/sounds/castling.wav"));
    m_checkSound.setSource(QUrl("qrc:/sounds/check.wav"));
    m_checkmateSound.setSource(QUrl("qrc:/sounds/checkmate.wav"));
}
```

### 音效播放
```cpp
void playSound(SoundType type) {
    if (!m_soundsEnabled) return;
    
    switch (type) {
        case SoundType::Move:
            if (m_moveSoundEnabled)
                m_moveSound.play();
            break;
        case SoundType::Capture:
            if (m_captureSoundEnabled)
                m_captureSound.play();
            break;
        // ... 其他音效
    }
}
```

### 移動時的音效判定
```cpp
void onMoveMade(const Move& move) {
    if (isCheckmate()) {
        playSound(SoundType::Checkmate);
    } else if (isCheck()) {
        playSound(SoundType::Check);
    } else if (isCastling(move)) {
        playSound(SoundType::Castling);
    } else if (isCapture(move)) {
        playSound(SoundType::Capture);
    } else {
        playSound(SoundType::Move);
    }
}
```

### 音量控制
```cpp
void setVolume(SoundType type, double volume) {
    // volume: 0.0 - 1.0
    switch (type) {
        case SoundType::Move:
            m_moveSound.setVolume(volume);
            break;
        // ... 其他音效
    }
}
```

## 音效資源

### 資源檔案
音效檔案嵌入在 `resources.qrc` 中：
```xml
<RCC>
    <qresource prefix="/">
        <file>sounds/move.wav</file>
        <file>sounds/capture.wav</file>
        <file>sounds/castling.wav</file>
        <file>sounds/check.wav</file>
        <file>sounds/checkmate.wav</file>
    </qresource>
</RCC>
```

### 音效格式建議

#### WAV 格式（推薦）
- ✅ 所有平台完全支援
- ✅ 即時播放，無延遲
- ✅ 品質最佳
- ⚠️ 檔案較大

#### MP3 格式
- ✅ 檔案小
- ⚠️ 某些平台需要額外編解碼器
- ⚠️ 可能有播放延遲

#### OGG 格式
- ✅ 開源格式
- ✅ 檔案適中
- ⚠️ 相容性不如 WAV

## 相關文檔

- [08_SOUND_SETTINGS.md](08_SOUND_SETTINGS.md) - 自訂音效設定
- [01_CHESS_RULES.md](01_CHESS_RULES.md) - 遊戲規則與音效對應
- [02_SPECIAL_MOVES.md](02_SPECIAL_MOVES.md) - 特殊走法音效

## 程式碼參考

- `src/qt_chess.h/cpp` - 音效播放邏輯
- `resources.qrc` - 音效資源定義
- `sounds/` - 音效檔案目錄

## 疑難排解

### 音效無法播放
- 檢查音效是否啟用
- 確認系統音量不為靜音
- 檢查音訊驅動程式
- 確認音效檔案存在

### 音效延遲
- 嘗試使用 WAV 格式
- 減小音效檔案大小
- 檢查系統音訊設定
- 關閉其他音訊應用程式

### 音效過大/過小
- 調整系統音量
- 使用音效設定調整個別音量
- 編輯音效檔案降低/提高音量
- 詳見 [08_SOUND_SETTINGS.md](08_SOUND_SETTINGS.md)

### 音效錯亂
- 重啟應用程式
- 檢查音效設定
- 恢復預設音效
- 清除設定檔重新配置
