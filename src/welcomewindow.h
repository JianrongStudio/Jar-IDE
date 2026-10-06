#ifndef WELCOMEWINDOW_H
#define WELCOMEWINDOW_H

#include <QWidget>

class QLabel;
class QPushButton;
class PythonChecker;

// First-run / open-project welcome screen. Animated intro, three actions:
//   Open folder | Open file | New Jianrong project
class WelcomeWindow : public QWidget
{
    Q_OBJECT
public:
    explicit WelcomeWindow(QWidget *parent = nullptr);

signals:
    void openFolder(const QString &path);
    void openFile(const QString &path);
    void openProject(const QString &projectRoot); // Jianrong project root

protected:
    void showEvent(QShowEvent *e) override;
    bool eventFilter(QObject *o, QEvent *e) override;

private slots:
    void chooseFolder();
    void chooseFile();
    void newProject();

private:
    QWidget *makeCard(const QString &icon, const QString &title,
                     const QString &desc, const char *slot);
    QLabel *m_status = nullptr;
};

#endif // WELCOMEWINDOW_H
