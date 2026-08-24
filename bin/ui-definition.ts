// ui-definition.ts
// TypeScript UI 定义文件 - 仿 VSCode 界面

interface UIComponent {
    type: 'window' | 'panel' | 'toolbar' | 'menu' | 'editor' | 'tree' | 'tab' | 'statusbar';
    id: string;
    properties: Record<string, any>;
    children?: UIComponent[];
    style?: string;
}

interface ThemeConfig {
    name: string;
    colors: {
        background: string;
        foreground: string;
        border: string;
        selection: string;
        hover: string;
        active: string;
        inactive: string;
    };
    fonts: {
        default: string;
        monospace: string;
        size: number;
    };
}

// VSCode Dark+ 主题
const vscodeTheme: ThemeConfig = {
    name: 'Dark+',
    colors: {
        background: '#1E1E1E',
        foreground: '#D4D4D4',
        border: 'rgba(255,255,255,0.1)',
        selection: '#264F78',
        hover: 'rgba(255,255,255,0.08)',
        active: '#0E639C',
        inactive: '#3C3C3C'
    },
    fonts: {
        default: 'Segoe UI, Microsoft YaHei, sans-serif',
        monospace: 'Consolas, Courier New, monospace',
        size: 13
    }
};

// 语法高亮配色 (VSCode Dark+)
const syntaxColors = {
    keyword: '#569CD6',
    string: '#CE9178',
    function: '#DCDCAA',
    comment: '#6A9955',
    number: '#B5CEA8',
    decorator: '#C586C0',
    className: '#4EC9B0',
    variable: '#9CDCFE',
    operator: '#D4D4D4',
    builtin: '#DCDCAA'
};

// UI 布局定义
const uiLayout: UIComponent = {
    type: 'window',
    id: 'mainWindow',
    properties: {
        title: 'JarIDE - Python Editor',
        width: 1400,
        height: 900,
        minWidth: 1000,
        minHeight: 700
    },
    children: [
        {
            type: 'menu',
            id: 'mainMenu',
            properties: {
                items: [
                    {
                        label: 'File',
                        children: [
                            { label: 'New', shortcut: 'Ctrl+N', action: 'newFile' },
                            { label: 'Open', shortcut: 'Ctrl+O', action: 'openFile' },
                            { label: 'Save', shortcut: 'Ctrl+S', action: 'saveFile' },
                            { label: 'Save As', shortcut: 'Ctrl+Shift+S', action: 'saveFileAs' },
                            { type: 'separator' },
                            { label: 'Exit', shortcut: 'Alt+F4', action: 'exit' }
                        ]
                    },
                    {
                        label: 'Run',
                        children: [
                            { label: 'Run Script', shortcut: 'F5', action: 'runScript' },
                            { label: 'Run in Terminal', shortcut: 'Ctrl+Shift+F5', action: 'runInTerminal' }
                        ]
                    },
                    {
                        label: 'View',
                        children: [
                            { label: 'Explorer', shortcut: 'Ctrl+Shift+E', action: 'toggleExplorer', checked: true },
                            { label: 'Terminal', shortcut: 'Ctrl+`', action: 'toggleTerminal', checked: true }
                        ]
                    },
                    {
                        label: 'Help',
                        children: [
                            { label: 'About', action: 'about' }
                        ]
                    }
                ]
            }
        },
        {
            type: 'toolbar',
            id: 'mainToolbar',
            properties: {
                items: [
                    { label: 'New', icon: '📄', action: 'newFile' },
                    { label: 'Open', icon: '📂', action: 'openFile' },
                    { label: 'Save', icon: '💾', action: 'saveFile' },
                    { type: 'separator' },
                    { label: 'Run', icon: '▶', action: 'runScript', style: 'primary' },
                    { type: 'separator' },
                    { label: 'Current File:', type: 'label', id: 'fileLabel' },
                    { label: 'Python 3', type: 'label', id: 'versionLabel', style: 'version' }
                ]
            }
        },
        {
            type: 'splitter',
            id: 'mainSplitter',
            properties: {
                orientation: 'horizontal'
            },
            children: [
                {
                    type: 'panel',
                    id: 'explorerPanel',
                    properties: {
                        width: 260,
                        minWidth: 180,
                        maxWidth: 400,
                        title: 'EXPLORER'
                    },
                    children: [
                        {
                            type: 'tree',
                            id: 'fileTree',
                            properties: {
                                showHidden: false,
                                fileExtensions: ['.py']
                            }
                        }
                    ]
                },
                {
                    type: 'panel',
                    id: 'editorPanel',
                    properties: {
                        flex: 1
                    },
                    children: [
                        {
                            type: 'tab',
                            id: 'editorTabs',
                            properties: {
                                tabs: [
                                    {
                                        id: 'editorTab',
                                        label: 'Untitled-1',
                                        closeable: true,
                                        active: true
                                    }
                                ]
                            },
                            children: [
                                {
                                    type: 'editor',
                                    id: 'mainEditor',
                                    properties: {
                                        language: 'python',
                                        placeholder: 'Write Python code here...',
                                        tabSize: 4
                                    }
                                }
                            ]
                        },
                        {
                            type: 'panel',
                            id: 'terminalPanel',
                            properties: {
                                height: 200,
                                minHeight: 80,
                                maxHeight: 400,
                                title: 'TERMINAL'
                            },
                            children: [
                                {
                                    type: 'output',
                                    id: 'outputView',
                                    properties: {
                                        readOnly: true,
                                        placeholder: 'Run output will appear here...'
                                    }
                                }
                            ]
                        }
                    ]
                }
            ]
        },
        {
            type: 'statusbar',
            id: 'statusBar',
            properties: {
                items: [
                    { label: 'Ready', id: 'statusMessage' },
                    { type: 'separator' },
                    { label: 'Python 3', id: 'pythonVersion' },
                    { type: 'separator' },
                    { label: 'UTF-8', id: 'encoding' },
                    { type: 'separator' },
                    { label: 'Ln 1, Col 1', id: 'cursorPosition' }
                ]
            }
        }
    ]
};

