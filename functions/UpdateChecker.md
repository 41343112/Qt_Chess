# UpdateChecker 自動更新功能

## 概述
`UpdateChecker` 類別負責檢查 GitHub 上是否有新版本可用，提供自動更新檢查功能。透過 GitHub API 查詢最新發行版本，並與當前版本進行比較。

## 檔案位置
- **標頭檔**: `src/updatechecker.h`
- **實作檔**: `src/updatechecker.cpp`

## 主要功能

### 1. 版本管理

#### 當前版本定義
```cpp
#define APP_VERSION "1.0.0"
```
定義應用程式的當前版本號（使用語義化版本 Semantic Versioning）。

#### getCurrentVersion()
```cpp
static QString getCurrentVersion()
```
取得當前應用程式版本。

**返回值**: 版本字串（如 "1.0.0"）

### 2. 更新檢查

#### checkForUpdates()
```cpp
void checkForUpdates()
```
檢查是否有新版本可用。

**執行流程**:
1. 建立 GitHub API 請求 URL
2. 設定必要的 HTTP headers（User-Agent）
3. 發送 GET 請求到 GitHub API
4. 等待回應並解析

**實作**:
```cpp
void UpdateChecker::checkForUpdates() {
    // GitHub API URL for latest release
    QString apiUrl = QString("https://api.github.com/repos/%1/%2/releases/latest")
                        .arg(GITHUB_REPO_OWNER, GITHUB_REPO_NAME);
    QUrl url(apiUrl);
    QNetworkRequest request(url);
    
    // 設定 User-Agent header (GitHub API 需要)
    QString userAgent = QString("Qt_Chess/%1").arg(APP_VERSION);
    request.setRawHeader("User-Agent", userAgent.toUtf8());
    
    m_networkManager->get(request);
}
```

**GitHub API 端點**:
```
https://api.github.com/repos/{owner}/{repo}/releases/latest
```

**範例**:
```
https://api.github.com/repos/41343112/Qt_Chess/releases/latest
```

### 3. 回應處理

#### onReplyFinished()
```cpp
void onReplyFinished(QNetworkReply* reply)
```
處理 GitHub API 的回應。

**處理步驟**:
1. 檢查網路錯誤
2. 讀取回應資料
3. 解析 JSON 格式
4. 提取版本資訊
5. 比較版本號
6. 發出對應信號

**實作**:
```cpp
void UpdateChecker::onReplyFinished(QNetworkReply* reply) {
    reply->deleteLater();
    
    // 1. 檢查錯誤
    if (reply->error() != QNetworkReply::NoError) {
        QString errorMsg;
        if (reply->error() == QNetworkReply::ContentNotFoundError) {
            errorMsg = "沒有找到可用的發行版本";
        } else {
            errorMsg = QString("網路錯誤: %1").arg(reply->errorString());
        }
        emit updateCheckFailed(errorMsg);
        return;
    }
    
    // 2-3. 讀取並解析 JSON
    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    
    if (!doc.isObject()) {
        emit updateCheckFailed("無法解析伺服器回應");
        return;
    }
    
    // 4. 提取版本資訊
    QJsonObject obj = doc.object();
    m_latestVersion = obj["tag_name"].toString();
    
    // 移除版本號前的 "v" 字首（如果存在）
    if (m_latestVersion.startsWith("v") || m_latestVersion.startsWith("V")) {
        m_latestVersion = m_latestVersion.mid(1);
    }
    
    m_releaseUrl = obj["html_url"].toString();
    m_releaseNotes = obj["body"].toString();
    
    // 5-6. 比較版本並發出信號
    QString currentVersion = getCurrentVersion();
    bool updateAvailable = compareVersions(m_latestVersion, currentVersion);
    
    emit updateCheckFinished(updateAvailable);
}
```

