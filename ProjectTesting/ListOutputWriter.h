#ifndef LISTOUTPUTWRITER_H
#define LISTOUTPUTWRITER_H

#include "OutputWriter.h"

#include <QListWidget>
#include <QMutex>
#include <QMutexLocker>

class ListOutputWriter : public OutputWriter
{
private:
    QListWidget* outputWidget;
    QMutex mutex;

public:
    void WriteLine(const QString& contents);
    void Clear();

    ListOutputWriter(QListWidget* outputWidget);
    ~ListOutputWriter();
};

#endif // LISTOUTPUTWRITER_H
