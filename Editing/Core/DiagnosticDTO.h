#ifndef DIAGNOSTICDTO
#define DIAGNOSTICDTO

#include <clang-c/Index.h>

#include <QString>

class DiagnosticDTO
{
public:
    unsigned int line;
    unsigned int col;
    unsigned int offset;
    unsigned int length;
    CXDiagnosticSeverity severity;
    QString message;

public:
    DiagnosticDTO() {}
    ~DiagnosticDTO() {}
};

#endif // DIAGNOSTICDTO

