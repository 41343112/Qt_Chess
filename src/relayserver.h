#ifndef RELAYSERVER_H
#define RELAYSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QTimer>

/**
 * @brief The RelayClient class handles communication with a relay/signaling server
 * 
 * This class enables cross-network online play by connecting to a relay server
 * that handles room management and peer discovery. Players only need a room code
 * to connect, eliminating the need for IP addresses and port forwarding.
 */
class RelayClient : public QObject
{
    Q_OBJECT

public:
    explicit RelayClient(QObject *parent = nullptr);
    ~RelayClient();

    // Connection management
    bool connectToRelay(const QString& serverUrl, quint16 port);
    void disconnectFromRelay();
    bool isConnected() const;
    
    // Room management
    void createRoom();
    void joinRoom(const QString& roomCode);
    void leaveRoom();
    
    // Game communication
    void sendGameMessage(const QJsonObject& message);
    
    // Getters
    QString getRoomCode() const { return m_roomCode; }
    QString getRelayServer() const { return m_relayServer; }
    bool isRoomHost() const { return m_isHost; }

signals:
    // Connection events
    void connected();
    void disconnected();
    void error(const QString& errorMessage);
    
    // Room events
    void roomCreated(const QString& roomCode);
    void roomJoined(const QString& roomCode);
    void opponentJoined(const QString& opponentId);
    void opponentLeft();
    
    // Game events
    void gameMessageReceived(const QJsonObject& message);
    
    // ICE/WebRTC events (for future P2P upgrade)
    void peerInfoReceived(const QJsonObject& peerInfo);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);
    void onHeartbeatTimeout();

private:
    void sendMessage(const QJsonObject& message);
    void processMessage(const QJsonObject& message);
    void startHeartbeat();
    void stopHeartbeat();
    
    QTcpSocket* m_socket;
    QString m_relayServer;
    quint16 m_relayPort;
    
    QString m_roomCode;
    QString m_clientId;
    QString m_opponentId;
    bool m_isHost;
    
    QByteArray m_receiveBuffer;
    QTimer* m_heartbeatTimer;
    
    bool m_connected;
};

#endif // RELAYSERVER_H
