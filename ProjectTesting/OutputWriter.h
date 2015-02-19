#ifndef OUTPUTWRITER_H
#define OUTPUTWRITER_H

#include <QString>

class OutputWriter
{
public:
    virtual void WriteLine(const QString& contents) = 0;
    virtual void Clear() = 0;

    OutputWriter() {}
    virtual ~OutputWriter() {}
};

#endif // OUTPUTWRITER_H
