# TimeControl 時間控制功能

## 概述
時間控制功能為西洋棋遊戲添加計時器，支援多種時間模式、每著加秒（increment）、超時判負等功能。計時器會根據當前輪到的玩家自動倒數，並在超時時結束遊戲。

## 主要檔案
時間控制功能整合在主遊戲類別中：
- **主要實作**: `src/qt_chess.cpp`
- **介面定義**: `src/qt_chess.h`

## 核心功能

### 1. 時間模式

#### 支援的時間模式
```cpp
enum class TimeMode {
    Unlimited,    // 無限制時間
    Bullet,       // 快棋（1-2分鐘）
    Blitz,        // 快速（3-5分鐘）
    Rapid,        // 快速（10-15分鐘）
    Classical,    // 經典（30-60分鐘）
    Custom        // 自訂時間
};
```

#### 預設時間設定
- **無限制**: 不計時
- **30秒**: 超快速模式
- **1分鐘**: 子彈模式
- **3分鐘**: 閃電戰
- **5分鐘**: 快速
- **10分鐘**: 快速
- **15分鐘**: 快速
- **30分鐘**: 經典
- **60分鐘**: 慢棋

### 2. 計時器管理

#### 計時器初始化
```cpp
void Qt_Chess::initializeTimers() {
    // 建立計時器物件
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &Qt_Chess::onTimerTick);
    
    // 初始化剩餘時間
    m_whiteTimeRemaining = m_totalTimeSeconds;
    m_blackTimeRemaining = m_totalTimeSeconds;
    
    // 更新顯示
    updateTimerDisplay();
}
```

#### 計時器啟動
```cpp
void Qt_Chess::startTimer() {
    if (m_totalTimeSeconds == 0) return;  // 無限制模式不啟動計時器
    
    // 在第一步棋走出後啟動
    if (!m_timerStarted) {
        m_timerStarted = true;
        m_gameTimer->start(1000);  // 每秒觸發一次
        qDebug() << "計時器已啟動";
    }
}
```

#### 計時器暫停/恢復
```cpp
void Qt_Chess::pauseTimer() {
    if (m_gameTimer->isActive()) {
        m_gameTimer->stop();
        m_timerPaused = true;
    }
}

void Qt_Chess::resumeTimer() {
    if (m_timerPaused && m_totalTimeSeconds > 0) {
        m_gameTimer->start(1000);
        m_timerPaused = false;
    }
}
```

### 3. 時間倒數邏輯

#### onTimerTick()
```cpp
void Qt_Chess::onTimerTick() {
    // 確定當前玩家
    bool isWhiteTurn = (m_board->getCurrentPlayer() == PieceColor::White);
    
    // 扣除當前玩家的時間
    if (isWhiteTurn) {
        m_whiteTimeRemaining--;
        if (m_whiteTimeRemaining <= 0) {
            onTimeOut(PieceColor::White);
            return;
        }
    } else {
        m_blackTimeRemaining--;
        if (m_blackTimeRemaining <= 0) {
            onTimeOut(PieceColor::Black);
            return;
        }
    }
    
    // 更新顯示
    updateTimerDisplay();
    
    // 時間警告（最後10秒）
    if ((isWhiteTurn && m_whiteTimeRemaining <= 10) ||
        (!isWhiteTurn && m_blackTimeRemaining <= 10)) {
        highlightTimerWarning();
    }
}
```

#### 超時處理
```cpp
void Qt_Chess::onTimeOut(PieceColor color) {
    // 停止計時器
    m_gameTimer->stop();
    
    // 判定輸贏
    QString message;
    if (color == PieceColor::White) {
        message = "白方超時！黑方獲勝！";
        m_board->setGameResult(GameResult::BlackWins);
    } else {
        message = "黑方超時！白方獲勝！";
        m_board->setGameResult(GameResult::WhiteWins);
    }
    
    // 顯示結果
    QMessageBox::information(this, "遊戲結束", message);
    
    // 停用棋盤
    disableBoard();
}
```

### 4. 加秒（Increment）功能

#### 每著加秒設定
```cpp
// 設定每著加秒時間（秒）
int m_incrementSeconds;  // 0-60秒
```

#### 移動後加秒
```cpp
void Qt_Chess::onPieceMoved(const QPoint& from, const QPoint& to) {
    // 執行移動
    bool moveSuccess = m_board->movePiece(from, to);
    
    if (moveSuccess) {
        // 為剛走完的玩家加秒
        PieceColor movedColor = m_board->getCurrentPlayer() == PieceColor::White 
                                ? PieceColor::Black 
                                : PieceColor::White;
        
        if (movedColor == PieceColor::White) {
            m_whiteTimeRemaining += m_incrementSeconds;
        } else {
            m_blackTimeRemaining += m_incrementSeconds;
        }
        
        // 更新顯示
        updateTimerDisplay();
        
        // 啟動計時器（首次移動後）
        if (!m_timerStarted) {
            startTimer();
        }
    }
}
```

