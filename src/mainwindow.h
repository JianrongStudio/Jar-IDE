#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "jroproject.h"

class QTreeView;
class QFileSystemModel;
class QTabWidget;
class CodeEditor;
class QPlainTextEdit;
class PythonRunner;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

    // Open a folder as project root, or a single file.
    void openPath(const QString &path);

private slots:
    void openFileInTab(const QString &path);
    void newFile();
    void saveCurrent();
    void runCurrent();
    void closeTab(int idx);

private:
    void buildUi();
    CodeEditor *currentEditor() const;

    QTreeView        *m_tree = nullptr;
    QFileSystemModel *m_fs   = nullptr;
    QTabWidget       *m_tabs = nullptr;
    QPlainTextEdit   *m_out  = nullptr;
    PythonRunner     *m_runner = nullptr;
    QLabel           *m_pyLabel = nullptr;

    QString  m_projectRoot;
    JroProject m_jro;
    QString  m_pythonExe;
};

#endif // MAINWINDOW_H
