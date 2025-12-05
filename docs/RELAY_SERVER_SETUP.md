# 中繼伺服器設定指南

## 概述

中繼伺服器（Relay Server）是實現跨網域線上對戰的關鍵元件。使用中繼伺服器後，玩家只需要輸入4位數字的房號即可連線，無需知道對方的IP地址，也不需要設定端口轉發。

## 為什麼需要中繼伺服器？

### 傳統直接連線的問題

在傳統的P2P連線模式下：
- 需要知道對方的公網IP地址
- 需要在路由器上設定端口轉發
- 防火牆可能會阻擋連線
- NAT（網路位址轉換）會造成連線困難
- 只能在同一區域網路中輕鬆連線

### 中繼伺服器的優勢

使用中繼伺服器後：
- ✅ **只需房號**：不需要知道IP地址
- ✅ **跨網域連線**：可以在不同WiFi、不同地點連線
- ✅ **無需設定**：不需要設定路由器端口轉發
- ✅ **穿透NAT**：自動處理NAT問題
- ✅ **更簡單**：使用體驗更友善

## 運作原理

```
玩家A (任何地點)              中繼伺服器               玩家B (任何地點)
    |                            |                           |
    |--- 創建房間 -------------->|                           |
    |<-- 房號: 1234 -------------|                           |
    |                            |<--- 加入房間: 1234 -------|
    |<-- 對手已加入 -------------|--- 加入成功 ------------>|
    |                            |                           |
    |--- 棋步訊息 -------------->|--- 轉發棋步 ------------>|
    |<-- 轉發棋步 ---------------|<-- 棋步訊息 -------------|
```

## 部署中繼伺服器

### 選項一：使用提供的Python伺服器（推薦用於測試）

專案包含一個簡單的Python中繼伺服器，適合本地測試或小規模使用。

#### 系統需求

- Python 3.6 或更高版本
- 無需額外依賴套件（僅使用標準函式庫）

#### 安裝步驟

1. 確認Python已安裝：
```bash
python3 --version
```

2. 進入專案目錄：
```bash
cd Qt_Chess
```

3. 運行伺服器：
```bash
python3 relay_server.py
```

預設情況下，伺服器會在 `0.0.0.0:8080` 上監聽。

#### 自訂配置

可以指定主機和端口：

```bash
# 指定端口
python3 relay_server.py --port 9000

# 指定主機和端口
python3 relay_server.py --host 192.168.1.100 --port 9000

# 查看說明
python3 relay_server.py --help
```

#### 在背景執行

如果要在背景持續運行：

```bash
# Linux/Mac
nohup python3 relay_server.py > relay_server.log 2>&1 &

# 或使用 screen
screen -S relay_server
python3 relay_server.py
# 按 Ctrl+A, D 離開 screen
```

### 選項二：使用雲端服務（推薦用於生產環境）

對於公開使用，建議將中繼伺服器部署到雲端：

#### 免費雲端選項

1. **Heroku**
   - 免費方案可用
   - 自動SSL/HTTPS
   - 簡單部署

2. **AWS Free Tier**
   - 12個月免費
   - t2.micro實例
   - 需要基本AWS知識

3. **Google Cloud Free Tier**
   - f1-micro實例永久免費
   - 需要信用卡驗證

4. **DigitalOcean**
   - 最便宜 $4/月
   - 簡單易用

#### 雲端部署步驟（以Ubuntu為例）

1. 創建雲端實例（Ubuntu 20.04+）

2. SSH連接到伺服器：
```bash
ssh user@your-server-ip
```

3. 安裝Python（如果尚未安裝）：
```bash
sudo apt update
sudo apt install python3 python3-pip
```

4. 上傳relay_server.py到伺服器：
```bash
# 在本地電腦執行
scp relay_server.py user@your-server-ip:~/
```

5. 在伺服器上運行：
```bash
python3 relay_server.py --host 0.0.0.0 --port 8080
```

6. 設定為系統服務（持續運行）：

創建服務檔案 `/etc/systemd/system/chess-relay.service`：
```ini
[Unit]
Description=Qt Chess Relay Server
After=network.target

[Service]
Type=simple
User=ubuntu
WorkingDirectory=/home/ubuntu
ExecStart=/usr/bin/python3 /home/ubuntu/relay_server.py --host 0.0.0.0 --port 8080
Restart=always

[Install]
WantedBy=multi-user.target
```

