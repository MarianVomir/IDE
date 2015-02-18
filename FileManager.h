#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDirIterator>

#include <QDebug>

#include "Exceptions.h"

class FileManager
{
private:
    FileManager();
    ~FileManager();

public:
    static void CreateFile(const QString& path);
    static void DeleteFile(const QString& path);
    static void CreateDir(const QString& path);
    static void DeleteDir(const QString& path);
    static QString ReadFile(const QString& filePath);
    static void WriteFile(const QString& filePath, const QString& contents);
    static bool Exists(const QString& path);
    static bool IsFile(const QString& path);
    static bool IsDir(const QString& path);
    static QStringList GetFolderContents(const QString& path);
    static void RenameFile(const QString& oldPath, const QString& newPath);
    static void RenameDir(const QString& oldPath, const QString& newPath);
};

#endif // FILEMANAGER_H
