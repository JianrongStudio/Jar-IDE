#include <QApplication>
#include <QDir>
#include <QFileInfo>

#include "appstyle.h"
#include "pythonchecker.h"
#include "welcomewindow.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    // High-DPI friendly on Win7~Win11
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    QApplication app(argc, argv);
    app.setApplicationName("Jar-IDE");
    app.setApplicationDisplayName("Jar-IDE");
    AppStyle::apply(&app);

    // Startup requirement: detect Python; if missing, download & open installer.
    if (!PythonChecker::isAvailable()) {
        PythonChecker *chk = new PythonChecker(&app);
        QObject::connect(chk, &PythonChecker::installerReady,
                         chk, &QObject::deleteLater);
        chk->downloadAndOpenInstaller();
    }

    WelcomeWindow welcome;
    MainWindow window;

    QObject::connect(&welcome, &WelcomeWindow::openFolder, [&](const QString &p){
        window.openPath(p);
        window.show();
        welcome.close();
    });
    QObject::connect(&welcome, &WelcomeWindow::openFile, [&](const QString &p){
        window.openPath(p);
        window.show();
        welcome.close();
    });
    QObject::connect(&welcome, &WelcomeWindow::openProject, [&](const QString &root){
        window.openPath(root);
        window.show();
        welcome.close();
    });

    welcome.show();
    return app.exec();
}