// UI 样式定义 (QSS)
const uiStyles = `
    QMainWindow {
        background: #1E1E1E;
    }
    QWidget {
        color: #D4D4D4;
        font-family: "Segoe UI", "Microsoft YaHei", sans-serif;
        background: transparent;
    }
    QMenuBar {
        background: #1E1E1E;
        color: #D4D4D4;
        border-bottom: 1px solid rgba(255, 255, 255, 0.08);
    }
    QMenuBar::item {
        background: transparent;
        padding: 6px 12px;
    }
    QMenuBar::item:selected {
        background: rgba(255, 255, 255, 0.08);
    }
    QMenu {
        background: #2D2D30;
        border: 1px solid rgba(255, 255, 255, 0.1);
        color: #D4D4D4;
        padding: 4px;
    }
    QMenu::item {
        padding: 6px 30px 6px 20px;
    }
    QMenu::item:selected {
        background: #094771;
    }
    QToolBar {
        background: #2D2D30;
        border: none;
        border-bottom: 1px solid rgba(255, 255, 255, 0.08);
        spacing: 4px;
        padding: 4px 8px;
    }
    QToolButton {
        background: transparent;
        border: none;
        padding: 4px 8px;
        color: #CCCCCC;
        font-weight: 500;
    }
    QToolButton:hover {
        background: rgba(255, 255, 255, 0.08);
        border-radius: 4px;
    }
    QPushButton {
        background: rgba(60, 60, 65, 200);
        border: 1px solid rgba(255, 255, 255, 0.1);
        border-radius: 4px;
        padding: 6px 16px;
        color: #D4D4D4;
        font-weight: 500;
    }
    QPushButton:hover {
        background: rgba(80, 80, 90, 220);
    }
    QPushButton#runButton {
        background: #0E639C;
        color: white;
        border: none;
        padding: 6px 20px;
        border-radius: 4px;
        font-weight: bold;
    }
    QPushButton#runButton:hover {
        background: #1177BB;
    }
    QTextEdit {
        background: #1E1E1E;
        border: none;
        font-family: "Consolas", "Courier New", monospace;
        font-size: 14px;
        color: #D4D4D4;
        selection-background-color: #264F78;
        padding: 8px;
    }
    QTreeView {
        background: #252526;
        border: none;
        color: #CCCCCC;
        font-size: 13px;
        outline: 0;
    }
    QTreeView::item {
        padding: 3px 4px;
    }
    QTreeView::item:hover {
        background: rgba(255, 255, 255, 0.08);
    }
    QTreeView::item:selected {
        background: rgba(255, 255, 255, 0.12);
    }
    QSplitter::handle {
        background: rgba(255, 255, 255, 0.08);
        width: 2px;
    }
    QStatusBar {
        background: #007ACC;
        color: white;
        border-top: 1px solid rgba(255, 255, 255, 0.1);
        min-height: 25px;
    }
    QTabWidget::pane {
        background: #1E1E1E;
        border: none;
    }
    QTabBar::tab {
        background: #2D2D30;
        color: #888888;
        padding: 6px 16px;
        border: none;
        border-right: 1px solid rgba(255, 255, 255, 0.05);
    }
    QTabBar::tab:selected {
        background: #1E1E1E;
        color: #D4D4D4;
    }
    QTabBar::tab:hover {
        background: #3C3C3C;
        color: #D4D4D4;
    }
    QFrame#panelTitle {
        background: #252526;
        color: #888888;
        font-size: 11px;
        font-weight: bold;
        padding: 8px 12px;
        border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        letter-spacing: 1px;
    }
`;

// UI 事件绑定定义
interface UIEventBindings {
    componentId: string;
    events: {
        eventName: string;
        handler: string; // Python 方法名
    }[];
}

const eventBindings: UIEventBindings[] = [
    {
        componentId: 'mainEditor',
        events: [
            { eventName: 'textChanged', handler: 'onEditorTextChanged' },
            { eventName: 'cursorPositionChanged', handler: 'onCursorPositionChanged' }
        ]
    },
    {
        componentId: 'fileTree',
        events: [
            { eventName: 'doubleClicked', handler: 'onFileTreeDoubleClicked' }
        ]
    },
    {
        componentId: 'runButton',
        events: [
            { eventName: 'clicked', handler: 'runScript' }
        ]
    }
];

// 导出配置供 Python 使用
export {
    vscodeTheme,
    syntaxColors,
    uiLayout,
    uiStyles,
    eventBindings,
    ThemeConfig,
    UIComponent
};