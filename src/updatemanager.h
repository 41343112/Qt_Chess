#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QJsonObject>

class UpdateManager : public QObject
{
    Q_OBJECT

public:
    explicit UpdateManager(QObject *parent = nullptr);
    ~UpdateManager();

    // 檢查更新
    void checkForUpdates(const QString& owner, const QString& repo);
    
    // 下載更新
    void downloadUpdate(const QString& downloadUrl, const QString& fileName);
    
    // 取得當前版本
    QString getCurrentVersion() const { return m_currentVersion; }
    
    // 設定當前版本
    void setCurrentVersion(const QString& version) { m_currentVersion = version; }

signals:
    void updateAvailable(const QString& latestVersion, const QString& downloadUrl, const QString& releaseNotes);
    void noUpdateAvailable();
    void checkError(const QString& errorMessage);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadComplete(const QString& filePath);
    void downloadError(const QString& errorMessage);

private slots:
    void onCheckReplyFinished();
    void onDownloadReplyFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentReply;
    QString m_currentVersion;
    QString m_downloadFilePath;
    
    bool compareVersions(const QString& version1, const QString& version2);
};

#endif // UPDATEMANAGER_H
