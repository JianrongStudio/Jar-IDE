#include "pythonrunner.h"

PythonRunner::PythonRunner(QObject *parent)
    : QObject(parent)
{
    m_proc.setProcessChannelMode(QProcess::MergedChannels);
    connect(&m_proc, &QProcess::readyReadStandardOutput, this, [this]() {
        emit append(QString::fromLocal8Bit(m_proc.readAll()));
    });
    connect(&m_proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int code, QProcess::ExitStatus) {
        emit append(QStringLiteral("\n[process exited with code %1]\n").arg(code));
        emit finished(code);
    });
}

PythonRunner::~PythonRunner()
{
    stop();
}

void PythonRunner::run(const QString &pythonExe, const QString &scriptPath,
                       const QString &workingDir)
{
    if (isRunning())
        stop();
    m_proc.setWorkingDirectory(workingDir);
    emit append(QStringLiteral("$ %1 %2\n").arg(pythonExe, scriptPath));
    m_proc.start(pythonExe, {scriptPath});
    if (!m_proc.waitForStarted(3000))
        emit append(QStringLiteral("[error] could not start python: %1\n")
                    .arg(m_proc.errorString()));
}

void PythonRunner::stop()
{
    if (m_proc.state() != QProcess::NotRunning) {
        m_proc.kill();
        m_proc.waitForFinished(1500);
    }
}

bool PythonRunner::isRunning() const
{
    return m_proc.state() != QProcess::NotRunning;
}
