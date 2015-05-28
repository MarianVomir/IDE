#include "DefaultEditorPage.h"

DefaultEditorPage::DefaultEditorPage()
{
}

DefaultEditorPage::~DefaultEditorPage()
{
}

void DefaultEditorPage::PerformAfterSetupOperations()
{
    this->document()->clearUndoRedoStacks();
}

void DefaultEditorPage::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Y && (e->modifiers() & (Qt::ControlModifier)))
    {
        EditorPage::redo();
    }
    else
    {
        EditorPage::keyPressEvent(e);
    }
}

