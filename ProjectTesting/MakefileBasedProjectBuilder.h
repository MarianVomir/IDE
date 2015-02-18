#ifndef MAKEFILEBASEDPROJECTBUILDER_H
#define MAKEFILEBASEDPROJECTBUILDER_H

#include "ProjectTesting/MakefileBuilder.h"
#include "ProjectTesting/ProjectBuilder.h"

#include "Exceptions.h"
#include "FileManager.h"

#include <QProcess>

class MakefileBasedProjectBuilder : public ProjectBuilder
{
private:
    MakefileBuilder* makefileBuilder;
public:
    MakefileBasedProjectBuilder(MakefileBuilder* makefileBuilder);

    virtual void Build(const Project& proj);
    virtual void Clean(const Project& proj);
    virtual void Rebuild(const Project& proj);
};

#endif // MAKEFILEBASEDPROJECTBUILDER_H
