# NetworkManager 線上對戰功能

## 概述
`NetworkManager` 類別負責線上多人對戰功能，透過 WebSocket 連接到中央伺服器，實現即時的棋步同步和遊戲狀態管理。使用 Qt WebSockets 模組與伺服器進行 JSON 格式的訊息交換。

## 檔案位置
- **標頭檔**: `src/networkmanager.h`
- **實作檔**: `src/networkmanager.cpp`

## 主要資料結構

### NetworkRole - 網路角色
```cpp
enum class NetworkRole {
    None,      // 未連線
    Host,      // 房主（創建房間者）
    Guest      // 訪客（加入房間者）
};
```

### ConnectionStatus - 連線狀態
```cpp
enum class ConnectionStatus {
    Disconnected,  // 已斷線
    Connecting,    // 連線中
    Connected,     // 已連線
    Error          // 錯誤
};
```

### MessageType - 訊息類型
```cpp
enum class MessageType {
    CreateRoom,         // 創建房間
    RoomCreated,        // 房間已創建
    JoinRoom,           // 加入房間
    JoinAccepted,       // 加入被接受
    JoinRejected,       // 加入被拒絕
    GameStart,          // 遊戲開始
    StartGame,          // 房主通知開始遊戲
    TimeSettings,       // 房主更新時間設定
    Move,               // 棋步
    GameOver,           // 遊戲結束
    Surrender,          // 投降
    Chat,               // 聊天訊息
    PlayerDisconnected, // 玩家斷線
    Ping,               // 心跳包
    Pong                // 心跳回應
};
```

## 類別成員

### 私有成員變數
```cpp
QWebSocket* m_webSocket;              // WebSocket 連接物件
NetworkRole m_role;                   // 網路角色（房主/訪客）
ConnectionStatus m_status;            // 連線狀態
QString m_serverUrl;                  // 伺服器 URL
QString m_roomNumber;                 // 房間號碼
PieceColor m_playerColor;             // 玩家顏色
PieceColor m_opponentColor;           // 對手顏色
```

## 主要功能

### 1. 連線管理

#### createRoom()
```cpp
bool createRoom()
```
創建新房間並連接到伺服器。

**執行流程**:
1. 檢查當前狀態（必須為已斷線）
2. 建立 WebSocket 物件
3. 連接信號槽（connected, disconnected, textMessageReceived, error）
4. 設定角色為 Host（房主）
5. 設定玩家顏色為白色（房主執白棋）
6. 連線到伺服器
7. 等待伺服器分配房號

**返回值**: `true` 表示開始連線流程

**實作**:
```cpp
bool NetworkManager::createRoom() {
    if (m_status != ConnectionStatus::Disconnected) {
        return false;
    }
    
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, 
            this, &NetworkManager::onConnected);
    connect(m_webSocket, &QWebSocket::disconnected, 
            this, &NetworkManager::onDisconnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, 
            this, &NetworkManager::onTextMessageReceived);
    connect(m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), 
            this, &NetworkManager::onError);
    
    m_role = NetworkRole::Host;
    m_status = ConnectionStatus::Connecting;
    m_playerColor = PieceColor::White;  // 房主執白
    m_opponentColor = PieceColor::Black;
    
    m_webSocket->open(QUrl(m_serverUrl));
    return true;
}
```

#### joinRoom()
```cpp
bool joinRoom(const QString& roomNumber)
```
使用房號加入現有房間。

**參數**:
- `roomNumber` - 房間號碼（由房主提供）

**執行流程**:
1. 驗證房號格式
2. 建立 WebSocket 連接
3. 設定角色為 Guest（訪客）
4. 設定玩家顏色為黑色（加入者執黑棋）
5. 儲存房號
6. 連線到伺服器

**實作**:
```cpp
bool NetworkManager::joinRoom(const QString& roomNumber) {
    if (m_status != ConnectionStatus::Disconnected) {
        return false;
    }
    
    m_webSocket = new QWebSocket();
    // ... 連接信號槽 ...
    
    m_role = NetworkRole::Guest;
    m_status = ConnectionStatus::Connecting;
    m_roomNumber = roomNumber;
    m_playerColor = PieceColor::Black;  // 加入者執黑
    m_opponentColor = PieceColor::White;
    
    m_webSocket->open(QUrl(m_serverUrl));
    return true;
}
```

#### leaveRoom()
```cpp
void leaveRoom()
```
離開當前房間並斷開連線。

**執行步驟**:
1. 發送離開房間訊息給伺服器
2. 確保訊息發送完成（flush）
3. 關閉 WebSocket 連接
4. 清理資源

