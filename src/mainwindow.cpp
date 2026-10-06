#include "mainwindow.h"
#include "codeeditor.h"
#include "pythonrunner.h"
#include "pythonchecker.h"
#include "appstyle.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QTabWidget>
#include <QTabBar>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDialog>
#include <QDir>
#include <QStatusBar>
#include <QIcon>
#include <QMouseEvent>
#include <QApplication>
#include <QTextStream>
#include <QShortcut>
#include <QKeySequence>
#include <QProcess>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QSettings set("JarIDE", "JarIDE");
    QString saved = set.value("pythonExe").toString();
    m_pythonExe = saved.isEmpty() ? PythonChecker::findPython() : saved;
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    buildUi();
}

QWidget *MainWindow::buildTitleBar()
{
    auto *bar = new QWidget;
    bar->setObjectName("titleBar");
    bar->setFixedHeight(38);
    auto *h = new QHBoxLayout(bar);
    h->setContentsMargins(12, 0, 8, 0);
    h->setSpacing(8);
    auto *logo = new QLabel(bar);
    logo->setPixmap(QIcon(AppStyle::iconPath("logo.svg")).pixmap(18, 18));
    h->addWidget(logo);
    m_titleLabel = new QLabel("Jar-IDE", bar);
    m_titleLabel->setObjectName("titleText");
    h->addWidget(m_titleLabel);
    h->addStretch();
    auto *btnMin = new QPushButton(bar);
    btnMin->setObjectName("winBtnMin");
    btnMin->setFixedSize(42, 28);
    btnMin->setText("_");
    connect(btnMin, &QPushButton::clicked, this, &QWidget::showMinimized);
    m_btnMax = new QPushButton(bar);
    m_btnMax->setObjectName("winBtnMax");
    m_btnMax->setFixedSize(42, 28);
    m_btnMax->setText("□");
    connect(m_btnMax, &QPushButton::clicked, this, &MainWindow::toggleMax);
    auto *btnClose = new QPushButton(bar);
    btnClose->setObjectName("winBtnClose");
    btnClose->setFixedSize(46, 28);
    btnClose->setText("X");
    connect(btnClose, &QPushButton::clicked, qApp, &QApplication::quit);
    h->addWidget(btnMin); h->addWidget(m_btnMax); h->addWidget(btnClose);
    bar->installEventFilter(this);
    return bar;
}

void MainWindow::toggleMax()
{
    if (m_maximized) { showNormal(); m_btnMax->setText("□"); }
    else { showMaximized(); m_btnMax->setText("❐"); }
    m_maximized = !m_maximized;
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_titleBar) {
        if (e->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(e);
            if (me->button() == Qt::LeftButton) { m_dragPos = me->globalPos() - frameGeometry().topLeft(); return true; }
        } else if (e->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent*>(e);
            if (me->buttons() & Qt::LeftButton && !m_maximized) { move(me->globalPos() - m_dragPos); return true; }
        } else if (e->type() == QEvent::MouseButtonDblClick) { toggleMax(); return true; }
    }
    return QMainWindow::eventFilter(o, e);
}

