#ifndef CEDITORPAGECREATOR_H
#define CEDITORPAGECREATOR_H

#include "Editing/EditorPageCreator.h"

class CEditorPageCreator : public EditorPageCreator
{
public:
    CEditorPageCreator();
    ~CEditorPageCreator();
    virtual EditorPage* CreateEditorPage();
};

#endif // CEDITORPAGECREATOR_H
