# 中繼伺服器實現說明

## 概述

本專案實現了使用中繼伺服器（Relay Server）達成跨網域線上對戰的功能。玩家只需要輸入4位數字的房號即可連線，無需知道對方的IP地址，也不需要設定路由器的端口轉發。

## 功能特點

### 使用者體驗改進

**之前（直接連接模式）：**
- 需要知道對方的IP地址
- 需要輸入房號
- 只能在同一網域（同一WiFi）或需要設定端口轉發
- 格式：`192.168.1.100:1234`

**現在（中繼伺服器模式）：**
- ✅ 只需要4位數字房號
- ✅ 跨網域連線（不同WiFi、不同地點）
- ✅ 無需設定路由器
- ✅ 更簡單易用
- 格式：`1234`

## 技術架構

### 元件說明

1. **RelayClient** (`src/relayserver.h/cpp`)
   - 處理與中繼伺服器的通訊
   - 管理房間創建和加入
   - 轉發遊戲訊息
   - 實現心跳保持連接

2. **NetworkManager** (`src/networkmanager.h/cpp`)
   - 整合中繼模式和直接連接模式
   - 提供統一的介面給上層應用
   - 自動選擇合適的通訊方式

3. **OnlineDialog** (`src/onlinedialog.h/cpp`)
   - 提供使用者介面
   - 支援切換中繼/直接模式
   - 簡化房號輸入

4. **Relay Server** (`relay_server.py`)
   - Python實現的中繼伺服器
   - 處理房間管理
   - 轉發遊戲訊息
   - 支援多個並發連線

### 通訊協議

使用JSON格式的訊息，透過TCP連接傳輸：

```json
// 創建房間
{
  "type": "create_room"
}

// 加入房間
{
  "type": "join_room",
  "room_code": "1234"
}

// 遊戲訊息
{
  "type": "game_message",
  "room_code": "1234",
  "data": {
    "type": "Move",
    "fromRow": 6,
    "fromCol": 4,
    "toRow": 4,
    "toCol": 4
  }
}
```

## 使用方式

### 1. 啟動中繼伺服器

```bash
cd Qt_Chess
python3 relay_server.py
```

預設監聽 `0.0.0.0:8080`

### 2. 配置應用程式

在 `src/networkmanager.cpp` 中設定中繼伺服器地址：

```cpp
static const QString DEFAULT_RELAY_SERVER = "localhost";  // 或您的伺服器IP
static const int DEFAULT_RELAY_PORT = 8080;
```

### 3. 編譯應用程式

```bash
qmake Qt_Chess.pro
make
```

### 4. 使用方式

**房主：**
1. 開啟應用程式
2. 點擊「線上」
3. 確保「使用中繼伺服器」已勾選
4. 選擇「我要創建房間」
5. 取得4位數字房號並傳給朋友

**加入者：**
1. 開啟應用程式
2. 點擊「線上」
3. 確保「使用中繼伺服器」已勾選
4. 選擇「我有房號」
5. 輸入4位數字房號

## 檔案清單

### 新增檔案

- `src/relayserver.h` - RelayClient 類別定義
- `src/relayserver.cpp` - RelayClient 實現
- `relay_server.py` - Python 中繼伺服器
- `docs/RELAY_SERVER_SETUP.md` - 中繼伺服器設定指南

### 修改檔案

- `src/networkmanager.h` - 新增中繼模式支援
- `src/networkmanager.cpp` - 實現中繼連接邏輯
- `src/onlinedialog.h` - 新增中繼模式UI
- `src/onlinedialog.cpp` - 實現模式切換
- `src/qt_chess.cpp` - 整合中繼模式到主應用
- `Qt_Chess.pro` - 加入新檔案到編譯
- `README.md` - 更新功能說明
- `docs/ONLINE_BEGINNER_GUIDE.md` - 更新使用指南

## 部署選項

### 本地測試

適合開發和小規模測試：

```bash
python3 relay_server.py
```

### 生產環境

建議部署到雲端服務：

1. **免費選項**：
   - Heroku Free Tier
   - AWS Free Tier (t2.micro)
   - Google Cloud Free Tier (f1-micro)

2. **付費選項**：
   - DigitalOcean Droplet ($4/月起)
   - Linode ($5/月起)
   - Vultr ($2.5/月起)

詳見 [RELAY_SERVER_SETUP.md](docs/RELAY_SERVER_SETUP.md)

## 未來改進

### 短期

- [ ] 加入TLS/SSL加密
- [ ] 實現房間密碼保護
- [ ] 加入使用者認證
- [ ] 實現速率限制

### 中期

- [ ] WebRTC ICE協商支援
- [ ] P2P降級（降低延遲）
- [ ] 房間瀏覽功能
- [ ] 管理API

### 長期

- [ ] 使用Redis橫向擴展
- [ ] 負載平衡器支援
- [ ] 統計和監控面板
- [ ] 多區域部署

## 安全性

### 當前限制

⚠️ 當前實現是基本版本，有以下安全限制：

1. **無加密**：遊戲訊息未加密傳輸
2. **無認證**：沒有使用者認證機制
3. **無速率限制**：可能受到濫用

### 建議

- 僅在信任的環境中使用
- 不要傳輸敏感資訊
- 如果公開部署，請考慮：
  - 加入TLS加密
  - 實現速率限制
  - 加入IP黑名單
  - 使用者認證系統

## 效能考量

### 延遲

- 本地網路：< 1ms
- 中繼伺服器（同城）：5-20ms
- 中繼伺服器（跨國）：50-200ms

### 容量

單一Python伺服器可以處理：
- ~100-200 並發連線
- ~50-100 活躍房間

如需更高容量，請參考橫向擴展方案。

## 故障排除

### 無法連接到中繼伺服器

1. 檢查伺服器是否運行：`ps aux | grep relay_server`
2. 檢查端口是否開放：`netstat -tuln | grep 8080`
3. 檢查防火牆設定
4. 驗證網路連通性：`telnet <server-ip> 8080`

### 房間已滿或不存在

1. 確認房號正確（4位數字）
2. 確認房主還在線上
3. 重啟中繼伺服器清除舊房間

### 連線後無法傳送棋步

1. 檢查雙方網路連線
2. 查看伺服器日誌
3. 嘗試重新連線

## 授權

此功能作為 Qt_Chess 專案的一部分，遵循相同的授權條款。

## 貢獻

歡迎貢獻改進！可能的改進方向：

- TLS/SSL支援
- WebRTC實現
- 使用者認證系統
- 效能優化
- 更好的錯誤處理
- 管理介面

## 聯絡

如有問題或建議，請開啟GitHub Issue。
