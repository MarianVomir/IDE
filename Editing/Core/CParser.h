#ifndef CPARSER_H
#define CPARSER_H

#include "DiagnosticDTO.h"

#include <clang-c/Index.h>
#include <QTimer>
#include <iostream>

class CParser;
#include "Editing/CEditorPage.h"

class CParser : QObject
{
    Q_OBJECT

#define PARSING_INTERVAL 500 // milliseconds

signals:
    void DiagnosticsReady(std::vector<DiagnosticDTO>);
    void CompletionSuggestionsReady(QStringList);

public:
    CParser(const CEditorPage* textEdit);
    ~CParser();

public slots:
    void Parse();

protected:
    const CEditorPage* textEdit;    // The page from the tab editor on which this parser operates
    CXIndex index;
    QTimer* timer;
    CXTranslationUnit translationUnit;
    QStringList completionList;

    void AddToCompletionList(const char* completionItem);
    void ClearCompletionList();

protected slots:
    void ActivateTimer();
};

#endif // CPARSER_H