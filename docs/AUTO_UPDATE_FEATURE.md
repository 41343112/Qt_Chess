# 自動更新功能說明

## 功能概述

Qt_Chess 現在支援從 GitHub Releases 自動檢查和下載最新版本的功能。

## 使用方法

1. 開啟應用程式
2. 點擊選單列的「幫助」→「檢查更新」
3. 應用程式會自動連接到 GitHub 並檢查是否有新版本
4. 如果有新版本：
   - 會顯示當前版本和最新版本
   - 顯示發行說明（Release Notes）
   - 提供「下載更新」按鈕
5. 點擊「下載更新」開始下載
   - 顯示下載進度條
   - 顯示已下載和總檔案大小
6. 下載完成後：
   - 顯示檔案儲存位置
   - 提供「開啟檔案位置」按鈕，方便用戶找到下載的檔案

## 技術細節

### UpdateManager 類別

負責處理版本檢查和檔案下載：

- `checkForUpdates()` - 檢查 GitHub Releases API 獲取最新版本
- `downloadUpdate()` - 下載更新檔案到下載資料夾
- 自動選擇適合當前平台的檔案（Windows、Linux、macOS）
- 使用 QVersionNumber 進行強健的語義化版本比較（Semantic Versioning）
- 支援預發布版本（例如：1.0.0-beta.1）
- 版本號正規化處理（自動移除 'v' 前綴）
- 安全的錯誤處理（無效版本號時不會誤報更新）

### UpdateDialog 類別

提供用戶界面：

- 顯示檢查更新狀態
- 顯示版本資訊和發行說明
- 顯示下載進度
- 處理錯誤訊息

## 版本號設定

當前版本號在 `qt_chess.cpp` 的常數中設定：

```cpp
const QString APP_VERSION = "1.0.0";           // 應用程式版本
```

發布新版本時，請記得更新此常數。版本號會在初始化時設定到 UpdateManager：

```cpp
m_updateManager->setCurrentVersion(APP_VERSION);
```

## GitHub 倉庫設定

GitHub 倉庫資訊也在常數中定義，便於維護：

```cpp
const QString GITHUB_REPO_OWNER = "41343112";  // GitHub 倉庫擁有者
const QString GITHUB_REPO_NAME = "Qt_Chess";   // GitHub 倉庫名稱
```

如果倉庫擁有者或名稱變更，只需修改這些常數即可。

## GitHub Release 要求

為了讓自動更新功能正常運作，GitHub Release 應該：

1. 使用標籤格式：`v1.0.0` 或 `1.0.0`
2. 包含平台特定的檔案名稱：
   - Windows: 包含 "windows"、".exe" 或 ".zip"
   - Linux: 包含 "linux"、".appimage" 或 ".tar.gz"
   - macOS: 包含 "mac"、"darwin" 或 ".dmg"
3. 提供詳細的發行說明（Release Notes）

## 安全性

- 僅從官方 GitHub 倉庫下載檔案
- 使用 HTTPS 連接確保傳輸安全
- 檔案下載到系統預設的下載資料夾
- 不會自動安裝更新，需要用戶手動操作

## 錯誤處理

應用程式會處理以下錯誤情況：

- 網路連接失敗
- GitHub API 回應錯誤
- 下載失敗
- 檔案儲存失敗

所有錯誤都會以友善的訊息顯示給用戶。
