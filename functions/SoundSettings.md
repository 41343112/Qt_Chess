# SoundSettings 音效設定功能

## 概述
`SoundSettingsDialog` 類別提供完整的音效自訂功能，讓使用者可以為遊戲中的各種事件（移動、吃子、王車易位、將軍、將死）設定自訂音效檔案、調整音量、啟用/停用個別音效。

## 檔案位置
- **標頭檔**: `src/soundsettingsdialog.h`
- **實作檔**: `src/soundsettingsdialog.cpp`

## 主要資料結構

### SoundSettings 結構
```cpp
struct SoundSettings {
    QString moveSound;              // 移動音效檔案路徑
    QString captureSound;           // 吃子音效檔案路徑
    QString castlingSound;          // 王車易位音效檔案路徑
    QString checkSound;             // 將軍音效檔案路徑
    QString checkmateSound;         // 將死音效檔案路徑
    
    double moveVolume;              // 移動音量 (0.0-1.0)
    double captureVolume;           // 吃子音量 (0.0-1.0)
    double castlingVolume;          // 王車易位音量 (0.0-1.0)
    double checkVolume;             // 將軍音量 (0.0-1.0)
    double checkmateVolume;         // 將死音量 (0.0-1.0)
    
    bool moveSoundEnabled;          // 移動音效啟用狀態
    bool captureSoundEnabled;       // 吃子音效啟用狀態
    bool castlingSoundEnabled;      // 王車易位音效啟用狀態
    bool checkSoundEnabled;         // 將軍音效啟用狀態
    bool checkmateSoundEnabled;     // 將死音效啟用狀態
    bool allSoundsEnabled;          // 所有音效總開關
};
```

## UI 元件

### 總開關
- **allSoundsCheckBox**: 啟用/停用所有音效的主開關

### 每個音效的控制項
每種音效都有以下控制項：
- **soundEdit**: 顯示當前音效檔案路徑
- **soundCheckBox**: 啟用/停用該音效
- **volumeSlider**: 音量滑桿（0-100）
- **volumeLabel**: 顯示當前音量百分比
- **browseButton**: 瀏覽按鈕，選擇音效檔案
- **previewButton**: 預覽按鈕，播放音效
- **resetButton**: 重設按鈕，恢復為預設音效

## 主要功能

### 1. 設定管理

#### getSettings()
```cpp
SoundSettings getSettings() const
```
取得當前的音效設定。

**返回值**: `SoundSettings` 結構包含所有音效設定

#### setSettings()
```cpp
void setSettings(const SoundSettings& settings)
```
套用音效設定到對話框。

**參數**:
- `settings` - 要套用的音效設定

**用途**: 從儲存的設定載入到 UI

#### getDefaultSettings()
```cpp
static SoundSettings getDefaultSettings()
```
取得預設的音效設定。

**返回值**: 包含預設音效檔案路徑和音量的 `SoundSettings`

**預設設定**:
```cpp
SoundSettings SoundSettingsDialog::getDefaultSettings() {
    SoundSettings settings;
    settings.moveSound = ":/sounds/move.wav";
    settings.captureSound = ":/sounds/capture.wav";
    settings.castlingSound = ":/sounds/castling.wav";
    settings.checkSound = ":/sounds/check.wav";
    settings.checkmateSound = ":/sounds/checkmate.wav";
    
    settings.moveVolume = 0.5;
    settings.captureVolume = 0.5;
    settings.castlingVolume = 0.5;
    settings.checkVolume = 0.5;
    settings.checkmateVolume = 0.5;
    
    settings.moveSoundEnabled = true;
    settings.captureSoundEnabled = true;
    settings.castlingSoundEnabled = true;
    settings.checkSoundEnabled = true;
    settings.checkmateSoundEnabled = true;
    settings.allSoundsEnabled = true;
    
    return settings;
}
```

### 2. 音效檔案瀏覽

#### browseForSoundFile()
```cpp
QString browseForSoundFile()
```
開啟檔案瀏覽對話框選擇音效檔案。

**支援的格式**:
- WAV（建議，最佳相容性）
- MP3
- OGG

**實作**:
```cpp
QString SoundSettingsDialog::browseForSoundFile() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "選擇音效檔案",
        QDir::homePath(),
        "音效檔案 (*.wav *.mp3 *.ogg);;所有檔案 (*.*)"
    );
    return filename;
}
```

#### onBrowseMove/Capture/Castling/Check/Checkmate()
```cpp
void onBrowseMove()
void onBrowseCapture()
void onBrowseCastling()
void onBrowseCheck()
void onBrowseCheckmate()
```
各個音效的瀏覽按鈕處理函式。

