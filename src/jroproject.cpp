#include "jroproject.h"

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QDebug>

QString JroProject::toString() const
{
    QString out;
    QTextStream ts(&out);
    ts << "<Jianrong project=\"" << name << "\">\n";
    ts << "  <python.version=\"" << pythonVersion << "\">\n";
    ts << "  <file>\n";
    for (const QString &f : files)
        ts << "    " << f << "\n";
    ts << "  </file>\n";
    ts << "</Jianrong>\n";
    return out;
}

bool JroProject::save() const
{
    if (rootPath.isEmpty())
        return false;
    QFile f(rootPath + "/project.jro");
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << toString();
    f.close();
    return true;
}

JroProject JroProject::fromFile(const QString &jroPath)
{
    JroProject p;
    QFile f(jroPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return p;
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    const QString text = ts.readAll();
    f.close();

    QDir d(jroPath);
    p.rootPath = d.absolutePath();   // folder that holds project.jro

    QRegularExpression nameRe("project=\"([^\"]*)\"");
    auto m = nameRe.match(text);
    if (m.hasMatch())
        p.name = m.captured(1);

    QRegularExpression verRe("python.version=\"([^\"]*)\"");
    m = verRe.match(text);
    if (m.hasMatch())
        p.pythonVersion = m.captured(1);

    // Collect file names listed between <file> and </file>.
    int s = text.indexOf("<file>");
    int e = text.indexOf("</file>");
    if (s >= 0 && e > s) {
        QString block = text.mid(s + 6, e - s - 6);
        for (const QString &line : block.split('\n')) {
            QString t = line.trimmed();
            if (t.isEmpty() || t.startsWith('<') || t.startsWith('('))
                continue;
            if (t.contains('.'))          // looks like a filename
                p.files << t;
        }
    }
    return p;
}

QString JroProject::createOnDisk(const QString &parentDir, const QString &projectName)
{
    QDir parent(parentDir);
    const QString root = parent.filePath(projectName);
    if (!parent.mkpath(projectName))
        return QString();

    QDir rootDir(root);
    rootDir.mkdir("src");

    // Seed main.py
    QFile mainPy(rootDir.filePath("src/main.py"));
    if (mainPy.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream ts(&mainPy);
        ts.setCodec("UTF-8");
        ts << "#!/usr/bin/env python3\n";
        ts << "# " << projectName << " — created by Jar-IDE\n\n";
        ts << "def main():\n";
        ts << "    print(\"Hello from " << projectName << "!\")\n\n";
        ts << "if __name__ == \"__main__\":\n";
        ts << "    main()\n";
        mainPy.close();
    }

    JroProject p;
    p.rootPath = QDir(root).absolutePath();
    p.name = projectName;
    p.pythonVersion = "3.12";
    p.files << "main.py";
    p.save();

    return p.rootPath;
}
