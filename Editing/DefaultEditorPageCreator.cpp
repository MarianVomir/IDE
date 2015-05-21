#include "DefaultEditorPageCreator.h"

DefaultEditorPageCreator::DefaultEditorPageCreator()
{

}

DefaultEditorPageCreator::~DefaultEditorPageCreator()
{

}

EditorPage *DefaultEditorPageCreator::CreateEditorPage(void *args)
{
    return new DefaultEditorPage();
}

