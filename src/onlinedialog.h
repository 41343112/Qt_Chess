#ifndef ONLINEDIALOG_H
#define ONLINEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>

class OnlineDialog : public QDialog
{
    Q_OBJECT

public:
    enum class Mode {
        None,
        CreateRoom,
        JoinRoom
    };

    explicit OnlineDialog(QWidget *parent = nullptr);
    ~OnlineDialog();

    Mode getMode() const { return m_mode; }
    QString getRoomNumber() const;

private slots:
    void onCreateRoomClicked();
    void onJoinRoomClicked();
    void onPasteConnectionInfo();

private:
    void setupUI();
    void parseConnectionInfo(const QString& info);

    Mode m_mode;
    
    QRadioButton* m_createRoomRadio;
    QRadioButton* m_joinRoomRadio;
    
    QWidget* m_joinRoomWidget;
    QLineEdit* m_roomNumberEdit;
    QTextEdit* m_connectionInfoEdit;
    QPushButton* m_pasteButton;
    
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    QLabel* m_instructionLabel;
};

#endif // ONLINEDIALOG_H
