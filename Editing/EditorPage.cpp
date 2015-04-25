#include "Editing/EditorPage.h"

EditorPage::EditorPage(QWidget* parent) : QPlainTextEdit(parent)
{
    this->setLineWrapMode(QPlainTextEdit::NoWrap);

    // set page font
    QFont font;

    font.setFamily("Consolas");
    font.setWeight(QFont::Normal);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(10);
    this->setFont(font);

    // set tab to be 4 characters
    int tabStop = 4;
    QFontMetrics metrics(font);
    this->setTabStopWidth(tabStop * metrics.width(' '));

    // line numbering
    lineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    m_countCache = QPair<int, int>(-1, -1);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}
EditorPage::~EditorPage()
{
    delete lineNumberArea;
}

int EditorPage::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax<int>(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        digits++;
    }
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}
void EditorPage::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}
void EditorPage::updateLineNumberArea(const QRect& rect, int dy)
{
    if (dy != 0)
    {
        lineNumberArea->scroll(0, dy);
    }
    else if (m_countCache.first != blockCount() || m_countCache.second != textCursor().block().lineCount())
    {
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
        m_countCache.first = blockCount();
        m_countCache.second = textCursor().block().lineCount();
    }

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}
void EditorPage::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}
void EditorPage::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(250, 250, 250);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}
void EditorPage::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
