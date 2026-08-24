import sys
import os
import subprocess
from PySide6.QtCore import Qt, QFileSystemWatcher, Signal, QThread, QSize, QTimer
from PySide6.QtGui import QAction, QFont, QColor, QTextCursor, QSyntaxHighlighter, QTextCharFormat, QKeySequence, QLinearGradient, QBrush, QPalette
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QTextEdit, QPushButton, QFileDialog, QMessageBox, QStatusBar,
    QToolBar, QLabel, QSplitter, QTreeView, QTabWidget, QMenu,
    QFileSystemModel, QToolButton, QFrame, QMenuBar, QSpacerItem,
    QSizePolicy
)

# ============================================================
# 1. TypeScript UI 配置 (中文界面)
# ============================================================
UI_CONFIG = {
    "theme": {
        "name": "Dark+",
        "colors": {
            "background": "#1E1E1E",
            "foreground": "#D4D4D4",
            "border": "rgba(255,255,255,0.1)",
            "selection": "#264F78",
            "hover": "rgba(255,255,255,0.08)",
            "active": "#0E639C",
            "inactive": "#3C3C3C"
        },
        "gradient": {
            "enabled": True,
            "start": "#1a1a2e",
            "middle": "#16213e",
            "end": "#0f3460"
        }
    },
    "syntax": {
        "keyword": "#569CD6",
        "string": "#CE9178",
        "function": "#DCDCAA",
        "comment": "#6A9955",
        "number": "#B5CEA8",
        "decorator": "#C586C0",
        "className": "#4EC9B0",
        "variable": "#9CDCFE"
    },
    "layout": {
        "window": {"title": "JarIDE - Python 编辑器", "width": 1400, "height": 900},
        "explorer": {"width": 260, "minWidth": 180},
        "terminal": {"height": 200, "minHeight": 80}
    },
    "i18n": {
        "new": "新建",
        "open": "打开",
        "save": "保存",
        "save_as": "另存为",
        "exit": "退出",
        "run": "运行",
        "run_script": "运行脚本",
        "run_in_terminal": "在终端运行",
        "view": "视图",
        "help": "帮助",
        "about": "关于",
        "explorer": "资源管理器",
        "terminal": "终端",
        "output": "输出",
        "ready": "就绪",
        "untitled": "未命名",
        "python_version": "Python 3",
        "encoding": "UTF-8",
        "write_code": "在此编写 Python 代码...",
        "run_output": "运行输出将显示在这里...",
        "program_finished": "程序运行完毕",
        "error": "错误",
        "info": "提示",
        "open_file": "打开 Python 文件",
        "save_file": "保存文件",
        "cannot_open": "无法打开文件",
        "save_failed": "保存失败",
        "please_save": "请先保存文件",
        "run_finished": "运行完成",
        "running": "正在运行脚本...",
        "about_title": "关于 JarIDE",
        "about_text": "JarIDE - 现代化 Python 编辑器\n\n版本 1.0\n基于 PySide6 构建\n界面使用 TypeScript 设计"
    }
}

# ============================================================
# 2. 渐变背景窗口
# ============================================================
class GradientMainWindow(QMainWindow):
    """支持渐变背景的主窗口"""
    
    def __init__(self):
        super().__init__()
        self.setAttribute(Qt.WA_StyledBackground, True)
        
    def paintEvent(self, event):
        """绘制渐变背景"""
        if UI_CONFIG["theme"]["gradient"]["enabled"]:
            painter = super().paintEvent
            # 使用 QPalette 设置渐变背景
            gradient = QLinearGradient(0, 0, self.width(), self.height())
            gradient.setColorAt(0.0, QColor(UI_CONFIG["theme"]["gradient"]["start"]))
            gradient.setColorAt(0.5, QColor(UI_CONFIG["theme"]["gradient"]["middle"]))
            gradient.setColorAt(1.0, QColor(UI_CONFIG["theme"]["gradient"]["end"]))
            
            palette = self.palette()
            palette.setBrush(QPalette.Window, QBrush(gradient))
            self.setPalette(palette)
        
        super().paintEvent(event)

