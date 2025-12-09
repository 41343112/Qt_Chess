# ReplayFeature 回放功能

## 概述
回放功能允許玩家在遊戲進行中或結束後回顧整局對弈，可以前後翻閱每一步棋，並在查看完畢後返回當前遊戲狀態繼續對弈。回放時計時器會自動暫停，退出回放後恢復。

## 主要檔案
回放功能整合在主遊戲類別和棋譜顯示中：
- **主要實作**: `src/qt_chess.cpp`
- **介面定義**: `src/qt_chess.h`

## 核心功能

### 1. 回放狀態管理

#### 回放模式標記
```cpp
bool m_isReplayMode;          // 是否處於回放模式
int m_currentReplayIndex;     // 當前回放的步驟索引
int m_totalMoveCount;         // 總移動數
ChessBoard m_replayBoard;     // 回放用的臨時棋盤
ChessBoard* m_originalBoard;  // 指向原始遊戲棋盤
```

#### 進入回放模式
```cpp
void Qt_Chess::enterReplayMode(int moveIndex) {
    if (m_isReplayMode) return;  // 已在回放模式
    
    // 儲存原始棋盤狀態
    m_originalBoard = m_board;
    
    // 建立回放棋盤副本
    m_replayBoard = *m_board;
    
    // 暫停計時器
    if (m_gameTimer && m_gameTimer->isActive()) {
        m_timerWasPausedForReplay = false;
        pauseTimer();
    } else {
        m_timerWasPausedForReplay = true;
    }
    
    // 設定回放模式
    m_isReplayMode = true;
    m_currentReplayIndex = moveIndex;
    
    // 重建棋盤到指定步驟
    reconstructBoardToMove(moveIndex);
    
    // 更新 UI
    updateReplayUI();
    showReplayControls(true);
    disableBoardInteraction();
    
    // 高亮當前步驟在棋譜列表
    highlightMoveInList(moveIndex);
    
    statusLabel->setText("回放模式 - 計時器已暫停");
}
```

#### 退出回放模式
```cpp
void Qt_Chess::exitReplayMode() {
    if (!m_isReplayMode) return;
    
    // 恢復原始棋盤
    m_board = m_originalBoard;
    
    // 恢復計時器
    if (!m_timerWasPausedForReplay && m_totalTimeSeconds > 0) {
        resumeTimer();
    }
    
    // 清除回放模式
    m_isReplayMode = false;
    
    // 更新 UI
    updateBoardDisplay();
    showReplayControls(false);
    enableBoardInteraction();
    clearMoveListHighlight();
    
    updateStatus();
}
```

### 2. 棋盤重建

#### reconstructBoardToMove()
```cpp
void Qt_Chess::reconstructBoardToMove(int moveIndex) {
    // 從初始狀態開始
    m_replayBoard.initializeBoard();
    
    // 重播所有移動直到指定步驟
    const auto& history = m_originalBoard->getMoveHistory();
    for (int i = 0; i <= moveIndex && i < history.size(); i++) {
        const MoveRecord& move = history[i];
        m_replayBoard.movePiece(move.from, move.to);
        
        // 處理兵升變
        if (move.isPromotion) {
            m_replayBoard.promotePawn(move.to, move.promotionType);
        }
    }
    
    // 更新顯示
    updateBoardDisplay();
}
```

**重建策略**:
- **優點**: 確保棋盤狀態完全正確
- **缺點**: 每次切換步驟都需要重新計算
- **優化**: 可以快取每一步的棋盤狀態

### 3. 回放導航

#### 導航按鈕
```cpp
QPushButton* m_firstMoveButton;   // ⏮ 跳到開始
QPushButton* m_prevMoveButton;    // ◀ 上一步
QPushButton* m_nextMoveButton;    // ▶ 下一步
QPushButton* m_lastMoveButton;    // ⏭ 跳到最後
QPushButton* m_exitReplayButton;  // 退出回放
```

