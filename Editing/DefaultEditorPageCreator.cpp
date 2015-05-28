#include "DefaultEditorPageCreator.h"

DefaultEditorPageCreator::DefaultEditorPageCreator()
{

}

DefaultEditorPageCreator::~DefaultEditorPageCreator()
{

}

EditorPage *DefaultEditorPageCreator::CreateEditorPage()
{
    return new DefaultEditorPage();
}

