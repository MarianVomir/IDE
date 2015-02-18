#include "EditorPageFactory.h"

QMap<QString, EditorPageCreator*> EditorPageFactory::creators;

EditorPageFactory::EditorPageFactory()
{

}

EditorPageFactory::~EditorPageFactory()
{
    // delete the creators
    for (int i = 0; i < creators.keys().size(); i++)
    {
        delete creators[creators.keys()[i]];
    }
}

EditorPage* EditorPageFactory::CreateEditorPage(const QString &fileExtension)
{
    if (creators.contains(fileExtension)) // supported extension, call the appropriate page creator
    {
        return creators[fileExtension]->CreateEditorPage();
    }
    else // default page, either no file extension or unsupported extension
    {
        return creators[""]->CreateEditorPage();
    }
}

