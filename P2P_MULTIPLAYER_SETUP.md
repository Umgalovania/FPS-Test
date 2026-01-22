# P2P 联机对战功能实现指南

## 已完成的代码修改

### 1. 网络会话管理
- ✅ 创建了 `NetworkSessionManager` 类用于管理网络会话
- ✅ 支持创建会话（Host Game）
- ✅ 支持搜索和加入会话（Join Game）
- ✅ 使用 LAN 模式进行 P2P 连接

### 2. GameInstance 集成
- ✅ 在 `FPS251106GameInstance` 中集成了 `NetworkSessionManager`
- ✅ 添加了 `HostGame()` 方法

### 3. 主菜单更新
- ✅ 在 `MainMenuUI` 中添加了 `HostGameButton` 和 `JoinGameButton`
- ✅ 实现了创建游戏和加入游戏的逻辑

### 4. 网络复制
- ✅ Character、Weapon、Projectile、NPC 都已启用网络复制
- ✅ 关键数据（血量、队伍ID）已正确标记为 Replicated

## 需要在 UE 编辑器中完成的配置

### 步骤 1：配置 Online Subsystem

1. 打开 **编辑 → 项目设置（Edit → Project Settings）**
2. 在左侧找到 **引擎（Engine）→ 在线子系统（Online Subsystem）**
3. 在 **默认在线子系统（Default Online Subsystem）** 中：
   - 选择 **Null**（用于局域网 P2P，无需外部服务）
   - 或者选择 **Steam**（如果已安装 Steam Online Subsystem 插件）

### 步骤 2：配置网络设置

1. 在项目设置中找到 **网络（Networking）**
2. 确保以下设置：
   - **启用网络（Enable Networking）**：✅ 已启用
   - **使用 Ipv4 Sockets**：✅ 已启用
   - **网络模式（Network Mode）**：设置为 **Listen Server**（监听服务器模式）

### 步骤 3：更新主菜单 UI 蓝图

1. 打开 `WBP_MainMenu` 蓝图（或你的主菜单 UI 蓝图）
2. 在 **Designer** 标签中：
   - 添加两个新按钮：
     - `HostGameButton`（创建游戏）
     - `JoinGameButton`（加入游戏）
3. 在 **Graph** 标签中：
   - 确保按钮变量名与 C++ 代码中的 `meta = (BindWidget)` 匹配：
     - `HostGameButton`
     - `JoinGameButton`
4. 编译并保存蓝图

### 步骤 4：配置 GameMode 蓝图

1. 打开你的 GameMode 蓝图（例如 `BP_ShooterGameMode` 或 `BP_MultiplayerGameMode`）
2. 在 **Details** 面板中：
   - **Net Mode**：设置为 **Listen Server**
   - 确保 **Player Controller Class** 设置为你的 PlayerController 蓝图
   - 确保 **Default Pawn Class** 设置为你的 Character 蓝图

### 步骤 5：确保网络复制设置

检查以下蓝图类的网络设置：

#### BP_ShooterCharacter
- **Replicates**：✅ 已启用
- **Replicate Movement**：✅ 已启用

#### BP_ShooterWeapon
- **Replicates**：✅ 已启用
- **Replicate Movement**：✅ 已启用

#### BP_ShooterProjectile
- **Replicates**：✅ 已启用
- **Replicate Movement**：✅ 已启用

#### BP_ShooterNPC
- **Replicates**：✅ 已启用
- **Replicate Movement**：✅ 已启用

### 步骤 6：测试网络功能

#### 测试创建游戏（Host）
1. 运行游戏
2. 在主菜单点击 **"创建游戏"** 按钮
3. 应该会创建一个网络会话并加载游戏关卡
4. 在游戏中，你应该看到自己是服务器（Server）

#### 测试加入游戏（Join）
1. 在另一台电脑或同一台电脑的另一个实例运行游戏
2. 在主菜单点击 **"加入游戏"** 按钮
3. 系统会搜索可用的会话
4. 如果找到会话，会自动加入并加载游戏关卡
5. 在游戏中，你应该能看到其他玩家

## 网络架构说明

### 服务器-客户端模式
- **Listen Server**：第一个玩家作为服务器，其他玩家作为客户端连接
- **优势**：无需专用服务器，适合 P2P 对战
- **限制**：服务器玩家退出会导致所有玩家断开

### 数据复制
- **服务器权威**：所有游戏逻辑在服务器上执行
- **客户端预测**：移动和输入在客户端预测，服务器验证
- **RPC**：使用 RPC（Remote Procedure Call）进行客户端-服务器通信

## 常见问题排查

### 问题 1：无法创建会话
**解决方案**：
- 检查 Online Subsystem 配置
- 确保防火墙允许游戏网络访问
- 查看 Output Log 中的错误信息

### 问题 2：无法找到会话
**解决方案**：
- 确保两台电脑在同一局域网
- 检查网络连接
- 确保防火墙设置允许 UDP 通信

### 问题 3：玩家看不到其他玩家
**解决方案**：
- 检查 Character 的 Replicates 设置
- 确保 PlayerController 和 Pawn 正确设置
- 检查网络角色（Role）是否正确

### 问题 4：延迟或卡顿
**解决方案**：
- 检查网络连接质量
- 优化网络复制频率
- 考虑使用客户端预测

## 下一步优化建议

1. **添加玩家名称显示**：在角色上方显示玩家名称
2. **添加网络延迟显示**：显示 ping 值
3. **添加断线重连**：处理网络断开情况
4. **优化网络复制**：只复制必要的数据
5. **添加反作弊**：服务器验证关键操作

## 代码文件清单

### 新增文件
- `Source/FPS251106/Menu/NetworkSessionManager.h`
- `Source/FPS251106/Menu/NetworkSessionManager.cpp`

### 修改文件
- `Source/FPS251106/FPS251106GameInstance.h`
- `Source/FPS251106/FPS251106GameInstance.cpp`
- `Source/FPS251106/Menu/MainMenuUI.h`
- `Source/FPS251106/Menu/MainMenuUI.cpp`
- `Source/FPS251106/FPS251106.Build.cs`

## 注意事项

1. **局域网限制**：当前实现使用 LAN 模式，只能在同一局域网内连接
2. **玩家数量**：默认支持 2 人，可以在 `HostGame()` 中修改 `MaxPlayers` 参数
3. **Steam 集成**：如需通过互联网连接，需要集成 Steam Online Subsystem
4. **安全性**：当前实现没有加密，仅适合本地测试

