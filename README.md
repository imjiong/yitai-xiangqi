# 弈台象棋 - 中国象棋客户端界面
## Warning
 - 这是个半成品，不建议使用。
 - 这个项目是用 Vibe Coding 造出来的，所以代码风格比较奇怪，请见谅。
## 简介
这是一个中国象棋客户端界面，使用`wxWidgets 3.2.11`开发。
现在功能不完善，仅支持显示棋盘和棋子，并且可以移动棋子，导出成PGN格式的棋谱，其他功能开发中。
理论上支持全平台。现在的`MAKEFILE`只能支持Windows平台，并且暂时只能用支持posix线程的`mingw32`的工具链编译。[工具链：i686-16.1.0-release-posix-dwarf-msvcrt-rt_v14-rev1.7z](https://github.com/niXman/mingw-builds-binaries/releases/download/16.1.0-rt_v14-rev1/i686-16.1.0-release-posix-dwarf-msvcrt-rt_v14-rev1.7z)(来源：[https://github.com/niXman/mingw-builds-binaries](https://github.com/niXman/mingw-builds-binaries))

目标：

- 实现一个功能完善的象棋客户端界面，可跨平台运行，能支持国产 Linux 发行版，如：银河麒麟、中标麒麟、深度等。
- 实现 UCI、UCCI 协议的引擎接入。
- 实现低成本，低占用算力的对局连线功能。

## 编译
### Windows
使用`MinGW`编译，需要先编译`wxWidgets 3.2.11`，编译出动态库，然后再编译`弈台象棋`。
### Linux
暂时没有测试，目前的`MAKEFILE`不能编译LINUX版本，未来会想办法解决。