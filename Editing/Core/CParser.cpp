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
}

void CParser::ActivateTimer()
{
    if (this->timer->isActive())
        return;
    this->timer->start(500);
}

void CParser::Parse()
{
    static const char* args[] = { "-c", "-x", "c", "-Weverything", "-fsyntax-only" };
    static int numArgs = 5;

    QByteArray b = textEdit->toPlainText().toLocal8Bit();
    unsigned int len = b.length();
    const char* text = b.constData();

    const char* fileName = textEdit->property("filePath").toByteArray().constData();
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

    if (diagDTOList.size() > 0)
        emit DiagnosticsReady(diagDTOList);

    delete unsavedFiles;
   // clang_disposeDiagnosticSet(diagnosticSet);
}

CParser::~CParser()
{
    clang_disposeTranslationUnit(translationUnit);
    delete timer;
}

