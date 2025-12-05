#include "onlinedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include <QClipboard>
#include <QApplication>

OnlineDialog::OnlineDialog(QWidget *parent)
    : QDialog(parent)
    , m_mode(Mode::None)
    , m_useRelay(true)  // Default to relay mode
    , m_connectionInfoEdit(nullptr)
    , m_pasteButton(nullptr)
    , m_roomCodeEdit(nullptr)
{
    setupUI();
    setWindowTitle(tr("線上對戰 - 簡易連線"));
    resize(500, 480);
}

OnlineDialog::~OnlineDialog()
{
}

void OnlineDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 指示標籤 - 更友善的說明
    m_instructionLabel = new QLabel(tr("🎮 歡迎使用線上對戰！\n只需要房號就能跨網域連線！"), this);
    QFont instructionFont = m_instructionLabel->font();
    instructionFont.setPointSize(12);
    instructionFont.setBold(true);
    m_instructionLabel->setFont(instructionFont);
    m_instructionLabel->setStyleSheet("QLabel { color: #2196F3; padding: 10px; }");
    mainLayout->addWidget(m_instructionLabel);
    
    mainLayout->addSpacing(10);
    
    // 連線模式選擇（中繼伺服器 vs 直接連線）
    m_useRelayCheckbox = new QCheckBox(tr("🌐 使用中繼伺服器（推薦 - 跨網域連線，只需房號）"), this);
    m_useRelayCheckbox->setChecked(true);
    m_useRelayCheckbox->setStyleSheet("QCheckBox { font-size: 10pt; padding: 5px; }");
    connect(m_useRelayCheckbox, &QCheckBox::toggled, this, &OnlineDialog::onUseRelayToggled);
    mainLayout->addWidget(m_useRelayCheckbox);
    
    mainLayout->addSpacing(5);
    
    // 模式選擇
    QGroupBox* modeGroup = new QGroupBox(tr("選擇角色"), this);
    modeGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* modeLayout = new QVBoxLayout(modeGroup);
    
    m_createRoomRadio = new QRadioButton(tr("📱 我要創建房間（取得房號給朋友）"), this);
    m_createRoomRadio->setStyleSheet("QRadioButton { font-size: 11pt; padding: 5px; }");
    m_joinRoomRadio = new QRadioButton(tr("🔗 我有房號（加入朋友的房間）"), this);
    m_joinRoomRadio->setStyleSheet("QRadioButton { font-size: 11pt; padding: 5px; }");
    
    modeLayout->addWidget(m_createRoomRadio);
    modeLayout->addWidget(m_joinRoomRadio);
    
    mainLayout->addWidget(modeGroup);
    
    mainLayout->addSpacing(10);
    
    // 加入房間的輸入區域
    m_joinRoomWidget = new QWidget(this);
    QVBoxLayout* joinMainLayout = new QVBoxLayout(m_joinRoomWidget);
    
    // 中繼伺服器模式（簡單版 - 只需房號）
    QGroupBox* relayConnectGroup = new QGroupBox(tr("📋 輸入房號"), this);
    relayConnectGroup->setObjectName("relayGroup");
    relayConnectGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #4CAF50; }");
    QVBoxLayout* relayLayout = new QVBoxLayout(relayConnectGroup);
    
    QLabel* relayLabel = new QLabel(tr("只需要輸入4位數字的房號："), this);
    relayLabel->setWordWrap(true);
    relayLayout->addWidget(relayLabel);
    
    m_roomCodeEdit = new QLineEdit(this);
    m_roomCodeEdit->setPlaceholderText(tr("例如：1234"));
    m_roomCodeEdit->setMaxLength(4);
    QIntValidator* roomValidator = new QIntValidator(1000, 9999, this);
    m_roomCodeEdit->setValidator(roomValidator);
    QFont roomCodeFont = m_roomCodeEdit->font();
    roomCodeFont.setPointSize(14);
    m_roomCodeEdit->setFont(roomCodeFont);
    relayLayout->addWidget(m_roomCodeEdit);
    
    joinMainLayout->addWidget(relayConnectGroup);
    
    // 直接連線模式（需要IP和房號）
    QGroupBox* directConnectGroup = new QGroupBox(tr("📋 輸入連線資訊"), this);
    directConnectGroup->setObjectName("directGroup");
    directConnectGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #4CAF50; }");
    directConnectGroup->setVisible(false);  // Initially hidden
    QVBoxLayout* directMainLayout = new QVBoxLayout(directConnectGroup);
    
    // 簡易連線區域（使用連線碼）
    QGroupBox* easyConnectGroup = new QGroupBox(tr("方法一：使用連線碼（推薦）"), this);
    easyConnectGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QVBoxLayout* easyLayout = new QVBoxLayout(easyConnectGroup);
    
    QLabel* easyLabel = new QLabel(tr("請朋友將連線碼複製後，貼到下方："), this);
    easyLabel->setWordWrap(true);
    easyLayout->addWidget(easyLabel);
    
    m_connectionInfoEdit = new QTextEdit(this);
    m_connectionInfoEdit->setPlaceholderText(tr("在此貼上朋友給您的連線碼\n格式如：192.168.1.100:1234"));
    m_connectionInfoEdit->setMaximumHeight(60);
    easyLayout->addWidget(m_connectionInfoEdit);
    
    m_pasteButton = new QPushButton(tr("📋 從剪貼簿貼上"), this);
    m_pasteButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; padding: 8px; font-weight: bold; }");
    connect(m_pasteButton, &QPushButton::clicked, this, &OnlineDialog::onPasteConnectionInfo);
    easyLayout->addWidget(m_pasteButton);
    
    directMainLayout->addWidget(easyConnectGroup);
    
    // 手動輸入區域
    QGroupBox* manualGroup = new QGroupBox(tr("方法二：手動輸入"), this);
    manualGroup->setStyleSheet("QGroupBox { font-weight: bold; }");
    QFormLayout* manualLayout = new QFormLayout(manualGroup);
    
    m_hostAddressEdit = new QLineEdit(this);
    m_hostAddressEdit->setPlaceholderText(tr("例如: 192.168.1.100"));
    
    m_roomNumberEdit = new QLineEdit(this);
    m_roomNumberEdit->setPlaceholderText(tr("4位數字"));
    m_roomNumberEdit->setMaxLength(4);
    QIntValidator* validator = new QIntValidator(1000, 9999, this);
    m_roomNumberEdit->setValidator(validator);
    
    manualLayout->addRow(tr("IP地址:"), m_hostAddressEdit);
    manualLayout->addRow(tr("房號:"), m_roomNumberEdit);
    
    directMainLayout->addWidget(manualGroup);
    
    joinMainLayout->addWidget(directConnectGroup);
    
    m_joinRoomWidget->setEnabled(false);
    mainLayout->addWidget(m_joinRoomWidget);
    
    // 添加簡化的說明文字
    QLabel* tipLabel = new QLabel(
        tr("💡 <b>簡單說明：</b><br>"
           "• <b>中繼伺服器模式（推薦）</b>：跨網域連線，只需要4位數字房號<br>"
           "• <b>直接連線模式</b>：同網域連線，需要IP地址和房號<br>"
           "• 房主（創建者）執白棋先走，加入者執黑棋"), this);
    tipLabel->setWordWrap(true);
    tipLabel->setStyleSheet("QLabel { color: #666; font-size: 10pt; padding: 10px; background-color: #f5f5f5; border-radius: 5px; }");
    mainLayout->addWidget(tipLabel);
    
    mainLayout->addStretch();
    
    // 按鈕
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_okButton = new QPushButton(tr("✓ 開始"), this);
    m_okButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 10px 20px; font-weight: bold; }");
    m_cancelButton = new QPushButton(tr("✗ 取消"), this);
    m_cancelButton->setStyleSheet("QPushButton { padding: 10px 20px; }");
    
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 連接信號
    connect(m_createRoomRadio, &QRadioButton::toggled, this, &OnlineDialog::onCreateRoomClicked);
    connect(m_joinRoomRadio, &QRadioButton::toggled, this, &OnlineDialog::onJoinRoomClicked);
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    // 默認選中創建房間
    m_createRoomRadio->setChecked(true);
    
    // Initialize UI mode
    updateUIForMode();
}