#### closeConnection()
```cpp
void closeConnection()
```
關閉 WebSocket 連接並清理資源。

**實作**:
```cpp
void NetworkManager::closeConnection() {
    if (m_webSocket) {
        m_webSocket->close();
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }
    
    m_status = ConnectionStatus::Disconnected;
    m_role = NetworkRole::None;
    m_roomNumber.clear();
}
```

### 2. 訊息傳送

#### sendMessage()
```cpp
void sendMessage(const QJsonObject& message)
```
發送 JSON 格式訊息到伺服器。

**實作**:
```cpp
void NetworkManager::sendMessage(const QJsonObject& message) {
    if (m_webSocket && m_webSocket->state() == QAbstractSocket::ConnectedState) {
        QJsonDocument doc(message);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        m_webSocket->sendTextMessage(jsonString);
    }
}
```

#### sendMove()
```cpp
void sendMove(const QPoint& from, const QPoint& to, 
              PieceType promotionType = PieceType::None)
```
發送棋步到對手。

**訊息格式**:
```json
{
    "action": "move",
    "room": "ABC123",
    "from": {"x": 4, "y": 6},
    "to": {"x": 4, "y": 4},
    "promotion": "queen"  // 可選，僅用於兵升變
}
```

**實作**:
```cpp
void NetworkManager::sendMove(const QPoint& from, const QPoint& to, 
                              PieceType promotionType) {
    QJsonObject message;
    message["action"] = "move";
    message["room"] = m_roomNumber;
    
    QJsonObject fromObj;
    fromObj["x"] = from.x();
    fromObj["y"] = from.y();
    message["from"] = fromObj;
    
    QJsonObject toObj;
    toObj["x"] = to.x();
    toObj["y"] = to.y();
    message["to"] = toObj;
    
    if (promotionType != PieceType::None) {
        message["promotion"] = pieceTypeToString(promotionType);
    }
    
    sendMessage(message);
}
```

#### sendSurrender()
```cpp
void sendSurrender()
```
發送認輸訊息。

**訊息格式**:
```json
{
    "action": "surrender",
    "room": "ABC123",
    "color": "white"
}
```

#### sendTimeSettings()
```cpp
void sendTimeSettings(int totalTimeMinutes, int incrementSeconds)
```
房主發送時間控制設定給對手。

**訊息格式**:
```json
{
    "action": "timeSettings",
    "room": "ABC123",
    "totalTime": 10,
    "increment": 5
}
```

### 3. 訊息接收處理

#### onTextMessageReceived()
```cpp
void onTextMessageReceived(const QString& message)
```
處理從伺服器接收的 JSON 訊息。

**處理流程**:
1. 解析 JSON 訊息
2. 根據 action 類型分派處理
3. 發出對應的信號

**實作範例**:
```cpp
void NetworkManager::onTextMessageReceived(const QString& message) {
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject()) return;
    
    QJsonObject obj = doc.object();
    QString action = obj["action"].toString();
    
    if (action == "roomCreated") {
        handleRoomCreated(obj);
    }
    else if (action == "joinAccepted") {
        handleJoinAccepted(obj);
    }
    else if (action == "joinRejected") {
        handleJoinRejected(obj);
    }
    else if (action == "move") {
        handleMove(obj);
    }
    else if (action == "gameStart") {
        handleGameStart(obj);
    }
    else if (action == "timeSettings") {
        handleTimeSettings(obj);
    }
    else if (action == "playerDisconnected") {
        handlePlayerDisconnected(obj);
    }
    else if (action == "surrender") {
        handleSurrender(obj);
    }
}
```

#### handleRoomCreated()
```cpp
void handleRoomCreated(const QJsonObject& obj)
```
處理房間創建成功的回應。

**伺服器回應格式**:
```json
{
    "action": "roomCreated",
    "room": "ABC123",
    "success": true
}
```

**實作**:
```cpp
void NetworkManager::handleRoomCreated(const QJsonObject& obj) {
    if (obj["success"].toBool()) {
        m_roomNumber = obj["room"].toString();
        m_status = ConnectionStatus::Connected;
        emit roomCreated(m_roomNumber);
        emit statusChanged(m_status);
    }
}
```

#### handleJoinAccepted()
```cpp
void handleJoinAccepted(const QJsonObject& obj)
```
處理加入房間被接受。

**伺服器回應格式**:
```json
{
    "action": "joinAccepted",
    "room": "ABC123"
}
```

#### handleMove()
```cpp
void handleMove(const QJsonObject& obj)
```
處理接收到的對手移動。

