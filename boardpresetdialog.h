#ifndef BOARDPRESETDIALOG_H
#define BOARDPRESETDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "chessboard.h"

class BoardPresetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BoardPresetDialog(QWidget *parent = nullptr);
    ~BoardPresetDialog();
    
    BoardPreset getSelectedPreset() const { return m_selectedPreset; }

private slots:
    void onOkClicked();
    void onCancelClicked();

private:
    void setupUI();
    
    QRadioButton* m_standardRadio;
    QRadioButton* m_midGameRadio;
    QRadioButton* m_endGameRadio;
    QButtonGroup* m_buttonGroup;
    
    BoardPreset m_selectedPreset;
};

#endif // BOARDPRESETDIALOG_H
