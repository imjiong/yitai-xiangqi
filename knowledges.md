## 一、UCCI 引擎协议（核心）

中国象棋通用引擎协议（Universal Chinese Chess Protocol，简称 UCCI）是界面与引擎之间基于文本的通讯协议[-24](https://www.xqbase.com/protocol/cchess_ucci.htm)[-25](https://baike.baidu.com/item/中国象棋通用引擎协议/0?fromModule=lemma_inlink)。

### 1.1 官方协议文档

- **权威来源**：象棋百科全书网《中国象棋电脑应用规范(五)：中国象棋通用引擎协议》[-24](https://www.xqbase.com/protocol/cchess_ucci.htm)
  - 版本：3.0（2007年11月修订）
  - 地址：https://www.xqbase.com/protocol/cchess_ucci.htm
- **备用镜像**：GitHub 上的协议文档副本
  - https://github.com/haogefeifei/ChessQ_Magic/blob/master/doc/中国象棋通用引擎协议v3.0.md

### 1.2 通讯方法

引擎必须是编译过的可执行文件，通过 **标准输入(stdin)** 和 **标准输出(stdout)** 与界面通讯[-24](https://www.xqbase.com/protocol/cchess_ucci.htm)：

| 平台       | 启动引擎方式                                                 |
| :--------- | :----------------------------------------------------------- |
| Windows    | `CreateProcess()` + 重定向管道[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| Linux/Unix | `fork()` + `exec()` + 重定向管道[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |

**关键规则**：

- 所有指令和反馈都以**行**为单位，以 `\n` 结尾[-24](https://www.xqbase.com/protocol/cchess_ucci.htm)
- 引擎必须用 `fflush(stdout)` 刷新缓冲区[-24](https://www.xqbase.com/protocol/cchess_ucci.htm)

### 1.3 引擎三种状态

| 状态         | 说明                                                         |
| :----------- | :----------------------------------------------------------- |
| **引导状态** | 引擎启动后等待 `ucci` 指令[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| **空闲状态** | 接收 `setoption`、`position`、`banmoves`、`go`、`quit`       |
| **思考状态** | 收到 `go` 后进入，输出 `bestmove` 或 `nobestmove` 退出       |

### 1.4 核心指令速查

| 指令                                                       | 方向      | 说明                                                         |
| :--------------------------------------------------------- | :-------- | :----------------------------------------------------------- |
| `ucci`                                                     | 界面→引擎 | 初始化引擎                                                   |
| `ucciok`                                                   | 引擎→界面 | 引擎就绪                                                     |
| `isready`                                                  | 界面→引擎 | 检查引擎是否就绪                                             |
| `readyok`                                                  | 引擎→界面 | 就绪确认                                                     |
| `setoption name <id> [value <x>]`                          | 界面→引擎 | 设置引擎选项[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| `position fen <FEN> [moves <moves>]`                       | 界面→引擎 | 设置局面[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| `banmoves <move>...`                                       | 界面→引擎 | 禁止走法[-32](https://manpages.org/eleeye_engine/6)          |
| `go [depth <d>] [movetime <ms>] [nodes <n>]`               | 界面→引擎 | 开始思考[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| `stop`                                                     | 界面→引擎 | 停止思考                                                     |
| `bestmove <move> [ponder <move>]`                          | 引擎→界面 | 返回最佳走法[-32](https://manpages.org/eleeye_engine/6)      |
| `info score <cp> depth <d> nodes <n> time <ms> pv <moves>` | 引擎→界面 | 思考信息反馈[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| `quit`                                                     | 界面→引擎 | 退出引擎                                                     |

### 1.5 注意事项

- **皮卡鱼(Pikafish)引擎**：本身不支持 UCCI 协议，需使用代理程序 `pikafish-proxy.exe` 将 UCI 转换为 UCCI[-36](https://www.pikafish.com/wiki/index.php?title=如何在UCCI界面（如象棋巫师）加载皮卡鱼？&oldid=387)-
- 代理程序下载：https://www.xqbase.com/xqwizard/pikafish.htm[reference:22]
- 代理程序开源地址：https://github.com/xqbase/eleeye/tree/master/pikafish-proxy[reference:23]
- UCI 协议没有 `banmoves` 指令（这是 UCCI 特有的）-

### 1.6 常用 UCCI 引擎

| 引擎名称            | 类型      | 备注                                                         |
| :------------------ | :-------- | :----------------------------------------------------------- |
| ElephantEye（象眼） | 开源/免费 | 经典 UCCI 引擎-                                              |
| 象棋旋风 (Cyclone)  | 商业      | 支持 UCCI-                                                   |
| 佳佳 (GG)           | 免费      | 支持 UCCI-                                                   |
| 皮卡鱼 (Pikafish)   | 开源/免费 | 需代理程序[-36](https://www.pikafish.com/wiki/index.php?title=如何在UCCI界面（如象棋巫师）加载皮卡鱼？&oldid=387) |

## 二、棋盘与规则（走法生成、局面判断）

### 2.1 走法生成

走法生成器是象棋程序的核心组成部分，负责罗列一个局面的所有合法走法-[-1](https://www.xqbase.com/computer/stepbystep2.htm)。

- 棋盘可用 **9×10 数组** 或 **16×16 数组**（一维 `[256]`）表示[-1](https://www.xqbase.com/computer/stepbystep2.htm)
- 棋子名称英文字母规范：仕(士)=A(a)，炮=C(c)[-8](https://www.xqbase.com/protocol/cchess_fen.htm)

**各棋子走法要点**：

| 棋子  | 关键规则                             |
| :---- | :----------------------------------- |
| 马    | 蹩马腿检测-                          |
| 相/象 | 塞象眼检测-                          |
| 炮    | 需炮架才能吃子                       |
| 兵/卒 | 未过河只能前进，过河可横移，不可后退 |
| 将/帅 | 不可出九宫格，不可将帅照面-          |

### 2.2 FEN 局面表示

FEN（Forsyth-Edwards Notation）是记录象棋局面的标准文本格式-[-8](https://www.xqbase.com/protocol/cchess_fen.htm)。

**格式**：`<棋子布局> <轮到谁走> <易位> <过路兵> <半回合数> <回合数>`

**中国象棋 FEN 示例**（初始局面）[-8](https://www.xqbase.com/protocol/cchess_fen.htm)：

text



复制



下载

```
rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1
```



**字段说明**[-8](https://www.xqbase.com/protocol/cchess_fen.htm)：

- 小写 = 黑方，大写 = 红方[-8](https://www.xqbase.com/protocol/cchess_fen.htm)
- 9 个 `/` 分隔 10 行[-8](https://www.xqbase.com/protocol/cchess_fen.htm)
- `w` = 红方走，`b` = 黑方走[-8](https://www.xqbase.com/protocol/cchess_fen.htm)
- 第3、4字段（易位/过路兵）中国象棋无此概念，始终用 `-`[-8](https://www.xqbase.com/protocol/cchess_fen.htm)

### 2.3 局面状态判断

| 状态         | 判断逻辑                                  |
| :----------- | :---------------------------------------- |
| **将军**     | 走子后己方棋子攻击到对方将/帅             |
| **应将**     | 被将军方必须走一步消除将军状态-           |
| **将死**     | 被将军方所有合法走法都无法应将-           |
| **困毙**     | 轮到行棋的一方无任何合法走法，但未被将军- |
| **将帅照面** | 将与帅在同一纵线且中间无棋子，为非法局面- |

**终局判定优先级**：将死 > 困毙 > 长将 > 和棋-

## 三、棋谱格式

### 3.1 PGN 格式（Portable Game Notation）

中国象棋 PGN 规范参见《中国象棋电脑应用规范(四)：PGN文件格式》-。

**关键规范**-：

- 第一个标签必须是 `Game`，值为 `"Chinese Chess"`
- 文件分为**标签部分**和**着法部分**
- 文本格式，可用任何文本编辑器修改-

### 3.2 其他棋谱格式

| 格式    | 关联软件             | 说明                      |
| :------ | :------------------- | :------------------------ |
| **CHE** | QQ象棋               | 腾讯平台格式              |
| **XQF** | 象棋演播室(XQStudio) | 高版本加密，低版本不加密- |
| **MXQ** | 弈天平台             | 弈天棋谱格式-             |

XQF 文件从 `0x0400` 开始存放棋谱记录，每步 8 字节 + 注解文本-。

## 四、开局库（OBK 格式）

### 4.1 OBK 格式说明

- OBK 是**冰河五四**象棋软件使用的开局库格式-
- 本质是 **SQLite 标准数据库文件**，可用 SQLite 管理软件打开-
- 但关键字段经过**变异处理**，常规 SQLite 工具无法正常显示和检索数据-

### 4.2 开局库处理策略

- UCI 协议中，开局库通常由**界面（UI）处理**而非引擎处理[-32](https://manpages.org/eleeye_engine/6)
- 界面从开局库选择走法，脱离开局库后才启动引擎计算[-32](https://manpages.org/eleeye_engine/6)
- ElephantEye 早期开局库为**文本格式**，每行记录：着法 + 权重 + 局面-

## 五、wxWidgets 框架（GUI）

### 5.1 版本与构建

- **指定版本**：wxWidgets 3.2.11-
- **构建系统**：CMake（官方推荐）-
- 使用 `find_package(wxWidgets REQUIRED ...)` 查找库-

### 5.2 可停靠窗口（AUI）

使用 `wxAuiManager` 管理可停靠子窗口-：

- `wxAuiPaneInfo` 控制每个窗格的位置、停靠/浮动/隐藏状态-
- `wxAuiNotebook` 实现标签页分组、拖拽重排-
- 窗格可浮动的属性：`Floatable()`-

### 5.3 高 DPI 支持

- wxWidgets 3.x 对高 DPI 有逐步改进，但部分控件（如 `wxAuiToolBar` 中的 `wxChoice`）在 DPI 变化时可能存在尺寸问题-
- 建议在开发过程中关注 DPI 缩放适配，特别是在 Windows 平台-

## 六、图像识别（外部对局连线）

### 6.1 常用技术方案

| 方案              | 说明                            |
| :---------------- | :------------------------------ |
| **OpenCV**        | 图像预处理、特征提取、分类识别- |
| **YOLO/深度学习** | 棋盘检测、棋子分类-             |
| **模板匹配**      | 基于像素颜色匹配的棋盘定位-     |

### 6.2 识别流程

1. **屏幕捕获** → 截取目标窗口区域-
2. **棋盘定位** → 透视变换 + 网格分割-
3. **棋子识别** → 颜色/形状/字符识别-
4. **局面转换** → 像素坐标映射到逻辑位置 → 生成 FEN-

### 6.3 容错参数

- 像素偏移容忍度
- 色差百分比阈值
- 找图周期（轮询间隔）
- 查找范围（ROI 区域）

## 七、参考资源汇总

### 7.1 核心网站

| 网站         | 地址                                                        | 内容                                                         |
| :----------- | :---------------------------------------------------------- | :----------------------------------------------------------- |
| 象棋百科全书 | [https://www.xqbase.com](https://www.xqbase.com/)           | UCCI、FEN、PGN 规范，电脑象棋教程[-24](https://www.xqbase.com/protocol/cchess_ucci.htm) |
| 皮卡鱼 Wiki  | https://www.pikafish.com/wiki                               | 引擎使用文档-                                                |
| ElephantBase | [http://www.elephantbase.net](http://www.elephantbase.net/) | UCCI 协议镜像[-32](https://manpages.org/eleeye_engine/6)     |

### 7.2 推荐阅读

- 《电脑象棋循序渐进》系列（[xqbase.com](https://xqbase.com/)）[-1](https://www.xqbase.com/computer/stepbystep2.htm)
- 《C/C++中国象棋程序入门与提高》（蒋鹏等著）-