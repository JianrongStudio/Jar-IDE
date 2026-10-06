<div align="center">

# Jar-IDE

**A lightweight, animated Python IDE built with C++ / Qt5.**

一款轻量、动画流畅、基于 C++ / Qt5 的 Python 编辑器。

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C)
![Qt](https://img.shields.io/badge/Qt-5.14-41CD52)
![Platform](https://img.shields.io/badge/Windows-7%20~%2011-0078D6)

[English](#english) · [中文](#中文)

</div>

## English

Jar-IDE is a small, good-looking Python editor for Windows, running on **Windows 7 through Windows 11**.

### Features
- Auto Python bootstrap: download python-3.12.0-amd64 when no interpreter is found.
- Guided welcome screen with a custom frameless title bar.
- Jianrong projects: scaffold `project.jro` + `src/`; explorer shows only `.py` files.
- Code editor with line numbers, Python syntax highlighting, multi-tab, run.
- Dark glass QSS theme, hover animations.

### Build
Requires Qt 5.14.2 (MinGW 7.3 64-bit):
```
qmake JarIDE.pro
mingw32-make -j4
```

### Portable build
A bare exe needs the Qt runtime. Copy the needed DLLs next to the exe (Qt5Core/Gui/Widgets/Network/Svg, MinGW runtime, platforms/qwindows.dll, imageformats/qsvg.dll).

## 中文

Jar-IDE 是面向 Windows 的轻量 Python 编辑器，兼容 Windows 7 ~ 11。

### 特性
- 启动检测 Python，缺失自动下载 3.12.0 安装包。
- 无边框自绘标题栏；欢迎页三张卡片。
- Jianrong 项目：自动生成 project.jro 与 src/，文件浏览器只显示 .py。
- 行号、语法高亮、多标签、一键运行。

### 构建
```
qmake JarIDE.pro
mingw32-make -j4
```

<div align="center"><sub>MIT License</sub></div>
