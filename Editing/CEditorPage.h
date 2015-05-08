#ifndef CEDITORPAGE_H
#define CEDITORPAGE_H

#include <memory>

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStringListModel>
#include <QToolTip>
#include <QMutex>

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
    QString textUnderCursor() const;
    std::vector<DiagnosticDTO> diags;

    QTextCharFormat errorFormat;
    QTextCharFormat warningFormat;
    QTextCharFormat standardFormat;

public slots:
    void ShowDiagnostics(std::vector<DiagnosticDTO> diags);
    void SetCompletionModel(QStringList l);

public:
    CEditorPage();
    ~CEditorPage();

protected:
    void keyPressEvent(QKeyEvent* e);
    void focusInEvent(QFocusEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void PopupCompleter();

private slots:
    void insertCompletion(QString completion);
    void DisplayTooltipForDiagnosticUnderCursor();

private:
    void DisplayTooltip(const QTextCursor& cursor, const QPoint& point);
};

#endif // CEDITORPAGE_H
