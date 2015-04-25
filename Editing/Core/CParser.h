#ifndef CPARSER_H
#define CPARSER_H

#include "DiagnosticDTO.h"

#include <clang-c/Index.h>
#include <QTimer>

class CParser;
#include "Editing/CEditorPage.h"

class CParser : QObject
{
    Q_OBJECT

signals:
    void DiagnosticsReady(std::vector<DiagnosticDTO>);

public:
    CParser(const CEditorPage* textEdit);
    ~CParser();

public slots:
    void ActivateTimer();
    void Parse();

private:
    const CEditorPage* textEdit;

    CXIndex index;
    QTimer* timer;
    CXTranslationUnit translationUnit;
};

#endif // CPARSER_H
