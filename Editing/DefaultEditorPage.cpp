#include "DefaultEditorPage.h"

DefaultEditorPage::DefaultEditorPage(const QString& contents)
{
    this->setPlainText(contents);
    this->document()->clearUndoRedoStacks();
}

DefaultEditorPage::~DefaultEditorPage()
{
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