**訊息格式**:
```json
{
    "action": "move",
    "from": {"x": 4, "y": 6},
    "to": {"x": 4, "y": 4},
    "promotion": "queen"  // 可選
}
```

**實作**:
```cpp
void NetworkManager::handleMove(const QJsonObject& obj) {
    QJsonObject fromObj = obj["from"].toObject();
    QJsonObject toObj = obj["to"].toObject();
    
    QPoint from(fromObj["x"].toInt(), fromObj["y"].toInt());
    QPoint to(toObj["x"].toInt(), toObj["y"].toInt());
    
    PieceType promotion = PieceType::None;
    if (obj.contains("promotion")) {
        promotion = stringToPieceType(obj["promotion"].toString());
    }
    
    emit moveReceived(from, to, promotion);
}
```

#### handlePlayerDisconnected()
```cpp
void handlePlayerDisconnected(const QJsonObject& obj)
```
處理對手斷線通知。

### 4. 連線事件處理

#### onConnected()
```cpp
void onConnected()
```
WebSocket 連線建立成功的回調。

**執行動作**:
- 如果是房主：發送創建房間請求
- 如果是訪客：發送加入房間請求

**實作**:
```cpp
void NetworkManager::onConnected() {
    qDebug() << "[NetworkManager] Connected to server";
    
    if (m_role == NetworkRole::Host) {
        // 發送創建房間請求
        QJsonObject message;
        message["action"] = "createRoom";
        sendMessage(message);
    }
    else if (m_role == NetworkRole::Guest) {
        // 發送加入房間請求
        QJsonObject message;
        message["action"] = "joinRoom";
        message["room"] = m_roomNumber;
        sendMessage(message);
    }
}
```

#### onDisconnected()
```cpp
void onDisconnected()
```
連線斷開的回調。

**執行動作**:
1. 更新連線狀態
2. 發出斷線信號
3. 清理資源

#### onError()
```cpp
void onError(QAbstractSocket::SocketError error)
```
連線錯誤的回調。

**常見錯誤**:
- `RemoteHostClosedError` - 伺服器關閉連接
- `HostNotFoundError` - 找不到伺服器
- `ConnectionRefusedError` - 連接被拒絕
- `NetworkError` - 網路錯誤

## 信號 (Signals)

### 連線相關
```cpp
void statusChanged(ConnectionStatus status);  // 連線狀態改變
void roomCreated(const QString& roomNumber);  // 房間創建成功
void joinedRoom();                            // 成功加入房間
void joinRejected(const QString& reason);     // 加入被拒絕
void disconnected();                          // 斷線
void errorOccurred(const QString& error);     // 錯誤發生
```

### 遊戲相關
```cpp
void gameStarted();                                      // 遊戲開始
void moveReceived(QPoint from, QPoint to, 
                  PieceType promotion);                  // 接收到對手移動
void opponentSurrendered();                              // 對手認輸
void opponentDisconnected();                             // 對手斷線
void timeSettingsReceived(int totalTime, int increment); // 接收時間設定
```

## 使用範例

### 創建房間
```cpp
NetworkManager* network = new NetworkManager(this);

// 連接信號
connect(network, &NetworkManager::roomCreated, this, [](const QString& roomNumber) {
    qDebug() << "房間創建成功！房號：" << roomNumber;
    // 顯示房號給玩家，讓他們分享給朋友
});

connect(network, &NetworkManager::gameStarted, this, []() {
    qDebug() << "對手已加入，遊戲開始！";
});

connect(network, &NetworkManager::moveReceived, this, 
        [this](QPoint from, QPoint to, PieceType promotion) {
    // 在棋盤上執行對手的移動
    board->movePiece(from, to);
    if (promotion != PieceType::None) {
        board->promotePawn(to, promotion);
    }
});

// 創建房間
if (network->createRoom()) {
    qDebug() << "正在連接伺服器...";
}
```

### 加入房間
```cpp
NetworkManager* network = new NetworkManager(this);

// 連接信號
connect(network, &NetworkManager::joinedRoom, this, []() {
    qDebug() << "成功加入房間！";
});

connect(network, &NetworkManager::joinRejected, this, [](const QString& reason) {
    qDebug() << "加入失敗：" << reason;
});

// 加入房間（使用房主提供的房號）
QString roomNumber = "ABC123";  // 從剪貼簿或輸入獲取
if (network->joinRoom(roomNumber)) {
    qDebug() << "正在加入房間...";
}
```

### 發送移動
```cpp
// 玩家移動棋子後
QPoint from(4, 6);  // e2
QPoint to(4, 4);    // e4

// 在本地執行移動
board->movePiece(from, to);

// 發送給對手
network->sendMove(from, to);
```