#### 跳到開始
```cpp
void Qt_Chess::onFirstMoveClicked() {
    if (!m_isReplayMode) return;
    
    m_currentReplayIndex = -1;  // -1 表示初始狀態
    reconstructBoardToMove(-1);
    updateNavigationButtons();
    highlightMoveInList(-1);
}
```

#### 上一步
```cpp
void Qt_Chess::onPrevMoveClicked() {
    if (!m_isReplayMode) return;
    if (m_currentReplayIndex <= -1) return;  // 已在開始
    
    m_currentReplayIndex--;
    reconstructBoardToMove(m_currentReplayIndex);
    updateNavigationButtons();
    highlightMoveInList(m_currentReplayIndex);
}
```

#### 下一步
```cpp
void Qt_Chess::onNextMoveClicked() {
    if (!m_isReplayMode) return;
    
    const auto& history = m_originalBoard->getMoveHistory();
    if (m_currentReplayIndex >= history.size() - 1) return;  // 已在最後
    
    m_currentReplayIndex++;
    reconstructBoardToMove(m_currentReplayIndex);
    updateNavigationButtons();
    highlightMoveInList(m_currentReplayIndex);
}
```

#### 跳到最後
```cpp
void Qt_Chess::onLastMoveClicked() {
    if (!m_isReplayMode) return;
    
    const auto& history = m_originalBoard->getMoveHistory();
    m_currentReplayIndex = history.size() - 1;
    reconstructBoardToMove(m_currentReplayIndex);
    updateNavigationButtons();
    highlightMoveInList(m_currentReplayIndex);
}
```

#### 更新導航按鈕狀態
```cpp
void Qt_Chess::updateNavigationButtons() {
    const auto& history = m_originalBoard->getMoveHistory();
    
    // 第一步和上一步按鈕
    bool atStart = (m_currentReplayIndex <= -1);
    m_firstMoveButton->setEnabled(!atStart);
    m_prevMoveButton->setEnabled(!atStart);
    
    // 下一步和最後一步按鈕
    bool atEnd = (m_currentReplayIndex >= history.size() - 1);
    m_nextMoveButton->setEnabled(!atEnd);
    m_lastMoveButton->setEnabled(!atEnd);
}
```

### 4. 棋譜列表整合

#### 雙擊進入回放
```cpp
void Qt_Chess::setupMoveList() {
    m_moveListWidget = new QListWidget();
    
    // 連接雙擊事件
    connect(m_moveListWidget, &QListWidget::itemDoubleClicked,
            this, &Qt_Chess::onMoveListItemDoubleClicked);
}

void Qt_Chess::onMoveListItemDoubleClicked(QListWidgetItem* item) {
    // 取得點擊的移動索引
    int moveIndex = m_moveListWidget->row(item);
    
    // 進入回放模式
    enterReplayMode(moveIndex);
}
```

#### 高亮當前步驟
```cpp
void Qt_Chess::highlightMoveInList(int moveIndex) {
    // 清除之前的高亮
    for (int i = 0; i < m_moveListWidget->count(); i++) {
        QListWidgetItem* item = m_moveListWidget->item(i);
        item->setBackground(Qt::white);
    }
    
    // 高亮當前步驟
    if (moveIndex >= 0 && moveIndex < m_moveListWidget->count()) {
        QListWidgetItem* item = m_moveListWidget->item(moveIndex);
        item->setBackground(QColor("#FFD700"));  // 金色高亮
        
        // 滾動到可見區域
        m_moveListWidget->scrollToItem(item);
    }
}
```

### 5. UI 控制

#### 顯示/隱藏回放控制
```cpp
void Qt_Chess::showReplayControls(bool show) {
    m_firstMoveButton->setVisible(show);
    m_prevMoveButton->setVisible(show);
    m_nextMoveButton->setVisible(show);
    m_lastMoveButton->setVisible(show);
    m_exitReplayButton->setVisible(show);
    
    // 隱藏遊戲控制按鈕
    m_newGameButton->setVisible(!show);
    m_resignButton->setVisible(!show);
}
```

