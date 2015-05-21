#ifndef TEXTCHANGECOMMAND_H
#define TEXTCHANGECOMMAND_H

#include <QUndoCommand>
#include <QTextDocument>
#include <QPlainTextEdit>
#include <QSyntaxHighlighter>

class TextChangeCommand : public QUndoCommand
{
private:
    int from;
    QString removed;
    QString added;
    QTextDocument* doc;
    QPlainTextEdit* textEdit;
    QSyntaxHighlighter* highlighter;

    bool firstTime;

public:
    TextChangeCommand(int from, const QString& removed, const QString& added, QPlainTextEdit *textEdit, QSyntaxHighlighter* highlighter);
    ~TextChangeCommand();

    virtual void undo();
    virtual void redo();
};

#endif // TEXTCHANGECOMMAND_H
