#ifndef CEDITORPAGE_H
#define CEDITORPAGE_H

#include "Editing/Utility/CSyntaxHighlighter.h"
#include "Editing/EditorPage.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStringListModel>
#include <QToolTip>
#include <memory>
#include <QMutex>

class CEditorPage;
#include "Editing/Core/CParser.h"

class CEditorPage : public EditorPage
{
    Q_OBJECT

public slots:
    void ShowDiagnostics(std::vector<DiagnosticDTO> diags);
    void SetCompletionModel(QStringList l);

private:
    CSyntaxHighlighter* highlighter;
    CParser* parser;
    QCompleter* completer;
    QStringListModel* completerModel;

    QMutex mtx_CompleterModelIsBeingAccessed;

    QString textUnderCursor() const;

    std::vector<DiagnosticDTO> diags;

public:
    CEditorPage();
    ~CEditorPage();

protected:
    void keyPressEvent(QKeyEvent* e);
    void focusInEvent(QFocusEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void DisplayTooltip(const QTextCursor& cursor, const QPoint& point);

private slots:
    void insertCompletion(QString completion);
    void DisplayTooltipForDiagnosticUnderCursor();
};

#endif // CEDITORPAGE_H
