#include "mainwindow.h"
#include "codeeditor.h"
#include "pythonrunner.h"
#include "pythonchecker.h"
#include "appstyle.h"

#include <QTreeView>
#include <QFileSystemModel>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileInfo>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QStatusBar>
#include <QIcon>
#include <QMouseEvent>
#include <QApplication>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_pythonExe = PythonChecker::findPython();
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
    btnMin->setText("\u2014");
    connect(btnMin, &QPushButton::clicked, this, &QWidget::showMinimized);

    m_btnMax = new QPushButton(bar);
    m_btnMax->setObjectName("winBtnMax");
    m_btnMax->setFixedSize(42, 28);
    m_btnMax->setText("\u25a1");
    connect(m_btnMax, &QPushButton::clicked, this, &MainWindow::toggleMax);

    auto *btnClose = new QPushButton(bar);
    btnClose->setObjectName("winBtnClose");
    btnClose->setFixedSize(46, 28);
    btnClose->setText("\u2715");
    connect(btnClose, &QPushButton::clicked, qApp, &QApplication::quit);

    h->addWidget(btnMin);
    h->addWidget(m_btnMax);
    h->addWidget(btnClose);

    bar->installEventFilter(this);
    return bar;
}

void MainWindow::toggleMax()
{
    if (m_maximized) { showNormal(); m_btnMax->setText("\u25a1"); }
    else { showMaximized(); m_btnMax->setText("\u2750"); }
    m_maximized = !m_maximized;
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_titleBar) {
        if (e->type() == QEvent::MouseButtonPress) {
            auto *me = static_cast<QMouseEvent*>(e);
            if (me->button() == Qt::LeftButton) {
                m_dragPos = me->globalPos() - frameGeometry().topLeft();
                return true;
            }
        } else if (e->type() == QEvent::MouseMove) {
            auto *me = static_cast<QMouseEvent*>(e);
            if (me->buttons() & Qt::LeftButton && !m_maximized) {
                move(me->globalPos() - m_dragPos);
                return true;
            }
        } else if (e->type() == QEvent::MouseButtonDblClick) {
            toggleMax();
            return true;
        }
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
    for (int i = 1; i < m_fs->columnCount(); ++i)
        m_tree->hideColumn(i);
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
    btnRun->setStyleSheet("QPushButton{background:#1f9d55; color:#fff;} QPushButton:hover{background:#26b665;}");
    btnNew->setObjectName("ghost");
    btnSave->setObjectName("ghost");
    bh->addWidget(btnNew); bh->addWidget(btnSave); bh->addStretch(); bh->addWidget(btnRun);
    vv->addWidget(bar);

    m_tabs = new QTabWidget;
    m_tabs->setTabsClosable(true);
    vv->addWidget(m_tabs, 1);

    m_out = new QPlainTextEdit;
    m_out->setObjectName("output");
    m_out->setReadOnly(true);
    m_out->setMaximumHeight(160);
    vv->addWidget(m_out);

    hl->addWidget(right, 1);
    rv->addWidget(central, 1);
    setCentralWidget(root);

    m_pyLabel = new QLabel;
    statusBar()->addPermanentWidget(m_pyLabel);
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
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
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

CodeEditor *MainWindow::currentEditor() const
{
    return qobject_cast<CodeEditor*>(m_tabs->currentWidget());
}

void MainWindow::openFileInTab(const QString &path)
{
    for (int i = 0; i < m_tabs->count(); ++i) {
        auto *ed = qobject_cast<CodeEditor*>(m_tabs->widget(i));
        if (ed && QFileInfo(ed->filePath()) == QFileInfo(path)) {
            m_tabs->setCurrentIndex(i); return;
        }
    }
    auto *ed = new CodeEditor;
    ed->loadFile(path);
    int idx = m_tabs->addTab(ed, QFileInfo(path).fileName());
    m_tabs->setCurrentIndex(idx);
}

void MainWindow::newFile()
{
    bool ok = false;
    QString name = QInputDialog::getText(
        this, tr("New File"), tr("File name (.py):"),
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
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << "# " << name << "\n\n";
    f.close();

    if (!m_jro.isNull() && !m_jro.files.contains(name)) {
        m_jro.files << name;
        m_jro.save();
    }
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

void MainWindow::runCurrent()
{
    CodeEditor *ed = currentEditor();
    if (!ed) return;
    if (ed->filePath().isEmpty()) {
        QMessageBox::information(this, tr("Jar-IDE"), tr("Save the file before running."));
        return;
    }
    if (m_pythonExe.isEmpty()) {
        m_out->appendPlainText("[!] Python not found. Downloading installer...");
        PythonChecker *chk = new PythonChecker(this);
        connect(chk, &PythonChecker::message, m_out, &QPlainTextEdit::appendPlainText);
        connect(chk, &PythonChecker::installerReady, chk, &QObject::deleteLater);
        chk->downloadAndOpenInstaller();
        return;
    }
    QString work = QFileInfo(ed->filePath()).absolutePath();
    m_runner->run(m_pythonExe, ed->filePath(), work);
}

void MainWindow::closeTab(int idx)
{
    auto *w = m_tabs->widget(idx);
    m_tabs->removeTab(idx);
    w->deleteLater();
}
