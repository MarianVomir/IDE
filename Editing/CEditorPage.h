#ifndef CEDITORPAGE_H
#define CEDITORPAGE_H

#include <memory>

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStringListModel>
#include <QToolTip>
#include <QMutex>
#include <QUndoStack>

#include "Editing/Utility/TextChangeCommand.h"

#include "Editing/Utility/CSyntaxHighlighter.h"
#include "Editing/EditorPage.h"
class CEditorPage;
#include "Editing/Core/CParser.h"

class CEditorPage : public EditorPage
{
    Q_OBJECT

private:
    CSyntaxHighlighter* highlighter;
    CParser* parser;
    QCompleter* completer;
    QStringListModel* completerModel;
    QMutex mtx_CompleterModelIsBeingAccessed;
    QString textUnderCursor(int moveLeft = 0) const;
    std::vector<DiagnosticDTO> diags;

    QTextCharFormat errorFormat;
    QTextCharFormat warningFormat;
    QTextCharFormat standardFormat;

    QUndoStack undoStack;
    QString currentText;

public slots:
    void ShowDiagnostics(std::vector<DiagnosticDTO> diags);
    void SetCompletionModel(QStringList l);

public:
    CEditorPage(const QString& contents);
    ~CEditorPage();
    void PerformAfterSetupOperations();

protected:
    void keyPressEvent(QKeyEvent* e);
    void focusInEvent(QFocusEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void PopupCompleter();

private slots:
    void insertCompletion(QString completion);
    void DisplayTooltipForDiagnosticUnderCursor();
    void contentsChanged(int from, int removed, int added);

private:
    void DisplayTooltip(const QTextCursor& cursor, const QPoint& point);
};

#endif // CEDITORPAGE_H
