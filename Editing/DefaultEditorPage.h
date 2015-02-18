#ifndef DEFAULTEDITORPAGE_H
#define DEFAULTEDITORPAGE_H

#include "Editing/EditorPage.h"
#include <QTextEdit>

class DefaultEditorPage : public EditorPage
{
private:

public:
    DefaultEditorPage(QWidget* parent = 0);
    virtual ~DefaultEditorPage();
};

#endif // DEFAULTEDITORPAGE_H
