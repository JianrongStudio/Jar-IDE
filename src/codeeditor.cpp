#include "codeeditor.h"
#include "pythonhighlighter.h"

#include <QPainter>
#include <QPaintEvent>
#include <QFile>
#include <QTextStream>
#include <QFontMetrics>

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(CodeEditor *ed) : QWidget(ed), editor(ed) {}
    QSize sizeHint() const override { return QSize(editor->lineNumberAreaWidth(), 0); }
protected:
    void paintEvent(QPaintEvent *e) override { editor->lineNumberAreaPaintEvent(e); }
private:
    CodeEditor *editor;
};

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    m_lineNumber = new LineNumberArea(this);
    new PythonHighlighter(document());

    connect(this, &CodeEditor::blockCountChanged,
            this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest,
            this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::modificationChanged,
            this, [this](bool m){ setModified(m); });

    updateLineNumberAreaWidth(0);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) { max /= 10; ++digits; }
    int space = 14 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy) m_lineNumber->scroll(0, dy);
    else    m_lineNumber->update(0, rect.y(), m_lineNumber->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    m_lineNumber->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_lineNumber);
    painter.fillRect(event->rect(), QColor("#0a0e14"));
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            painter.setPen(QColor("#5b6b7f"));
            painter.drawText(0, top, m_lineNumber->width() - 6,
                             fontMetrics().height(), Qt::AlignRight,
                             QString::number(blockNumber + 1));
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

void CodeEditor::setModified(bool m)
{
    m_modified = m;
}

void CodeEditor::loadFile(const QString &path)
{
    QFile f(path);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(&f);
        ts.setCodec("UTF-8");
        setPlainText(ts.readAll());
        f.close();
        m_path = path;
        document()->setModified(false);
        m_modified = false;
    }
}

bool CodeEditor::saveFile(const QString &path)
{
    QFile f(path.isEmpty() ? m_path : path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream ts(&f);
    ts.setCodec("UTF-8");
    ts << toPlainText();
    f.close();
    m_path = f.fileName();
    document()->setModified(false);
    m_modified = false;
    return true;
}
