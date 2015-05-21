#include "CEditorPage.h"

CEditorPage::CEditorPage()
{
    highlighter = new CSyntaxHighlighter(this->document());
    completer = new QCompleter(this);
    parser = new CParser(this);

    completer->setWidget(this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setModelSorting(QCompleter::UnsortedModel);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(DisplayTooltipForDiagnosticUnderCursor()));

    completerModel = new QStringListModel();
    completer->setModel(completerModel);

    this->setMouseTracking(true);

    errorFormat.setFontWeight(QFont::Bold);
    errorFormat.setFontUnderline(true);
    errorFormat.setUnderlineColor(QColor(255, 0, 0));
    standardFormat.setForeground(QBrush(QColor(0, 0, 0)));
    warningFormat.setFontUnderline(true);
    warningFormat.setUnderlineColor(QColor(255, 255, 0));

    undoStack.setUndoLimit(300);
    connect(this->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(contentsChanged(int,int,int)));
}

CEditorPage::~CEditorPage()
{
    delete completerModel;
    delete highlighter;
    delete completer;
    delete parser;
}

void CEditorPage::DisplayTooltip(const QTextCursor& cursor, const QPoint& point)
{
    bool messageShown = false;
    if (!cursor.isNull())
    {
        int pos = cursor.position();
        for (DiagnosticDTO& diag : this->diags)
        {
            int diagStart = diag.offset;
            int diagEnd = diagStart + diag.length;

            if (pos >= diagStart && pos <= diagEnd)
            {
                QToolTip::showText(point, diag.message, this);
                messageShown = true;
            }
        }
    }

    if (!messageShown)
    {
        QToolTip::showText(point, "", this);
    }
}

void CEditorPage::DisplayTooltipForDiagnosticUnderCursor()
{
    QTextCursor cursor = this->textCursor();
    DisplayTooltip(cursor, this->viewport()->mapToGlobal(this->cursorRect().topLeft() + QPoint(0, -50)));
}

void CEditorPage::mouseMoveEvent(QMouseEvent* e)
{
    QTextCursor cursor = cursorForPosition(e->pos());
    DisplayTooltip(cursor, this->mapToGlobal(e->pos()));

    QPlainTextEdit::mouseMoveEvent(e);
}

void CEditorPage::ShowDiagnostics(std::vector<DiagnosticDTO> diags)
{
    this->document()->blockSignals(true);
    this->blockSignals(true);

    // stop this text page from emitting any signals
    // what we are interested in is to not emit textChanged() when setting the text format

    int verticalScrollBarValue = this->verticalScrollBar()->value();
    // get the current value of the vertical scrollbar
    // we need this because the text page autoscrolls where the text was last changed

    this->diags = diags;
    // keep the new diagnostics

    QTextCursor c = this->textCursor();
    QTextCursor cursor = this->textCursor();

    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.setCharFormat(standardFormat);

    this->setTextCursor(cursor);
    for (DiagnosticDTO& diag : diags)
    {
        cursor = this->textCursor();

        cursor.setPosition(diag.offset, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, diag.length);
        if (diag.severity == CXDiagnostic_Error || diag.severity == CXDiagnostic_Fatal)
            cursor.setCharFormat(errorFormat);
        if (diag.severity == CXDiagnostic_Warning)
            cursor.setCharFormat(warningFormat);

        this->setTextCursor(cursor);
    }
    this->setTextCursor(c);

    this->verticalScrollBar()->setValue(verticalScrollBarValue);

    this->blockSignals(false);
    this->document()->blockSignals(false);
}

void CEditorPage::SetCompletionModel(QStringList l)
{
 /*   qDebug() << l;
    qDebug() << "\n\n\n";
   */ QMutexLocker lock(&mtx_CompleterModelIsBeingAccessed);

    completerModel->setStringList(l);

    this->PopupCompleter();
}

void CEditorPage::insertCompletion(QString completion)
{
    if (completer->widget() != this)
        return;

    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);

    tc.insertText(completion);

    setTextCursor(tc);
}

QString CEditorPage::textUnderCursor(int moveLeft) const
{
    QTextCursor tc = textCursor();

    if (moveLeft > 0)
        tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, moveLeft);

    tc.select(QTextCursor::WordUnderCursor);

    return tc.selectedText();
}

void CEditorPage::focusInEvent(QFocusEvent *e)
{
    if (completer != NULL)
        completer->setWidget(this);
    QPlainTextEdit::focusInEvent(e);
}

void CEditorPage::contentsChanged(int from, int removed, int added)
{
    QString removedText = currentText.mid(from, removed);

    currentText = document()->toPlainText();
    QString addedText = currentText.mid(from, added);

    TextChangeCommand* cmd = new TextChangeCommand(from, removedText, addedText, this);
    qDebug() << " PUSHED " << from << " " << removedText << " " << addedText;
    undoStack.push(cmd);
}

void CEditorPage::keyPressEvent(QKeyEvent *e)
{
    if (completer && completer->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
       switch (e->key()) {
       case Qt::Key_Enter:
       case Qt::Key_Return:
       case Qt::Key_Escape:
       case Qt::Key_Tab:
       case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
       default:
           break;
       }
    }

    /** UNDO REDO FRAMEWORK
     */
    bool isControlModifier = (e->modifiers() & Qt::ControlModifier);
    bool isShiftModifier = (e->modifiers() & Qt::ShiftModifier);

    if (isControlModifier && isShiftModifier && e->key() == Qt::Key_Z) // disallow default ctrl-shift-Z redo
        return;

    if (isControlModifier)
    {
        if (e->key() == Qt::Key_Z)  // ctrl-Z -> undo
        {
            if (undoStack.canUndo())
                undoStack.undo();
            return;
        }
        else if (e->key() == Qt::Key_Y) // ctrl-Y -> redo
        {
            if (undoStack.canRedo())
                undoStack.redo();
            return;
        }
    }
    /** End of UNDO REDO FRAMEWORK **/

    QPlainTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (!completer || (ctrlOrShift && e->text().isEmpty()))
        return;

     static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();

     if (eow.contains(completionPrefix))
     {
         completionPrefix = textUnderCursor(1);
     }

     this->completer->setCompletionMode(QCompleter::PopupCompletion);

     if (/*!isShortcut && */(hasModifier || e->text().isEmpty()|| completionPrefix.length() < 1
                       /*|| eow.contains(e->text().right(1))*/))
     {
         completer->popup()->hide();
         return;
     }

     if (completionPrefix != completer->completionPrefix())
     {
         completer->setCompletionPrefix(completionPrefix);
         completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
     }

     parser->GenerateCompleterSuggestions();
}

void CEditorPage::PopupCompleter()
{
    QRect cr = cursorRect();
    cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());
    completer->complete(cr);
}