**實作範例**:
```cpp
void SoundSettingsDialog::onBrowseMove() {
    QString filename = browseForSoundFile();
    if (!filename.isEmpty()) {
        m_moveSoundEdit->setText(filename);
        m_settings.moveSound = filename;
    }
}
```

### 3. 音效預覽

#### previewSound()
```cpp
void previewSound(const QString& soundFile, double volume)
```
播放指定的音效檔案進行預覽。

**參數**:
- `soundFile` - 音效檔案路徑
- `volume` - 音量（0.0-1.0）

**實作**:
```cpp
void SoundSettingsDialog::previewSound(const QString& soundFile, double volume) {
    if (soundFile.isEmpty()) return;
    
    m_previewSound.stop();  // 停止之前的預覽
    m_previewSound.setSource(QUrl::fromLocalFile(soundFile));
    m_previewSound.setVolume(volume);
    m_previewSound.play();
}
```

#### onPreviewMove/Capture/Castling/Check/Checkmate()
```cpp
void onPreviewMove()
void onPreviewCapture()
void onPreviewCastling()
void onPreviewCheck()
void onPreviewCheckmate()
```
各個音效的預覽按鈕處理函式。

**實作範例**:
```cpp
void SoundSettingsDialog::onPreviewMove() {
    previewSound(m_settings.moveSound, m_settings.moveVolume);
}
```

### 4. 重設功能

#### onResetMove/Capture/Castling/Check/Checkmate()
```cpp
void onResetMove()
void onResetCapture()
void onResetCastling()
void onResetCheck()
void onResetCheckmate()
```
重設個別音效為預設值。

**實作範例**:
```cpp
void SoundSettingsDialog::onResetMove() {
    SoundSettings defaults = getDefaultSettings();
    m_settings.moveSound = defaults.moveSound;
    m_settings.moveVolume = defaults.moveVolume;
    m_moveSoundEdit->setText(m_settings.moveSound);
    m_moveVolumeSlider->setValue(m_settings.moveVolume * 100);
}
```

#### onResetToDefaults()
```cpp
void onResetToDefaults()
```
重設所有音效為預設值。

**實作**:
```cpp
void SoundSettingsDialog::onResetToDefaults() {
    int result = QMessageBox::question(
        this,
        "重設為預設值",
        "確定要將所有音效設定重設為預設值嗎？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (result == QMessageBox::Yes) {
        setSettings(getDefaultSettings());
    }
}
```

### 5. 啟用/停用控制

#### onAllSoundsToggled()
```cpp
void onAllSoundsToggled(bool enabled)
```
處理總開關的切換。

**實作**:
```cpp
void SoundSettingsDialog::onAllSoundsToggled(bool enabled) {
    m_settings.allSoundsEnabled = enabled;
    
    // 啟用/停用所有個別音效開關
    m_moveSoundCheckBox->setEnabled(enabled);
    m_captureSoundCheckBox->setEnabled(enabled);
    m_castlingSoundCheckBox->setEnabled(enabled);
    m_checkSoundCheckBox->setEnabled(enabled);
    m_checkmateSoundCheckBox->setEnabled(enabled);
    
    // 如果停用總開關，灰化所有控制項
    if (!enabled) {
        // 停用所有音量滑桿和按鈕
        m_moveVolumeSlider->setEnabled(false);
        m_moveBrowseButton->setEnabled(false);
        m_movePreviewButton->setEnabled(false);
        // ... 對其他音效重複
    }
}
```

#### updateAllSoundsCheckBox()
```cpp
void updateAllSoundsCheckBox()
```
根據個別音效的啟用狀態更新總開關。

**邏輯**:
- 如果所有音效都停用 → 總開關取消勾選
- 如果至少有一個音效啟用 → 總開關勾選

### 6. UI 設置

#### setupUI()
```cpp
void setupUI()
```
建立對話框的使用者介面。

**佈局結構**:
```
┌─────────────────────────────────────┐
│ 音效設定                            │
├─────────────────────────────────────┤
│ ☑ 啟用所有音效                     │
├─────────────────────────────────────┤
│ 移動音效:                           │
│ ☑ 啟用  [檔案路徑___] [瀏覽] [預覽] │
│ 音量: [━━━━━━━━━━] 50%    [重設]   │
├─────────────────────────────────────┤
│ 吃子音效:                           │
│ ☑ 啟用  [檔案路徑___] [瀏覽] [預覽] │
│ 音量: [━━━━━━━━━━] 50%    [重設]   │
├─────────────────────────────────────┤
│ ... (其他音效) ...                  │
├─────────────────────────────────────┤
│         [重設為預設值]              │
│         [確定] [取消]               │
└─────────────────────────────────────┘
```

