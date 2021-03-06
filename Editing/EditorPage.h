#ifndef EDITORPAGE
#define EDITORPAGE

#include <QPlainTextEdit>
#include <QWidget>
#include <QRect>
#include <QPainter>
#include <QTextBlock>
#include <QPair>

#include "Global.h"

class LineNumberArea; // forward declaration because we have circular dependency
#include "Editing/Utility/LineNumberArea.h"

class EditorPage : public QPlainTextEdit
{
    Q_OBJECT

public:
    EditorPage(QWidget* parent = 0);
    virtual ~EditorPage() = 0;

// line numbering
public:
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();

    virtual void PerformAfterSetupOperations() = 0;

protected:
    void resizeEvent(QResizeEvent *);

public slots:
   // void keyPressEvent(QKeyEvent* e);

private slots:
    void updateLineNumberAreaWidth(int);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect& rect, int dy);

private:
    LineNumberArea* lineNumberArea;
    QPair<int, int> m_countCache;
};

#endif // EDITORPAGE