### 處理兵升變
```cpp
// 檢查是否需要兵升變
if (board->needsPromotion(to)) {
    // 彈出對話框讓玩家選擇
    PieceType promotion = showPromotionDialog();
    
    // 在本地執行升變
    board->promotePawn(to, promotion);
    
    // 發送給對手（包含升變資訊）
    network->sendMove(from, to, promotion);
}
```

### 認輸
```cpp
void onSurrenderClicked() {
    // 發送認輸訊息
    network->sendSurrender();
    
    // 更新本地遊戲狀態
    board->resign(myColor);
}
```

### 處理斷線
```cpp
connect(network, &NetworkManager::opponentDisconnected, this, []() {
    QMessageBox::information(this, "對手斷線", 
                            "對手已斷線。您可以選擇等待或結束遊戲。");
});

connect(network, &NetworkManager::errorOccurred, this, [](const QString& error) {
    QMessageBox::critical(this, "連線錯誤", error);
});
```

## 伺服器通訊協議

### WebSocket URL
```
wss://chess-server-mjg6.onrender.com
```

### 訊息格式
所有訊息使用 JSON 格式，必須包含 `action` 欄位。

#### 客戶端 -> 伺服器

**創建房間**:
```json
{"action": "createRoom"}
```

**加入房間**:
```json
{"action": "joinRoom", "room": "ABC123"}
```

**移動棋子**:
```json
{
    "action": "move",
    "room": "ABC123",
    "from": {"x": 4, "y": 6},
    "to": {"x": 4, "y": 4},
    "promotion": "queen"  // 可選
}
```

**認輸**:
```json
{
    "action": "surrender",
    "room": "ABC123",
    "color": "white"
}
```

**離開房間**:
```json
{
    "action": "leaveRoom",
    "room": "ABC123"
}
```

#### 伺服器 -> 客戶端

**房間創建成功**:
```json
{
    "action": "roomCreated",
    "room": "ABC123",
    "success": true
}
```

**加入被接受**:
```json
{
    "action": "joinAccepted",
    "room": "ABC123"
}
```

**加入被拒絕**:
```json
{
    "action": "joinRejected",
    "reason": "房間不存在"
}
```

**遊戲開始**:
```json
{
    "action": "gameStart",
    "room": "ABC123"
}
```

**對手移動**:
```json
{
    "action": "move",
    "from": {"x": 4, "y": 1},
    "to": {"x": 4, "y": 3}
}
```

**對手斷線**:
```json
{
    "action": "playerDisconnected"
}
```

## 安全性考量

### 資料驗證
- 驗證所有接收的 JSON 資料
- 檢查移動的合法性（防止作弊）
- 限制訊息發送頻率（防止洪水攻擊）

### 連線管理
- 實作重連機制
- 處理超時情況
- 適當的錯誤處理和使用者反饋

### 房號安全
- 使用難以猜測的房號格式
- 考慮加入房間密碼（可選功能）
- 限制房間存活時間

## 效能優化

### 訊息壓縮
- 使用 Compact JSON 格式減少傳輸大小
- 考慮使用二進位格式（如 MessagePack）

### 延遲優化
- 使用 WebSocket 而非 HTTP（減少建立連線開銷）
- 最小化不必要的訊息交換
- 實作樂觀 UI 更新（先更新 UI，再等待確認）

### 連線穩定性
- 實作心跳機制（Ping/Pong）
- 自動重連失敗的連線
- 快取未送達的訊息，重連後重發

## 相關類別
- `OnlineDialog` - 提供線上對戰的 UI 介面
- `Qt_Chess` - 整合網路功能到主遊戲邏輯
- `ChessBoard` - 驗證接收到的移動是否合法

## 依賴套件
- **Qt WebSockets**: `QWebSocket`, `QWebSocketServer`
- **Qt Network**: `QAbstractSocket`
- **Qt Core**: `QJsonDocument`, `QJsonObject`, `QJsonArray`

## 疑難排解

### 無法連接伺服器
- 檢查網路連線
- 確認伺服器 URL 正確
- 檢查防火牆設定

### 訊息無法送達
- 檢查 WebSocket 連線狀態
- 驗證 JSON 格式正確
- 查看伺服器日誌

### 對手看不到移動
- 確認雙方都在同一個房間
- 檢查移動訊息格式
- 驗證座標轉換正確

## 參考資源
- [Qt WebSockets 文檔](https://doc.qt.io/qt-5/qtwebsockets-index.html)
- [WebSocket 協議 RFC 6455](https://tools.ietf.org/html/rfc6455)
- [JSON 格式規範](https://www.json.org/)