#### createSoundRow()
```cpp
void createSoundRow(QGridLayout* gridLayout, int& row, 
                   const QString& label, ...)
```
建立單一音效的設定列（包含所有控制項）。

**參數**:
- `gridLayout` - 要加入的網格佈局
- `row` - 當前列號（會自動遞增）
- `label` - 音效標籤（如「移動音效」）
- 其他參數為各種 UI 元件的參考

**用途**: 減少重複程式碼，統一建立所有音效的 UI

#### applyModernDialogStyle()
```cpp
void applyModernDialogStyle()
```
套用現代科技風格的 CSS 樣式。

**樣式特點**:
- 深色背景
- 圓角按鈕
- 現代滑桿樣式
- 柔和的陰影效果

## 使用範例

### 開啟設定對話框
```cpp
void MainWindow::onSoundSettingsClicked() {
    SoundSettingsDialog dialog(this);
    
    // 載入當前設定
    dialog.setSettings(m_currentSoundSettings);
    
    // 顯示對話框
    if (dialog.exec() == QDialog::Accepted) {
        // 使用者點擊「確定」，取得新設定
        m_currentSoundSettings = dialog.getSettings();
        
        // 儲存設定
        saveSoundSettings(m_currentSoundSettings);
        
        // 套用新設定
        applySoundSettings(m_currentSoundSettings);
    }
}
```

### 播放音效
```cpp
void MainWindow::playMoveSound() {
    if (!m_soundSettings.allSoundsEnabled) return;
    if (!m_soundSettings.moveSoundEnabled) return;
    
    m_soundEffect.stop();
    m_soundEffect.setSource(QUrl::fromLocalFile(m_soundSettings.moveSound));
    m_soundEffect.setVolume(m_soundSettings.moveVolume);
    m_soundEffect.play();
}

void MainWindow::playCaptureSound() {
    if (!m_soundSettings.allSoundsEnabled) return;
    if (!m_soundSettings.captureSoundEnabled) return;
    
    m_soundEffect.stop();
    m_soundEffect.setSource(QUrl::fromLocalFile(m_soundSettings.captureSound));
    m_soundEffect.setVolume(m_soundSettings.captureVolume);
    m_soundEffect.play();
}
```

### 儲存和載入設定
```cpp
// 儲存設定到 QSettings
void saveSoundSettings(const SoundSettings& settings) {
    QSettings config("MyCompany", "Qt_Chess");
    
    config.setValue("sound/moveSound", settings.moveSound);
    config.setValue("sound/captureSound", settings.captureSound);
    config.setValue("sound/castlingSound", settings.castlingSound);
    config.setValue("sound/checkSound", settings.checkSound);
    config.setValue("sound/checkmateSound", settings.checkmateSound);
    
    config.setValue("sound/moveVolume", settings.moveVolume);
    config.setValue("sound/captureVolume", settings.captureVolume);
    config.setValue("sound/castlingVolume", settings.castlingVolume);
    config.setValue("sound/checkVolume", settings.checkVolume);
    config.setValue("sound/checkmateVolume", settings.checkmateVolume);
    
    config.setValue("sound/moveSoundEnabled", settings.moveSoundEnabled);
    config.setValue("sound/captureSoundEnabled", settings.captureSoundEnabled);
    config.setValue("sound/castlingSoundEnabled", settings.castlingSoundEnabled);
    config.setValue("sound/checkSoundEnabled", settings.checkSoundEnabled);
    config.setValue("sound/checkmateSoundEnabled", settings.checkmateSoundEnabled);
    config.setValue("sound/allSoundsEnabled", settings.allSoundsEnabled);
}

// 從 QSettings 載入設定
SoundSettings loadSoundSettings() {
    QSettings config("MyCompany", "Qt_Chess");
    SoundSettings defaults = SoundSettingsDialog::getDefaultSettings();
    
    SoundSettings settings;
    settings.moveSound = config.value("sound/moveSound", defaults.moveSound).toString();
    settings.captureSound = config.value("sound/captureSound", defaults.captureSound).toString();
    // ... 載入其他設定 ...
    
    settings.allSoundsEnabled = config.value("sound/allSoundsEnabled", true).toBool();
    
    return settings;
}
```

