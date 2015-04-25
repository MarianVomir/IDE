#ifndef PROJECTFILECONVERTER
#define PROJECTFILECONVERTER

#include "Project.h"

#include <QString>

// << interface >>
class ProjectFileConverter
{
public:
    virtual void ProjectToFile(const Project* proj, const QString& filePath) = 0;
    virtual Project* FileToProject(const QString& filePath) = 0;

    virtual ~ProjectFileConverter() {}
};
#endif // PROJECTFILECONVERTER