# ============================================================
# 3. 语法高亮器
# ============================================================
class PythonHighlighter(QSyntaxHighlighter):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.highlighting_rules = []
        colors = UI_CONFIG["syntax"]

        # 关键字
        keyword_format = QTextCharFormat()
        keyword_format.setForeground(QColor(colors["keyword"]))
        keyword_format.setFontWeight(QFont.Bold)
        keywords = [
            "and", "as", "assert", "break", "class", "continue", "def",
            "del", "elif", "else", "except", "finally", "for", "from",
            "global", "if", "import", "in", "is", "lambda", "nonlocal",
            "not", "or", "pass", "raise", "return", "try", "while",
            "with", "yield", "True", "False", "None"
        ]
        for word in keywords:
            self.highlighting_rules.append((fr'\b{word}\b', keyword_format))

        # 字符串
        string_format = QTextCharFormat()
        string_format.setForeground(QColor(colors["string"]))
        self.highlighting_rules.append((r"'[^'\\]*(\\.[^'\\]*)*'", string_format))
        self.highlighting_rules.append((r'"[^"\\]*(\\.[^"\\]*)*"', string_format))
        self.highlighting_rules.append((r"'''[^']*'''", string_format))
        self.highlighting_rules.append((r'"""[^"]*"""', string_format))

        # 函数调用
        function_format = QTextCharFormat()
        function_format.setForeground(QColor(colors["function"]))
        self.highlighting_rules.append((r'\b[A-Za-z_][A-Za-z0-9_]*(?=\()', function_format))

        # 类名
        class_format = QTextCharFormat()
        class_format.setForeground(QColor(colors["className"]))
        self.highlighting_rules.append((r'\bclass\s+([A-Za-z_][A-Za-z0-9_]*)', class_format))

        # 装饰器
        decorator_format = QTextCharFormat()
        decorator_format.setForeground(QColor(colors["decorator"]))
        self.highlighting_rules.append((r'@[A-Za-z_][A-Za-z0-9_]*', decorator_format))

        # 注释
        comment_format = QTextCharFormat()
        comment_format.setForeground(QColor(colors["comment"]))
        self.highlighting_rules.append((r'#.*', comment_format))

        # 数字
        number_format = QTextCharFormat()
        number_format.setForeground(QColor(colors["number"]))
        self.highlighting_rules.append((r'\b[0-9]+\b', number_format))

    def highlightBlock(self, text):
        for pattern, format_ in self.highlighting_rules:
            import re
            for match in re.finditer(pattern, text):
                start, end = match.span()
                self.setFormat(start, end - start, format_)

# ============================================================
# 4. 运行线程
# ============================================================
class RunScriptThread(QThread):
    output_signal = Signal(str)

    def __init__(self, script_path):
        super().__init__()
        self.script_path = script_path

    def run(self):
        try:
            process = subprocess.Popen(
                [sys.executable, self.script_path],
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
                encoding='utf-8',
                cwd=os.path.dirname(self.script_path)
            )
            output, _ = process.communicate()
            i18n = UI_CONFIG["i18n"]
            self.output_signal.emit(output if output else f"[{i18n['program_finished']}]")
        except Exception as e:
            self.output_signal.emit(f"[{UI_CONFIG['i18n']['error']}] {str(e)}")

