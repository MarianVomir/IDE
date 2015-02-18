#ifndef LINENUMBERAREA_H
#define LINENUMBERAREA_H

#include <QPaintEvent>

class EditorPage; // forward declaration because we have circular dependency
#include "Editing/EditorPage.h"

class LineNumberArea : public QWidget
{
private:
    EditorPage* page;
protected:
    void paintEvent(QPaintEvent* event);

public:
    LineNumberArea(EditorPage* page);
    ~LineNumberArea();

    QSize sizeHint() const;
};

#endif // LINENUMBERAREA_H
