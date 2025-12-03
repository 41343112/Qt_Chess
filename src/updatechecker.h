#ifndef UPDATECHECKER_H
#define UPDATECHECKER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class UpdateChecker : public QObject
{
    Q_OBJECT

public:
    explicit UpdateChecker(QObject *parent = nullptr);
    ~UpdateChecker();

    void checkForUpdates();
    QString getCurrentVersion() const;
    QString getLatestVersion() const;
    QString getDownloadUrl() const;
    bool isUpdateAvailable() const;

signals:
    void updateAvailable(const QString& version, const QString& downloadUrl, const QString& releaseNotes);
    void noUpdateAvailable();
    void checkFailed(const QString& error);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(const QString& filePath);
    void downloadFailed(const QString& error);

public slots:
    void startDownload();

private slots:
    void onCheckReplyFinished();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onDownloadFinished();

private:
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentReply;
    QString m_currentVersion;
    QString m_latestVersion;
    QString m_downloadUrl;
    QString m_releaseNotes;

    bool compareVersions(const QString& version1, const QString& version2) const;
    QString extractVersionFromTag(const QString& tag) const;
};

#endif // UPDATECHECKER_H
