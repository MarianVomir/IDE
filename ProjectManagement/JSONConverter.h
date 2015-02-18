#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include "ProjectManagement/ProjectFileConverter.h"
#include "Exceptions.h"
#include "FileManager.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

class JSONConverter : public ProjectFileConverter
{
public:

    virtual void ProjectToFile(const Project *proj, const QString &filePath);
    virtual Project* FileToProject(const QString &filePath);

    JSONConverter();
    ~JSONConverter();
};

#endif // JSONCONVERTER_H
