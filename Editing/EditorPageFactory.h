#ifndef EDITORPAGEFACTORY_H
#define EDITORPAGEFACTORY_H

#include <QString>
#include <QMap>

#include "Editing/EditorPage.h"
#include "Editing/EditorPageCreator.h"
#include "Editing/DefaultEditorPageCreator.h"
#include "Editing/CEditorPageCreator.h"

class EditorPageFactory
{
private:
    static QMap<QString, EditorPageCreator*> creators;

    EditorPageFactory();
    ~EditorPageFactory();

public:
    static EditorPage* CreateEditorPage(const QString& fileExtension, void* args);
    static QMap<QString, EditorPageCreator*>& Creators() { return creators; }
};

#endif // EDITORPAGEFACTORY_H
