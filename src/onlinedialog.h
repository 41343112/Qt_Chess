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
#include <QCheckBox>

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
    QString getHostAddress() const;
    quint16 getPort() const;
    QString getRoomCode() const;  // For relay mode
    bool useRelayServer() const { return m_useRelay; }

private slots:
    void onCreateRoomClicked();
    void onJoinRoomClicked();
    void onPasteConnectionInfo();
    void onUseRelayToggled(bool checked);

private:
    void setupUI();
    void parseConnectionInfo(const QString& info);
    void updateUIForMode();

    Mode m_mode;
    bool m_useRelay;
    
    QRadioButton* m_createRoomRadio;
    QRadioButton* m_joinRoomRadio;
    
    QCheckBox* m_useRelayCheckbox;
    
    QWidget* m_joinRoomWidget;
    QLineEdit* m_hostAddressEdit;
    QLineEdit* m_roomNumberEdit;
    QTextEdit* m_connectionInfoEdit;
    QPushButton* m_pasteButton;
    
    // Relay mode widgets
    QLineEdit* m_roomCodeEdit;
    
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;
    
    QLabel* m_instructionLabel;
};

#endif // ONLINEDIALOG_H
