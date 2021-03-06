#ifndef DEFAULTEDITORPAGE_H
#define DEFAULTEDITORPAGE_H

#include <QTextEdit>

#include "Editing/EditorPage.h"

/**
This is a simple EditorPage used when no specialized page is available for a given file type/extension.
**/
class DefaultEditorPage : public EditorPage
{
private:

public:
    DefaultEditorPage();
    virtual ~DefaultEditorPage();

    void PerformAfterSetupOperations();
    void keyPressEvent(QKeyEvent* e);
};

#endif // DEFAULTEDITORPAGE_H
