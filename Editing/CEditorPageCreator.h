#ifndef CEDITORPAGECREATOR_H
#define CEDITORPAGECREATOR_H

#include "Editing/EditorPageCreator.h"
#include "Editing/CEditorPage.h"

class CEditorPageCreator : public EditorPageCreator
{
public:
    CEditorPageCreator();
    ~CEditorPageCreator();
    virtual EditorPage* CreateEditorPage();
};

#endif // CEDITORPAGECREATOR_H
