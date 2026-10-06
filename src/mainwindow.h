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
class QLineEdit;
class PythonRunner;
class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void openPath(const QString &path);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void openFileInTab(const QString &path);
    void newFile();
    void saveCurrent();
    void runCurrent();
    void closeTab(int idx);
    void updateTabModified(bool m);
    void runCommand(const QString &cmd);
    void openSettings();

private:
    void buildUi();
    QWidget *buildTitleBar();
    CodeEditor *currentEditor() const;
    void toggleMax();
    int  editorIndex(CodeEditor *ed) const;

    QTreeView        *m_tree = nullptr;
    QFileSystemModel *m_fs   = nullptr;
    QTabWidget       *m_tabs = nullptr;
    QPlainTextEdit   *m_out  = nullptr;
    QLineEdit        *m_termInput = nullptr;
    PythonRunner     *m_runner = nullptr;
    QLabel           *m_pyLabel = nullptr;
    QLabel           *m_titleLabel = nullptr;
    QPushButton      *m_btnMax = nullptr;
    QWidget          *m_titleBar = nullptr;

    QString  m_projectRoot;
    QString  m_explorerRoot;
    JroProject m_jro;
    QString  m_pythonExe;
    QString  m_termCwd;
    QPoint   m_dragPos;
    bool     m_maximized = false;
};

#endif // MAINWINDOW_H
