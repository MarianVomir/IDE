#ifndef CEDITORPAGE_H
#define CEDITORPAGE_H

#include "Editing/Utility/CSyntaxHighlighter.h"
#include "Editing/EditorPage.h"

class CEditorPage : public EditorPage
{
private:
    CSyntaxHighlighter* highlighter;
    // TODO: Parser
    // TODO: Completer

public:
    CEditorPage();
    ~CEditorPage();
};

#endif // CEDITORPAGE_H