void OnlineDialog::updateUIForMode()
{
    // Find the group boxes by object name
    QGroupBox* relayGroup = m_joinRoomWidget->findChild<QGroupBox*>("relayGroup");
    QGroupBox* directGroup = m_joinRoomWidget->findChild<QGroupBox*>("directGroup");
    
    if (m_useRelay) {
        if (relayGroup) relayGroup->setVisible(true);
        if (directGroup) directGroup->setVisible(false);
    } else {
        if (relayGroup) relayGroup->setVisible(false);
        if (directGroup) directGroup->setVisible(true);
    }
}

void OnlineDialog::onUseRelayToggled(bool checked)
{
    m_useRelay = checked;
    updateUIForMode();
    
    if (checked) {
        m_instructionLabel->setText(tr("🎮 歡迎使用線上對戰！\n只需要房號就能跨網域連線！"));
    } else {
        m_instructionLabel->setText(tr("🎮 歡迎使用線上對戰！\n需要在同一網域或設定端口轉發"));
    }
}

void OnlineDialog::onCreateRoomClicked()
{
    if (m_createRoomRadio->isChecked()) {
        m_mode = Mode::CreateRoom;
        m_joinRoomWidget->setEnabled(false);
        if (m_useRelay) {
            m_instructionLabel->setText(tr("🎮 您選擇了「創建房間」\n點擊「開始」後，系統會給您一個4位數字房號"));
        } else {
            m_instructionLabel->setText(tr("🎮 您選擇了「創建房間」\n點擊「開始」後，系統會給您一個連線碼"));
        }
    }
}

