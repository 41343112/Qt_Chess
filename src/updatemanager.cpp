#include "updatemanager.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

UpdateManager::UpdateManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_currentVersion("1.0.0")  // 預設版本
{
}

UpdateManager::~UpdateManager()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

void UpdateManager::checkForUpdates(const QString& owner, const QString& repo)
{
    QString url = QString("https://api.github.com/repos/%1/%2/releases/latest").arg(owner, repo);
    
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt_Chess_Updater");
    
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    
    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &UpdateManager::onCheckReplyFinished);
}

void UpdateManager::onCheckReplyFinished()
{
    if (!m_currentReply) {
        return;
    }
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        QByteArray data = m_currentReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        
        if (!doc.isNull() && doc.isObject()) {
            QJsonObject obj = doc.object();
            QString latestVersion = obj["tag_name"].toString();
            QString releaseNotes = obj["body"].toString();
            
            // 移除版本號前面的 'v' 字符（如果有）
            if (latestVersion.startsWith("v", Qt::CaseInsensitive)) {
                latestVersion = latestVersion.mid(1);
            }
            
            // 比較版本
            if (compareVersions(latestVersion, m_currentVersion)) {
                // 尋找適合當前平台的下載連結
                QJsonArray assets = obj["assets"].toArray();
                QString downloadUrl;
                
                for (const QJsonValue& assetValue : assets) {
                    QJsonObject asset = assetValue.toObject();
                    QString name = asset["name"].toString().toLower();
                    QString browserDownloadUrl = asset["browser_download_url"].toString();
                    
                    // 根據平台選擇合適的檔案
#ifdef Q_OS_WIN
                    if (name.contains("windows") || name.endsWith(".exe") || name.endsWith(".zip")) {
                        downloadUrl = browserDownloadUrl;
                        break;
                    }
#elif defined(Q_OS_LINUX)
                    if (name.contains("linux") || name.endsWith(".appimage") || name.endsWith(".tar.gz")) {
                        downloadUrl = browserDownloadUrl;
                        break;
                    }
#elif defined(Q_OS_MAC)
                    if (name.contains("mac") || name.contains("darwin") || name.endsWith(".dmg")) {
                        downloadUrl = browserDownloadUrl;
                        break;
                    }
#endif
                }
                
                // 如果沒有找到平台特定的檔案，使用第一個資產
                if (downloadUrl.isEmpty() && !assets.isEmpty()) {
                    downloadUrl = assets[0].toObject()["browser_download_url"].toString();
                }
                
                if (!downloadUrl.isEmpty()) {
                    emit updateAvailable(latestVersion, downloadUrl, releaseNotes);
                } else {
                    emit noUpdateAvailable();
                }
            } else {
                emit noUpdateAvailable();
            }
        } else {
            emit checkError("無法解析 GitHub API 回應");
        }
    } else {
        emit checkError(m_currentReply->errorString());
    }
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void UpdateManager::downloadUpdate(const QString& downloadUrl, const QString& fileName)
{
    // 建立下載目錄
    QString downloadPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QDir downloadDir(downloadPath);
    if (!downloadDir.exists()) {
        downloadDir.mkpath(".");
    }
    
    m_downloadFilePath = downloadDir.filePath(fileName);
    
    QNetworkRequest request;
    request.setUrl(QUrl(downloadUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, "Qt_Chess_Updater");
    
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    
    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &UpdateManager::onDownloadReplyFinished);
    connect(m_currentReply, &QNetworkReply::downloadProgress, this, &UpdateManager::onDownloadProgress);
}

void UpdateManager::onDownloadReplyFinished()
{
    if (!m_currentReply) {
        return;
    }
    
    if (m_currentReply->error() == QNetworkReply::NoError) {
        QFile file(m_downloadFilePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(m_currentReply->readAll());
            file.close();
            emit downloadComplete(m_downloadFilePath);
        } else {
            emit downloadError("無法儲存檔案：" + file.errorString());
        }
    } else {
        emit downloadError(m_currentReply->errorString());
    }
    
    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void UpdateManager::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

bool UpdateManager::compareVersions(const QString& version1, const QString& version2)
{
    // 簡單的版本比較（假設格式為 x.y.z）
    QStringList v1Parts = version1.split('.');
    QStringList v2Parts = version2.split('.');
    
    int maxLen = qMax(v1Parts.size(), v2Parts.size());
    
    for (int i = 0; i < maxLen; ++i) {
        int v1Part = (i < v1Parts.size()) ? v1Parts[i].toInt() : 0;
        int v2Part = (i < v2Parts.size()) ? v2Parts[i].toInt() : 0;
        
        if (v1Part > v2Part) {
            return true;  // version1 較新
        } else if (v1Part < v2Part) {
            return false; // version2 較新
        }
    }
    
    return false; // 版本相同
}
