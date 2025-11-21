#include "pieceappearancesettingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSettings>
#include <QFont>

PieceAppearanceSettingsDialog::PieceAppearanceSettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("棋子外觀設定");
    setModal(true);
    setMinimumWidth(500);
    setMinimumHeight(400);
    
    setupUI();
    
    // Load saved settings or use defaults
    AppearanceSettings defaults = getDefaultSettings();
    QSettings settings("QtChess", "AppearanceSettings");
    
    int styleInt = settings.value("style", static_cast<int>(defaults.style)).toInt();
    m_settings.style = static_cast<PieceAppearanceStyle>(styleInt);
    m_settings.fontSize = settings.value("fontSize", defaults.fontSize).toInt();
    
    setSettings(m_settings);
}

PieceAppearanceSettingsDialog::~PieceAppearanceSettingsDialog()
{
}

void PieceAppearanceSettingsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Style selection group
    QGroupBox* styleGroup = new QGroupBox("棋子樣式", this);
    QVBoxLayout* styleLayout = new QVBoxLayout(styleGroup);
    
    m_styleButtonGroup = new QButtonGroup(this);
    
    m_unicodeRadio = new QRadioButton("標準 Unicode 符號 (♔♕♖♗♘♙)", this);
    m_unicodeAltRadio = new QRadioButton("替代 Unicode 符號 (♚♛♜♝♞♟)", this);
    m_textBasedRadio = new QRadioButton("文字符號 (K Q R B N P)", this);
    
    m_styleButtonGroup->addButton(m_unicodeRadio, static_cast<int>(PieceAppearanceStyle::UnicodeSymbols));
    m_styleButtonGroup->addButton(m_unicodeAltRadio, static_cast<int>(PieceAppearanceStyle::UnicodeAlternate));
    m_styleButtonGroup->addButton(m_textBasedRadio, static_cast<int>(PieceAppearanceStyle::TextBased));
    
    styleLayout->addWidget(m_unicodeRadio);
    styleLayout->addWidget(m_unicodeAltRadio);
    styleLayout->addWidget(m_textBasedRadio);
    
    connect(m_styleButtonGroup, QOverload<int>::of(&QButtonGroup::idClicked),
            this, [this]() { onStyleChanged(); });
    
    mainLayout->addWidget(styleGroup);
    
    // Font size selection
    QHBoxLayout* fontSizeLayout = new QHBoxLayout();
    QLabel* fontSizeTextLabel = new QLabel("字型大小:", this);
    m_fontSizeSpinBox = new QSpinBox(this);
    m_fontSizeSpinBox->setRange(20, 72);
    m_fontSizeSpinBox->setValue(36);
    m_fontSizeSpinBox->setSuffix(" pt");
    m_fontSizeLabel = new QLabel(this);
    
    connect(m_fontSizeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PieceAppearanceSettingsDialog::onFontSizeChanged);
    
    fontSizeLayout->addWidget(fontSizeTextLabel);
    fontSizeLayout->addWidget(m_fontSizeSpinBox);
    fontSizeLayout->addStretch();
    
    mainLayout->addLayout(fontSizeLayout);
    
    // Preview group
    m_previewGroup = new QGroupBox("預覽", this);
    QVBoxLayout* previewLayout = new QVBoxLayout(m_previewGroup);
    
    // White pieces preview
    QHBoxLayout* whiteLayout = new QHBoxLayout();
    QLabel* whiteLabel = new QLabel("<b>白方:</b>", this);
    whiteLayout->addWidget(whiteLabel);
    
    m_whiteKingPreview = new QLabel(this);
    m_whiteQueenPreview = new QLabel(this);
    m_whiteRookPreview = new QLabel(this);
    m_whiteBishopPreview = new QLabel(this);
    m_whiteKnightPreview = new QLabel(this);
    m_whitePawnPreview = new QLabel(this);
    
    whiteLayout->addWidget(m_whiteKingPreview);
    whiteLayout->addWidget(m_whiteQueenPreview);
    whiteLayout->addWidget(m_whiteRookPreview);
    whiteLayout->addWidget(m_whiteBishopPreview);
    whiteLayout->addWidget(m_whiteKnightPreview);
    whiteLayout->addWidget(m_whitePawnPreview);
    whiteLayout->addStretch();
    
    previewLayout->addLayout(whiteLayout);
    
    // Black pieces preview
    QHBoxLayout* blackLayout = new QHBoxLayout();
    QLabel* blackLabel = new QLabel("<b>黑方:</b>", this);
    blackLayout->addWidget(blackLabel);
    
    m_blackKingPreview = new QLabel(this);
    m_blackQueenPreview = new QLabel(this);
    m_blackRookPreview = new QLabel(this);
    m_blackBishopPreview = new QLabel(this);
    m_blackKnightPreview = new QLabel(this);
    m_blackPawnPreview = new QLabel(this);
    
    blackLayout->addWidget(m_blackKingPreview);
    blackLayout->addWidget(m_blackQueenPreview);
    blackLayout->addWidget(m_blackRookPreview);
    blackLayout->addWidget(m_blackBishopPreview);
    blackLayout->addWidget(m_blackKnightPreview);
    blackLayout->addWidget(m_blackPawnPreview);
    blackLayout->addStretch();
    
    previewLayout->addLayout(blackLayout);
    
    mainLayout->addWidget(m_previewGroup);
    mainLayout->addStretch();
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    QPushButton* resetButton = new QPushButton("重設為預設值", this);
    connect(resetButton, &QPushButton::clicked, this, &PieceAppearanceSettingsDialog::onResetToDefaults);
    
    QPushButton* okButton = new QPushButton("確定", this);
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &PieceAppearanceSettingsDialog::onAccept);
    
    QPushButton* cancelButton = new QPushButton("取消", this);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    
    buttonLayout->addWidget(resetButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
}