**加秒時機**:
- Fischer 模式：每次移動後立即加秒
- Bronstein 模式：使用的時間不超過加秒時間時才補償
- 本實作採用 Fischer 模式（標準模式）

### 5. UI 顯示

#### 計時器顯示元件
```cpp
QLabel* m_whiteTimerLabel;  // 白方計時器
QLabel* m_blackTimerLabel;  // 黑方計時器
QWidget* m_whiteTimerWidget;  // 白方計時器容器
QWidget* m_blackTimerWidget;  // 黑方計時器容器
```

#### 時間格式化
```cpp
QString Qt_Chess::formatTime(int seconds) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(secs, 2, 10, QChar('0'));
    } else {
        return QString("%1:%2")
            .arg(minutes)
            .arg(secs, 2, 10, QChar('0'));
    }
}
```

**顯示範例**:
- `5:00` - 5分鐘
- `0:30` - 30秒
- `1:05:30` - 1小時5分30秒

#### 更新顯示
```cpp
void Qt_Chess::updateTimerDisplay() {
    // 更新白方計時器
    m_whiteTimerLabel->setText(formatTime(m_whiteTimeRemaining));
    
    // 更新黑方計時器
    m_blackTimerLabel->setText(formatTime(m_blackTimeRemaining));
    
    // 高亮當前玩家的計時器
    bool isWhiteTurn = (m_board->getCurrentPlayer() == PieceColor::White);
    
    if (isWhiteTurn) {
        m_whiteTimerWidget->setStyleSheet("background-color: #90EE90;");  // 綠色
        m_blackTimerWidget->setStyleSheet("background-color: #E0E0E0;");  // 灰色
    } else {
        m_whiteTimerWidget->setStyleSheet("background-color: #E0E0E0;");
        m_blackTimerWidget->setStyleSheet("background-color: #90EE90;");
    }
}
```

#### 時間警告顯示
```cpp
void Qt_Chess::highlightTimerWarning() {
    bool isWhiteTurn = (m_board->getCurrentPlayer() == PieceColor::White);
    
    if (isWhiteTurn && m_whiteTimeRemaining <= 10) {
        m_whiteTimerWidget->setStyleSheet(
            "background-color: #FF6B6B; color: white;"  // 紅色警告
        );
    } else if (!isWhiteTurn && m_blackTimeRemaining <= 10) {
        m_blackTimerWidget->setStyleSheet(
            "background-color: #FF6B6B; color: white;"
        );
    }
}
```

### 6. 時間設定 UI

#### 時間選擇下拉選單
```cpp
void Qt_Chess::setupTimeControlUI() {
    m_timeComboBox = new QComboBox();
    m_timeComboBox->addItem("無限制", 0);
    m_timeComboBox->addItem("30 秒", 30);
    m_timeComboBox->addItem("1 分鐘", 60);
    m_timeComboBox->addItem("3 分鐘", 180);
    m_timeComboBox->addItem("5 分鐘", 300);
    m_timeComboBox->addItem("10 分鐘", 600);
    m_timeComboBox->addItem("15 分鐘", 900);
    m_timeComboBox->addItem("30 分鐘", 1800);
    m_timeComboBox->addItem("60 分鐘", 3600);
    
    connect(m_timeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &Qt_Chess::onTimeSettingChanged);
}
```

#### 加秒設定滑桿
```cpp
void Qt_Chess::setupIncrementUI() {
    m_incrementLabel = new QLabel("每著加秒:");
    
    m_incrementSlider = new QSlider(Qt::Horizontal);
    m_incrementSlider->setMinimum(0);
    m_incrementSlider->setMaximum(60);
    m_incrementSlider->setValue(0);
    
    m_incrementValueLabel = new QLabel("0 秒");
    
    connect(m_incrementSlider, &QSlider::valueChanged, this, [this](int value) {
        m_incrementSeconds = value;
        m_incrementValueLabel->setText(QString("%1 秒").arg(value));
    });
}
```

#### 設定變更處理
```cpp
void Qt_Chess::onTimeSettingChanged(int index) {
    int seconds = m_timeComboBox->itemData(index).toInt();
    m_totalTimeSeconds = seconds;
    
    // 如果遊戲正在進行中，詢問是否重置
    if (m_timerStarted) {
        int result = QMessageBox::question(
            this,
            "變更時間設定",
            "變更時間設定將重置當前遊戲。是否繼續？",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (result == QMessageBox::Yes) {
            newGame();
        } else {
            // 恢復原設定
            m_timeComboBox->blockSignals(true);
            // 設定回原值...
            m_timeComboBox->blockSignals(false);
        }
    } else {
        // 遊戲未開始，直接更新
        m_whiteTimeRemaining = seconds;
        m_blackTimeRemaining = seconds;
        updateTimerDisplay();
    }
}
```

