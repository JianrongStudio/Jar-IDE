#ifndef JROPROJECT_H
#define JROPROJECT_H

#include <QString>
#include <QStringList>

// ---------------------------------------------------------------
// Jianrong project (.jro) model.
//
// On-disk layout produced by the wizard:
//   <chosen dir>/<project name>/
//       project.jro          <- project manifest
//       src/                <- code files live here
//           main.py
//
// project.jro uses a small human-readable XML-ish manifest:
//   <Jianrong project="MyApp">
//       <python.version="3.12">
//       <file>
//           main.py
//           utils.py
//       </file>
// ---------------------------------------------------------------
struct JroProject
{
    QString name;
    QString pythonVersion = "3.12";
    QStringList files;      // file names listed in the manifest
    QString rootPath;       // absolute project root (folder containing project.jro)

    bool isNull() const { return rootPath.isEmpty(); }

    // Read an existing project.jro.
    static JroProject fromFile(const QString &jroPath);

    // Serialize back to the manifest text format.
    QString toString() const;

    // Save manifest to rootPath/project.jro.
    bool save() const;

    // Create a brand-new Jianrong project on disk under parentDir.
    // Returns the created project root path, or empty on failure.
    static QString createOnDisk(const QString &parentDir, const QString &projectName);

    // Absolute path to the src/ folder.
    QString srcDir() const { return rootPath + "/src"; }
};

#endif // JROPROJECT_H
