#include "relayserver.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QDebug>
#include <QHostAddress>

// Heartbeat interval in milliseconds
static const int HEARTBEAT_INTERVAL = 30000;

RelayClient::RelayClient(QObject *parent)
    : QObject(parent)
    , m_socket(nullptr)
    , m_relayPort(0)
    , m_isHost(false)
    , m_heartbeatTimer(nullptr)
    , m_connected(false)
{
    m_heartbeatTimer = new QTimer(this);
    connect(m_heartbeatTimer, &QTimer::timeout, this, &RelayClient::onHeartbeatTimeout);
}

RelayClient::~RelayClient()
{
    disconnectFromRelay();
}

bool RelayClient::connectToRelay(const QString& serverUrl, quint16 port)
{
    if (m_socket && m_socket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << "[RelayClient] Already connected to relay server";
        return true;
    }
    
    disconnectFromRelay();
    
    m_relayServer = serverUrl;
    m_relayPort = port;
    
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &RelayClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &RelayClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &RelayClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &RelayClient::onSocketError);
    
    qDebug() << "[RelayClient] Connecting to relay server:" << serverUrl << ":" << port;
    m_socket->connectToHost(serverUrl, port);
    
    return true;
}

void RelayClient::disconnectFromRelay()
{
    stopHeartbeat();
    
    if (m_socket) {
        // Send leave room message if in a room
        if (!m_roomCode.isEmpty()) {
            QJsonObject message;
            message["type"] = "leave_room";
            message["room_code"] = m_roomCode;
            sendMessage(message);
        }
        
        m_socket->disconnectFromHost();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
    
    m_roomCode.clear();
    m_clientId.clear();
    m_opponentId.clear();
    m_isHost = false;
    m_connected = false;
}

bool RelayClient::isConnected() const
{
    return m_connected && m_socket && m_socket->state() == QAbstractSocket::ConnectedState;
}

void RelayClient::createRoom()
{
    if (!isConnected()) {
        emit error(tr("未連接到中繼伺服器"));
        return;
    }
    
    QJsonObject message;
    message["type"] = "create_room";
    sendMessage(message);
    
    m_isHost = true;
}

void RelayClient::joinRoom(const QString& roomCode)
{
    if (!isConnected()) {
        emit error(tr("未連接到中繼伺服器"));
        return;
    }
    
    QJsonObject message;
    message["type"] = "join_room";
    message["room_code"] = roomCode;
    sendMessage(message);
    
    m_isHost = false;
}

void RelayClient::leaveRoom()
{
    if (!isConnected() || m_roomCode.isEmpty()) {
        return;
    }
    
    QJsonObject message;
    message["type"] = "leave_room";
    message["room_code"] = m_roomCode;
    sendMessage(message);
    
    m_roomCode.clear();
    m_opponentId.clear();
}

void RelayClient::sendGameMessage(const QJsonObject& gameMessage)
{
    if (!isConnected() || m_roomCode.isEmpty()) {
        qDebug() << "[RelayClient] Cannot send game message: not connected or not in room";
        return;
    }
    
    QJsonObject message;
    message["type"] = "game_message";
    message["room_code"] = m_roomCode;
    message["data"] = gameMessage;
    sendMessage(message);
}

void RelayClient::onConnected()
{
    qDebug() << "[RelayClient] Connected to relay server";
    m_connected = true;
    
    // Start heartbeat
    startHeartbeat();
    
    // Send hello message
    QJsonObject message;
    message["type"] = "hello";
    message["version"] = "1.0";
    sendMessage(message);
    
    emit connected();
}

void RelayClient::onDisconnected()
{
    qDebug() << "[RelayClient] Disconnected from relay server";
    stopHeartbeat();
    
    m_connected = false;
    m_roomCode.clear();
    m_clientId.clear();
    m_opponentId.clear();
    
    emit disconnected();
}

void RelayClient::onReadyRead()
{
    if (!m_socket) {
        return;
    }
    
    m_receiveBuffer.append(m_socket->readAll());
    
    // Process all complete messages
    while (true) {
        int messageEnd = m_receiveBuffer.indexOf('\n');
        if (messageEnd == -1) {
            break;  // No complete message
        }
        
        QByteArray messageData = m_receiveBuffer.left(messageEnd);
        m_receiveBuffer.remove(0, messageEnd + 1);
        
        QJsonDocument doc = QJsonDocument::fromJson(messageData);
        if (!doc.isNull() && doc.isObject()) {
            processMessage(doc.object());
        }
    }
}

void RelayClient::onSocketError(QAbstractSocket::SocketError socketError)
{
    if (m_socket) {
        QString errorMsg = m_socket->errorString();
        qDebug() << "[RelayClient] Socket error:" << errorMsg;
        emit error(errorMsg);
    }
}

void RelayClient::onHeartbeatTimeout()
{
    if (isConnected()) {
        QJsonObject message;
        message["type"] = "ping";
        sendMessage(message);
    }
}

void RelayClient::sendMessage(const QJsonObject& message)
{
    if (!m_socket || m_socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "[RelayClient] Cannot send message: socket not connected";
        return;
    }
    
    QJsonDocument doc(message);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    
    qDebug() << "[RelayClient] Sending message:" << message["type"].toString();
    
    m_socket->write(data);
    m_socket->flush();
}

void RelayClient::processMessage(const QJsonObject& message)
{
    QString type = message["type"].toString();
    
    qDebug() << "[RelayClient] Received message:" << type;
    
    if (type == "welcome") {
        // Server welcome message with client ID
        m_clientId = message["client_id"].toString();
        qDebug() << "[RelayClient] Received client ID:" << m_clientId;
    }
    else if (type == "room_created") {
        // Room created successfully
        m_roomCode = message["room_code"].toString();
        qDebug() << "[RelayClient] Room created with code:" << m_roomCode;
        emit roomCreated(m_roomCode);
    }
    else if (type == "room_joined") {
        // Joined room successfully
        m_roomCode = message["room_code"].toString();
        qDebug() << "[RelayClient] Joined room:" << m_roomCode;
        emit roomJoined(m_roomCode);
    }
    else if (type == "opponent_joined") {
        // Opponent joined the room
        m_opponentId = message["opponent_id"].toString();
        qDebug() << "[RelayClient] Opponent joined:" << m_opponentId;
        emit opponentJoined(m_opponentId);
    }
    else if (type == "opponent_left") {
        // Opponent left the room
        qDebug() << "[RelayClient] Opponent left";
        m_opponentId.clear();
        emit opponentLeft();
    }
    else if (type == "game_message") {
        // Game message from opponent
        QJsonObject data = message["data"].toObject();
        qDebug() << "[RelayClient] Received game message";
        emit gameMessageReceived(data);
    }
    else if (type == "error") {
        // Error message from server
        QString errorMsg = message["message"].toString();
        qDebug() << "[RelayClient] Server error:" << errorMsg;
        emit error(errorMsg);
    }
    else if (type == "pong") {
        // Heartbeat response
        qDebug() << "[RelayClient] Heartbeat acknowledged";
    }
    else if (type == "peer_info") {
        // ICE/peer information for future P2P
        emit peerInfoReceived(message);
    }
}

void RelayClient::startHeartbeat()
{
    if (m_heartbeatTimer) {
        m_heartbeatTimer->start(HEARTBEAT_INTERVAL);
    }
}

void RelayClient::stopHeartbeat()
{
    if (m_heartbeatTimer) {
        m_heartbeatTimer->stop();
    }
}
