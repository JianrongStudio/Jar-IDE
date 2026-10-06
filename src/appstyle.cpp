#include "appstyle.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace AppStyle {

void apply(QApplication *app)
{
    QFile f(":/res/style.qss");
    if (f.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream ts(&f);
        app->setStyleSheet(ts.readAll());
        f.close();
    } else {
        qWarning() << "[JarIDE] stylesheet not found in resources";
    }
}

QString iconPath(const QString &name)
{
    return QStringLiteral(":/res/icons/") + name;
}

} // namespace AppStyle
