#ifndef LISTOUTPUTWRITER_H
#define LISTOUTPUTWRITER_H

#include <QListWidget>
#include <QMutex>
#include <QMutexLocker>

#include "OutputWriter.h"

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