### 在遊戲中使用
```cpp
void ChessGame::onPieceMoved(const QPoint& from, const QPoint& to) {
    // 判斷移動類型並播放對應音效
    if (isCastling(from, to)) {
        playSound(SoundType::Castling);
    } else if (isCapture(to)) {
        playSound(SoundType::Capture);
    } else {
        playSound(SoundType::Move);
    }
    
    // 檢查是否將軍或將死
    if (isCheckmate()) {
        playSound(SoundType::Checkmate);
    } else if (isCheck()) {
        playSound(SoundType::Check);
    }
}

void ChessGame::playSound(SoundType type) {
    if (!m_soundSettings.allSoundsEnabled) return;
    
    QString soundFile;
    double volume;
    bool enabled;
    
    switch (type) {
        case SoundType::Move:
            soundFile = m_soundSettings.moveSound;
            volume = m_soundSettings.moveVolume;
            enabled = m_soundSettings.moveSoundEnabled;
            break;
        case SoundType::Capture:
            soundFile = m_soundSettings.captureSound;
            volume = m_soundSettings.captureVolume;
            enabled = m_soundSettings.captureSoundEnabled;
            break;
        // ... 其他類型 ...
    }
    
    if (!enabled) return;
    
    m_soundEffect.stop();
    m_soundEffect.setSource(QUrl::fromLocalFile(soundFile));
    m_soundEffect.setVolume(volume);
    m_soundEffect.play();
}
```

## 音效檔案管理

### 預設音效檔案
預設音效檔案嵌入在應用程式資源中：
```qrc
<RCC>
    <qresource prefix="/sounds">
        <file>move.wav</file>
        <file>capture.wav</file>
        <file>castling.wav</file>
        <file>check.wav</file>
        <file>checkmate.wav</file>
    </qresource>
</RCC>
```

### 自訂音效檔案
使用者可以選擇本地檔案系統中的音效檔案。檔案路徑直接儲存在設定中。

### 音效檔案格式建議
- **WAV**: 最佳相容性，所有平台支援
- **MP3**: 較小的檔案大小，需要額外的解碼器
- **OGG**: 開源格式，較小檔案大小

### 音效檔案特性建議
- **取樣率**: 44.1kHz 或 48kHz
- **位元深度**: 16-bit
- **長度**: 0.5-2 秒
- **音量**: 正規化到 -3dB 以避免削波

## 使用者體驗優化

### 即時預覽
使用者調整音量滑桿時，可以即時聽到效果：
```cpp
connect(m_moveVolumeSlider, &QSlider::valueChanged, this, [this](int value) {
    m_settings.moveVolume = value / 100.0;
    m_moveVolumeLabel->setText(QString("%1%").arg(value));
    
    // 可選：即時預覽（調整時自動播放）
    // previewSound(m_settings.moveSound, m_settings.moveVolume);
});
```

### 視覺回饋
- 音量滑桿旁顯示百分比數字
- 停用的控制項呈現灰化狀態
- 預覽按鈕按下時有視覺效果

### 錯誤處理
```cpp
void SoundSettingsDialog::onBrowseMove() {
    QString filename = browseForSoundFile();
    if (!filename.isEmpty()) {
        // 驗證檔案是否存在
        QFile file(filename);
        if (!file.exists()) {
            QMessageBox::warning(this, "錯誤", "找不到指定的音效檔案");
            return;
        }
        
        // 驗證檔案格式（簡單檢查副檔名）
        QString ext = QFileInfo(filename).suffix().toLower();
        if (ext != "wav" && ext != "mp3" && ext != "ogg") {
            QMessageBox::warning(this, "錯誤", "不支援的音效格式");
            return;
        }
        
        m_moveSoundEdit->setText(filename);
        m_settings.moveSound = filename;
    }
}
```

## 依賴項目
- **Qt Multimedia**: `QSoundEffect`
- **Qt Widgets**: `QDialog`, `QSlider`, `QCheckBox`, `QPushButton`
- **Qt Core**: `QSettings`（用於儲存設定）

## 相關類別
- `Qt_Chess` - 使用音效設定播放遊戲音效
- `ChessBoard` - 遊戲邏輯觸發音效播放

## 參考資源
- [Qt Multimedia 文檔](https://doc.qt.io/qt-5/qtmultimedia-index.html)
- [QSoundEffect 類別參考](https://doc.qt.io/qt-5/qsoundeffect.html)
- [音訊檔案格式比較](https://en.wikipedia.org/wiki/Audio_file_format)
