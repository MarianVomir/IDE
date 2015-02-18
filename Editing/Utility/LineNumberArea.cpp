#include "LineNumberArea.h"

LineNumberArea::LineNumberArea(EditorPage *page)
    : QWidget((QWidget*)page), page(page)
{

}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    page->lineNumberAreaPaintEvent(event);
}

LineNumberArea::~LineNumberArea()
{

}

QSize LineNumberArea::sizeHint() const
{
    return QSize(page->lineNumberAreaWidth(), 0);
}

