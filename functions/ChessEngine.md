# ChessEngine AI引擎功能

## 概述
`ChessEngine` 類別負責整合 Stockfish 西洋棋引擎，實現人機對弈功能。透過 UCI（Universal Chess Interface）協議與 Stockfish 引擎通訊，提供不同難度等級的 AI 對手。

## 檔案位置
- **標頭檔**: `src/chessengine.h`
- **實作檔**: `src/chessengine.cpp`

## 主要資料結構

### EngineDifficulty - 引擎難度等級
```cpp
enum class EngineDifficulty {
    VeryEasy = 1,   // 等級 1 - 非常簡單
    Easy = 5,       // 等級 5 - 簡單
    Medium = 10,    // 等級 10 - 中等
    Hard = 15,      // 等級 15 - 困難
    VeryHard = 20   // 等級 20 - 非常困難
};
```

### GameMode - 遊戲模式
```cpp
enum class GameMode {
    HumanVsHuman,       // 雙人對弈
    HumanVsComputer,    // 人機對弈（玩家執白）
    ComputerVsHuman,    // 人機對弈（玩家執黑）
    OnlineGame          // 線上對戰
};
```

## 類別成員

### 私有成員變數
```cpp
QProcess* m_process;              // Stockfish 引擎進程
QString m_enginePath;             // 引擎可執行檔路徑
QString m_bestMove;               // 引擎計算出的最佳移動
QString m_currentPosition;        // 當前棋局位置（FEN 或移動列表）

GameMode m_gameMode;              // 當前遊戲模式
int m_skillLevel;                 // 技能等級（0-20）
int m_thinkingTimeMs;             // 思考時間（毫秒）
int m_searchDepth;                // 搜尋深度（1-30）

bool m_isReady;                   // 引擎是否就緒
bool m_isThinking;                // 引擎是否正在思考
```

## 主要功能

### 1. 引擎生命週期管理

#### startEngine()
```cpp
bool startEngine(const QString& enginePath)
```
啟動 Stockfish 引擎進程。

**實作步驟**:
1. 建立 QProcess 物件
2. 連接信號槽（輸出、錯誤、完成）
3. 啟動引擎進程
4. 發送 `uci` 命令初始化 UCI 協議
5. 等待引擎回應 `uciok`
6. 配置引擎參數（難度、技能等級）
7. 發送 `isready` 命令
8. 等待 `readyok` 回應

**返回值**: `true` 表示引擎成功啟動

#### stopEngine()
```cpp
void stopEngine()
```
停止引擎進程。

**實作**:
```cpp
if (m_process && m_process->state() == QProcess::Running) {
    sendCommand("quit");
    m_process->waitForFinished(1000);
    m_process->kill();  // 強制結束
}
```

#### isEngineRunning()
```cpp
bool isEngineRunning() const
```
檢查引擎是否正在運行。

### 2. UCI 協議通訊

#### sendCommand()
```cpp
void sendCommand(const QString& command)
```
向引擎發送 UCI 命令。

**常用 UCI 命令**:
- `uci` - 初始化 UCI 模式
- `isready` - 檢查引擎是否就緒
- `ucinewgame` - 開始新遊戲
- `position startpos moves e2e4 e7e5` - 設定棋局位置
- `position fen <fen_string>` - 使用 FEN 設定位置
- `go movetime 1000` - 思考 1000 毫秒
- `go depth 15` - 搜尋到深度 15
- `stop` - 停止思考
- `quit` - 退出引擎

**實作**:
```cpp
void ChessEngine::sendCommand(const QString& command) {
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->write((command + "\n").toUtf8());
    }
}
```

#### parseOutput()
```cpp
void parseOutput(const QString& line)
```
解析引擎的輸出。

**重要輸出訊息**:
- `uciok` - UCI 初始化完成
- `readyok` - 引擎就緒
- `bestmove e2e4` - 最佳移動
- `info depth 15 score cp 25` - 搜尋資訊（深度、評分）

