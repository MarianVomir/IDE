#ifndef DEFAULTEDITORPAGECREATOR_H
#define DEFAULTEDITORPAGECREATOR_H

#include "Editing/EditorPageCreator.h"
#include "Editing/DefaultEditorPage.h"

class DefaultEditorPageCreator : public EditorPageCreator
{
public:
    DefaultEditorPageCreator();
    ~DefaultEditorPageCreator();
    virtual EditorPage* CreateEditorPage(void* args);
};

#endif // DEFAULTEDITORPAGECREATOR_H
