#include "CEditorPage.h"

CEditorPage::CEditorPage()
{
    highlighter = new CSyntaxHighlighter(this->document());
    completer = new QCompleter(this);

    completer->setWidget(this);

    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));

    QStringListModel* l = new QStringListModel(QStringList(Ckeywords.toList()));

    completer->setModel(l);
}

CEditorPage::~CEditorPage()
{
    delete highlighter;
    delete completer;
}

void CEditorPage::insertCompletion(QString completion)
{
    if (completer->widget() != this)
    {
        return;
    }

    QTextCursor tc = textCursor();

    int remaining = completion.size() - completer->completionPrefix().size();
/*
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
*/
    tc.insertText(completion.right(remaining));

    setTextCursor(tc);
}

QString CEditorPage::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}

void CEditorPage::focusInEvent(QFocusEvent *e)
{
    if (completer != NULL)
        completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CEditorPage::keyPressEvent(QKeyEvent *e)
{
    if (completer != NULL && completer->popup()->isVisible())
    {
        switch(e->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if (completer == NULL || !isShortcut)
    {
        QPlainTextEdit::keyPressEvent(e);
    }
    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (completer == NULL || (ctrlOrShift && e->text().isEmpty()))
         return;

     static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();

     if (!isShortcut && (hasModifier || e->text().isEmpty() || eow.contains(e->text().right(1)) || completionPrefix.length() < 1))
     {
         completer->popup()->hide();
         return;
     }

     if (completionPrefix != completer->completionPrefix()) {
         completer->setCompletionPrefix(completionPrefix);
         completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
     }


    QRect cr = cursorRect();
    cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());

    completer->complete(cr);
}

QString CEditorPage::prefixTabs()
{
    QString s = "";
    int pos = textCursor().position();
    while (pos >= 0 && this->toPlainText()[pos] != '\n') pos--;
    qDebug() << "Pos of endl: " << pos;
    pos++;
    while (this->toPlainText()[pos] == '\t') pos++, s += "\t";
    qDebug() << "Pos after tabs: " << pos;

    return s;
}
