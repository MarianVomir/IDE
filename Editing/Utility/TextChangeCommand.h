#ifndef TEXTCHANGECOMMAND_H
#define TEXTCHANGECOMMAND_H

#include <QUndoCommand>
#include <QTextDocument>
#include <QDebug>
#include <QPlainTextEdit>

class TextChangeCommand : public QUndoCommand
{
private:
    int from;
    QString removed;
    QString added;
    QTextDocument* doc;
    QPlainTextEdit* textEdit;

    bool firstTime;

public:
    TextChangeCommand(int from, const QString& removed, const QString& added, QPlainTextEdit *textEdit);
    ~TextChangeCommand();

    virtual void undo();
    virtual void redo();
};

#endif // TEXTCHANGECOMMAND_H
