# 快速測試指南

## 本地測試中繼伺服器功能

本指南幫助您快速測試中繼伺服器功能。

### 前置條件

- Python 3.6+
- Qt5 開發環境
- 兩個終端視窗（用於測試）

### 步驟1：啟動中繼伺服器

在第一個終端中：

```bash
cd Qt_Chess
python3 relay_server.py
```

您應該會看到：
```
[2025-12-05 12:00:00] Relay server started on 0.0.0.0:8080
```

### 步驟2：編譯應用程式

在第二個終端中：

```bash
cd Qt_Chess
qmake Qt_Chess.pro
make
```

### 步驟3：配置伺服器地址（如果需要）

如果中繼伺服器不在本機，需要修改 `src/networkmanager.cpp`：

```cpp
// 修改這兩行
static const QString DEFAULT_RELAY_SERVER = "localhost";  // 改成您的伺服器IP
static const int DEFAULT_RELAY_PORT = 8080;
```

然後重新編譯：
```bash
make
```

### 步驟4：測試創建房間

1. 啟動第一個應用程式實例：
   ```bash
   ./Qt_Chess
   ```

2. 點擊「線上」按鈕

3. 確保「使用中繼伺服器」已勾選

4. 選擇「我要創建房間」

5. 點擊「開始」

6. 記下房號（例如：1234）

7. 中繼伺服器日誌應該顯示：
   ```
   [2025-12-05 12:01:00] New client connected: client_127.0.0.1_xxxxx
   [2025-12-05 12:01:00] Received create_room from client_127.0.0.1_xxxxx
   [2025-12-05 12:01:00] Room 1234 created by client_127.0.0.1_xxxxx
   ```

### 步驟5：測試加入房間

1. 啟動第二個應用程式實例（在另一個終端）：
   ```bash
   ./Qt_Chess
   ```

2. 點擊「線上」按鈕

3. 確保「使用中繼伺服器」已勾選

4. 選擇「我有房號」

5. 輸入步驟4中記下的房號（例如：1234）

6. 點擊「開始」

7. 中繼伺服器日誌應該顯示：
   ```
   [2025-12-05 12:02:00] New client connected: client_127.0.0.1_yyyyy
   [2025-12-05 12:02:00] Received join_room from client_127.0.0.1_yyyyy
   [2025-12-05 12:02:00] Client client_127.0.0.1_yyyyy joined room 1234
   ```

8. 兩個應用程式都應該顯示「✅ 連線成功！遊戲開始」

### 步驟6：測試遊戲進行

1. 在房主（白棋）視窗中，移動一個棋子

2. 加入者（黑棋）視窗應該立即顯示相同的移動

3. 中繼伺服器日誌應該顯示遊戲訊息轉發：
   ```
   [2025-12-05 12:03:00] Received game_message from client_127.0.0.1_xxxxx
   ```

4. 在加入者視窗中移動一個棋子

5. 房主視窗應該立即顯示相同的移動

### 預期結果

✅ **成功的標誌：**
- 中繼伺服器正常運行並輸出日誌
- 可以成功創建房間並取得房號
- 可以使用房號成功加入房間
- 雙方可以看到對方的棋步
- 遊戲流程正常進行

❌ **如果出現問題：**

**問題1：無法連接到中繼伺服器**
- 檢查伺服器是否運行
- 檢查端口8080是否被其他程式佔用
- 檢查防火牆設定

**問題2：房間已滿或不存在**
- 確認房號輸入正確
- 確認房主還在線上
- 重啟中繼伺服器

**問題3：看不到對方的棋步**
- 檢查網路連線
- 查看伺服器日誌是否有錯誤
- 重新連線

### 進階測試

#### 測試1：跨網路連線

1. 在一台電腦上運行中繼伺服器
2. 獲取該電腦的IP地址（例如：192.168.1.100）
3. 在另一台電腦上修改應用程式配置：
   ```cpp
   static const QString DEFAULT_RELAY_SERVER = "192.168.1.100";
   ```
4. 重新編譯並測試

#### 測試2：斷線重連

1. 建立連線後，關閉加入者應用程式
2. 房主應該看到「對手已斷線」訊息
3. 重新啟動加入者應用程式
4. 使用相同房號重新加入

#### 測試3：多房間並發

1. 啟動多個應用程式實例
2. 創建多個不同的房間
3. 確認每個房間獨立運作

### 清理

測試完成後：

1. 關閉所有應用程式實例
2. 按 Ctrl+C 停止中繼伺服器
3. 檢查是否有殘留的進程：
   ```bash
   ps aux | grep Qt_Chess
   ps aux | grep relay_server
   ```

### 日誌分析

中繼伺服器會輸出詳細的日誌，包括：

- 客戶端連接/斷開
- 房間創建/加入/離開
- 遊戲訊息轉發
- 錯誤訊息

範例日誌：
```
[2025-12-05 12:00:00] Relay server started on 0.0.0.0:8080
[2025-12-05 12:01:00] New client connected: client_127.0.0.1_12345 from ('127.0.0.1', 12345)
[2025-12-05 12:01:00] Received hello from client_127.0.0.1_12345
[2025-12-05 12:01:00] Received create_room from client_127.0.0.1_12345
[2025-12-05 12:01:00] Room 1234 created by client_127.0.0.1_12345
[2025-12-05 12:02:00] New client connected: client_127.0.0.1_67890 from ('127.0.0.1', 67890)
[2025-12-05 12:02:00] Received hello from client_127.0.0.1_67890
[2025-12-05 12:02:00] Received join_room from client_127.0.0.1_67890
[2025-12-05 12:02:00] Client client_127.0.0.1_67890 joined room 1234
```

### 效能監控

監控中繼伺服器效能：

```bash
# CPU和記憶體使用
top -p $(pgrep -f relay_server.py)

# 網路連線
ss -t | grep 8080

# 活躍連線數
ss -t | grep 8080 | wc -l
```

### 下一步

測試成功後：

1. 考慮部署到雲端伺服器（參見 [RELAY_SERVER_SETUP.md](docs/RELAY_SERVER_SETUP.md)）
2. 實現TLS加密（生產環境必需）
3. 加入更多功能（認證、速率限制等）

### 需要幫助？

如有問題：
1. 檢查中繼伺服器日誌
2. 檢查應用程式是否有錯誤訊息
3. 參閱 [RELAY_SERVER_SETUP.md](docs/RELAY_SERVER_SETUP.md)
4. 在GitHub開啟Issue
