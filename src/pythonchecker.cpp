#include "pythonchecker.h"

#include <QProcess>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QUrl>
#include <QFile>
#include <QDebug>

namespace {
const QString INSTALLER_URL =
    "https://www.python.org/ftp/python/3.12.0/python-3.12.0-amd64.exe";
}

PythonChecker::PythonChecker(QObject *parent)
    : QObject(parent)
{
    m_nam.setAutoDeleteReplies(true);
}

QString PythonChecker::runVersion(const QString &exe, const QStringList &args)
{
    QProcess p;
    p.start(exe, args);
    if (!p.waitForStarted(2000))
        return QString();
    p.waitForFinished(2500);
    QString out = QString::fromLocal8Bit(p.readAllStandardOutput()) +
                  QString::fromLocal8Bit(p.readAllStandardError());
    return out;
}

QString PythonChecker::findPython()
{
    // 1) PATH: python / py launcher
    for (const QString &exe : {QStringLiteral("python"), QStringLiteral("py")}) {
        QString args = (exe == QLatin1String("py")) ? QStringLiteral("-3 --version")
                                                    : QStringLiteral("--version");
        QString out = runVersion(exe, args.split(' '));
        if (out.contains("Python 3", Qt::CaseInsensitive))
            return exe;
    }

    // 2) Common install locations on Windows
    const QStringList roots = {
        QDir::rootPath() + "Python312/python.exe",
        QDir::rootPath() + "Python311/python.exe",
        QDir::rootPath() + "Python310/python.exe",
        qEnvironmentVariable("LOCALAPPDATA") + "/Programs/Python/Python312/python.exe",
        qEnvironmentVariable("LOCALAPPDATA") + "/Programs/Python/Python311/python.exe",
    };
    for (const QString &candidate : roots) {
        if (QFileInfo::exists(candidate))
            return candidate;
    }
    return QString();
}

bool PythonChecker::isAvailable()
{
    return !findPython().isEmpty();
}

void PythonChecker::downloadAndOpenInstaller()
{
    emit message(tr("Python not detected. Downloading official installer (3.12.0)..."));

    QNetworkRequest req{QUrl(INSTALLER_URL)};
    req.setHeader(QNetworkRequest::UserAgentHeader, "Jar-IDE/1.0");
    QNetworkReply *reply = m_nam.get(req);

    connect(reply, &QNetworkReply::downloadProgress, this,
            [this](qint64 got, qint64 total) {
        if (total > 0)
            emit progress(int(got * 100 / total));
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() != QNetworkReply::NoError) {
            emit message(tr("Download failed: %1").arg(reply->errorString()));
            emit installerReady(QString());
            reply->deleteLater();
            return;
        }
        const QString dir =
            QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        const QString path = QDir(dir).filePath("python-3.12.0-amd64.exe");
        QFile out(path);
        if (out.open(QIODevice::WriteOnly)) {
            out.write(reply->readAll());
            out.close();
            emit message(tr("Installer saved. Launching setup..."));
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
            emit installerReady(path);
        } else {
            emit message(tr("Could not write installer to temp folder."));
            emit installerReady(QString());
        }
        reply->deleteLater();
    });
}
