#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QHash>

// Lightweight Python syntax highlighter (no external deps).
class PythonHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit PythonHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct Rule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QList<Rule> m_rules;
    QTextCharFormat m_comment;
    QTextCharFormat m_string;
    QRegularExpression m_triSingle;
    QRegularExpression m_triDouble;
};

#endif // PYTHONHIGHLIGHTER_H
