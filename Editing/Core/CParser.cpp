#include "CParser.h"

CParser::CParser(const CEditorPage* textEdit)
{
    this->textEdit = textEdit;

    index = clang_createIndex(0,0);
    timer = new QTimer();
    timer->setSingleShot(true);

    connect(textEdit, SIGNAL(textChanged()), this, SLOT(ActivateTimer()));
    connect(timer, SIGNAL(timeout()), this, SLOT(Parse()));
    connect(this, SIGNAL(DiagnosticsReady(std::vector<DiagnosticDTO>)), this->textEdit, SLOT(ShowDiagnostics(std::vector<DiagnosticDTO>)));
    connect(this, SIGNAL(CompletionSuggestionsReady(QStringList)), this->textEdit, SLOT(SetCompletionModel(QStringList)));

}

void CParser::ActivateTimer()
{
    if (this->timer->isActive())
        return;
    this->timer->start(PARSING_INTERVAL);
}

void CParser::Parse()
{
    static const char* args[] = { "-c", "-x", "c", "-Wall", "-fsyntax-only" };
    static int numArgs = 5;

    QByteArray b = textEdit->toPlainText().toLocal8Bit();
    unsigned int len = b.length();
    const char* text = b.constData();

    QByteArray ba = textEdit->property("filePath").toByteArray();

    const char* fileName = ba.constData();
    CXUnsavedFile unsavedFile = { .Filename = fileName,  .Contents = text, .Length = strlen(text) };
    CXUnsavedFile* unsavedFiles = new CXUnsavedFile[1];
    unsavedFiles[0] = unsavedFile;

    translationUnit = clang_parseTranslationUnit(index, fileName, args, numArgs, unsavedFiles, 1, CXTranslationUnit_DetailedPreprocessingRecord);

    CXDiagnosticSet diagnosticSet = clang_getDiagnosticSetFromTU(translationUnit);
    unsigned int numDiags = clang_getNumDiagnosticsInSet(diagnosticSet);

    std::vector<DiagnosticDTO> diagDTOList;

    for (unsigned int i = 0; i < numDiags; ++i)
    {
        CXDiagnostic diag = clang_getDiagnosticInSet(diagnosticSet, i);
        int numRanges = clang_getDiagnosticNumRanges(diag);
        if (numRanges > 0)
        {
            for (int j = 0; j < numRanges; j++)
            {
                CXSourceRange range = clang_getDiagnosticRange(diag, j);
                CXSourceLocation startLoc = clang_getRangeStart(range);
                CXSourceLocation endLoc = clang_getRangeEnd(range);
                unsigned int offset1;
                unsigned int line1;
                unsigned int col1;

                unsigned int offset2;
                unsigned int line2;
                unsigned int col2;

                clang_getFileLocation(startLoc, NULL, &line1, &col1, &offset1);
                clang_getFileLocation(endLoc, NULL, &line2, &col2, &offset2);

                DiagnosticDTO diagDTO;

                CXString message = clang_getDiagnosticSpelling(diag);
                diagDTO.line = line1;
                diagDTO.col = col1;
                diagDTO.offset = offset1;
                diagDTO.severity = clang_getDiagnosticSeverity(diag);
                diagDTO.length = offset2 - offset1;
                diagDTO.message = clang_getCString(message);
                diagDTOList.push_back(diagDTO);

                clang_disposeString(message);
            }
        }
        else
        {
            CXSourceLocation loc = clang_getDiagnosticLocation(diag);
            unsigned int offset;
            unsigned int line;
            unsigned int col;

            clang_getFileLocation(loc, NULL, &line, &col, &offset);

            DiagnosticDTO diagDTO;

            diagDTO.line = line;
            diagDTO.col = col;

            if (offset + 1 > len)
                if (offset > 0)
                    offset--;

            if (offset < len && (text[offset] == '\n' || text[offset] == '\r'))
                offset--;

            CXString message = clang_getDiagnosticSpelling(diag);
            diagDTO.offset = offset;
            diagDTO.severity = clang_getDiagnosticSeverity(diag);

            int k = offset;
            while (text[k] != '\0' && text[k] != '\n' && text[k] != ';')
                k++;

            diagDTO.length = k - offset;
            diagDTO.message = clang_getCString(message);
            diagDTOList.push_back(diagDTO);

            clang_disposeString(message);
        }
    }

    emit DiagnosticsReady(diagDTOList);

    ClearCompletionList(); // clean the previous code completion list

    QTextCursor c = textEdit->textCursor();
    int col = c.columnNumber() + 1;
    int line = c.blockNumber() + 1;

    if (line != 0 && col != 0)
    {
        auto completions = clang_codeCompleteAt(translationUnit, fileName, line, col, unsavedFiles, 1, clang_defaultCodeCompleteOptions());

        for (auto i = 0u; i < completions->NumResults; ++i)
        {
            auto& completionString = completions->Results[i].CompletionString;

            for (auto j = 0u; j < clang_getNumCompletionChunks(completionString); ++j)
            {
                auto chunkType = clang_getCompletionChunkKind(completionString, j);

                if (chunkType == CXCompletionChunk_TypedText)
                {
                    auto chunkString = clang_getCompletionChunkText(completionString, j);
                    const char* completionItem = clang_getCString(chunkString);

                    this->AddToCompletionList(completionItem);

                    //qDebug() << completionItem << " ";

                    clang_disposeString(chunkString);
                }
            }
        }

        clang_disposeCodeCompleteResults(completions);
    }

    //qDebug() << "\n\n\n\n";
    delete unsavedFiles;

    emit CompletionSuggestionsReady(this->completionList);

    clang_disposeDiagnosticSet(diagnosticSet);
}

void CParser::AddToCompletionList(const char *completionItem)
{
    QString s(completionItem);
    s = s.trimmed();
    if (s == "")
        return;

    this->completionList.append(s);
    //qDebug() << "Appended " << completionItem << " to model.";
}
void CParser::ClearCompletionList()
{
    this->completionList.clear();
}

CParser::~CParser()
{
    clang_disposeTranslationUnit(translationUnit);
    delete timer;
}
