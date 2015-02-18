#include "CEditorPage.h"

CEditorPage::CEditorPage()
{
    highlighter = new CSyntaxHighlighter(this->document());
}

CEditorPage::~CEditorPage()
{
    delete highlighter;
}