#### 停用/啟用棋盤互動
```cpp
void Qt_Chess::disableBoardInteraction() {
    // 停用所有方格的點擊事件
    for (auto& button : m_squareButtons) {
        button->setEnabled(false);
    }
    
    // 或設定標記，在事件處理中檢查
    m_boardInteractionEnabled = false;
}

void Qt_Chess::enableBoardInteraction() {
    for (auto& button : m_squareButtons) {
        button->setEnabled(true);
    }
    m_boardInteractionEnabled = true;
}

// 在滑鼠事件處理中
void Qt_Chess::onSquareClicked(int row, int col) {
    if (!m_boardInteractionEnabled) return;  // 回放模式中忽略點擊
    
    // 正常遊戲邏輯...
}
```

### 6. 鍵盤快捷鍵

#### 快捷鍵設定
```cpp
void Qt_Chess::setupReplayKeyboardShortcuts() {
    // Home: 跳到開始
    QShortcut* homeShortcut = new QShortcut(QKeySequence(Qt::Key_Home), this);
    connect(homeShortcut, &QShortcut::activated, this, &Qt_Chess::onFirstMoveClicked);
    
    // Left: 上一步
    QShortcut* leftShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(leftShortcut, &QShortcut::activated, this, &Qt_Chess::onPrevMoveClicked);
    
    // Right: 下一步
    QShortcut* rightShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(rightShortcut, &QShortcut::activated, this, &Qt_Chess::onNextMoveClicked);
    
    // End: 跳到最後
    QShortcut* endShortcut = new QShortcut(QKeySequence(Qt::Key_End), this);
    connect(endShortcut, &QShortcut::activated, this, &Qt_Chess::onLastMoveClicked);
    
    // Escape: 退出回放
    QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, &QShortcut::activated, this, [this]() {
        if (m_isReplayMode) {
            exitReplayMode();
        }
    });
}
```

### 7. 回放中的資訊顯示

#### 顯示移動資訊
```cpp
void Qt_Chess::updateReplayInfo() {
    const auto& history = m_originalBoard->getMoveHistory();
    
    QString info;
    if (m_currentReplayIndex == -1) {
        info = "初始位置";
    } else if (m_currentReplayIndex < history.size()) {
        const MoveRecord& move = history[m_currentReplayIndex];
        info = QString("第 %1 步: %2")
                   .arg(m_currentReplayIndex + 1)
                   .arg(move.algebraicNotation);
        
        // 顯示額外資訊
        if (move.isCapture) info += " (吃子)";
        if (move.isCastling) info += " (王車易位)";
        if (move.isCheck) info += " (將軍)";
        if (move.isCheckmate) info += " (將死)";
    }
    
    m_replayInfoLabel->setText(info);
}
```

#### 顯示進度
```cpp
void Qt_Chess::updateReplayProgress() {
    const auto& history = m_originalBoard->getMoveHistory();
    int total = history.size();
    int current = m_currentReplayIndex + 1;  // +1 因為索引從0開始
    
    QString progress = QString("步驟: %1 / %2").arg(current).arg(total);
    m_replayProgressLabel->setText(progress);
    
    // 可選：進度條
    m_replayProgressBar->setMaximum(total);
    m_replayProgressBar->setValue(current);
}
```

## 進階功能

