#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QPoint>
#include "chesspiece.h"
#include "relayserver.h"

enum class NetworkRole {
    None,
    Server,    // 房主
    Client     // 加入者
};

enum class ConnectionMode {
    Direct,    // 直接連接（需要IP地址）
    Relay      // 中繼伺服器（只需房號）
};

enum class ConnectionStatus {
    Disconnected,
    Connecting,
    Connected,
    Error
};

enum class MessageType {
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

class NetworkManager : public QObject
{
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    // 房間管理
    bool createRoom(quint16 port = 0);  // 創建房間，返回房號（直接連接模式）
    bool createRoomViaRelay();  // 通過中繼伺服器創建房間（只需房號）
    bool joinRoom(const QString& hostAddress, quint16 port);  // 直接連接模式
    bool joinRoomViaRelay(const QString& roomCode);  // 通過中繼伺服器加入房間
    void closeConnection();
    
    // 中繼伺服器設定
    void setRelayServer(const QString& serverUrl, quint16 port);
    QString getRelayServer() const { return m_relayServerUrl; }
    bool isUsingRelay() const { return m_connectionMode == ConnectionMode::Relay; }
    
    QString getRoomNumber() const { return m_roomNumber; }
    quint16 getPort() const { return m_port; }
    NetworkRole getRole() const { return m_role; }
    ConnectionStatus getStatus() const { return m_status; }
    
    // 遊戲同步
    void sendMove(const QPoint& from, const QPoint& to, PieceType promotionType = PieceType::None);
    void sendGameStart(PieceColor playerColor);
    void sendStartGame(int whiteTimeMs, int blackTimeMs, int incrementMs, PieceColor hostColor);  // 房主通知開始遊戲（包含時間設定和顏色選擇）
    void sendTimeSettings(int whiteTimeMs, int blackTimeMs, int incrementMs);  // 房主發送時間設定更新
    void sendSurrender();  // 發送投降訊息
    void sendGameOver(const QString& result);
    void sendChat(const QString& message);
    
    // 玩家顏色管理
    PieceColor getOpponentColor() const { return m_opponentColor; }
    PieceColor getPlayerColor() const { return m_playerColor; }
    void setPlayerColors(PieceColor playerColor);  // 設定玩家顏色（同時設定對手顏色為相反顏色）


signals:
    void connected();
    void disconnected();
    void connectionError(const QString& error);
    void roomCreated(const QString& roomNumber, quint16 port);  // port will be 0 for relay mode
    void opponentJoined();
    void opponentMove(const QPoint& from, const QPoint& to, PieceType promotionType);
    void gameStartReceived(PieceColor playerColor);
    void startGameReceived(int whiteTimeMs, int blackTimeMs, int incrementMs, PieceColor hostColor);  // 收到開始遊戲通知（包含時間設定和房主顏色）
    void timeSettingsReceived(int whiteTimeMs, int blackTimeMs, int incrementMs);  // 收到時間設定更新
    void surrenderReceived();  // 收到投降訊息
    void gameOverReceived(const QString& result);
    void chatReceived(const QString& message);
    void opponentDisconnected();

private slots:
    void onNewConnection();
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError);
    void onServerError(QAbstractSocket::SocketError socketError);
    
    // Relay server slots
    void onRelayConnected();
    void onRelayDisconnected();
    void onRelayError(const QString& error);
    void onRelayRoomCreated(const QString& roomCode);
    void onRelayRoomJoined(const QString& roomCode);
    void onRelayOpponentJoined(const QString& opponentId);
    void onRelayOpponentLeft();
    void onRelayGameMessageReceived(const QJsonObject& message);

private:
    QTcpServer* m_server;
    QTcpSocket* m_socket;
    QTcpSocket* m_clientSocket;  // 用於服務器端連接的客戶端套接字
    
    // Relay server
    RelayClient* m_relayClient;
    QString m_relayServerUrl;
    quint16 m_relayServerPort;
    ConnectionMode m_connectionMode;
    
    NetworkRole m_role;
    ConnectionStatus m_status;
    QString m_roomNumber;
    quint16 m_port;
    
    PieceColor m_playerColor;
    PieceColor m_opponentColor;
    
    QByteArray m_receiveBuffer;
    
    void sendMessage(const QJsonObject& message);
    void processMessage(const QJsonObject& message);
    QString generateRoomNumber() const;
    MessageType stringToMessageType(const QString& type) const;
    QString messageTypeToString(MessageType type) const;
    QTcpSocket* getActiveSocket() const;
};

#endif // NETWORKMANAGER_H