**實作範例**:
```cpp
void ChessEngine::parseOutput(const QString& line) {
    if (line.startsWith("uciok")) {
        // UCI 初始化完成
        configureEngine();
    }
    else if (line.startsWith("readyok")) {
        m_isReady = true;
        emit engineReady();
    }
    else if (line.startsWith("bestmove")) {
        // 解析最佳移動: "bestmove e2e4 ponder e7e5"
        QStringList parts = line.split(' ');
        if (parts.size() >= 2) {
            m_bestMove = parts[1];
            m_isThinking = false;
            emit bestMoveFound(m_bestMove);
            emit thinkingStopped();
        }
    }
}
```

### 3. 遊戲控制

#### newGame()
```cpp
void newGame()
```
開始新遊戲。

**實作**:
```cpp
void ChessEngine::newGame() {
    sendCommand("ucinewgame");
    sendCommand("isready");
    m_currentPosition = "startpos";
    m_bestMove.clear();
}
```

#### setPosition()
```cpp
void setPosition(const QString& fen)
```
使用 FEN（Forsyth-Edwards Notation）設定棋局位置。

**FEN 範例**:
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

**實作**:
```cpp
void ChessEngine::setPosition(const QString& fen) {
    sendCommand("position fen " + fen);
    m_currentPosition = fen;
}
```

#### setPositionFromMoves()
```cpp
void setPositionFromMoves(const QStringList& moves)
```
使用移動列表設定棋局位置。

**實作**:
```cpp
void ChessEngine::setPositionFromMoves(const QStringList& moves) {
    QString cmd = "position startpos";
    if (!moves.isEmpty()) {
        cmd += " moves " + moves.join(" ");
    }
    sendCommand(cmd);
}
```

**範例**:
```cpp
QStringList moves = {"e2e4", "e7e5", "g1f3", "b8c6"};
engine.setPositionFromMoves(moves);
```

#### requestMove()
```cpp
void requestMove()
```
請求引擎計算最佳移動。

**實作**:
```cpp
void ChessEngine::requestMove() {
    if (!m_isReady) return;
    
    m_isThinking = true;
    emit thinkingStarted();
    
    // 根據設定決定使用時間限制或深度限制
    if (m_thinkingTimeMs > 0) {
        sendCommand(QString("go movetime %1").arg(m_thinkingTimeMs));
    } else if (m_searchDepth > 0) {
        sendCommand(QString("go depth %1").arg(m_searchDepth));
    } else {
        sendCommand("go movetime 1000");  // 預設 1 秒
    }
}
```

#### stop()
```cpp
void stop()
```
停止引擎思考（在遊戲結束或切換模式時使用）。

### 4. 難度和參數設定

#### setDifficulty()
```cpp
void setDifficulty(int level)  // 0-20
```
設定 AI 難度等級。

**Stockfish Skill Level**:
- **0-20**: 技能等級範圍
- **0**: 最弱（適合初學者）
- **10**: 中等（約 1500 ELO）
- **20**: 最強（約 3000+ ELO）

**實作**:
```cpp
void ChessEngine::setDifficulty(int level) {
    m_skillLevel = qBound(0, level, 20);
    if (m_isReady) {
        sendCommand(QString("setoption name Skill Level value %1")
                    .arg(m_skillLevel));
    }
}
```

#### setThinkingTime()
```cpp
void setThinkingTime(int milliseconds)
```
設定引擎思考時間（毫秒）。

**建議值**:
- 快速模式: 500-1000ms
- 普通模式: 2000-5000ms
- 深度思考: 10000ms+

#### setSearchDepth()
```cpp
void setSearchDepth(int depth)  // 1-30
```
設定搜尋深度。

**深度說明**:
- **1-5**: 非常快，較弱
- **6-10**: 快速，中等
- **11-15**: 較強
- **16-20**: 強
- **20+**: 非常強，但很慢

#### configureEngine()
```cpp
void configureEngine()
```
配置引擎的選項（在初始化後調用）。

**實作**:
```cpp
void ChessEngine::configureEngine() {
    // 設定技能等級
    sendCommand(QString("setoption name Skill Level value %1")
                .arg(m_skillLevel));
    
    // 限制 ELO 等級（可選）
    // sendCommand("setoption name UCI_LimitStrength value true");
    // sendCommand("setoption name UCI_Elo value 1500");
    
    // 設定執行緒數（可選）
    // sendCommand("setoption name Threads value 2");
    
    sendCommand("isready");
}
```

