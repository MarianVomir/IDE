#ifndef DEFAULTEDITORPAGE_H
#define DEFAULTEDITORPAGE_H

#include "Editing/EditorPage.h"
#include <QTextEdit>

/**
This is a simple EditorPage used when no specialized page is available for a given file type/extension.
**/
class DefaultEditorPage : public EditorPage
{
private:

public:
    DefaultEditorPage(QWidget* parent = 0);
    virtual ~DefaultEditorPage();
};

#endif // DEFAULTEDITORPAGE_H
