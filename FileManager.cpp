#include "FileManager.h"

FileManager::FileManager()
{
}

FileManager::~FileManager()
{

}

void FileManager::CreateFile(const QString &path)
{
    if (path.endsWith("/") || path.endsWith("\\")) // last character is / or \ => cannot be a file
    {
        throw InvalidPathException(path + " cannot represent a file");
    }

    QFile file(path);
    QFileInfo info(path);
    info.setCaching(false);
    if (info.exists())
        throw AlreadyExistsException("CreateFile failed: same name already exists");

    QFileInfo containingDir(info.path());
    containingDir.setCaching(false);
    if (containingDir.exists() && !containingDir.permission(QFile::WriteUser))
    {
        throw ImproperPermissionsException("CreateFile failed: Improper Permissions");
    }

    if (!containingDir.exists())
        FileManager::CreateDir(containingDir.absoluteFilePath());
    file.open(QFile::WriteOnly | QFile::Text);
    if (file.error() == QFile::OpenError)
    {
        throw OperationFailedException("CreateFile failed: Check folder locks and make sure file name is valid");
    }
    file.close();
}
void FileManager::DeleteFile(const QString& path)
{
    int i = path.size() - 1;
    while (QString("\n\t\r ").contains(path[i])) i--;
    if (path[i] == '/' || path[i] == '\\') // last non-space character is / or \ => cannot be a file
    {
        throw InvalidPathException(path + " cannot represent a file!");
    }

    QFileInfo info(path);
    info.setCaching(false);

    if (!info.exists())
        return;

    if (!info.permission(QFile::WriteUser))
        throw ImproperPermissionsException("DeleteFile failed: Improper Permissions!");

    QFile file(path);
    file.open(QFile::WriteOnly);
    if (file.error() == QFile::OpenError)
    {
        throw OperationFailedException("DeleteFile failed: Check file/folder locks and make sure file name is valid");
    }
    file.close();
    QFile::remove(path);
}
void FileManager::CreateDir(const QString &path)
{
    QString trimmedPath = path;

    if (path[path.size() - 1] == '/' || path[path.size() - 1] == '\\')
    {
        trimmedPath = path.left(path.size() - 1);
    }

    if (trimmedPath == "")
        throw ImproperPermissionsException("CreateDir failed: Improper Permissions");

    QFileInfo info(trimmedPath);
    info.setCaching(false);

    if (info.exists())
        throw AlreadyExistsException("CreateDir failed: same name already exists");

    if (QFileInfo(info.path()).exists() && !QFileInfo(info.path()).permission(QFile::WriteUser))
    {
        throw ImproperPermissionsException("CreateDirectory failed: Improper Permissions");
    }
    QDir(path).mkpath(".");
    if (!QDir(path).exists())
    {
        throw OperationFailedException("CreateDirectory failed: Check locks and make sure directory name is valid");
    }
}
void FileManager::DeleteDir(const QString &path)
{
    QDir dir(path);
    QFileInfo info(path);
    info.setCaching(false);

    if (info.exists())
    {
        if (QFileInfo(info.path()).permission(QFile::ReadUser) && dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() == 0)
        {
            if (!QFileInfo(info.path()).permission(QFile::WriteUser))
            {
                throw ImproperPermissionsException("DeleteDir failed: Improper Permissions");
            }
        }
        else
        {
            if (!QFileInfo(info.path()).permission(QFile::WriteUser) || !QFileInfo(info.path()).permission(QFile::ExeUser))
            {
                throw ImproperPermissionsException("DeleteDir failed: Improper Permissions");
            }
        }
        dir.removeRecursively();
        if (info.exists())
        {
            throw OperationFailedException("DeleteDir failed: Check locks and permissions!");
        }
    }
}
QString FileManager::ReadFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists())
    {
        throw NotFoundException("ReadFile error: File not found!");
    }
    file.open(QFile::ReadOnly | QFile::Text);
    if (QFileInfo(file).permission(QFile::ReadUser) == false || file.error() == QFile::PermissionsError)
    {
        throw ImproperPermissionsException("ReadFile error: Improper Permissions");
    }
    QString result(file.readAll());
    if (file.error() == QFile::ReadError || file.error() == QFile::OpenError)
    {
        OperationFailedException("ReadFile error: Check file/folder locks");
    }
    file.close();

    return result;
}
void FileManager::WriteFile(const QString &filePath, const QString &contents)
{
    QFile file(filePath);
    file.open(QFile::WriteOnly | QFile::Text);

    if (QFileInfo(file).permission(QFile::WriteUser) == false || file.error() == QFile::PermissionsError)
    {
        throw ImproperPermissionsException("WriteFile error: Improper Permissions");
    }

    file.write(contents.toLocal8Bit());
    if (file.error() == QFile::WriteError || file.error() == QFile::OpenError)
    {
        throw OperationFailedException("WriteFile error: Check file/folder locks and make sure file name is valid");
    }
    file.close();
}
bool FileManager::Exists(const QString &path)
{
    return QFileInfo(path).exists() || QDir(path).exists();
}
bool FileManager::IsFile(const QString& path)
{
    return QFileInfo(path).isFile();
}
bool FileManager::IsDir(const QString& path)
{
    return QFileInfo(path).isDir();
}
QStringList FileManager::GetFolderContents(const QString &path)
{
    QDir dir(path);
    QStringList result;
    if (dir.exists())
    {
        QDirIterator dirIt(dir, QDirIterator::Subdirectories);
        while (dirIt.hasNext())
        {
            dirIt.next();
            QString path = dirIt.filePath();

            if (dirIt.fileName() != ".." && dirIt.fileName() != ".") // no Dot and DotDot
            {
                result << path;
            }
        }
    }

    return result;
}
void FileManager::RenameFile(const QString &oldPath, const QString &newPath)
{
    QFile file(oldPath);

    if (!QFileInfo(file).exists())
    {
        throw NotFoundException("File does not exist!");
    }
    if (QFileInfo(newPath).exists())
    {
        throw AlreadyExistsException("File with the same name already exists");
    }

    file.rename(newPath);
    if (QFileInfo(file).permission(QFile::WriteUser) == false)
    {
        throw ImproperPermissionsException("RenameFile failed: Improper Permissions");
    }
    if (!QFile(newPath).exists() || file.error() == QFile::RenameError)
    {
        throw OperationFailedException("RenameFile failed: Check file/folder locks and make sure the new name is valid!");
    }
}
void FileManager::RenameDir(const QString &oldPath, const QString &newPath)
{
    QDir dir(oldPath);
    if (!dir.exists())
    {
        throw NotFoundException("RenameDir failed: Directory does not exist!");
    }
    if (QDir(newPath).exists())
    {
        throw AlreadyExistsException("RenameDir failed: Directory with the same name already exists");
    }
    if (QFileInfo(QFileInfo(oldPath).path()).permission(QFile::WriteUser) == false || QFileInfo(QFileInfo(newPath).path()).permission(QFile::WriteUser) == false)
    {
        throw ImproperPermissionsException("RenameDir failed: Improper Permissions");
    }
    dir.rename(oldPath, newPath);

    if (!QDir(newPath).exists())
    {
        throw OperationFailedException("RenameDir failed: Check locks and make sure the new name is valid");
    }
}

