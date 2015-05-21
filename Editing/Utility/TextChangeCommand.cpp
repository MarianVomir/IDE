#include "TextChangeCommand.h"

TextChangeCommand::TextChangeCommand(int from, const QString &removed, const QString &added, QPlainTextEdit *textEdit, QSyntaxHighlighter *highlighter)
    : from(from), removed(removed), added(added), textEdit(textEdit), highlighter(highlighter)
{
    doc = this->textEdit->document();
    firstTime = true;
}

void TextChangeCommand::redo()
{
    if (firstTime)
    {
        firstTime = false;
        return;
    }

    doc->blockSignals(true);
    textEdit->blockSignals(true);

    QString text = doc->toPlainText();
    text.remove(from, removed.length()); // remove text (can be empty)
    text.insert(from, added);            // add text (can be empty)
    doc->setPlainText(text);

    highlighter->rehighlight();

    textEdit->blockSignals(false);
    doc->blockSignals(false);

    qDebug() << "Executed redo: " << from << " " << removed << " " << added;
}

void TextChangeCommand::undo()
{
    doc->blockSignals(true);
    textEdit->blockSignals(true);

    QString text = doc->toPlainText();
    text.remove(from, added.length());  // remove added text (can be empty)
    text.insert(from, removed);         // re-insert removed text (can be empty)
    doc->setPlainText(text);

    highlighter->rehighlight();
    textEdit->blockSignals(false);
    doc->blockSignals(false);

    qDebug() << "Executed undo: " << from << " " << removed << " " << added;
}

TextChangeCommand::~TextChangeCommand()
{

}

