# 联机对战 UI 配置指南

## 已完成的代码修改

### 1. 创建了新的联机对战 UI 类
- ✅ 创建了 `MultiplayerMenuUI` 类
- ✅ 包含 `HostGameButton`（创建游戏）
- ✅ 包含 `JoinGameButton`（加入游戏）
- ✅ 包含 `BackButton`（返回主菜单）

### 2. 更新了主菜单 UI
- ✅ 移除了 `HostGameButton` 和 `JoinGameButton`
- ✅ 添加了 `MultiplayerButton`（联机对战按钮）
- ✅ 添加了打开联机对战界面的逻辑

## 在 UE 编辑器中需要完成的配置

### 步骤 1：创建联机对战 UI 蓝图

1. **创建新的 Widget 蓝图**
   - 在内容浏览器中，右键点击 `Content/Menu` 文件夹（如果没有则创建）
   - 选择 **用户界面 → Widget 蓝图**（User Interface → Widget Blueprint）
   - 命名为 `WBP_MultiplayerMenu`

2. **打开 `WBP_MultiplayerMenu` 蓝图**
   - 双击打开蓝图编辑器

3. **在 Designer 标签中设计界面**
   - 添加一个 **Canvas Panel** 作为根容器（如果还没有）
   - 添加以下按钮：
     - `HostGameButton`（创建游戏）
     - `JoinGameButton`（加入游戏）
     - `BackButton`（返回）
   - 调整按钮位置和样式

4. **设置按钮变量名**
   - 选中每个按钮，在 **Details** 面板中：
     - 确保 **Is Variable** 已勾选
     - 变量名必须与 C++ 代码中的 `meta = (BindWidget)` 匹配：
       - `HostGameButton`
       - `JoinGameButton`
       - `BackButton`

5. **编译并保存蓝图**

### 步骤 2：更新主菜单 UI 蓝图

1. **打开主菜单 UI 蓝图**（例如 `WBP_MainMenu`）

2. **在 Designer 标签中**
   - 移除 `HostGameButton` 和 `JoinGameButton`（如果存在）
   - 添加一个新按钮：`MultiplayerButton`（联机对战）
   - 调整按钮位置和样式

3. **设置按钮变量名**
   - 选中 `MultiplayerButton`，在 **Details** 面板中：
     - 确保 **Is Variable** 已勾选
     - 变量名必须为：`MultiplayerButton`

4. **在 Graph 标签中**
   - 找到 **Class Defaults**（类默认值）
   - 在 **Details** 面板中找到 **Multiplayer Menu UI Class**
   - 设置为 `WBP_MultiplayerMenu`（你刚创建的联机对战 UI 蓝图）

5. **编译并保存蓝图**

### 步骤 3：测试界面

1. **运行游戏**
   - 在主菜单应该看到：
     - "开始游戏" 按钮
     - "联机对战" 按钮
     - "退出游戏" 按钮

2. **点击"联机对战"按钮**
   - 应该打开联机对战界面
   - 界面中应该显示：
     - "创建游戏" 按钮
     - "加入游戏" 按钮
     - "返回" 按钮

3. **点击"返回"按钮**
   - 应该返回到主菜单

## UI 结构说明

### 主菜单（MainMenuUI）
```
WBP_MainMenu
├── StartGameButton（开始游戏）
├── MultiplayerButton（联机对战） ← 新增
└── QuitGameButton（退出游戏）
```

### 联机对战菜单（MultiplayerMenuUI）
```
WBP_MultiplayerMenu
├── HostGameButton（创建游戏）
├── JoinGameButton（加入游戏）
└── BackButton（返回）
```

## 代码文件清单

### 新增文件
- `Source/FPS251106/Menu/MultiplayerMenuUI.h`
- `Source/FPS251106/Menu/MultiplayerMenuUI.cpp`

### 修改文件
- `Source/FPS251106/Menu/MainMenuUI.h`
- `Source/FPS251106/Menu/MainMenuUI.cpp`

## 注意事项

1. **变量名必须匹配**：按钮的变量名必须与 C++ 代码中的 `meta = (BindWidget)` 完全匹配
2. **蓝图类设置**：确保在主菜单蓝图的 **Class Defaults** 中设置了 `MultiplayerMenuUIClass`
3. **UI 层级**：联机对战 UI 会覆盖在主菜单 UI 之上，返回时会隐藏联机对战 UI 并显示主菜单

## 常见问题

### 问题 1：点击"联机对战"按钮没有反应
**解决方案**：
- 检查 `MultiplayerButton` 变量名是否正确
- 检查 `MultiplayerMenuUIClass` 是否已设置
- 查看 Output Log 中的错误信息

### 问题 2：联机对战界面显示但按钮无法点击
**解决方案**：
- 检查按钮变量名是否与 C++ 代码匹配
- 确保按钮的 **Is Variable** 已勾选
- 检查按钮是否被其他 UI 元素遮挡

### 问题 3：返回按钮无法返回主菜单
**解决方案**：
- 检查主菜单 UI 的可见性设置
- 确保主菜单 UI 没有被销毁

## 下一步

完成 UI 配置后，可以：
1. 测试创建游戏功能
2. 测试加入游戏功能
3. 美化 UI 界面（添加背景、图标等）
4. 添加加载提示和错误提示

