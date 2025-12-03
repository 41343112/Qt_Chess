#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = nullptr);
    ~UpdateDialog();

    void setUpdateInfo(const QString& currentVersion, const QString& latestVersion, 
                      const QString& downloadUrl, const QString& releaseNotes);
    void showCheckingUpdate();
    void showNoUpdate();
    void showError(const QString& errorMessage);
    void startDownload();
    void updateProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadComplete(const QString& filePath);
    void downloadFailed(const QString& errorMessage);
    
    QString getDownloadUrl() const { return m_downloadUrl; }

signals:
    void downloadRequested();

private slots:
    void onDownloadClicked();
    void onCancelClicked();

private:
    QLabel* m_titleLabel;
    QLabel* m_messageLabel;
    QTextEdit* m_releaseNotesEdit;
    QProgressBar* m_progressBar;
    QPushButton* m_downloadButton;
    QPushButton* m_cancelButton;
    
    QString m_downloadUrl;
    QString m_filePath;
};

#endif // UPDATEDIALOG_H