**GitHub API 回應格式**:
```json
{
  "tag_name": "v1.2.0",
  "name": "Qt Chess v1.2.0",
  "body": "## What's New\n- Added online multiplayer\n- Bug fixes",
  "html_url": "https://github.com/41343112/Qt_Chess/releases/tag/v1.2.0",
  "published_at": "2024-01-15T10:00:00Z",
  "assets": [
    {
      "name": "Qt_Chess-1.2.0-Linux.tar.gz",
      "browser_download_url": "https://..."
    }
  ]
}
```

### 4. 版本比較

#### compareVersions()
```cpp
bool compareVersions(const QString& version1, const QString& version2)
```
比較兩個版本號。

**參數**:
- `version1` - 第一個版本號（通常是最新版本）
- `version2` - 第二個版本號（通常是當前版本）

**返回值**: `true` 如果 version1 > version2（有新版本可用）

**實作**:
```cpp
bool UpdateChecker::compareVersions(const QString& version1, const QString& version2) {
    // 使用 QVersionNumber 進行版本比較
    QVersionNumber v1 = QVersionNumber::fromString(version1);
    QVersionNumber v2 = QVersionNumber::fromString(version2);
    
    return QVersionNumber::compare(v1, v2) > 0;
}
```

**版本比較範例**:
```cpp
compareVersions("1.2.0", "1.1.0")  // true  (1.2.0 > 1.1.0)
compareVersions("1.1.0", "1.2.0")  // false (1.1.0 < 1.2.0)
compareVersions("2.0.0", "1.9.9")  // true  (2.0.0 > 1.9.9)
compareVersions("1.0.0", "1.0.0")  // false (相同版本)
```

#### QVersionNumber 的優點
- 正確處理語義化版本（Semantic Versioning）
- 支援任意數量的版本段（如 1.2.3.4）
- 自動處理前導零和數字比較

### 5. 資訊存取

#### getLatestVersion()
```cpp
QString getLatestVersion() const
```
取得最新版本號。

#### getReleaseUrl()
```cpp
QString getReleaseUrl() const
```
取得發行版本的 GitHub 頁面 URL。

#### getReleaseNotes()
```cpp
QString getReleaseNotes() const
```
取得發行說明（Markdown 格式）。

## 信號 (Signals)

### updateCheckFinished()
```cpp
void updateCheckFinished(bool updateAvailable)
```
更新檢查完成。

**參數**:
- `updateAvailable` - `true` 表示有新版本可用

### updateCheckFailed()
```cpp
void updateCheckFailed(const QString& error)
```
更新檢查失敗。

**參數**:
- `error` - 錯誤訊息

## 使用範例

### 基本使用
```cpp
UpdateChecker* checker = new UpdateChecker(this);

// 連接信號
connect(checker, &UpdateChecker::updateCheckFinished, 
        this, [this, checker](bool updateAvailable) {
    if (updateAvailable) {
        QString message = QString(
            "發現新版本 %1！\n\n"
            "當前版本: %2\n"
            "最新版本: %3\n\n"
            "更新說明:\n%4\n\n"
            "是否要前往下載？"
        ).arg(checker->getLatestVersion())
         .arg(UpdateChecker::getCurrentVersion())
         .arg(checker->getLatestVersion())
         .arg(checker->getReleaseNotes());
        
        int result = QMessageBox::question(
            this, "發現新版本", message,
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (result == QMessageBox::Yes) {
            // 開啟瀏覽器前往下載頁面
            QDesktopServices::openUrl(QUrl(checker->getReleaseUrl()));
        }
    } else {
        // 手動檢查時顯示已是最新版本
        if (m_manualCheck) {
            QMessageBox::information(
                this, "檢查更新", 
                "您已經在使用最新版本！"
            );
        }
    }
});

connect(checker, &UpdateChecker::updateCheckFailed, 
        this, [this](const QString& error) {
    if (m_manualCheck) {
        QMessageBox::warning(
            this, "更新檢查失敗", 
            "無法檢查更新：" + error
        );
    }
});

// 執行檢查
checker->checkForUpdates();
```

