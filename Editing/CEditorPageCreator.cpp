#include "CEditorPageCreator.h"

CEditorPageCreator::CEditorPageCreator()
{

}

CEditorPageCreator::~CEditorPageCreator()
{

}

EditorPage* CEditorPageCreator::CreateEditorPage(void* args)
{
    QString* contents = static_cast<QString*>(args);
    return new CEditorPage(*contents);
}