void MainWindow::buildUi()
{
    setWindowTitle("Jar-IDE");
    setWindowIcon(QIcon(AppStyle::iconPath("logo.svg")));
    resize(1180, 760);
    auto *root = new QWidget(this);
    auto *rv = new QVBoxLayout(root);
    rv->setContentsMargins(1, 1, 1, 1);
    rv->setSpacing(0);
    m_titleBar = buildTitleBar();
    rv->addWidget(m_titleBar);
    auto *central = new QWidget(root);
    auto *hl = new QHBoxLayout(central);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);
    auto *side = new QFrame;
    side->setObjectName("sidebar");
    side->setFixedWidth(240);
    auto *sv = new QVBoxLayout(side);
    sv->setContentsMargins(10, 12, 10, 10);
    sv->setSpacing(8);
    auto *projTitle = new QLabel("Explorer", side);
    projTitle->setStyleSheet("font-weight:600; font-size:14px; color:#fff;");
    sv->addWidget(projTitle);
    m_tree = new QTreeView(side);
    m_fs = new QFileSystemModel(m_tree);
    m_fs->setNameFilters({"*.py"});
    m_fs->setNameFilterDisables(false);
    m_tree->setModel(m_fs);
    m_tree->setHeaderHidden(true);
    for (int i = 1; i < m_fs->columnCount(); ++i) m_tree->hideColumn(i);
    sv->addWidget(m_tree);
    hl->addWidget(side);
    auto *right = new QWidget;
    auto *vv = new QVBoxLayout(right);
    vv->setContentsMargins(0, 0, 0, 0);
    vv->setSpacing(0);
    auto *bar = new QWidget;
    auto *bh = new QHBoxLayout(bar);
    bh->setContentsMargins(12, 8, 12, 8);
    bh->setSpacing(8);
    auto *btnNew  = new QPushButton(QIcon(AppStyle::iconPath("new-project.svg")), tr("New File"));
    auto *btnSave = new QPushButton(QIcon(AppStyle::iconPath("file.svg")), tr("Save"));
    auto *btnRun  = new QPushButton(QIcon(AppStyle::iconPath("run.svg")), tr("Run"));
    auto *btnSet  = new QPushButton(QIcon(AppStyle::iconPath("python.svg")), tr("Settings"));
    btnRun->setStyleSheet("QPushButton{background:#1f9d55; color:#fff;} QPushButton:hover{background:#26b665;}");
    btnNew->setObjectName("ghost"); btnSave->setObjectName("ghost"); btnSet->setObjectName("ghost");
    bh->addWidget(btnNew); bh->addWidget(btnSave); bh->addStretch();
    bh->addWidget(btnSet);  bh->addWidget(btnRun);
    vv->addWidget(bar);
    m_tabs = new QTabWidget;
    m_tabs->setTabsClosable(true);
    vv->addWidget(m_tabs, 1);
    auto *term = new QWidget;
    auto *tv = new QVBoxLayout(term);
    tv->setContentsMargins(0, 0, 0, 0); tv->setSpacing(0);
    m_out = new QPlainTextEdit;
    m_out->setObjectName("terminalOut");
    m_out->setReadOnly(true);
    m_out->setMaximumHeight(150);
    tv->addWidget(m_out);
    auto *inRow = new QWidget;
    auto *ih = new QHBoxLayout(inRow);
    ih->setContentsMargins(8, 4, 8, 6); ih->setSpacing(6);
    auto *prompt = new QLabel(">");
    prompt->setObjectName("termPrompt");
    m_termInput = new QLineEdit;
    m_termInput->setObjectName("termInput");
    m_termInput->setPlaceholderText(tr("type command, e.g. python main.py"));
    ih->addWidget(prompt); ih->addWidget(m_termInput, 1);
    tv->addWidget(inRow);
    vv->addWidget(term);
    hl->addWidget(right, 1);
    rv->addWidget(central, 1);
    setCentralWidget(root);
    m_pyLabel = new QLabel;
    statusBar()->addPermanentWidget(m_pyLabel);
    auto *scLabel = new QLabel("  Ctrl+S save   F5 run   Ctrl+W close tab   ");
    statusBar()->addWidget(scLabel);
    m_pyLabel->setText(m_pythonExe.isEmpty() ? tr("Python: not found") : tr("Python: %1").arg(m_pythonExe));
    m_runner = new PythonRunner(this);
    connect(m_runner, &PythonRunner::append, m_out, &QPlainTextEdit::appendPlainText);
    connect(m_tree, &QTreeView::doubleClicked, this, [this](const QModelIndex &idx){
        QString p = m_fs->filePath(idx);
        if (!m_fs->isDir(idx)) openFileInTab(p);
    });
    connect(btnNew,  &QPushButton::clicked, this, &MainWindow::newFile);
    connect(btnSave, &QPushButton::clicked, this, &MainWindow::saveCurrent);
    connect(btnRun,  &QPushButton::clicked, this, &MainWindow::runCurrent);
    connect(btnSet,  &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_termInput, &QLineEdit::returnPressed, this, [this](){
        QString c = m_termInput->text().trimmed();
        if (!c.isEmpty()) runCommand(c);
        m_termInput->clear();
    });
    auto *scSave = new QShortcut(QKeySequence("Ctrl+S"), this);
    connect(scSave, &QShortcut::activated, this, &MainWindow::saveCurrent);
    auto *scRun = new QShortcut(QKeySequence("F5"), this);
    connect(scRun, &QShortcut::activated, this, &MainWindow::runCurrent);
    auto *scClose = new QShortcut(QKeySequence("Ctrl+W"), this);
    connect(scClose, &QShortcut::activated, this, [this]{ closeTab(m_tabs->currentIndex()); });
    auto *scNew = new QShortcut(QKeySequence("Ctrl+N"), this);
    connect(scNew, &QShortcut::activated, this, &MainWindow::newFile);
}

