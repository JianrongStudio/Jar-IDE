#include "pythonhighlighter.h"

PythonHighlighter::PythonHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    QTextCharFormat kw;    kw.setForeground(QColor("#c586e0")); kw.setFontWeight(QFont::Bold);
    QTextCharFormat built; built.setForeground(QColor("#4ec9b0"));
    QTextCharFormat num;   num.setForeground(QColor("#b5cea8"));
    QTextCharFormat func;  func.setForeground(QColor("#dcdcaa"));
    m_comment.setForeground(QColor("#6a9955"));
    m_string.setForeground(QColor("#ce9178"));

    const QStringList keywords = {
        "and","as","assert","async","await","break","class","continue","def",
        "del","elif","else","except","finally","for","from","global","if",
        "import","in","is","lambda","nonlocal","not","or","pass","raise",
        "return","try","while","with","yield","True","False","None"
    };
    for (const QString &k : keywords) {
        Rule r;
        r.pattern = QRegularExpression("\\b" + k + "\\b");
        r.format = kw;
        m_rules << r;
    }
    const QStringList builtins = {
        "print","len","range","str","int","float","list","dict","set","tuple",
        "self","super","open","input","type","enumerate","zip","map","filter"
    };
    for (const QString &b : builtins) {
        Rule r;
        r.pattern = QRegularExpression("\\b" + b + "\\b");
        r.format = built;
        m_rules << r;
    }
    {
        Rule r; r.pattern = QRegularExpression("\\b\\d+(\\.\\d+)?\\b"); r.format = num;
        m_rules << r;
    }
    {
        Rule r; r.pattern = QRegularExpression("\\b[A-Za-z_]\\w*(?=\\()"); r.format = func;
        m_rules << r;
    }
    {
        Rule r; r.pattern = QRegularExpression("#[^\\n]*"); r.format = m_comment;
        m_rules << r;
    }
    {
        Rule r; r.pattern = QRegularExpression("\"([^\"\\\\]|\\\\.)*\""); r.format = m_string;
        m_rules << r;
    }
    {
        Rule r; r.pattern = QRegularExpression("'([^'\\\\]|\\\\.)*'"); r.format = m_string;
        m_rules << r;
    }

    m_triSingle = QRegularExpression("'''");
    m_triDouble = QRegularExpression('"""');
}

void PythonHighlighter::highlightBlock(const QString &text)
{
    for (const Rule &r : m_rules) {
        QRegularExpressionMatchIterator it = r.pattern.globalMatch(text);
        while (it.hasNext()) {
            QRegularExpressionMatch m = it.next();
            setFormat(m.capturedStart(), m.capturedLength(), r.format);
        }
    }

    // Triple-quoted multi-line strings (simple state tracking)
    setCurrentBlockState(0);
    QRegularExpression tri = text.contains("'''") ? m_triSingle : m_triDouble;
    int start = 0;
    if (previousBlockState() != 1)
        start = text.indexOf(tri);
    while (start >= 0) {
        int end = text.indexOf(tri, start + 3);
        if (end == -1) {
            setCurrentBlockState(1);
            setFormat(start, text.length() - start, m_string);
            break;
        } else {
            setFormat(start, end - start + 3, m_string);
            start = text.indexOf(tri, end + 3);
        }
    }
}