void PieceAppearanceSettingsDialog::updatePreview()
{
    updatePreviewLabel(m_whiteKingPreview, PieceType::King, PieceColor::White);
    updatePreviewLabel(m_whiteQueenPreview, PieceType::Queen, PieceColor::White);
    updatePreviewLabel(m_whiteRookPreview, PieceType::Rook, PieceColor::White);
    updatePreviewLabel(m_whiteBishopPreview, PieceType::Bishop, PieceColor::White);
    updatePreviewLabel(m_whiteKnightPreview, PieceType::Knight, PieceColor::White);
    updatePreviewLabel(m_whitePawnPreview, PieceType::Pawn, PieceColor::White);
    
    updatePreviewLabel(m_blackKingPreview, PieceType::King, PieceColor::Black);
    updatePreviewLabel(m_blackQueenPreview, PieceType::Queen, PieceColor::Black);
    updatePreviewLabel(m_blackRookPreview, PieceType::Rook, PieceColor::Black);
    updatePreviewLabel(m_blackBishopPreview, PieceType::Bishop, PieceColor::Black);
    updatePreviewLabel(m_blackKnightPreview, PieceType::Knight, PieceColor::Black);
    updatePreviewLabel(m_blackPawnPreview, PieceType::Pawn, PieceColor::Black);
}

void PieceAppearanceSettingsDialog::updatePreviewLabel(QLabel* label, PieceType type, PieceColor color)
{
    QString symbol = ChessPiece::getSymbolForStyle(type, color, m_settings.style);
    label->setText(symbol);
    
    QFont font;
    font.setPointSize(m_settings.fontSize);
    label->setFont(font);
    
    // Set colors for text-based style
    if (m_settings.style == PieceAppearanceStyle::TextBased) {
        if (color == PieceColor::White) {
            label->setStyleSheet("QLabel { color: #FFFFFF; background-color: #000000; padding: 5px; }");
        } else {
            label->setStyleSheet("QLabel { color: #000000; background-color: #FFFFFF; padding: 5px; border: 1px solid #000000; }");
        }
    } else {
        label->setStyleSheet("");
    }
}

PieceAppearanceSettingsDialog::AppearanceSettings PieceAppearanceSettingsDialog::getSettings() const
{
    return m_settings;
}

void PieceAppearanceSettingsDialog::setSettings(const AppearanceSettings& settings)
{
    m_settings = settings;
    
    // Set radio button
    int buttonId = static_cast<int>(settings.style);
    QAbstractButton* button = m_styleButtonGroup->button(buttonId);
    if (button) {
        button->setChecked(true);
    }
    
    // Set font size
    m_fontSizeSpinBox->setValue(settings.fontSize);
    
    updatePreview();
}

PieceAppearanceSettingsDialog::AppearanceSettings PieceAppearanceSettingsDialog::getDefaultSettings()
{
    AppearanceSettings defaults;
    defaults.style = PieceAppearanceStyle::UnicodeSymbols;
    defaults.fontSize = 36;
    return defaults;
}

void PieceAppearanceSettingsDialog::onStyleChanged()
{
    int checkedId = m_styleButtonGroup->checkedId();
    if (checkedId >= 0) {
        m_settings.style = static_cast<PieceAppearanceStyle>(checkedId);
        updatePreview();
    }
}

void PieceAppearanceSettingsDialog::onFontSizeChanged(int size)
{
    m_settings.fontSize = size;
    updatePreview();
}

void PieceAppearanceSettingsDialog::onResetToDefaults()
{
    AppearanceSettings defaults = getDefaultSettings();
    setSettings(defaults);
}

void PieceAppearanceSettingsDialog::onAccept()
{
    // Save settings
    QSettings settings("QtChess", "AppearanceSettings");
    settings.setValue("style", static_cast<int>(m_settings.style));
    settings.setValue("fontSize", m_settings.fontSize);
    
    accept();
}
