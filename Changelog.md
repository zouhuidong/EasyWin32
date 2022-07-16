# 更新日志

**Ver3.1.1** (2022.07.16)

1. 加入 `MouseDrag` 鼠标拖动消息处理类，可以更方便地处理鼠标拖动
2. `GetImageHBitmap` 改为 `EasyWin32::Image2Bitmap`
3. `HICONFromHBitmap` 改为 `EasyWin32::Bitmap2Icon`
4. `outtextxy_format` 改为 `EasyWin32::outtextxy_format`

---

**Ver3.1.0** (2022.07.15)

1. 消息处理机制改为单队列
2. 新增 `GetMessageType` 函数
3. 删除 `EM_ALL` 宏

---

**Ver3.0.0** (2022.07.15)

1. 内部结构深度优化，修复新建窗口时可能崩溃的 bug
2. 重写消息处理模块，支持所有 EasyX 消息（`EM_MOUSE`, `EM_KEY`, `EM_CHAR`, `EM_WINDOW`），完全兼容 `MOUSEMSG` 系列函数
3. 新增 `EM_ALL` 宏，代指所有消息
4. 修改 `EasyWindow` 结构，增加窗口存活标志（`isAlive`），删除 `isSentCreateMsg`
5. `DeleteTray` 参数从 `EasyWindow*` 改为 `HWND`
6. 删除 `WaitForProcessing` 函数
7. 删除 `GetCreatedWindowList` 函数
8. 新增 `outtextxy_format` 函数
9. 新增 `DARKYELLOW`，`PINKWHITE` 颜色宏
10. `initgraph` 宏支持可变参数
11. `WM_DESTROY` 消息的处理移动到自定义窗口过程函数后

---

**Ver2.6.3** (2022.07.12)

1. 可以在创建窗口前设置窗口样式，窗口位置（详见 `PreSetWindowStyle` 函数和 `PreSetWindowPos` 函数）
2. 新增快速创建程序托盘图标功能，详见 `CreateTray` 系列函数
3. 修复 Ver2.6.2 的 Bug：在窗口过程函数中创建子窗口时，窗口卡死

---

**Ver2.6.2** (2022.05.03)

1. 新增有损绘图加速功能
2. 支持判断某一窗口是否在任务中
3. 修复 Bug：启动任务时，不会因为已经在任务中而卡死
4. 支持所有 EW_ 前缀的窗口属性

---

**Ver2.6.1** (2022.04.10)

1. 新增两个窗口样式设置宏 `DisableSystemMenu`，`EnableToolWindowStyle`。
2. `BeginTask` 函数增加返回值以判断是否成功开启任务，其系列宏也增强了安全性。
3. 关闭窗口后活动窗口将自动置空，而不是指向最近的窗口。
4. 优化代码，清除已知隐患。

---

**Ver2.6.0** (2022.04.03)

* `EasyWindow` 结构体新增成员 `isBusyProcessing`，表示是否正在处理内部消息
 
  新增 `WaitForProcessing` 函数用以等待内部消息处理。
   
  此举增强了程序稳定性，防止了内部消息处理与程序执行产生冲突而导致崩溃。
   
* 修复绘图内容向窗口映射时内存泄漏的 bug。

* 修复了 `closegraph_win32` 无法关闭所有窗口的 bug。

* 优化了双缓冲算法，提升了绘图内容映射速率。

* 窗口图标相关函数更名为 `GetCustomIconState`，`SetCustomIcon` 和`GetDefaultIconImage`。

  修复了无法加载资源图标作为程序图标的 bug，并支持了大小图标的分别设置。
  
* 新增绘图窗口初始化参数 `EW_NORMAL`（0），表示以正常方式创建窗口。
  
* `initgraph_win32` 函数的布尔型参数 `isCmd` 更名为整型参数 `flag`，与原生 EasyX 契合度更高

  默认将隐藏控制台窗口，新建窗口时传入 `EW_NORMAL` 不会改变当前的控制台窗口显示状态，若传入 `EW_SHOWCONSOLE` 则会显示控制台（如果存在）。

