#ifndef TESTER_FILEMANAGER_H
#define TESTER_FILEMANAGER_H

#include <QString>
#include <QDir>
#include <QDebug>

#include "FileManager.h"
class Tester_FileManager
{
private:
    QString home;
public:
    Tester_FileManager();
    ~Tester_FileManager();
    void Test();
};

#endif // TESTER_FILEMANAGER_H
