#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H

#include <QObject>
#include <QProcess>
#include <QString>

// Runs a python script in a QProcess and streams stdout/stderr out.
class PythonRunner : public QObject
{
    Q_OBJECT
public:
    explicit PythonRunner(QObject *parent = nullptr);
    ~PythonRunner();

    // Run scriptPath with the detected python interpreter.
    void run(const QString &pythonExe, const QString &scriptPath,
             const QString &workingDir);
    void stop();
    bool isRunning() const;

signals:
    void append(const QString &text);
    void finished(int code);

private:
    QProcess m_proc;
};

#endif // PYTHONRUNNER_H