# ============================================================
# 5. 主窗口
# ============================================================
class JarIDE(GradientMainWindow):
    def __init__(self):
        super().__init__()
        
        self.config = UI_CONFIG
        self.i18n = self.config["i18n"]
        self.current_file = None
        self.run_thread = None
        self.file_modified = False
        
        # 设置窗口
        win_config = self.config["layout"]["window"]
        self.setWindowTitle(win_config["title"])
        self.setGeometry(50, 50, win_config["width"], win_config["height"])
        self.setMinimumSize(1000, 700)
        
        # 应用样式
        self.setup_styles()
        
        # 创建 UI
        self.setup_ui()
        
        # 文件监控
        self.file_watcher = QFileSystemWatcher()
        self.file_watcher.fileChanged.connect(self.reload_file)

    def setup_styles(self):
        """应用样式 - 支持渐变背景"""
        gradient = self.config["theme"]["gradient"]
        
        # 计算渐变背景样式
        gradient_style = ""
        if gradient["enabled"]:
            gradient_style = f"""
                QMainWindow {{
                    background: qlineargradient(
                        x1:0, y1:0, x2:1, y2:1,
                        stop:0 {gradient["start"]},
                        stop:0.5 {gradient["middle"]},
                        stop:1 {gradient["end"]}
                    );
                }}
            """
        
        base_style = f"""
            {gradient_style}
            QWidget {{
                color: #D4D4D4;
                font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
                background: transparent;
            }}
            QMenuBar {{
                background: rgba(30, 30, 30, 180);
                color: #D4D4D4;
                border-bottom: 1px solid rgba(255, 255, 255, 0.08);
            }}
            QMenuBar::item {{
                background: transparent;
                padding: 6px 12px;
            }}
            QMenuBar::item:selected {{
                background: rgba(255, 255, 255, 0.08);
            }}
            QMenu {{
                background: rgba(45, 45, 48, 230);
                border: 1px solid rgba(255, 255, 255, 0.1);
                color: #D4D4D4;
                padding: 4px;
            }}
            QMenu::item {{
                padding: 6px 30px 6px 20px;
            }}
            QMenu::item:selected {{
                background: #094771;
            }}
            QToolBar {{
                background: rgba(45, 45, 48, 180);
                border: none;
                border-bottom: 1px solid rgba(255, 255, 255, 0.08);
                spacing: 4px;
                padding: 4px 8px;
            }}
            QToolButton {{
                background: transparent;
                border: none;
                padding: 6px 12px;
                color: #CCCCCC;
                font-weight: 500;
                border-radius: 4px;
            }}
            QToolButton:hover {{
                background: rgba(255, 255, 255, 0.08);
            }}
            QPushButton {{
                background: rgba(60, 60, 65, 200);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 4px;
                padding: 6px 16px;
                color: #D4D4D4;
                font-weight: 500;
            }}
            QPushButton:hover {{
                background: rgba(80, 80, 90, 220);
            }}
            QPushButton#runButton {{
                background: #0E639C;
                color: white;
                border: none;
                padding: 6px 20px;
                border-radius: 4px;
                font-weight: bold;
            }}
            QPushButton#runButton:hover {{
                background: #1177BB;
            }}
            QTextEdit {{
                background: rgba(30, 30, 30, 200);
                border: none;
                font-family: "Consolas", "Courier New", monospace;
                font-size: 14px;
                color: #D4D4D4;
                selection-background-color: #264F78;
                padding: 8px;
                border-radius: 4px;
            }}
            QTreeView {{
                background: rgba(37, 37, 38, 180);
                border: none;
                color: #CCCCCC;
                font-size: 13px;
                outline: 0;
            }}
            QTreeView::item {{
                padding: 3px 4px;
            }}
            QTreeView::item:hover {{
                background: rgba(255, 255, 255, 0.08);
            }}
            QTreeView::item:selected {{
                background: rgba(255, 255, 255, 0.12);
            }}
            QSplitter::handle {{
                background: rgba(255, 255, 255, 0.08);
                width: 2px;
            }}
            QStatusBar {{
                background: rgba(0, 122, 204, 180);
                color: white;
                border-top: 1px solid rgba(255, 255, 255, 0.1);
                min-height: 25px;
            }}
            QTabWidget::pane {{
                background: rgba(30, 30, 30, 150);
                border: none;
                border-radius: 4px;
            }}
            QTabBar::tab {{
                background: rgba(45, 45, 48, 180);
                color: #888888;
                padding: 6px 16px;
                border: none;
                border-right: 1px solid rgba(255, 255, 255, 0.05);
            }}
            QTabBar::tab:selected {{
                background: rgba(30, 30, 30, 200);
                color: #D4D4D4;
            }}
            QTabBar::tab:hover {{
                background: rgba(60, 60, 65, 200);
                color: #D4D4D4;
            }}
            QFrame#panelTitle {{
                background: rgba(37, 37, 38, 180);
                color: #888888;
                font-size: 11px;
                font-weight: bold;
                padding: 8px 12px;
                border-bottom: 1px solid rgba(255, 255, 255, 0.05);
                letter-spacing: 1px;
                border-radius: 4px 4px 0 0;
            }}
        """
        self.setStyleSheet(base_style)

    def setup_ui(self):
        """构建 UI"""
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(8, 8, 8, 8)
        main_layout.setSpacing(8)

        # 菜单栏
        self.create_menu_bar()
        
        # 工具栏
        self.create_toolbar()
        
        # 主分割器
        splitter = QSplitter(Qt.Horizontal)
        main_layout.addWidget(splitter)
        
        # 左侧: 资源管理器
        explorer_panel = self.create_explorer_panel()
        splitter.addWidget(explorer_panel)
        
        # 右侧: 编辑器区域
        editor_container = QWidget()
        editor_layout = QVBoxLayout(editor_container)
        editor_layout.setContentsMargins(0, 0, 0, 0)
        editor_layout.setSpacing(8)
        
        # 编辑器标签页
        self.tab_widget = QTabWidget()
        self.tab_widget.setDocumentMode(True)
        self.tab_widget.setTabsClosable(True)
        self.tab_widget.tabCloseRequested.connect(self.close_tab)
        editor_layout.addWidget(self.tab_widget)
        
        # 终端面板
        terminal_panel = self.create_terminal_panel()
        editor_layout.addWidget(terminal_panel)
        
        splitter.addWidget(editor_container)
        splitter.setStretchFactor(1, 1)
        
        # 状态栏
        self.create_status_bar()
        
        # 初始化编辑器
        self.new_file()

    def create_menu_bar(self):
        """创建菜单栏"""
        menubar = self.menuBar()
        
        # 文件菜单
        file_menu = menubar.addMenu(self.i18n["file"] if "file" in self.i18n else "文件")
        
        new_action = QAction(self.i18n["new"], self)
        new_action.setShortcut("Ctrl+N")
        new_action.triggered.connect(self.new_file)
        file_menu.addAction(new_action)
        
        open_action = QAction(self.i18n["open"], self)
        open_action.setShortcut("Ctrl+O")
        open_action.triggered.connect(self.open_file)
        file_menu.addAction(open_action)
        
        save_action = QAction(self.i18n["save"], self)
        save_action.setShortcut("Ctrl+S")
        save_action.triggered.connect(self.save_file)
        file_menu.addAction(save_action)
        
        save_as_action = QAction(self.i18n["save_as"], self)
        save_as_action.setShortcut("Ctrl+Shift+S")
        save_as_action.triggered.connect(self.save_file_as)
        file_menu.addAction(save_as_action)
        
        file_menu.addSeparator()
        
        exit_action = QAction(self.i18n["exit"], self)
        exit_action.setShortcut("Alt+F4")
        exit_action.triggered.connect(self.close)
        file_menu.addAction(exit_action)
        
        # 运行菜单
        run_menu = menubar.addMenu(self.i18n["run"])
        run_action = QAction(self.i18n["run_script"], self)
        run_action.setShortcut("F5")
        run_action.triggered.connect(self.run_script)
        run_menu.addAction(run_action)
        
        # 视图菜单
        view_menu = menubar.addMenu(self.i18n["view"])
        view_menu.addAction(self.i18n["explorer"])
        view_menu.addAction(self.i18n["terminal"])
        
        # 帮助菜单
        help_menu = menubar.addMenu(self.i18n["help"])
        about_action = QAction(self.i18n["about"], self)
        about_action.triggered.connect(self.show_about)
        help_menu.addAction(about_action)

    def create_toolbar(self):
        """创建工具栏"""
        toolbar = QToolBar()
        toolbar.setMovable(False)
        self.addToolBar(Qt.TopToolBarArea, toolbar)
        
        # 使用 QWidget 作为工具栏容器
        toolbar_widget = QWidget()
        toolbar_layout = QHBoxLayout(toolbar_widget)
        toolbar_layout.setContentsMargins(4, 2, 4, 2)
        toolbar_layout.setSpacing(4)
        
        # 新建按钮
        new_btn = QPushButton(self.i18n["new"])
        new_btn.setStyleSheet("""
            QPushButton {
                background: transparent;
                border: none;
                padding: 6px 12px;
                color: #CCCCCC;
                font-weight: 500;
            }
            QPushButton:hover {
                background: rgba(255, 255, 255, 0.08);
                border-radius: 4px;
            }
        """)
        new_btn.clicked.connect(self.new_file)
        toolbar_layout.addWidget(new_btn)
        
        # 打开按钮
        open_btn = QPushButton(self.i18n["open"])
        open_btn.setStyleSheet("""
            QPushButton {
                background: transparent;
                border: none;
                padding: 6px 12px;
                color: #CCCCCC;
                font-weight: 500;
            }
            QPushButton:hover {
                background: rgba(255, 255, 255, 0.08);
                border-radius: 4px;
            }
        """)
        open_btn.clicked.connect(self.open_file)
        toolbar_layout.addWidget(open_btn)
        
        # 保存按钮
        save_btn = QPushButton(self.i18n["save"])
        save_btn.setStyleSheet("""
            QPushButton {
                background: transparent;
                border: none;
                padding: 6px 12px;
                color: #CCCCCC;
                font-weight: 500;
            }
            QPushButton:hover {
                background: rgba(255, 255, 255, 0.08);
                border-radius: 4px;
            }
        """)
        save_btn.clicked.connect(self.save_file)
        toolbar_layout.addWidget(save_btn)
        
        # 分隔符
        separator = QFrame()
        separator.setFrameShape(QFrame.VLine)
        separator.setStyleSheet("background: rgba(255, 255, 255, 0.1); max-width: 1px;")
        toolbar_layout.addWidget(separator)
        
        # 运行按钮
        self.run_btn = QPushButton(self.i18n["run"])
        self.run_btn.setObjectName("runButton")
        self.run_btn.clicked.connect(self.run_script)
        toolbar_layout.addWidget(self.run_btn)
        
        # 分隔符
        separator2 = QFrame()
        separator2.setFrameShape(QFrame.VLine)
        separator2.setStyleSheet("background: rgba(255, 255, 255, 0.1); max-width: 1px;")
        toolbar_layout.addWidget(separator2)
        
        # 文件标签
        self.file_label = QLabel(f"{self.i18n['untitled']}-1")
        self.file_label.setStyleSheet("color: #9CDCFE; font-weight: 500; padding: 4px 8px;")
        toolbar_layout.addWidget(self.file_label)
        
        # 弹性空间
        spacer = QWidget()
        spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        toolbar_layout.addWidget(spacer)
        
        # Python 版本
        version_label = QLabel(self.i18n["python_version"])
        version_label.setStyleSheet("color: #6A9955; padding: 4px 8px;")
        toolbar_layout.addWidget(version_label)
        
        toolbar.addWidget(toolbar_widget)

    def create_explorer_panel(self):
        """创建资源管理器"""
        panel = QWidget()
        panel.setMaximumWidth(260)
        panel.setMinimumWidth(180)
        
        layout = QVBoxLayout(panel)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        # 标题
        title = QFrame()
        title.setObjectName("panelTitle")
        title_layout = QHBoxLayout(title)
        title_layout.setContentsMargins(12, 8, 12, 8)
        title_label = QLabel(self.i18n["explorer"].upper())
        title_label.setStyleSheet("color: #888888; font-size: 11px; font-weight: bold; letter-spacing: 1px;")
        title_layout.addWidget(title_label)
        title_layout.addStretch()
        layout.addWidget(title)
        
        # 文件树
        self.file_tree = QTreeView()
        self.file_model = QFileSystemModel()
        self.file_model.setRootPath("")
        self.file_tree.setModel(self.file_model)
        self.file_tree.setRootIndex(self.file_model.index(""))
        self.file_tree.hideColumn(1)
        self.file_tree.hideColumn(2)
        self.file_tree.hideColumn(3)
        self.file_tree.setHeaderHidden(True)
        self.file_tree.doubleClicked.connect(self.on_file_tree_double_clicked)
        layout.addWidget(self.file_tree)
        
        return panel

    def create_terminal_panel(self):
        """创建终端面板"""
        panel = QFrame()
        panel.setMaximumHeight(200)
        panel.setMinimumHeight(80)
        
        layout = QVBoxLayout(panel)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)
        
        # 标题
        title = QFrame()
        title.setObjectName("panelTitle")
        title_layout = QHBoxLayout(title)
        title_layout.setContentsMargins(12, 8, 12, 8)
        title_label = QLabel(self.i18n["terminal"].upper())
        title_label.setStyleSheet("color: #888888; font-size: 11px; font-weight: bold; letter-spacing: 1px;")
        title_layout.addWidget(title_label)
        title_layout.addStretch()
        layout.addWidget(title)
        
        # 输出窗口
        self.output_view = QTextEdit()
        self.output_view.setReadOnly(True)
        self.output_view.setPlaceholderText(self.i18n["run_output"])
        self.output_view.setStyleSheet("""
            QTextEdit {
                background: rgba(30, 30, 30, 200);
                border: none;
                font-family: "Consolas", monospace;
                font-size: 13px;
                color: #CCCCCC;
                padding: 8px;
                border-radius: 0 0 4px 4px;
            }
        """)
        layout.addWidget(self.output_view)
        
        return panel

    def create_status_bar(self):
        """创建状态栏"""
        self.status_bar = QStatusBar()
        self.setStatusBar(self.status_bar)
        
        self.status_message = QLabel(self.i18n["ready"])
        self.status_bar.addWidget(self.status_message)
        
        self.status_bar.addPermanentWidget(QLabel(self.i18n["python_version"]))
        self.status_bar.addPermanentWidget(QLabel(self.i18n["encoding"]))
        
        self.cursor_pos = QLabel("Ln 1, Col 1")
        self.status_bar.addPermanentWidget(self.cursor_pos)

    # ============================================================
    # 6. 事件处理方法
    # ============================================================
    
    def new_file(self):
        """新建文件"""
        tab_index = self.tab_widget.count()
        tab_name = f"{self.i18n['untitled']}-{tab_index + 1}"
        
        editor = QTextEdit()
        editor.setPlaceholderText(self.i18n["write_code"])
        editor.setStyleSheet("""
            QTextEdit {
                background: rgba(30, 30, 30, 200);
                border: none;
                font-family: "Consolas", "Courier New", monospace;
                font-size: 14px;
                color: #D4D4D4;
                selection-background-color: #264F78;
                padding: 8px;
                border-radius: 4px;
            }
        """)
        editor.setTabStopDistance(editor.fontMetrics().horizontalAdvance(' ') * 4)
        
        # 语法高亮
        PythonHighlighter(editor.document())
        
        # 信号连接
        editor.textChanged.connect(self.on_editor_text_changed)
        editor.cursorPositionChanged.connect(self.update_cursor_position)
        
        self.tab_widget.addTab(editor, tab_name)
        self.tab_widget.setCurrentWidget(editor)
        
        self.current_file = None
        self.file_modified = False
        self.file_label.setText(tab_name)
        self.status_message.setText(self.i18n["ready"])

    def open_file(self):
        """打开文件"""
        file_path, _ = QFileDialog.getOpenFileName(
            self, self.i18n["open_file"], "", "Python Files (*.py);;All Files (*)"
        )
        if file_path:
            self.load_file(file_path)

    def load_file(self, file_path):
        """加载文件"""
        try:
            with open(file_path, 'r', encoding='utf-8') as f:
                content = f.read()
            
            tab_name = os.path.basename(file_path)
            
            editor = QTextEdit()
            editor.setText(content)
            editor.setStyleSheet("""
                QTextEdit {
                    background: rgba(30, 30, 30, 200);
                    border: none;
                    font-family: "Consolas", "Courier New", monospace;
                    font-size: 14px;
                    color: #D4D4D4;
                    selection-background-color: #264F78;
                    padding: 8px;
                    border-radius: 4px;
                }
            """)
            editor.setTabStopDistance(editor.fontMetrics().horizontalAdvance(' ') * 4)
            
            # 语法高亮
            PythonHighlighter(editor.document())
            
            # 信号连接
            editor.textChanged.connect(self.on_editor_text_changed)
            editor.cursorPositionChanged.connect(self.update_cursor_position)
            
            self.tab_widget.addTab(editor, tab_name)
            self.tab_widget.setCurrentWidget(editor)
            
            self.current_file = file_path
            self.file_modified = False
            self.file_label.setText(tab_name)
            self.status_message.setText(f"{self.i18n['open']}: {file_path}")
            
            # 文件监控
            if self.file_watcher.files():
                self.file_watcher.removePaths(self.file_watcher.files())
            self.file_watcher.addPath(file_path)
            
        except Exception as e:
            QMessageBox.critical(self, self.i18n["error"], f"{self.i18n['cannot_open']}:\n{str(e)}")

    def save_file(self):
        """保存文件"""
        current_editor = self.tab_widget.currentWidget()
        if not current_editor:
            return
            
        if self.current_file:
            try:
                with open(self.current_file, 'w', encoding='utf-8') as f:
                    f.write(current_editor.toPlainText())
                self.file_modified = False
                tab_name = os.path.basename(self.current_file)
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), tab_name)
                self.file_label.setText(tab_name)
                self.status_message.setText(f"{self.i18n['save']}: {self.current_file}")
            except Exception as e:
                QMessageBox.critical(self, self.i18n["error"], f"{self.i18n['save_failed']}:\n{str(e)}")
        else:
            self.save_file_as()

    def save_file_as(self):
        """另存为"""
        current_editor = self.tab_widget.currentWidget()
        if not current_editor:
            return
            
        file_path, _ = QFileDialog.getSaveFileName(
            self, self.i18n["save_file"], "", "Python Files (*.py);;All Files (*)"
        )
        if file_path:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(current_editor.toPlainText())
                self.current_file = file_path
                self.file_modified = False
                tab_name = os.path.basename(file_path)
                self.tab_widget.setTabText(self.tab_widget.currentIndex(), tab_name)
                self.file_label.setText(tab_name)
                self.status_message.setText(f"{self.i18n['save']}: {file_path}")
            except Exception as e:
                QMessageBox.critical(self, self.i18n["error"], f"{self.i18n['save_failed']}:\n{str(e)}")

    def close_tab(self, index):
        """关闭标签页"""
        if self.tab_widget.count() > 1:
            self.tab_widget.removeTab(index)
        else:
            # 清空内容
            editor = self.tab_widget.widget(index)
            if editor:
                editor.clear()
                self.current_file = None
                self.file_modified = False
                self.file_label.setText(f"{self.i18n['untitled']}-1")
                self.tab_widget.setTabText(index, f"{self.i18n['untitled']}-1")

    def reload_file(self, path):
        """重新加载文件"""
        if path == self.current_file:
            try:
                with open(path, 'r', encoding='utf-8') as f:
                    content = f.read()
                current_editor = self.tab_widget.currentWidget()
                if current_editor:
                    cursor = current_editor.textCursor()
                    current_editor.setText(content)
                    current_editor.setTextCursor(cursor)
                self.status_message.setText(f"重新加载: {path}")
            except Exception as e:
                self.status_message.setText(f"重新加载失败: {str(e)}")

    def on_file_tree_double_clicked(self, index):
        """文件树双击"""
        file_path = self.file_model.filePath(index)
        if not self.file_model.isDir(index) and file_path.endswith('.py'):
            self.load_file(file_path)

    def on_editor_text_changed(self):
        """编辑器内容变化"""
        if not self.file_modified:
            self.file_modified = True
            current_index = self.tab_widget.currentIndex()
            current_text = self.tab_widget.tabText(current_index)
            if not current_text.endswith("*"):
                self.tab_widget.setTabText(current_index, f"{current_text}*")
                self.file_label.setText(f"{current_text}*")

    def update_cursor_position(self):
        """更新光标位置"""
        editor = self.tab_widget.currentWidget()
        if editor:
            cursor = editor.textCursor()
            line = cursor.blockNumber() + 1
            col = cursor.columnNumber() + 1
            self.cursor_pos.setText(f"Ln {line}, Col {col}")

    def run_script(self):
        """运行脚本"""
        if not self.current_file:
            QMessageBox.information(self, self.i18n["info"], self.i18n["please_save"])
            return
        
        self.save_file()
        self.output_view.clear()
        self.status_message.setText(self.i18n["running"])
        self.run_btn.setEnabled(False)
        
        self.run_thread = RunScriptThread(self.current_file)
        self.run_thread.output_signal.connect(self.display_output)
        self.run_thread.finished.connect(self.on_run_finished)
        self.run_thread.start()

    def display_output(self, text):
        """显示输出"""
        self.output_view.append(text)

    def on_run_finished(self):
        """运行完成"""
        self.run_btn.setEnabled(True)
        self.status_message.setText(self.i18n["run_finished"])

    def show_about(self):
        """显示关于"""
        QMessageBox.about(
            self,
            self.i18n["about_title"],
            self.i18n["about_text"]
        )

# ============================================================
# 7. 主程序
# ============================================================
if __name__ == "__main__":
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    
    # 设置全局字体 - 使用微软雅黑支持中文
    font = QFont("Microsoft YaHei", 10)
    app.setFont(font)
    
    window = JarIDE()
    window.show()
    sys.exit(app.exec())