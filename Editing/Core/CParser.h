#ifndef CPARSER_H
#define CPARSER_H

#include <iostream>

#include <clang-c/Index.h>

#include <QTimer>

#include "DiagnosticDTO.h"
class CParser;
#include "Editing/CEditorPage.h"

class CParser : QObject
{
    Q_OBJECT

#define PARSING_INTERVAL 900 // milliseconds

private:
    QMutex clangMutex;
    bool translationUnitHasBeenParsed;

signals:
    void DiagnosticsReady(std::vector<DiagnosticDTO>);
    void CompletionSuggestionsReady(QStringList);

public:
    CParser(const CEditorPage* textEdit);
    ~CParser();

public slots:
    void Parse();
    void GenerateCompleterSuggestions();

protected:
    const CEditorPage* textEdit;    // The page from the tab editor on which this parser operates
    CXIndex index;
    QTimer* timer;
    CXTranslationUnit translationUnit;

protected slots:
    void ActivateTimer();

};

#endif // CPARSER_H
