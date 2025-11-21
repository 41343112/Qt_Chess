#include "soundsettingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

SoundSettingsDialog::SoundSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("音效設定");
    setModal(true);
    setMinimumWidth(600);
    
    setupUI();
    
    // Load saved settings or use defaults
    QSettings settings("QtChess", "SoundSettings");
    m_settings.moveSound = settings.value("moveSound", "qrc:/resources/sounds/move.wav").toString();
    m_settings.captureSound = settings.value("captureSound", "qrc:/resources/sounds/capture.wav").toString();
    m_settings.castlingSound = settings.value("castlingSound", "qrc:/resources/sounds/castling.wav").toString();
    m_settings.checkSound = settings.value("checkSound", "qrc:/resources/sounds/check.wav").toString();
    m_settings.checkmateSound = settings.value("checkmateSound", "qrc:/resources/sounds/checkmate.wav").toString();
    
    m_settings.moveVolume = settings.value("moveVolume", 0.5).toDouble();
    m_settings.captureVolume = settings.value("captureVolume", 0.5).toDouble();
    m_settings.castlingVolume = settings.value("castlingVolume", 0.5).toDouble();
    m_settings.checkVolume = settings.value("checkVolume", 0.5).toDouble();
    m_settings.checkmateVolume = settings.value("checkmateVolume", 0.6).toDouble();
    
    m_settings.moveSoundEnabled = settings.value("moveSoundEnabled", true).toBool();
    m_settings.captureSoundEnabled = settings.value("captureSoundEnabled", true).toBool();
    m_settings.castlingSoundEnabled = settings.value("castlingSoundEnabled", true).toBool();
    m_settings.checkSoundEnabled = settings.value("checkSoundEnabled", true).toBool();
    m_settings.checkmateSoundEnabled = settings.value("checkmateSoundEnabled", true).toBool();
    m_settings.allSoundsEnabled = settings.value("allSoundsEnabled", true).toBool();
    
    setSettings(m_settings);
}

SoundSettingsDialog::~SoundSettingsDialog()
{
}

void SoundSettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Master enable/disable checkbox
    m_allSoundsCheckBox = new QCheckBox("啟用所有音效", this);
    m_allSoundsCheckBox->setChecked(true);
    connect(m_allSoundsCheckBox, &QCheckBox::toggled, this, &SoundSettingsDialog::onAllSoundsToggled);
    mainLayout->addWidget(m_allSoundsCheckBox);
    
    // Create a group box for all sound settings
    QGroupBox* soundsGroupBox = new QGroupBox("音效設定", this);
    QGridLayout* gridLayout = new QGridLayout(soundsGroupBox);
    
    int row = 0;
    
    // Helper lambda to create a row of controls
    auto createSoundRow = [this, gridLayout, &row](
        const QString& label,
        QLineEdit** editPtr,
        QCheckBox** checkBoxPtr,
        QSlider** sliderPtr,
        QLabel** volumeLabelPtr,
        QPushButton** browsePtr,
        QPushButton** previewPtr,
        std::function<void()> browseSlot,
        std::function<void()> previewSlot
    ) {
        // Label
        QLabel* nameLabel = new QLabel(label, this);
        gridLayout->addWidget(nameLabel, row, 0);
        
        // Enable checkbox
        *checkBoxPtr = new QCheckBox("啟用", this);
        (*checkBoxPtr)->setChecked(true);
        gridLayout->addWidget(*checkBoxPtr, row, 1);
        
        // File path edit
        *editPtr = new QLineEdit(this);
        (*editPtr)->setReadOnly(true);
        gridLayout->addWidget(*editPtr, row, 2);
        
        // Browse button
        *browsePtr = new QPushButton("瀏覽...", this);
        connect(*browsePtr, &QPushButton::clicked, this, browseSlot);
        gridLayout->addWidget(*browsePtr, row, 3);
        
        // Volume label
        QLabel* volLabel = new QLabel("音量:", this);
        gridLayout->addWidget(volLabel, row, 4);
        
        // Volume slider
        *sliderPtr = new QSlider(Qt::Horizontal, this);
        (*sliderPtr)->setRange(0, 100);
        (*sliderPtr)->setValue(50);
        (*sliderPtr)->setFixedWidth(100);
        gridLayout->addWidget(*sliderPtr, row, 5);
        
        // Volume percentage label
        *volumeLabelPtr = new QLabel("50%", this);
        (*volumeLabelPtr)->setFixedWidth(40);
        gridLayout->addWidget(*volumeLabelPtr, row, 6);
        
        // Connect slider to update label
        connect(*sliderPtr, &QSlider::valueChanged, [volumeLabelPtr](int value) {
            (*volumeLabelPtr)->setText(QString("%1%").arg(value));
        });
        
        // Preview button
        *previewPtr = new QPushButton("預覽", this);
        connect(*previewPtr, &QPushButton::clicked, this, previewSlot);
        gridLayout->addWidget(*previewPtr, row, 7);
        
        row++;
    };
    
    // Create rows for each sound type
    createSoundRow("移動音效:", &m_moveSoundEdit, &m_moveSoundCheckBox, &m_moveVolumeSlider, 
                   &m_moveVolumeLabel, &m_moveBrowseButton, &m_movePreviewButton,
                   &SoundSettingsDialog::onBrowseMove, &SoundSettingsDialog::onPreviewMove);
    
    createSoundRow("吃子音效:", &m_captureSoundEdit, &m_captureSoundCheckBox, &m_captureVolumeSlider,
                   &m_captureVolumeLabel, &m_captureBrowseButton, &m_capturePreviewButton,
                   &SoundSettingsDialog::onBrowseCapture, &SoundSettingsDialog::onPreviewCapture);
    
    createSoundRow("王車易位:", &m_castlingSoundEdit, &m_castlingSoundCheckBox, &m_castlingVolumeSlider,
                   &m_castlingVolumeLabel, &m_castlingBrowseButton, &m_castlingPreviewButton,
                   &SoundSettingsDialog::onBrowseCastling, &SoundSettingsDialog::onPreviewCastling);
    
    createSoundRow("將軍音效:", &m_checkSoundEdit, &m_checkSoundCheckBox, &m_checkVolumeSlider,
                   &m_checkVolumeLabel, &m_checkBrowseButton, &m_checkPreviewButton,
                   &SoundSettingsDialog::onBrowseCheck, &SoundSettingsDialog::onPreviewCheck);
    
    createSoundRow("將死音效:", &m_checkmateSoundEdit, &m_checkmateSoundCheckBox, &m_checkmateVolumeSlider,
                   &m_checkmateVolumeLabel, &m_checkmateBrowseButton, &m_checkmatePreviewButton,
                   &SoundSettingsDialog::onBrowseCheckmate, &SoundSettingsDialog::onPreviewCheckmate);
    
    mainLayout->addWidget(soundsGroupBox);
    
    // Buttons at bottom
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton* resetButton = new QPushButton("重設為預設值", this);
    connect(resetButton, &QPushButton::clicked, this, &SoundSettingsDialog::onResetToDefaults);
    buttonLayout->addWidget(resetButton);
    
    QPushButton* okButton = new QPushButton("確定", this);
    connect(okButton, &QPushButton::clicked, this, &SoundSettingsDialog::onAccept);
    buttonLayout->addWidget(okButton);
    
    QPushButton* cancelButton = new QPushButton("取消", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

SoundSettingsDialog::SoundSettings SoundSettingsDialog::getSettings() const
{
    return m_settings;
}

void SoundSettingsDialog::setSettings(const SoundSettings& settings)
{
    m_settings = settings;
    
    m_allSoundsCheckBox->setChecked(m_settings.allSoundsEnabled);
    
    m_moveSoundEdit->setText(m_settings.moveSound);
    m_moveSoundCheckBox->setChecked(m_settings.moveSoundEnabled);
    m_moveVolumeSlider->setValue(static_cast<int>(m_settings.moveVolume * 100));
    
    m_captureSoundEdit->setText(m_settings.captureSound);
    m_captureSoundCheckBox->setChecked(m_settings.captureSoundEnabled);
    m_captureVolumeSlider->setValue(static_cast<int>(m_settings.captureVolume * 100));
    
    m_castlingSoundEdit->setText(m_settings.castlingSound);
    m_castlingSoundCheckBox->setChecked(m_settings.castlingSoundEnabled);
    m_castlingVolumeSlider->setValue(static_cast<int>(m_settings.castlingVolume * 100));
    
    m_checkSoundEdit->setText(m_settings.checkSound);
    m_checkSoundCheckBox->setChecked(m_settings.checkSoundEnabled);
    m_checkVolumeSlider->setValue(static_cast<int>(m_settings.checkVolume * 100));
    
    m_checkmateSoundEdit->setText(m_settings.checkmateSound);
    m_checkmateSoundCheckBox->setChecked(m_settings.checkmateSoundEnabled);
    m_checkmateVolumeSlider->setValue(static_cast<int>(m_settings.checkmateVolume * 100));
}

void SoundSettingsDialog::onBrowseMove()
{
    QString file = browseForSoundFile();
    if (!file.isEmpty()) {
        m_moveSoundEdit->setText(file);
    }
}

void SoundSettingsDialog::onBrowseCapture()
{
    QString file = browseForSoundFile();
    if (!file.isEmpty()) {
        m_captureSoundEdit->setText(file);
    }
}

void SoundSettingsDialog::onBrowseCastling()
{
    QString file = browseForSoundFile();
    if (!file.isEmpty()) {
        m_castlingSoundEdit->setText(file);
    }
}

void SoundSettingsDialog::onBrowseCheck()
{
    QString file = browseForSoundFile();
    if (!file.isEmpty()) {
        m_checkSoundEdit->setText(file);
    }
}

void SoundSettingsDialog::onBrowseCheckmate()
{
    QString file = browseForSoundFile();
    if (!file.isEmpty()) {
        m_checkmateSoundEdit->setText(file);
    }
}

void SoundSettingsDialog::onPreviewMove()
{
    previewSound(m_moveSoundEdit->text(), m_moveVolumeSlider->value() / 100.0);
}

void SoundSettingsDialog::onPreviewCapture()
{
    previewSound(m_captureSoundEdit->text(), m_captureVolumeSlider->value() / 100.0);
}

void SoundSettingsDialog::onPreviewCastling()
{
    previewSound(m_castlingSoundEdit->text(), m_castlingVolumeSlider->value() / 100.0);
}

void SoundSettingsDialog::onPreviewCheck()
{
    previewSound(m_checkSoundEdit->text(), m_checkVolumeSlider->value() / 100.0);
}

void SoundSettingsDialog::onPreviewCheckmate()
{
    previewSound(m_checkmateSoundEdit->text(), m_checkmateVolumeSlider->value() / 100.0);
}

void SoundSettingsDialog::onAllSoundsToggled(bool enabled)
{
    m_moveSoundCheckBox->setEnabled(enabled);
    m_captureSoundCheckBox->setEnabled(enabled);
    m_castlingSoundCheckBox->setEnabled(enabled);
    m_checkSoundCheckBox->setEnabled(enabled);
    m_checkmateSoundCheckBox->setEnabled(enabled);
}

void SoundSettingsDialog::onResetToDefaults()
{
    m_settings.moveSound = "qrc:/resources/sounds/move.wav";
    m_settings.captureSound = "qrc:/resources/sounds/capture.wav";
    m_settings.castlingSound = "qrc:/resources/sounds/castling.wav";
    m_settings.checkSound = "qrc:/resources/sounds/check.wav";
    m_settings.checkmateSound = "qrc:/resources/sounds/checkmate.wav";
    
    m_settings.moveVolume = 0.5;
    m_settings.captureVolume = 0.5;
    m_settings.castlingVolume = 0.5;
    m_settings.checkVolume = 0.5;
    m_settings.checkmateVolume = 0.6;
    
    m_settings.moveSoundEnabled = true;
    m_settings.captureSoundEnabled = true;
    m_settings.castlingSoundEnabled = true;
    m_settings.checkSoundEnabled = true;
    m_settings.checkmateSoundEnabled = true;
    m_settings.allSoundsEnabled = true;
    
    setSettings(m_settings);
}

void SoundSettingsDialog::onAccept()
{
    // Collect settings from UI
    m_settings.allSoundsEnabled = m_allSoundsCheckBox->isChecked();
    
    m_settings.moveSound = m_moveSoundEdit->text();
    m_settings.moveSoundEnabled = m_moveSoundCheckBox->isChecked();
    m_settings.moveVolume = m_moveVolumeSlider->value() / 100.0;
    
    m_settings.captureSound = m_captureSoundEdit->text();
    m_settings.captureSoundEnabled = m_captureSoundCheckBox->isChecked();
    m_settings.captureVolume = m_captureVolumeSlider->value() / 100.0;
    
    m_settings.castlingSound = m_castlingSoundEdit->text();
    m_settings.castlingSoundEnabled = m_castlingSoundCheckBox->isChecked();
    m_settings.castlingVolume = m_castlingVolumeSlider->value() / 100.0;
    
    m_settings.checkSound = m_checkSoundEdit->text();
    m_settings.checkSoundEnabled = m_checkSoundCheckBox->isChecked();
    m_settings.checkVolume = m_checkVolumeSlider->value() / 100.0;
    
    m_settings.checkmateSound = m_checkmateSoundEdit->text();
    m_settings.checkmateSoundEnabled = m_checkmateSoundCheckBox->isChecked();
    m_settings.checkmateVolume = m_checkmateVolumeSlider->value() / 100.0;
    
    // Save settings
    QSettings settings("QtChess", "SoundSettings");
    settings.setValue("moveSound", m_settings.moveSound);
    settings.setValue("captureSound", m_settings.captureSound);
    settings.setValue("castlingSound", m_settings.castlingSound);
    settings.setValue("checkSound", m_settings.checkSound);
    settings.setValue("checkmateSound", m_settings.checkmateSound);
    
    settings.setValue("moveVolume", m_settings.moveVolume);
    settings.setValue("captureVolume", m_settings.captureVolume);
    settings.setValue("castlingVolume", m_settings.castlingVolume);
    settings.setValue("checkVolume", m_settings.checkVolume);
    settings.setValue("checkmateVolume", m_settings.checkmateVolume);
    
    settings.setValue("moveSoundEnabled", m_settings.moveSoundEnabled);
    settings.setValue("captureSoundEnabled", m_settings.captureSoundEnabled);
    settings.setValue("castlingSoundEnabled", m_settings.castlingSoundEnabled);
    settings.setValue("checkSoundEnabled", m_settings.checkSoundEnabled);
    settings.setValue("checkmateSoundEnabled", m_settings.checkmateSoundEnabled);
    settings.setValue("allSoundsEnabled", m_settings.allSoundsEnabled);
    
    accept();
}

QString SoundSettingsDialog::browseForSoundFile()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "選擇音效檔案",
        QString(),
        "音效檔案 (*.wav *.mp3 *.ogg);;所有檔案 (*.*)"
    );
    return file;
}

void SoundSettingsDialog::previewSound(const QString& soundFile, double volume)
{
    if (soundFile.isEmpty()) {
        QMessageBox::warning(this, "錯誤", "未選擇音效檔案");
        return;
    }
    
    m_previewSound.stop();
    
    // Handle both qrc: URLs and file paths
    if (soundFile.startsWith("qrc:")) {
        m_previewSound.setSource(QUrl(soundFile));
    } else {
        m_previewSound.setSource(QUrl::fromLocalFile(soundFile));
    }
    
    m_previewSound.setVolume(volume);
    m_previewSound.play();
}