### 1. 自動播放
```cpp
class Qt_Chess {
private:
    QTimer* m_autoPlayTimer;
    int m_autoPlaySpeed;  // 毫秒/步
    
public:
    void startAutoPlay() {
        if (!m_isReplayMode) return;
        
        m_autoPlayTimer = new QTimer(this);
        connect(m_autoPlayTimer, &QTimer::timeout, this, [this]() {
            if (m_currentReplayIndex >= m_originalBoard->getMoveHistory().size() - 1) {
                stopAutoPlay();  // 到達最後一步
                return;
            }
            onNextMoveClicked();
        });
        
        m_autoPlayTimer->start(m_autoPlaySpeed);
        m_playButton->setText("暫停");
    }
    
    void stopAutoPlay() {
        if (m_autoPlayTimer) {
            m_autoPlayTimer->stop();
            delete m_autoPlayTimer;
            m_autoPlayTimer = nullptr;
        }
        m_playButton->setText("播放");
    }
};
```

### 2. 速度控制
```cpp
void Qt_Chess::setupAutoPlaySpeedControl() {
    QComboBox* speedCombo = new QComboBox();
    speedCombo->addItem("0.5x", 2000);  // 2秒/步
    speedCombo->addItem("1x", 1000);    // 1秒/步
    speedCombo->addItem("2x", 500);     // 0.5秒/步
    speedCombo->addItem("4x", 250);     // 0.25秒/步
    
    connect(speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this, speedCombo](int index) {
        m_autoPlaySpeed = speedCombo->itemData(index).toInt();
    });
}
```

### 3. 回放時顯示移動箭頭
```cpp
void Qt_Chess::drawMoveArrow(const QPoint& from, const QPoint& to) {
    // 在棋盤上繪製箭頭顯示移動
    QGraphicsLineItem* arrow = new QGraphicsLineItem();
    arrow->setPen(QPen(QColor("#4A90E2"), 3));
    // 設定箭頭座標...
    m_boardScene->addItem(arrow);
}
```

### 4. 註解功能
```cpp
struct MoveComment {
    int moveIndex;
    QString comment;
    QString annotation;  // !, !!, ?, ??, !?, ?!
};

std::vector<MoveComment> m_moveComments;

void Qt_Chess::addMoveComment(int moveIndex, const QString& comment) {
    MoveComment mc;
    mc.moveIndex = moveIndex;
    mc.comment = comment;
    m_moveComments.push_back(mc);
}

void Qt_Chess::showMoveComment() {
    for (const auto& mc : m_moveComments) {
        if (mc.moveIndex == m_currentReplayIndex) {
            m_commentLabel->setText(mc.comment);
            return;
        }
    }
    m_commentLabel->clear();
}
```

## 使用範例

### 基本回放
```cpp
// 使用者雙擊棋譜列表中的第5步
void onMoveDoubleClicked(int moveIndex) {
    enterReplayMode(moveIndex);  // moveIndex = 5
    
    // 使用者可以使用按鈕或鍵盤導航
    // ← → 鍵前後翻閱
    // Home/End 跳到開始/結束
    // ESC 退出回放
}
```

### 遊戲結束後回顧
```cpp
void Qt_Chess::onGameEnded() {
    // 顯示結果
    showGameResult();
    
    // 提示使用者可以回放
    int result = QMessageBox::question(
        this,
        "遊戲結束",
        "是否要回顧本局對弈？",
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (result == QMessageBox::Yes) {
        enterReplayMode(0);  // 從第一步開始
        startAutoPlay();     // 自動播放
    }
}
```

## 相關文檔
- 回放功能說明: `docs/REPLAY_FEATURE.md`
- 回放按鈕: `docs/REPLAY_BUTTONS_FEATURE.md`
- 遊戲進行中回放: `docs/REPLAY_DURING_GAME_FEATURE.md`

## 相關類別
- `ChessBoard` - 提供移動歷史記錄
- `Qt_Chess` - 整合回放 UI 和邏輯

## 測試要點
1. 回放時計時器正確暫停
2. 退出回放後恢復到正確的遊戲狀態
3. 導航按鈕在邊界情況下正確停用
4. 棋譜列表高亮正確
5. 鍵盤快捷鍵正常工作
6. 棋盤互動在回放模式中正確停用
