#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>

class Exception
{
protected:
    QString message;
public:
    Exception(const QString& message = "An exception has been raised!") : message(message) {}
    virtual ~Exception() {}
    virtual const QString& Message() const { return message; }
};

class FileSystemException : public Exception
{
protected:
    QString message;
public:
    FileSystemException(const QString& message = "An exception has been raised during a file system operation!") : Exception(message) {}
    ~FileSystemException() {}
};

class ImproperPermissionsException : public FileSystemException
{
public:
    ImproperPermissionsException(const QString& message = "Improper permissions to perform operation!") : FileSystemException(message){}
};
class InvalidPathException : public FileSystemException
{
public:
    InvalidPathException(const QString& message = "The given path is invalid!") : FileSystemException(message){}
};
class OperationFailedException : public FileSystemException
{
public:
    OperationFailedException(const QString& message = "File or Folder operation has failed!") : FileSystemException(message){}
};
class NotFoundException : public FileSystemException
{
public:
    NotFoundException(const QString& message = "File or folder not found!") : FileSystemException(message){}
};
class AlreadyExistsException : public FileSystemException
{
public:
    AlreadyExistsException(const QString& message = "File or folder already exists!") : FileSystemException(message){}
};

#endif // EXCEPTIONS_H