### 啟動時自動檢查
```cpp
void MainWindow::onApplicationStartup() {
    // 在主視窗顯示後延遲 2 秒再檢查更新
    QTimer::singleShot(2000, this, [this]() {
        m_manualCheck = false;  // 標記為自動檢查
        m_updateChecker->checkForUpdates();
    });
}
```

### 手動檢查更新（選單項目）
```cpp
void MainWindow::onCheckForUpdatesClicked() {
    m_manualCheck = true;  // 標記為手動檢查
    m_updateChecker->checkForUpdates();
    
    // 顯示檢查中的提示
    statusBar()->showMessage("正在檢查更新...", 3000);
}
```

### 顯示更新對話框
```cpp
void MainWindow::showUpdateDialog(UpdateChecker* checker) {
    QDialog dialog(this);
    dialog.setWindowTitle("發現新版本");
    
    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    
    // 版本資訊
    QLabel* versionLabel = new QLabel(
        QString("最新版本: %1\n當前版本: %2")
            .arg(checker->getLatestVersion())
            .arg(UpdateChecker::getCurrentVersion())
    );
    layout->addWidget(versionLabel);
    
    // 更新說明
    QTextEdit* notesEdit = new QTextEdit();
    notesEdit->setReadOnly(true);
    notesEdit->setMarkdown(checker->getReleaseNotes());
    layout->addWidget(notesEdit);
    
    // 按鈕
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* downloadButton = new QPushButton("前往下載");
    QPushButton* cancelButton = new QPushButton("稍後再說");
    
    connect(downloadButton, &QPushButton::clicked, [checker, &dialog]() {
        QDesktopServices::openUrl(QUrl(checker->getReleaseUrl()));
        dialog.accept();
    });
    
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);
    
    buttonLayout->addWidget(downloadButton);
    buttonLayout->addWidget(cancelButton);
    layout->addLayout(buttonLayout);
    
    dialog.exec();
}
```

## 配置選項

### GitHub Repository 資訊
```cpp
#define GITHUB_REPO_OWNER "41343112"
#define GITHUB_REPO_NAME "Qt_Chess"
```
需要根據實際的 GitHub repository 修改。

### User-Agent 設定
```cpp
QString userAgent = QString("Qt_Chess/%1").arg(APP_VERSION);
request.setRawHeader("User-Agent", userAgent.toUtf8());
```
GitHub API 要求設定 User-Agent，建議格式: `AppName/Version`

## 語義化版本 (Semantic Versioning)

### 版本格式
```
主版本.次版本.修訂版本
MAJOR.MINOR.PATCH
```

### 版本號規則
- **MAJOR (主版本)**: 不相容的 API 變更
- **MINOR (次版本)**: 新增向下相容的功能
- **PATCH (修訂版本)**: 向下相容的問題修正

### 範例
- `1.0.0` - 首次發行
- `1.1.0` - 新增線上對戰功能
- `1.1.1` - 修正線上對戰的連線問題
- `2.0.0` - 重大架構改變，不相容舊版

## 最佳實踐

### 1. 檢查時機
```cpp
// ✅ 好：在應用程式完全載入後檢查
QTimer::singleShot(2000, this, &MainWindow::checkForUpdates);

// ❌ 不好：在建構函式中立即檢查
MainWindow::MainWindow() {
    checkForUpdates();  // 會阻塞 UI 初始化
}
```

### 2. 使用者體驗
```cpp
// 自動檢查時
if (updateAvailable) {
    // 顯示通知，但不打斷使用者
    showUpdateNotification();
} else {
    // 沒有新版本時不顯示任何訊息
}

// 手動檢查時
if (updateAvailable) {
    showUpdateDialog();
} else {
    // 顯示「已是最新版本」訊息
    QMessageBox::information(this, "檢查更新", "您已經在使用最新版本！");
}
```

