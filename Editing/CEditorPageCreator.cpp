#include "CEditorPageCreator.h"

CEditorPageCreator::CEditorPageCreator()
{

}

CEditorPageCreator::~CEditorPageCreator()
{

}

EditorPage* CEditorPageCreator::CreateEditorPage()
{
    return new CEditorPage();
}