### 7. 不同模式的計時器行為

#### 雙人模式
```cpp
// 標準計時：每個玩家的回合計時
// 第一步移動後啟動計時器
// 切換回合時切換計時玩家
```

#### 電腦模式
```cpp
void Qt_Chess::onComputerThinking() {
    // 暫停玩家計時器
    pauseTimer();
    
    // 顯示「電腦思考中...」
    statusLabel->setText("電腦思考中...");
}

void Qt_Chess::onComputerMoveComplete() {
    // 恢復計時器（現在輪到玩家）
    resumeTimer();
    
    // 更新狀態
    statusLabel->setText("輪到您了");
}
```

#### 線上模式
```cpp
// 雙方計時器同步
// 對手移動時本地計時器也要相應更新
void Qt_Chess::onOpponentMove() {
    // 對手走完，輪到我方，啟動我方計時器
    resumeTimer();
}
```

### 8. 回放模式時的計時器

#### 進入回放模式
```cpp
void Qt_Chess::enterReplayMode() {
    // 暫停計時器
    if (m_gameTimer->isActive()) {
        m_timerWasPausedForReplay = false;
        pauseTimer();
    } else {
        m_timerWasPausedForReplay = true;
    }
    
    // 標記回放模式
    m_isReplayMode = true;
    
    // 顯示提示
    statusLabel->setText("回放模式 - 計時器已暫停");
}
```

#### 退出回放模式
```cpp
void Qt_Chess::exitReplayMode() {
    // 恢復計時器（如果之前在運行）
    if (!m_timerWasPausedForReplay) {
        resumeTimer();
    }
    
    // 清除回放模式標記
    m_isReplayMode = false;
    
    // 更新狀態
    updateStatus();
}
```

## 持久化設定

### 儲存設定
```cpp
void Qt_Chess::saveTimeSettings() {
    QSettings settings("MyCompany", "Qt_Chess");
    settings.setValue("time/totalSeconds", m_totalTimeSeconds);
    settings.setValue("time/incrementSeconds", m_incrementSeconds);
}
```

### 載入設定
```cpp
void Qt_Chess::loadTimeSettings() {
    QSettings settings("MyCompany", "Qt_Chess");
    m_totalTimeSeconds = settings.value("time/totalSeconds", 300).toInt();  // 預設5分鐘
    m_incrementSeconds = settings.value("time/incrementSeconds", 0).toInt();
    
    // 更新 UI
    updateTimeControlUI();
}
```

## 使用範例

### 設定快速模式（3+2）
```cpp
// 3分鐘 + 每著加2秒
m_totalTimeSeconds = 180;
m_incrementSeconds = 2;
initializeTimers();
```

### 設定經典模式（30+0）
```cpp
// 30分鐘，無加秒
m_totalTimeSeconds = 1800;
m_incrementSeconds = 0;
initializeTimers();
```

### 設定子彈模式（1+0）
```cpp
// 1分鐘，無加秒
m_totalTimeSeconds = 60;
m_incrementSeconds = 0;
initializeTimers();
```

## 進階功能

### 延時模式（Delay）
```cpp
// 可選：實作延時模式
// 每次移動前有 N 秒延時，延時期間不扣時間
int m_delaySeconds;

void Qt_Chess::onTurnStart() {
    m_inDelay = true;
    m_delayRemaining = m_delaySeconds;
    
    QTimer::singleShot(m_delaySeconds * 1000, this, [this]() {
        m_inDelay = false;
        startTimer();
    });
}
```

### 時間控制階段
```cpp
// 可選：複雜時間控制（如：前40步每方90分鐘，之後30分鐘）
struct TimeControlPhase {
    int moveCount;      // 步數限制
    int timeSeconds;    // 該階段的時間
    int incrementSeconds;
};

std::vector<TimeControlPhase> m_timePhases;
int m_currentPhase = 0;
```

## 相關文檔
- 詳細的倒數邏輯: `docs/TIMER_COUNTDOWN_LOGIC.md`
- 時間控制功能說明: `docs/TIME_CONTROL_FEATURE.md`

## 相關類別
- `ChessBoard` - 追蹤當前玩家和移動
- `Qt_Chess` - 主視窗，整合計時器 UI 和邏輯

## 參考資源
- [FIDE 時間控制規則](https://handbook.fide.com/chapter/E012023)
- [Chess Time Controls 說明](https://en.wikipedia.org/wiki/Time_control#Chess)
