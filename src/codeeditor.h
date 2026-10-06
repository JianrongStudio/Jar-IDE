#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>

class LineNumberArea;

// Editor with a gutter line-number panel and python highlighting.
class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();

    void loadFile(const QString &path);
    bool saveFile(const QString &path);
    QString filePath() const { return m_path; }
    void setFilePath(const QString &p) { m_path = p; }
    bool isModified() const { return m_modified; }
    void setModified(bool m);

protected:
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);

private:
    QWidget *m_lineNumber;
    QString  m_path;
    bool     m_modified = false;
};

#endif // CODEEDITOR_H
