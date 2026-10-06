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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_pythonExe = PythonChecker::findPython();
    buildUi();
}

void MainWindow::buildUi()
{
    setWindowTitle("Jar-IDE");
    setWindowIcon(QIcon(AppStyle::iconPath("logo.svg")));
    resize(1180, 760);

    auto *central = new QWidget(this);
    auto *hl = new QHBoxLayout(central);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    // ---- sidebar ----
    auto *side = new QFrame;
    side->setObjectName("sidebar");
    side->setFixedWidth(260);
    auto *sv = new QVBoxLayout(side);
    sv->setContentsMargins(10, 12, 10, 10);
    sv->setSpacing(8);

    auto *projTitle = new QLabel("Explorer", side);
    projTitle->setStyleSheet("font-weight:600; font-size:14px; color:#fff;");
    sv->addWidget(projTitle);

    m_tree = new QTreeView(side);
    m_fs = new QFileSystemModel(m_tree);
    m_fs->setNameFilters({"*.py", "*.jro", "*.txt", "*.md"});
    m_fs->setNameFilterDisables(false);
    m_tree->setModel(m_fs);
    m_tree->setHeaderHidden(true);
    for (int i = 1; i < m_fs->columnCount(); ++i)
        m_tree->hideColumn(i);
    sv->addWidget(m_tree);

    hl->addWidget(side);

    // ---- right: toolbar + tabs + output ----
    auto *right = new QWidget;
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(0, 0, 0, 0);
    rv->setSpacing(0);

    auto *bar = new QWidget;
    auto *bh = new QHBoxLayout(bar);
    bh->setContentsMargins(12, 8, 12, 8);
    bh->setSpacing(8);

    auto *btnNew  = new QPushButton(QIcon(AppStyle::iconPath("new-project.svg")), tr("New File"));
    auto *btnSave = new QPushButton(QIcon(AppStyle::iconPath("file.svg")), tr("Save"));
    auto *btnRun  = new QPushButton(QIcon(AppStyle::iconPath("run.svg")), tr("Run"));
    btnRun->setStyleSheet("QPushButton{background:#1f9d55;} QPushButton:hover{background:#26b665;}");
    btnNew->setObjectName("ghost");
    btnSave->setObjectName("ghost");
    bh->addWidget(btnNew);
    bh->addWidget(btnSave);
    bh->addStretch();
    bh->addWidget(btnRun);
    rv->addWidget(bar);

    m_tabs = new QTabWidget;
    m_tabs->setTabsClosable(true);
    rv->addWidget(m_tabs, 1);

    m_out = new QPlainTextEdit;
    m_out->setObjectName("output");
    m_out->setReadOnly(true);
    m_out->setMaximumHeight(170);
    rv->addWidget(m_out);

    hl->addWidget(right, 1);
    setCentralWidget(central);

    // status bar
    m_pyLabel = new QLabel;
    statusBar()->addPermanentWidget(m_pyLabel);
    m_pyLabel->setText(m_pythonExe.isEmpty()
                        ? tr("Python: not found")
                        : tr("Python: %1").arg(m_pythonExe));

    // runner
    m_runner = new PythonRunner(this);
    connect(m_runner, &PythonRunner::append, m_out, &QPlainTextEdit::appendPlainText);

    // connections
    connect(m_tree, &QTreeView::doubleClicked, this, [this](const QModelIndex &idx){
        QString p = m_fs->filePath(idx);
        if (!m_fs->isDir(idx))
            openFileInTab(p);
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
        m_fs->setRootPath(m_projectRoot);
        m_tree->setRootIndex(m_fs->index(m_projectRoot));
        QString jro = QDir(m_projectRoot).filePath("project.jro");
        if (QFileInfo::exists(jro)) {
            m_jro = JroProject::fromFile(jro);
            setWindowTitle(QString("Jar-IDE  —  %1").arg(m_jro.name));
            QString mainPy = QDir(m_jro.srcDir()).filePath("main.py");
            if (QFileInfo::exists(mainPy))
                openFileInTab(mainPy);
        } else {
            setWindowTitle(QString("Jar-IDE  —  %1").arg(fi.fileName()));
        }
    } else {
        m_projectRoot = fi.absolutePath();
        m_fs->setRootPath(m_projectRoot);
        m_tree->setRootIndex(m_fs->index(m_projectRoot));
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
            m_tabs->setCurrentIndex(i);
            return;
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
    if (!ok || name.isEmpty())
        return;
    if (!name.endsWith(".py", Qt::CaseInsensitive))
        name += ".py";

    QString dir = m_jro.isNull() ? m_projectRoot : m_jro.srcDir();
    if (dir.isEmpty())
        dir = m_projectRoot;
    QDir().mkpath(dir);
    QString full = QDir(dir).filePath(name);
    QFile f(full);
    if (!f.exists()) {
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Jar-IDE"), tr("Cannot create file."));
            return;
        }
        f.close();
    }

    if (!m_jro.isNull()) {
        if (!m_jro.files.contains(name)) {
            m_jro.files << name;
            m_jro.save();
        }
    }
    m_fs->setRootPath(m_projectRoot); // refresh
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
    } else {
        ed->saveFile(ed->filePath());
    }
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
