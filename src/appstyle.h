#ifndef APPSTYLE_H
#define APPSTYLE_H

#include <QApplication>
#include <QString>

// Loads the bundled QSS theme and provides tiny shared helpers.
namespace AppStyle {
    // Apply RES/style.qss to the whole application.
    void apply(QApplication *app);
    // Convenience: resolve an icon resource path like "logo.svg".
    QString iconPath(const QString &name);
}

#endif // APPSTYLE_H
