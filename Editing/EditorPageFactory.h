#ifndef EDITORPAGEFACTORY_H
#define EDITORPAGEFACTORY_H

#include "Editing/EditorPage.h"
#include "Editing/EditorPageCreator.h"
#include "Editing/DefaultEditorPageCreator.h"
#include "Editing/CEditorPageCreator.h"

#include <QString>
#include <QMap>

class EditorPageFactory
{
private:
    static QMap<QString, EditorPageCreator*> creators;

    EditorPageFactory();
    ~EditorPageFactory();

public:
    static EditorPage* CreateEditorPage(const QString& fileExtension);
    static QMap<QString, EditorPageCreator*>& Creators() { return creators; }
};

#endif // EDITORPAGEFACTORY_H
