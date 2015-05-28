#ifndef EDITORPAGECREATOR_H
#define EDITORPAGECREATOR_H

#include "Editing/EditorPage.h"

class EditorPageCreator
{
public:
    EditorPageCreator();
    virtual ~EditorPageCreator() = 0;
    virtual EditorPage* CreateEditorPage() = 0;
};


#endif // EDITORPAGECREATOR_H
