#include "DefaultEditorPageCreator.h"

DefaultEditorPageCreator::DefaultEditorPageCreator()
{

}

DefaultEditorPageCreator::~DefaultEditorPageCreator()
{

}

EditorPage *DefaultEditorPageCreator::CreateEditorPage(void *args)
{
    QString* contents = static_cast<QString*>(args);
    return new DefaultEditorPage(*contents);
}