void MainWindow::openPath(const QString &path)
{
    QFileInfo fi(path);
    if (fi.isDir()) {
        m_projectRoot = fi.absoluteFilePath();
        QString jro = QDir(m_projectRoot).filePath("project.jro");
        if (QFileInfo::exists(jro)) {
            m_jro = JroProject::fromFile(jro);
            m_explorerRoot = QDir(m_jro.srcDir()).absolutePath();
            m_titleLabel->setText("Jar-IDE  -  " + m_jro.name);
        } else {
            m_jro = JroProject();
            m_explorerRoot = m_projectRoot;
            m_titleLabel->setText("Jar-IDE  -  " + fi.fileName());
        }
    } else {
        m_projectRoot = fi.absolutePath();
        m_explorerRoot = m_projectRoot;
        m_jro = JroProject();
        m_titleLabel->setText("Jar-IDE  -  " + fi.fileName());
    }
    QDir().mkpath(m_explorerRoot);
    m_fs->setRootPath(m_explorerRoot);
    m_tree->setRootIndex(m_fs->index(m_explorerRoot));
    if (!m_jro.isNull()) {
        QString mainPy = QDir(m_explorerRoot).filePath("main.py");
        if (QFileInfo::exists(mainPy)) openFileInTab(mainPy);
    } else if (!fi.isDir()) {
        openFileInTab(path);
    }
}

CodeEditor *MainWindow::currentEditor() const { return qobject_cast<CodeEditor*>(m_tabs->currentWidget()); }

int MainWindow::editorIndex(CodeEditor *ed) const
{
    for (int i = 0; i < m_tabs->count(); ++i)
        if (m_tabs->widget(i) == ed) return i;
    return -1;
}

void MainWindow::updateTabModified(bool m)
{
    CodeEditor *ed = qobject_cast<CodeEditor*>(sender());
    if (!ed) return;
    int i = editorIndex(ed);
    if (i < 0) return;
    QString name = QFileInfo(ed->filePath()).fileName();
    QTabBar *bar = m_tabs->tabBar();
    if (m) { bar->setTabText(i, name + "  *"); bar->setTabTextColor(i, QColor("#ff6b6b")); }
    else   { bar->setTabText(i, name);        bar->setTabTextColor(i, QColor("#8fd3ff")); }
}

void MainWindow::openFileInTab(const QString &path)
{
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto *ed = qobject_cast<CodeEditor*>(m_tabs->widget(i));
        if (ed && QFileInfo(ed->filePath()) == QFileInfo(path)) { m_tabs->setCurrentIndex(i); return; }
    }
    auto *ed = new CodeEditor;
    ed->loadFile(path);
    connect(ed, &CodeEditor::modificationChanged, this, &MainWindow::updateTabModified);
    int idx = m_tabs->addTab(ed, QFileInfo(path).fileName());
    m_tabs->setCurrentIndex(idx);
}

