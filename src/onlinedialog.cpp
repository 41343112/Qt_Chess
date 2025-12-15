#include "onlinedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

OnlineDialog::OnlineDialog(QWidget *parent)
    : QDialog(parent)
    , m_standardModeCheckbox(nullptr)
    , m_rapidModeCheckbox(nullptr)
    , m_blitzModeCheckbox(nullptr)
    , m_handicapModeCheckbox(nullptr)
    , m_customRulesCheckbox(nullptr)
{
    setupUI();
    setWindowTitle(tr("選擇遊戲模式"));
    resize(400, 350);
}

OnlineDialog::~OnlineDialog()
{
}

void OnlineDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // 遊戲模式選擇
    QGroupBox* gameModeGroup = new QGroupBox(tr("🎯 選擇遊戲模式（可複選）"), this);
    gameModeGroup->setStyleSheet("QGroupBox { font-weight: bold; color: #2196F3; }");
    QVBoxLayout* gameModeLayout = new QVBoxLayout(gameModeGroup);
    
    m_standardModeCheckbox = new QCheckBox(tr("⚔️ 標準模式 - 經典國際象棋規則"), this);
    m_standardModeCheckbox->setStyleSheet("QCheckBox { font-size: 10pt; padding: 3px; }");
    m_standardModeCheckbox->setChecked(true); // 默認勾選標準模式
    
    m_rapidModeCheckbox = new QCheckBox(tr("⚡ 快棋模式 - 限時10分鐘"), this);
    m_rapidModeCheckbox->setStyleSheet("QCheckBox { font-size: 10pt; padding: 3px; }");
    
    m_blitzModeCheckbox = new QCheckBox(tr("💨 閃電戰 - 限時3分鐘"), this);
    m_blitzModeCheckbox->setStyleSheet("QCheckBox { font-size: 10pt; padding: 3px; }");
    
    m_handicapModeCheckbox = new QCheckBox(tr("🎲 讓子模式 - 可移除部分棋子"), this);
    m_handicapModeCheckbox->setStyleSheet("QCheckBox { font-size: 10pt; padding: 3px; }");
    
    m_customRulesCheckbox = new QCheckBox(tr("🔧 自訂規則 - 特殊變體玩法"), this);
    m_customRulesCheckbox->setStyleSheet("QCheckBox { font-size: 10pt; padding: 3px; }");
    
    gameModeLayout->addWidget(m_standardModeCheckbox);
    gameModeLayout->addWidget(m_rapidModeCheckbox);
    gameModeLayout->addWidget(m_blitzModeCheckbox);
    gameModeLayout->addWidget(m_handicapModeCheckbox);
    gameModeLayout->addWidget(m_customRulesCheckbox);
    
    mainLayout->addWidget(gameModeGroup);
    
    mainLayout->addStretch();
    
    // 按鈕
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_okButton = new QPushButton(tr("確定"), this);
    m_okButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; padding: 10px 20px; font-weight: bold; }");
    m_cancelButton = new QPushButton(tr("取消"), this);
    m_cancelButton->setStyleSheet("QPushButton { padding: 10px 20px; }");
    
    buttonLayout->addWidget(m_okButton);
    buttonLayout->addWidget(m_cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 連接信號
    connect(m_okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QMap<QString, bool> OnlineDialog::getGameModes() const
{
    QMap<QString, bool> gameModes;
    
    if (m_standardModeCheckbox) {
        gameModes["標準模式"] = m_standardModeCheckbox->isChecked();
    }
    if (m_rapidModeCheckbox) {
        gameModes["快棋模式"] = m_rapidModeCheckbox->isChecked();
    }
    if (m_blitzModeCheckbox) {
        gameModes["閃電戰"] = m_blitzModeCheckbox->isChecked();
    }
    if (m_handicapModeCheckbox) {
        gameModes["讓子模式"] = m_handicapModeCheckbox->isChecked();
    }
    if (m_customRulesCheckbox) {
        gameModes["自訂規則"] = m_customRulesCheckbox->isChecked();
    }
    
    return gameModes;
}