* 删除 `isInListWindow` 函数，仅保留 `isAliveWindow` 函数

* 新增 `rectangle_RECT` 系列快捷宏

* 新增颜色 `CLASSICGRAY`

---

**Ver2.5.6** (2022.03.27)

修复 `EasyWin32::closegraph_win32()` 函数销毁绘图窗口失败的 bug。

修复若模态窗口创建失败，父窗口陷入禁用状态的 bug。

---

**Ver2.5.5** (2022.03.26)

函数 `EasyWin32::isWindowSizeChanged()` 改为可以传入一个 HWND 类型的参数，用以区分不同的窗口。

---

**Ver2.5.4** (2022.03.26)

新增函数 `EasyWin32::GetWindowExStyle()` 和 `EasyWin32::SetWindowExStyle()`。

---

**Ver2.5.3** (2022.03.26)

修复了使用 `EasyWin32::initgraph_win32` 函数创建模态窗口时，导致阻塞的问题。

---

**Ver2.5.2** (2022.02.26)

对内部代码进行了一些微小修改，且删除了对 AHGraphics 库的依赖。

---

**Ver2.5.1** (2022.02.13)

使用 `initgraph()` 创建窗口时，若用户关闭窗口，则将自动退出程序，提升了对原 EasyX 代码的兼容性。

---

**Ver2.5** (2022.02.12)

1. 修改了部分宏规则，如下：
   * `BEGIN_DRAW` 改为 `BEGIN_TASK` 和 `BEGIN_TASK_WND`，前者无需指定窗口，后者需要
   * `END_DRAW` 更名为 `END_TASK`
   * `FLUSH_DRAW` 仅有强制重绘功能，不再输出绘图缓冲
   
   相对于旧版来说：
   
   这意味着无论是在顺序代码结构还是在 Win32 消息派发式的代码结构中，每执行一个（绘图）任务，都必须使用 `BEGIN_TASK`（或 `BEGIN_TASK_WND`）和 `END_TASK`。
   
   但是在顺序代码结构中，如果绘图任务执行完，则还需要在 `END_TASK` 后加上一句 `FLUSH_DRAW` 进行强制重绘。
   
2. 修复了顺序代码结构下，反复拉伸窗口，或关闭多窗口中的一个，可能导致程序崩溃的 bug，程序稳定性大大增加，现在几乎不会意外崩溃。

3. 新增 `EasyWin32::AutoExit()` 函数，执行后，当窗口都被销毁时，自动退出程序（非阻塞函数）

4. 新增 `EasyWin32::SetWindowStyle()` 系列函数。

5. 新增 `DisableResizing()` 宏，方便设置是否允许窗体拉伸

6. 修复一些细节

其他

重新润色所有示例代码，且修复 Sample2 CPU 占用过高的 bug。

---

**Ver2.4** (2022.02.11)

程序图标将默认使用自绘 EasyX 图标，这个图标模仿自 EasyX 官网的页面图标。

---

**Ver2.3** (2022.02.11)

1. 修复数个潜在安全隐患

2. 修复初始化绘图窗口时，窗口大小不对应真实绘图区大小的问题

3. 鼠标消息都被处理，且堆积数量超过阈值时，将自动清空

其他：

增加了一个示例（Sample3）

---

**Ver2.2** (2022.02.08)

添加了对 EasyX 原生消息函数的宏定义，支持了 MOUSEMSG 和 ExMessage 消息相关函数的直接调用。

但 ExMessage 相关函数仍只支持 EM_MOUSE 消息，即鼠标消息。

---

**Ver2.1** (2022.02.07)

1. 新增对创建多窗口的支持，且可以直接创建模态窗口

2. 修复 Win32 控件显示不稳定的 Bug

3. 新增对 `_getch` 等一系列控制台输入函数的支持

4. 新增对获取鼠标消息 `ExMessage` 的支持

---

**Ver1.0** (2020.12.06)

First version.

支持在 EasyX 中使用 Win32 控件。