### 5. 工具函數

#### boardToFEN()
```cpp
static QString boardToFEN(const ChessBoard& board)
```
將 ChessBoard 物件轉換為 FEN 字串。

**FEN 格式組成**:
1. **棋盤位置**: 從第0列到第7列，空格用數字表示
2. **當前玩家**: `w` (白) 或 `b` (黑)
3. **王車易位權利**: `KQkq` 或 `-`
4. **吃過路兵目標**: `e3` 或 `-`
5. **半回合計數器**: 自上次吃子或兵移動的回合數
6. **回合數**: 當前回合數

**實作範例**:
```cpp
QString ChessEngine::boardToFEN(const ChessBoard& board) {
    QString fen;
    
    // 1. 棋盤位置
    for (int row = 0; row < 8; row++) {
        int emptyCount = 0;
        for (int col = 0; col < 8; col++) {
            ChessPiece piece = board.getPiece(row, col);
            if (piece.getType() == PieceType::None) {
                emptyCount++;
            } else {
                if (emptyCount > 0) {
                    fen += QString::number(emptyCount);
                    emptyCount = 0;
                }
                fen += pieceToFENChar(piece);
            }
        }
        if (emptyCount > 0) {
            fen += QString::number(emptyCount);
        }
        if (row < 7) fen += '/';
    }
    
    // 2. 當前玩家
    fen += (board.getCurrentPlayer() == PieceColor::White) ? " w " : " b ";
    
    // 3-5. 其他資訊...
    return fen;
}
```

#### moveToUCI()
```cpp
static QString moveToUCI(const QPoint& from, const QPoint& to, 
                         PieceType promotionType = PieceType::None)
```
將移動轉換為 UCI 格式。

**UCI 移動格式**: `e2e4`（從 e2 到 e4）

**實作**:
```cpp
QString ChessEngine::moveToUCI(const QPoint& from, const QPoint& to, 
                               PieceType promotionType) {
    QString move;
    // 轉換為 UCI 座標 (a1-h8)
    move += QChar('a' + from.x());
    move += QChar('8' - from.y());
    move += QChar('a' + to.x());
    move += QChar('8' - to.y());
    
    // 兵升變
    if (promotionType != PieceType::None) {
        switch (promotionType) {
            case PieceType::Queen:  move += 'q'; break;
            case PieceType::Rook:   move += 'r'; break;
            case PieceType::Bishop: move += 'b'; break;
            case PieceType::Knight: move += 'n'; break;
        }
    }
    
    return move;  // 如: "e2e4" 或 "e7e8q"
}
```

#### uciToMove()
```cpp
static void uciToMove(const QString& uci, QPoint& from, QPoint& to, 
                      PieceType& promotionType)
```
將 UCI 格式的移動轉換為座標和升變類型。

**實作**:
```cpp
void ChessEngine::uciToMove(const QString& uci, QPoint& from, QPoint& to,
                            PieceType& promotionType) {
    if (uci.length() < 4) return;
    
    // 解析起始位置: "e2"
    from.setX(uci[0].toLatin1() - 'a');
    from.setY('8' - uci[1].toLatin1());
    
    // 解析目標位置: "e4"
    to.setX(uci[2].toLatin1() - 'a');
    to.setY('8' - uci[3].toLatin1());
    
    // 解析升變（如果有）
    promotionType = PieceType::None;
    if (uci.length() >= 5) {
        switch (uci[4].toLatin1()) {
            case 'q': promotionType = PieceType::Queen; break;
            case 'r': promotionType = PieceType::Rook; break;
            case 'b': promotionType = PieceType::Bishop; break;
            case 'n': promotionType = PieceType::Knight; break;
        }
    }
}
```

## 信號 (Signals)

### engineReady()
```cpp
void engineReady()
```
引擎已就緒，可以開始遊戲。

### bestMoveFound()
```cpp
void bestMoveFound(const QString& move)
```
引擎找到最佳移動（UCI 格式）。

### engineError()
```cpp
void engineError(const QString& error)
```
引擎發生錯誤。

### thinkingStarted()
```cpp
void thinkingStarted()
```
引擎開始思考（用於顯示「思考中...」提示）。

