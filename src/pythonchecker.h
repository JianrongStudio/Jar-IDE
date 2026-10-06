#ifndef PYTHONCHECKER_H
#define PYTHONCHECKER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

// Detects a usable Python 3 interpreter. If none is found, downloads the
// official python-3.12.0-amd64 installer and launches it for the user.
class PythonChecker : public QObject
{
    Q_OBJECT
public:
    explicit PythonChecker(QObject *parent = nullptr);

    // Returns the python executable if a working interpreter is found,
    // otherwise an empty string.
    static QString findPython();

    // True when findPython() is non-empty.
    static bool isAvailable();

    // Download the official installer to a temp folder and open it.
    void downloadAndOpenInstaller();

signals:
    // progress 0..100 during download; done message afterwards.
    void progress(int percent);
    void message(const QString &text);
    void installerReady(const QString &localPath);

private:
    QNetworkAccessManager m_nam;
    static QString runVersion(const QString &exe, const QStringList &args);
};

#endif // PYTHONCHECKER_H
