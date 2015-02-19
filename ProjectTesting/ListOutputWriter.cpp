#include "ListOutputWriter.h"

void ListOutputWriter::WriteLine(const QString &contents)
{
    QMutexLocker locker(&mutex);
    if (this->outputWidget != NULL)
    {
        this->outputWidget->addItem(contents);
    }
}

void ListOutputWriter::Clear()
{
    QMutexLocker locker(&mutex);
    if (this->outputWidget != NULL)
    {
        this->outputWidget->clear();
    }
}

ListOutputWriter::ListOutputWriter(QListWidget* outputWidget)
    : outputWidget(outputWidget)
{

}

ListOutputWriter::~ListOutputWriter()
{

}