### thinkingStopped()
```cpp
void thinkingStopped()
```
引擎完成思考。

## 使用範例

### 初始化引擎
```cpp
ChessEngine* engine = new ChessEngine(this);
connect(engine, &ChessEngine::engineReady, this, &MyClass::onEngineReady);
connect(engine, &ChessEngine::bestMoveFound, this, &MyClass::onBestMove);

// 啟動引擎
QString enginePath = "./engine/stockfish";  // Linux/Mac
// QString enginePath = "./engine/stockfish.exe";  // Windows
if (!engine->startEngine(enginePath)) {
    qDebug() << "引擎啟動失敗";
}
```

### 設定遊戲模式和難度
```cpp
engine->setGameMode(GameMode::HumanVsComputer);
engine->setDifficulty(10);  // 中等難度
engine->setThinkingTime(2000);  // 思考 2 秒
```

### 開始新遊戲
```cpp
engine->newGame();
```

### 更新棋局並請求移動
```cpp
// 方法 1: 使用移動列表
QStringList moves = {"e2e4", "e7e5", "g1f3"};
engine->setPositionFromMoves(moves);
engine->requestMove();

// 方法 2: 使用 FEN
QString fen = board.toFEN();
engine->setPosition(fen);
engine->requestMove();
```

### 處理引擎的移動
```cpp
void MyClass::onBestMove(const QString& move) {
    QPoint from, to;
    PieceType promotion;
    ChessEngine::uciToMove(move, from, to, promotion);
    
    // 在棋盤上執行移動
    board.movePiece(from, to);
    
    // 如果是兵升變
    if (promotion != PieceType::None) {
        board.promotePawn(to, promotion);
    }
}
```

## 整合流程

### 人機對弈的完整流程
```cpp
// 1. 玩家選擇執白或執黑
engine->setGameMode(isPlayerWhite ? GameMode::HumanVsComputer 
                                  : GameMode::ComputerVsHuman);

// 2. 如果玩家執黑，AI 先走
if (!isPlayerWhite) {
    engine->newGame();
    engine->requestMove();
}

// 3. 玩家走棋後，更新引擎並請求移動
void onPlayerMove() {
    // 更新棋盤狀態
    QStringList allMoves = getMoveList();
    engine->setPositionFromMoves(allMoves);
    
    // 請求 AI 回應
    engine->requestMove();
}

// 4. 接收 AI 的移動並執行
void onBestMove(const QString& move) {
    QPoint from, to;
    PieceType promotion;
    ChessEngine::uciToMove(move, from, to, promotion);
    board.movePiece(from, to);
    updateUI();
}
```

## 效能優化

### 非同步處理
- 使用 QProcess 的信號槽機制，不阻塞 UI 執行緒
- 在引擎思考時顯示載入動畫

### 快取和預測
- 可以在玩家回合時讓引擎預先思考（Ponder 模式）
- 快取常見開局的評估結果

### 資源管理
- 遊戲結束或切換模式時停止引擎思考
- 程式關閉時正確終止引擎進程

## Stockfish 引擎檔案

### 檔案位置
- **Linux/Mac**: `engine/stockfish`
- **Windows**: `engine/stockfish.exe`

### 取得 Stockfish
1. 從官網下載: https://stockfishchess.org/download/
2. 解壓縮到 `engine/` 資料夾
3. 確保有執行權限（Linux/Mac）:
```bash
chmod +x engine/stockfish
```

## 疑難排解

### 引擎無法啟動
- 檢查引擎檔案路徑是否正確
- 檢查是否有執行權限
- 檢查是否為正確的平台版本（32位元/64位元）

### 引擎回應緩慢
- 降低搜尋深度
- 減少思考時間
- 降低難度等級

### 記憶體使用過高
- 限制 Hash Table 大小:
```cpp
sendCommand("setoption name Hash value 64");  // 64 MB
```

## 相關類別
- `ChessBoard` - 提供棋局狀態給引擎
- `Qt_Chess` - 管理引擎和 UI 的互動

## 參考資源
- [UCI 協議文檔](http://wbec-ridderkerk.nl/html/UCIProtocol.html)
- [Stockfish 官方網站](https://stockfishchess.org/)
- [FEN 格式說明](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)
