<div align="center">

# 🪶 Jar-IDE

**A lightweight, animated Python IDE built with C++ / Qt5.**  
一款轻量、动画拉满、基于 C++ / Qt5 的 Python 编辑器。

![C++](https://img.shields.io/badge/C%2B%2B-17-00599C)
![Qt](https://img.shields.io/badge/Qt-5.14-41CD52)
![Platform](https://img.shields.io/badge/Windows-7~11-0078D6)

[English](#english) · [中文](#中文)

</div>

---

## English

Jar-IDE is a small, good-looking Python editor for Windows. It starts fast, animates everything it can, and runs on **Windows 7 through Windows 11** (that is exactly why Qt 5.14 was chosen).

### ✨ Features

- 🌐 **Auto Python bootstrap** — on launch it checks for a Python 3 install. If none is found, it downloads the official `python-3.12.0-amd64.exe` installer and opens it.
- 🧭 **Guided welcome screen** — three animated cards: *Open Folder*, *Open File*, *New Jianrong Project*.
- 🗂️ **Jianrong projects** — a first-class project type created inside the IDE: choose a directory + name, scaffold `project.jro` + `src/` with a ready `main.py`.
- 🖋️ **Code editor** — line-number gutter, Python syntax highlighting, multi-tab editing, save / run.
- ▶️ **One-click run** — runs the current script via QProcess and streams stdout/stderr to a bottom panel.
- 🎨 **Dark glass UI** — gradient brand card, hover animations, hand-rolled QSS theme.

### 📁 Project structure

```
Jar-IDE/
├── JarIDE.pro
├── src/                 # C++ sources
└── RES/                 # icons, style.qss, resources.qrc
```

### 🧱 Build

Requires **Qt 5.14.2 (MinGW 7.3 64-bit)**.

```bash
qmake JarIDE.pro
mingw32-make -j4
# output: bin/JarIDE.exe
```

### 📄 The `.jro` manifest

```xml
<Jianrong project="MyProject">
  <python.version="3.12">
  <file>
    main.py
  </file>
</Jianrong>
```

---

## 中文

Jar-IDE 是一款面向 Windows 的轻量 Python 编辑器，启动快、动画足，兼容 **Windows 7 到 Windows 11**。

### ✨ 特性

- 🌐 **自动检测 Python**：启动检测系统是否装有 Python 3；缺失时自动下载官方 `python-3.12.0-amd64.exe` 并打开安装。
- 🧭 **动画欢迎页**：打开文件夹 / 打开文件 / 新建 Jianrong 项目三张卡片。
- 🗂️ **Jianrong 项目**：IDE 内建项目类型。选目录、输项目名，自动生成 `project.jro` 与 `src/main.py`。
- 🖋️ **代码编辑**：行号、Python 语法高亮、多标签、保存与运行。
- ▶️ **一键运行**：QProcess 执行脚本，输出实时显示在底部面板。
- 🎨 **深色玻璃风界面**：渐变卡片、悬停动效、手写 QSS。

### 🧱 构建

需要 **Qt 5.14.2（MinGW 7.3 64 位）**。

```bash
qmake JarIDE.pro
mingw32-make -j4
# 产物：bin/JarIDE.exe
```

<div align="center"><sub>Built with C++17 & Qt 5.14 · MIT License</sub></div>