void OnlineDialog::onJoinRoomClicked()
{
    if (m_joinRoomRadio->isChecked()) {
        m_mode = Mode::JoinRoom;
        m_joinRoomWidget->setEnabled(true);
        if (m_useRelay) {
            m_instructionLabel->setText(tr("🎮 您選擇了「加入房間」\n請輸入朋友給您的4位數字房號"));
        } else {
            m_instructionLabel->setText(tr("🎮 您選擇了「加入房間」\n請貼上朋友給您的連線碼，或手動輸入"));
        }
    }
}

void OnlineDialog::onPasteConnectionInfo()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString clipText = clipboard->text().trimmed();
    
    if (!clipText.isEmpty()) {
        m_connectionInfoEdit->setPlainText(clipText);
        parseConnectionInfo(clipText);
    } else {
        QMessageBox::information(this, tr("提示"), tr("剪貼簿是空的，請先複製連線碼"));
    }
}

void OnlineDialog::parseConnectionInfo(const QString& info)
{
    // 支援多種格式：
    // 1. IP:房號 格式 (192.168.1.100:1234)
    // 2. IP 房號 格式 (192.168.1.100 1234)
    // 3. 連線碼：IP:房號 格式
    
    QString text = info.trimmed();
    
    // 移除可能的前綴文字
    if (text.contains("連線碼") || text.contains("Connection")) {
        int colonPos = text.lastIndexOf(':');
        if (colonPos > 0) {
            text = text.mid(colonPos + 1).trimmed();
        }
    }
    
    // 嘗試解析 IP:房號 格式
    QStringList parts;
    if (text.contains(':')) {
        parts = text.split(':');
    } else if (text.contains(' ')) {
        parts = text.split(' ', Qt::SkipEmptyParts);
    }
    
    if (parts.size() >= 2) {
        QString ip = parts[0].trimmed();
        QString room = parts[parts.size() - 1].trimmed();
        
        // 驗證房號是4位數字
        if (room.length() == 4) {
            bool ok;
            int roomNum = room.toInt(&ok);
            if (ok && roomNum >= 1000 && roomNum <= 9999) {
                m_hostAddressEdit->setText(ip);
                m_roomNumberEdit->setText(room);
                QMessageBox::information(this, tr("成功"), 
                    tr("已自動填入：\nIP: %1\n房號: %2").arg(ip, room));
                return;
            }
        }
    }
    
    QMessageBox::warning(this, tr("格式錯誤"), 
        tr("無法識別連線碼格式\n\n正確格式範例：\n192.168.1.100:1234\n或\n192.168.1.100 1234"));
}

QString OnlineDialog::getHostAddress() const
{
    return m_hostAddressEdit->text().trimmed();
}

quint16 OnlineDialog::getPort() const
{
    // 從房號計算端口（10000 + 房號）
    QString roomNumber = m_roomNumberEdit->text().trimmed();
    if (roomNumber.length() == 4) {
        bool ok;
        int roomNum = roomNumber.toInt(&ok);
        if (ok && roomNum >= 1000 && roomNum <= 9999) {
            return 10000 + roomNum;
        }
    }
    return 0;
}

QString OnlineDialog::getRoomCode() const
{
    if (m_useRelay) {
        // Relay mode: return room code directly
        return m_roomCodeEdit ? m_roomCodeEdit->text().trimmed() : QString();
    } else {
        // Direct mode: return room number
        return m_roomNumberEdit ? m_roomNumberEdit->text().trimmed() : QString();
    }
}
