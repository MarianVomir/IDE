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
    this->timer->start(500);
}

/*
 * NOT USED, this code will be removed in the future
 *
CXChildVisitResult visit(CXCursor cursor, CXCursor parent, CXClientData clientData)
{
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXSourceLocation loc1 = clang_getRangeStart(range);
    CXSourceLocation loc2 = clang_getRangeEnd(range);

   // CXSourceLocation loc = clang_getCursorLocation(cursor);
    unsigned int offset1;
    unsigned int line1;
    unsigned int col1;
    unsigned int offset2;
    unsigned int line2;
    unsigned int col2;
    clang_getFileLocation(loc1, NULL, &line1, &col1, &offset1);
    clang_getFileLocation(loc2, NULL, &line2, &col2, &offset2);

    CXCursorKind cursorKind = clang_getCursorKind(cursor);

    if (
            cursorKind == CXCursor_VarDecl ||
            cursorKind == CXCursor_FunctionDecl ||
            cursorKind == CXCursor_StructDecl ||
            cursorKind == CXCursor_EnumDecl ||
            cursorKind == CXCursor_FieldDecl
            )
    {
        CXCompletionString completionString = clang_getCursorCompletionString(cursor);

        for (int j = 0; j < clang_getNumCompletionChunks(completionString); j++)
        {
            CXCompletionChunkKind k = clang_getCompletionChunkKind(completionString, j);
            CXString clangstr = clang_getCompletionChunkText(completionString, j);
            const char* cstr = clang_getCString(clangstr);
            std::cout << cstr << " ";
            clang_disposeString(clangstr);
        }
        std::cout << std::endl;
        return CXChildVisit_Recurse;
    }

    return CXChildVisit_Recurse;
}
*/

void CParser::Parse()
{
    static const char* args[] = { "-c", "-x", "c", "-Weverything", "-fsyntax-only" };
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

        clang_disposeDiagnostic(diag);
    }

    emit DiagnosticsReady(diagDTOList);

    ClearCompletionList(); // clean the previous code completion list

    QTextCursor c = textEdit->textCursor();
    int col = c.columnNumber() + 1;
    int line = c.blockNumber() + 1;

    if (line != 0 && col != 0)
    {
        // qDebug() << "LINE " << line << " COL " << col;
        auto completions = clang_codeCompleteAt(translationUnit, fileName, line, col, unsavedFiles, 1, clang_defaultCodeCompleteOptions());
/*
        for (i = 0; i < results->NumResults; i++)
        {
            CXCompletionResult result = results->Results[i];
            CXCompletionString str = result.CompletionString;

            for (int j = 0; j < clang_getNumCompletionChunks(str); j++)
            {
                CXCompletionChunkKind k = clang_getCompletionChunkKind(str, j);
                const char* cstr = clang_getCString(clang_getCompletionChunkText(str, j));

                if (k == CXCompletionChunk_TypedText)
                ui->txt_Debug->appendPlainText(QString("From ") + std::to_string(i).c_str() + " Kind " +  std::to_string(k).c_str() + " " + QString(cstr));
            }
        }
*/

        for (auto i = 0u; i < completions->NumResults; ++i)
        {
            auto& completionString = completions->Results[i].CompletionString;

            for (auto j = 0u; j < clang_getNumCompletionChunks(completionString); ++j)
            {
                auto chunkType = clang_getCompletionChunkKind(completionString, j);

                if (
                        (text[c.position() - 1] == '.')
                        ||
                        (text[c.position()] - 2 == '-' && text[c.position() - 1 == '>'])
                   )    // . or -> used => most likely a member access
                {
                    if (chunkType == CXCompletionChunk_TypedText) // this is to get the members' names, not their types too
                    {
                        auto chunkString = clang_getCompletionChunkText(completionString, j);
                        const char* completionItem = clang_getCString(chunkString);

                        this->AddToCompletionList(completionItem);

                        qDebug() << completionItem << " ";
                        clang_disposeString(chunkString);
                    }
                }

                else
                {
                    if (chunkType == CXCompletionChunk_TypedText)
                    {
                        auto chunkString = clang_getCompletionChunkText(completionString, j);
                        const char* completionItem = clang_getCString(chunkString);

                        this->AddToCompletionList(completionItem);

                        qDebug() << completionItem << " ";
                        clang_disposeString(chunkString);
                    }
                }

            }
        }

        clang_disposeCodeCompleteResults(completions);
    }

    qDebug() << "\n\n\n\n";
    delete unsavedFiles;

    emit CompletionSuggestionsReady(this->completionList);

    // clang_disposeDiagnosticSet(diagnosticSet);
}

void CParser::AddToCompletionList(const char *completionItem)
{
    if (strcmp("", completionItem) == 0)
        return;

    this->completionList.append(QString(completionItem));
    qDebug() << "Appended " << completionItem << " to model.";
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

