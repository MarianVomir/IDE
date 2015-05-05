#include "CEditorPage.h"

CEditorPage::CEditorPage()
{
    highlighter = new CSyntaxHighlighter(this->document());
    completer = new QCompleter(this);
    parser = new CParser(this);

    completer->setWidget(this);

    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);

    connect(completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(DisplayTooltipForDiagnosticUnderCursor()));

    completerModel = new QStringListModel();
    completer->setModel(completerModel);

    this->setMouseTracking(true);
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
    DisplayTooltip(cursor, this->viewport()->mapToGlobal(this->cursorRect().topLeft()));
}

void CEditorPage::mouseMoveEvent(QMouseEvent* e)
{
    QTextCursor cursor = cursorForPosition(e->pos());
    DisplayTooltip(cursor, this->mapToGlobal(e->pos()));

    QPlainTextEdit::mouseMoveEvent(e);
}

void CEditorPage::ShowDiagnostics(std::vector<DiagnosticDTO> diags)
{
    this->blockSignals(true);

    this->diags = diags;

    QTextCharFormat errorFormat;
    errorFormat.setFontWeight(QFont::Bold);
    errorFormat.setFontUnderline(true);
    errorFormat.setUnderlineColor(QColor(255, 0, 0));

    QTextCharFormat warningFormat;
    warningFormat.setFontUnderline(true);
    warningFormat.setUnderlineColor(QColor(255, 255, 0));

    QTextCharFormat standardFormat;
    standardFormat.setForeground(QBrush(QColor(0, 0, 0)));


    QTextCursor c = this->textCursor();
    QTextCursor cursor = this->textCursor();

   // cursor.beginEditBlock();
    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor.setCharFormat(standardFormat);
   // cursor.endEditBlock();
    this->setTextCursor(cursor);
    for (DiagnosticDTO& diag : diags)
    {
        cursor = this->textCursor();

      //  cursor.beginEditBlock();
        cursor.setPosition(diag.offset, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, diag.length);
        if (diag.severity == CXDiagnostic_Error || diag.severity == CXDiagnostic_Fatal)
            cursor.setCharFormat(errorFormat);
        if (diag.severity == CXDiagnostic_Warning)
            cursor.setCharFormat(warningFormat);
    //    cursor.endEditBlock();
        this->setTextCursor(cursor);
    }
    this->setTextCursor(c);

    this->blockSignals(false);
}

void CEditorPage::SetCompletionModel(QStringList l)
{
    QMutexLocker lock(&mtx_CompleterModelIsBeingAccessed);

    completerModel->setStringList(l);
}

void CEditorPage::insertCompletion(QString completion)
{
    if (completer->widget() != this)
    {
        return;
    }

    QTextCursor tc = textCursor();

    int remaining = completion.size() - completer->completionPrefix().size();
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
   /* QPlainTextEdit::keyPressEvent(e);
    return;*/

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

    QMutexLocker lock(&mtx_CompleterModelIsBeingAccessed);

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Space);
    if (completer == NULL || !isShortcut)
    {
        QPlainTextEdit::keyPressEvent(e);
    }
    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (completer == NULL || (ctrlOrShift && e->text().isEmpty()))
         return;

     static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
     //static QString eow("~!@#$%^&*()_+{}|:\"<>?,/;'[]\\-="); // end of word

     bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
     QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty() || eow.contains(e->text().right(1)) || completionPrefix.length() < 1))
    {
        completer->popup()->hide();
        return;
    }

    if (completionPrefix != completer->completionPrefix())
    {
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
    }

    QRect cr = cursorRect();
    cr.setWidth(completer->popup()->sizeHintForColumn(0) + completer->popup()->verticalScrollBar()->sizeHint().width());

    completer->complete(cr);
}
