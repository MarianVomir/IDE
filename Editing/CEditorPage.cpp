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

    QStringListModel* l = new QStringListModel(QStringList(Ckeywords.toList()));

    completer->setModel(l);

    this->setMouseTracking(true);
}

CEditorPage::~CEditorPage()
{
    delete highlighter;
    delete completer;
    delete parser;
}

void CEditorPage::mouseMoveEvent(QMouseEvent* e)
{
  /*  qDebug() << e->pos();
    //QToolTip::showText(this->mapToGlobal(e->pos()), "", this);
    QToolTip::showText(this->mapToGlobal(e->pos()), "Diag goes here", this);*/
}

void CEditorPage::ShowDiagnostics(std::vector<DiagnosticDTO> diags)
{

    QTextCharFormat errorFormat;
    errorFormat.setFontWeight(QFont::Bold);
    errorFormat.setFontUnderline(true);
    errorFormat.setUnderlineColor(QColor(255, 0, 0));

    QTextCharFormat warningFormat;
    warningFormat.setFontUnderline(true);
    warningFormat.setUnderlineColor(QColor(255, 255, 0));

    QTextCharFormat standardFormat;
    standardFormat.setForeground(QBrush(QColor(0, 0, 0)));

    QTextCursor cursor = this->textCursor();
    QTextCursor c = this->textCursor();

    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);

    this->blockSignals(true);
    this->setTextCursor(cursor);
    this->setCurrentCharFormat(standardFormat);

    for (DiagnosticDTO& diag : diags)
    {
        cursor.setPosition(diag.offset, QTextCursor::MoveAnchor);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, diag.length);

        this->setTextCursor(cursor);

        if (diag.severity == CXDiagnostic_Error || diag.severity == CXDiagnostic_Fatal)
            this->setCurrentCharFormat(errorFormat);
        if (diag.severity == CXDiagnostic_Warning)
            this->setCurrentCharFormat(warningFormat);
    }

    this->setTextCursor(c);

    setUndoRedoEnabled(true);
    this->blockSignals(false);
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
    QPlainTextEdit::keyPressEvent(e);
    return;

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
