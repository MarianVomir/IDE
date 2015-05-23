#include "CParser.h"

CParser::CParser(const CEditorPage* textEdit)
{
    this->textEdit = textEdit;

    index = clang_createIndex(0,0);
    timer = new QTimer();
    timer->setSingleShot(true);

    connect(textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(ActivateTimer()));
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
    static const char* args[] = { "-c", "-x", "c",
                                  "-Wall",
                                  "-Wunreachable-code",
                                  "-Wconversion",
                                  "-Wempty-body",
                                  "-Wenum-compare",
                                  "-Wenum-conversion",
                                  "-Wfloat-equal",
                                  "-Wformat",
                                  "-Wformat-extra-args",
                                  "-Wformat-invalid-specifier",
                                  "-Wformat-zero-length",
                                  "-Wheader-guard",
                                  "-Wnewline-eof"
                                };
    static int numArgs = 16;

    QByteArray b = textEdit->toPlainText().toLocal8Bit();
    unsigned int len = b.length();
    const char* text = b.constData();

    QByteArray ba = textEdit->property("filePath").toByteArray();
    const char* fileName = ba.constData();

    CXUnsavedFile unsavedFile = { .Filename = fileName,  .Contents = text, .Length = len };
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
            /*for (int j = 0; j < numRanges; j++)*/
            {
                CXSourceRange range = clang_getDiagnosticRange(diag, 0); // j);
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
                diagDTO.length = qMax(offset2 - offset1, (unsigned int)1);
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

            static char separators[] = " ,\n\r[];(){}=<>*+-\0";
            if (offset < len && (text[offset] == '\n' || text[offset] == '\r'))
                offset--;

/*            if (offset < len && strchr(separators, text[offset]))
                offset--;
*/
            CXString message = clang_getDiagnosticSpelling(diag);
            diagDTO.offset = offset;
            diagDTO.severity = clang_getDiagnosticSeverity(diag);

            int k = offset;
            /*while (text[k] != '\0' && text[k] != '\n' && text[k] != ';')
                k++;
*/
            while (!strchr(separators, text[k]))
                k++;
            diagDTO.length = qMax(k - offset, (unsigned int)1);
            diagDTO.message = clang_getCString(message);
            diagDTOList.push_back(diagDTO);

            clang_disposeString(message);
            clang_disposeDiagnostic(diag);
        }
    }

    emit DiagnosticsReady(diagDTOList);

    //clang_disposeDiagnosticSet(diagnosticSet);
}

void CParser::GenerateCompleterSuggestions()
{
    QByteArray b = textEdit->toPlainText().toLocal8Bit();
    unsigned int len = b.length();
    const char* text = b.constData();

    QByteArray ba = textEdit->property("filePath").toByteArray();
    const char* fileName = ba.constData();

    CXUnsavedFile unsavedFile = { .Filename = fileName,  .Contents = text, .Length = len };
    CXUnsavedFile* unsavedFiles = new CXUnsavedFile[1];
    unsavedFiles[0] = unsavedFile;

    ClearCompletionList(); // clean the previous code completion list

    QTextCursor cursor = textEdit->textCursor();

    static char structSeparators[] = ";[](){}~+-*\\/%#$^&|";
    int structMemberAccessorIndex = cursor.position();
    int oldCursorPos = structMemberAccessorIndex;

    bool entered = false;
    while (structMemberAccessorIndex > 0)
    {
        if (text[structMemberAccessorIndex] == '.' || (text[structMemberAccessorIndex] == '>' && text[structMemberAccessorIndex - 1] == '-'))
        {
            entered = true;
            cursor.setPosition(structMemberAccessorIndex + 1);
            break;
        }
        if (strchr(structSeparators, text[structMemberAccessorIndex]))
        {
            break;
        }

        structMemberAccessorIndex--;
    }
    if (!entered)
        cursor.setPosition(oldCursorPos);

    int col = cursor.columnNumber() + 1;
    int line = cursor.blockNumber() + 1;
    cursor.setPosition(oldCursorPos);

    if (line > 0 && col > 0)
    {
        auto completions = clang_codeCompleteAt(translationUnit, fileName, line, col, unsavedFiles, 1, clang_defaultCodeCompleteOptions());

        if (completions != NULL)
        {
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

                        clang_disposeString(chunkString);
                    }
                }
            }

        clang_disposeCodeCompleteResults(completions);
        }
    }


    delete unsavedFiles;
    emit CompletionSuggestionsReady(this->completionList);
}

void CParser::AddToCompletionList(const char *completionItem)
{
    QString s(completionItem);
    s = s.trimmed();
    if (s == "")
        return;

    this->completionList.append(s);
}
void CParser::ClearCompletionList()
{
    this->completionList.clear();
}

CParser::~CParser()
{
    clang_disposeTranslationUnit(translationUnit);
    clang_disposeIndex(index);
    delete timer;
}

