#ifndef PIECEAPPEARANCESETTINGSDIALOG_H
#define PIECEAPPEARANCESETTINGSDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include "chesspiece.h"

class PieceAppearanceSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PieceAppearanceSettingsDialog(QWidget *parent = nullptr);
    ~PieceAppearanceSettingsDialog();

    enum class AppearanceStyle {
        UnicodeSymbols,
        UnicodeAlternate,
        TextBased
    };

    struct AppearanceSettings {
        AppearanceStyle style;
        int fontSize;
    };

    AppearanceSettings getSettings() const;
    void setSettings(const AppearanceSettings& settings);
    
    static AppearanceSettings getDefaultSettings();
    static QString getPieceSymbol(PieceType type, PieceColor color, AppearanceStyle style);

private slots:
    void onStyleChanged();
    void onFontSizeChanged(int size);
    void onResetToDefaults();
    void onAccept();

private:
    AppearanceSettings m_settings;
    
    QRadioButton* m_unicodeRadio;
    QRadioButton* m_unicodeAltRadio;
    QRadioButton* m_textBasedRadio;
    QButtonGroup* m_styleButtonGroup;
    
    QSpinBox* m_fontSizeSpinBox;
    QLabel* m_fontSizeLabel;
    
    QGroupBox* m_previewGroup;
    QLabel* m_whiteKingPreview;
    QLabel* m_whiteQueenPreview;
    QLabel* m_whiteRookPreview;
    QLabel* m_whiteBishopPreview;
    QLabel* m_whiteKnightPreview;
    QLabel* m_whitePawnPreview;
    
    QLabel* m_blackKingPreview;
    QLabel* m_blackQueenPreview;
    QLabel* m_blackRookPreview;
    QLabel* m_blackBishopPreview;
    QLabel* m_blackKnightPreview;
    QLabel* m_blackPawnPreview;
    
    void setupUI();
    void updatePreview();
    void updatePreviewLabel(QLabel* label, PieceType type, PieceColor color);
};

#endif // PIECEAPPEARANCESETTINGSDIALOG_H
