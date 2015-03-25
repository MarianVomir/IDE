#ifndef CEDITORPAGE_H
#define CEDITORPAGE_H

#include "Editing/Utility/CSyntaxHighlighter.h"
#include "Editing/EditorPage.h"

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>
#include <QStringListModel>

class CEditorPage : public EditorPage
{
    Q_OBJECT

private:
    CSyntaxHighlighter* highlighter;
    // TODO: Parser
    QCompleter* completer;

    QString textUnderCursor() const;

public:
    CEditorPage();
    ~CEditorPage();

protected:
    void keyPressEvent(QKeyEvent* e);
    void focusInEvent(QFocusEvent* e);

    QString prefixTabs();

private slots:
    void insertCompletion(QString completion);
};

#endif // CEDITORPAGE_H