### 3. 錯誤處理
```cpp
connect(checker, &UpdateChecker::updateCheckFailed, 
        this, [this](const QString& error) {
    // 自動檢查失敗時靜默處理
    if (!m_manualCheck) {
        qDebug() << "自動更新檢查失敗:" << error;
        return;
    }
    
    // 手動檢查失敗時顯示錯誤
    QMessageBox::warning(this, "更新檢查失敗", error);
});
```

### 4. 快取檢查結果
```cpp
class MainWindow {
private:
    QDateTime m_lastUpdateCheck;
    
    void checkForUpdates() {
        // 如果最近檢查過（24小時內），跳過
        if (m_lastUpdateCheck.isValid() && 
            m_lastUpdateCheck.secsTo(QDateTime::currentDateTime()) < 86400) {
            return;
        }
        
        m_updateChecker->checkForUpdates();
        m_lastUpdateCheck = QDateTime::currentDateTime();
    }
};
```

## 進階功能

### 自動下載更新（可選）
```cpp
void MainWindow::downloadUpdate(const QString& downloadUrl) {
    QNetworkAccessManager* manager = new QNetworkAccessManager(this);
    QNetworkRequest request(downloadUrl);
    QNetworkReply* reply = manager->get(request);
    
    connect(reply, &QNetworkReply::downloadProgress, 
            this, [this](qint64 received, qint64 total) {
        int percentage = (total > 0) ? (received * 100 / total) : 0;
        m_progressBar->setValue(percentage);
    });
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            // 儲存下載的檔案
            QFile file("Qt_Chess_Update.exe");
            if (file.open(QIODevice::WriteOnly)) {
                file.write(reply->readAll());
                file.close();
                
                // 提示使用者安裝
                QMessageBox::information(
                    this, "下載完成", 
                    "更新檔案已下載完成，請手動安裝。"
                );
            }
        }
        reply->deleteLater();
    });
}
```

### 版本通道（Release Channels）
```cpp
enum class ReleaseChannel {
    Stable,   // 穩定版
    Beta,     // 測試版
    Nightly   // 每日建置
};

QString getApiUrl(ReleaseChannel channel) {
    QString baseUrl = "https://api.github.com/repos/41343112/Qt_Chess/releases";
    
    switch (channel) {
        case ReleaseChannel::Stable:
            return baseUrl + "/latest";
        case ReleaseChannel::Beta:
            return baseUrl + "/tags/beta";
        case ReleaseChannel::Nightly:
            return baseUrl + "/tags/nightly";
    }
}
```

## 疑難排解

### GitHub API 速率限制
- 未認證: 60 次/小時
- 認證: 5000 次/小時

**解決方案**: 加入 GitHub Personal Access Token
```cpp
request.setRawHeader("Authorization", "token YOUR_TOKEN");
```

### SSL/TLS 錯誤
```cpp
// 忽略 SSL 錯誤（僅用於開發測試）
connect(reply, QOverload<const QList<QSslError>&>::of(&QNetworkReply::sslErrors),
        [reply](const QList<QSslError>& errors) {
    reply->ignoreSslErrors();
});
```

### 代理伺服器設定
```cpp
QNetworkProxy proxy;
proxy.setType(QNetworkProxy::HttpProxy);
proxy.setHostName("proxy.example.com");
proxy.setPort(8080);
QNetworkProxy::setApplicationProxy(proxy);
```

## 依賴項目
- **Qt Network**: `QNetworkAccessManager`, `QNetworkReply`, `QNetworkRequest`
- **Qt Core**: `QJsonDocument`, `QJsonObject`, `QVersionNumber`

## 相關類別
- `Qt_Chess` - 在主視窗中整合更新檢查功能

## 參考資源
- [GitHub API 文檔](https://docs.github.com/en/rest/releases/releases)
- [語義化版本規範](https://semver.org/lang/zh-TW/)
- [Qt Network 文檔](https://doc.qt.io/qt-5/qtnetwork-index.html)