啟用並啟動服務：
```bash
sudo systemctl daemon-reload
sudo systemctl enable chess-relay
sudo systemctl start chess-relay
sudo systemctl status chess-relay
```

7. 開啟防火牆端口：
```bash
sudo ufw allow 8080/tcp
```

## 配置應用程式使用中繼伺服器

### 設定伺服器地址

中繼伺服器地址需要在應用程式中配置。預設值在 `src/networkmanager.cpp` 中：

```cpp
static const QString DEFAULT_RELAY_SERVER = "relay.qtchess.example.com";
static const int DEFAULT_RELAY_PORT = 8080;
```

如果您部署了自己的中繼伺服器，需要修改這些值：

```cpp
// 改為您的伺服器地址
static const QString DEFAULT_RELAY_SERVER = "your-server.com";
static const int DEFAULT_RELAY_PORT = 8080;
```

然後重新編譯應用程式。

### 動態配置（未來改進）

未來版本可以加入：
- 設定對話框讓使用者輸入伺服器地址
- 配置檔案 (config.ini)
- 多個伺服器選項

## 使用中繼伺服器連線

### 創建房間

1. 點擊「線上」按鈕
2. 確保「使用中繼伺服器」已勾選
3. 選擇「我要創建房間」
4. 點擊「開始」
5. 系統會顯示一個4位數字的房號（例如：1234）
6. 將房號傳給朋友

### 加入房間

1. 點擊「線上」按鈕
2. 確保「使用中繼伺服器」已勾選
3. 選擇「我有房號」
4. 輸入朋友給的4位數字房號
5. 點擊「開始」

## 安全性考慮

### 當前版本的限制

⚠️ **重要**：當前實現是基本版本，有以下限制：

1. **無加密**：遊戲訊息未加密
2. **無認證**：沒有使用者認證機制
3. **無速率限制**：可能受到濫用

### 建議

- 僅在信任的網路環境中使用
- 不要傳輸敏感資訊
- 如果公開部署，考慮加入：
  - TLS/SSL加密
  - 速率限制
  - IP黑名單
  - 使用者認證

### 未來改進

計劃在未來版本中加入：
- TLS加密通訊
- 使用者認證系統
- 房間密碼保護
- WebRTC P2P降級（提升效能）

## 監控和維護

### 查看日誌

Python伺服器會輸出日誌到標準輸出：

```bash
# 如果使用systemd
sudo journalctl -u chess-relay -f

# 如果使用nohup
tail -f relay_server.log
```

### 效能監控

監控伺服器資源使用：

```bash
# CPU和記憶體
top

# 網路連線
netstat -an | grep 8080

# 活躍連線數
ss -s
```

### 故障排除

#### 問題：無法連接到中繼伺服器

解決方法：
1. 檢查伺服器是否運行：`ps aux | grep relay_server`
2. 檢查端口是否開啟：`netstat -tuln | grep 8080`
3. 檢查防火牆設定
4. 驗證網路連通性：`telnet your-server-ip 8080`

#### 問題：房間已滿或不存在

解決方法：
1. 確認房號正確
2. 確認房主還在線上
3. 重啟中繼伺服器清除舊房間

#### 問題：連線後無法傳送棋步

解決方法：
1. 檢查網路連線
2. 查看伺服器日誌
3. 重新連線

## 擴展性

### 單伺服器限制

單一Python伺服器可以處理：
- 約100-200個並發連線
- 50-100個活躍房間

### 擴展方案

如需支援更多使用者：

1. **垂直擴展**：升級伺服器硬體
2. **水平擴展**：
   - 使用負載平衡器
   - 運行多個伺服器實例
   - 使用Redis共享房間狀態
3. **使用專業解決方案**：
   - Socket.IO伺服器
   - WebSocket雲端服務

## 貢獻

歡迎改進中繼伺服器實現！可能的改進方向：

- 加入TLS/SSL支援
- 實現WebRTC ICE協商
- 加入使用者認證
- 改善錯誤處理
- 加入管理API
- 提供統計數據

## 授權

中繼伺服器代碼與Qt_Chess專案使用相同授權。