void MainWindow::newFile()
{
    bool ok = false;
    QString name = QInputDialog::getText(this, tr("New File"), tr("File name (.py):"),
        QLineEdit::Normal, "untitled.py", &ok).trimmed();
    if (!ok || name.isEmpty()) return;
    if (!name.endsWith(".py", Qt::CaseInsensitive)) name += ".py";
    QString dir = m_explorerRoot.isEmpty() ? m_projectRoot : m_explorerRoot;
    QDir().mkpath(dir);
    QString full = QDir(dir).absoluteFilePath(name);
    QFile f(full);
    if (f.exists()) { openFileInTab(full); return; }
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Jar-IDE"), tr("Cannot create file:\n%1").arg(full));
        return;
    }
    QTextStream ts(&f); ts.setCodec("UTF-8"); ts << "# " << name << "\n\n";
    f.close();
    if (!m_jro.isNull() && !m_jro.files.contains(name)) { m_jro.files << name; m_jro.save(); }
    m_fs->setRootPath(m_explorerRoot);
    openFileInTab(full);
}

void MainWindow::saveCurrent()
{
    CodeEditor *ed = currentEditor();
    if (!ed) return;
    if (ed->filePath().isEmpty()) {
        QString p = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), "*.py");
        if (p.isEmpty()) return;
        ed->saveFile(p);
    } else ed->saveFile(ed->filePath());
}

void MainWindow::runCommand(const QString &cmd)
{
    m_out->appendPlainText("> " + cmd);
    QStringList parts; QString cur; bool inQuote = false;
    for (int i = 0; i < cmd.size(); ++i) {
        QChar c = cmd[i];
        if (c == '"') { inQuote = !inQuote; continue; }
        if ((c == ' ' || c == '\t') && !inQuote) { if (!cur.isEmpty()) { parts << cur; cur.clear(); } }
        else cur += c;
    }
    if (!cur.isEmpty()) parts << cur;
    if (parts.isEmpty()) return;
    QString prog = parts.takeFirst();
    auto *proc = new QProcess(this);
    proc->setProcessChannelMode(QProcess::MergedChannels);
    QString work = m_explorerRoot.isEmpty() ? m_projectRoot : m_explorerRoot;
    proc->setWorkingDirectory(work);
    connect(proc, &QProcess::readyReadStandardOutput, this, [this, proc](){
        m_out->appendPlainText(QString::fromLocal8Bit(proc->readAll()));
    });
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, proc](int code){
        m_out->appendPlainText(QString("[exit code %1]\n").arg(code));
        proc->deleteLater();
    });
    proc->start(prog, parts);
    if (!proc->waitForStarted(3000))
        m_out->appendPlainText("[error] could not start: " + proc->errorString());
}

void MainWindow::runCurrent()
{
    CodeEditor *ed = currentEditor();
    if (!ed) return;
    if (ed->filePath().isEmpty()) { QMessageBox::information(this, tr("Jar-IDE"), tr("Save the file before running.")); return; }
    if (m_pythonExe.isEmpty()) { m_out->appendPlainText("[!] Python not found. Use Settings to set python.exe, or type it below."); return; }
    runCommand(QString("%1 \"%2\"").arg(m_pythonExe, ed->filePath()));
}

void MainWindow::openSettings()
{
    QDialog d(this);
    d.setWindowTitle(tr("Settings"));
    d.setMinimumWidth(420);
    auto *v = new QVBoxLayout(&d);
    v->addWidget(new QLabel(tr("Python interpreter (python.exe):")));
    auto *edit = new QLineEdit(m_pythonExe, &d);
    v->addWidget(edit);
    auto *browse = new QPushButton(tr("Browse..."), &d);
    v->addWidget(browse);
    connect(browse, &QPushButton::clicked, &d, [&](){
        QString f = QFileDialog::getOpenFileName(&d, tr("Select python.exe"), QString(), "python.exe (python.exe)");
        if (!f.isEmpty()) edit->setText(f);
    });
    auto *ok = new QPushButton(tr("Save"), &d);
    v->addWidget(ok);
    connect(ok, &QPushButton::clicked, &d, &QDialog::accept);
    if (d.exec() == QDialog::Accepted) {
        m_pythonExe = edit->text().trimmed();
        QSettings set("JarIDE", "JarIDE");
        set.setValue("pythonExe", m_pythonExe);
        m_pyLabel->setText(tr("Python: %1").arg(m_pythonExe.isEmpty() ? "(not set)" : m_pythonExe));
    }
}

void MainWindow::closeTab(int idx)
{
    auto *w = m_tabs->widget(idx);
    m_tabs->removeTab(idx);
    w->deleteLater();
}
