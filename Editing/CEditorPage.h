#ifndef CEDITORPAGE_H
#define CEDITORPAGE_H

#include "Editing/Utility/CSyntaxHighlighter.h"
#include "Editing/EditorPage.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStringListModel>
#include <QToolTip>

class CEditorPage;
#include "Editing/Core/CParser.h"

class CEditorPage : public EditorPage
{
    Q_OBJECT

public slots:
    void ShowDiagnostics(std::vector<DiagnosticDTO> diags);

private:
    CSyntaxHighlighter* highlighter;
    CParser* parser;
    QCompleter* completer;
    QString textUnderCursor() const;

    std::vector<DiagnosticDTO> diags;

public:
    CEditorPage();
    ~CEditorPage();

protected:
    void keyPressEvent(QKeyEvent* e);
    void focusInEvent(QFocusEvent* e);
    void mouseMoveEvent(QMouseEvent* e);

private slots:
    void insertCompletion(QString completion);
};

#endif // CEDITORPAGE_H
