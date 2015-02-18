#ifndef EDITORPAGE
#define EDITORPAGE

#include <QPlainTextEdit>
#include <QWidget>
#include <QRect>
#include <QPainter>
#include <QTextBlock>
#include <QPair>

class LineNumberArea; // forward declaration because we have circular dependency
#include "Editing/Utility/LineNumberArea.h"

class EditorPage : public QPlainTextEdit
{
public:
    EditorPage(QWidget* parent = 0);
    virtual ~EditorPage() = 0;

// line numbering
public:
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    LineNumberArea* lineNumberArea;
    QPair<int, int> m_countCache;
};

#endif // EDITORPAGE

