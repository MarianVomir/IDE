#ifndef MAKEFILEBASEDPROJECTBUILDER_H
#define MAKEFILEBASEDPROJECTBUILDER_H

#include "ProjectTesting/MakefileBuilder.h"
#include "ProjectTesting/ProjectBuilder.h"
#include "OutputWriter.h"

#include "Exceptions.h"
#include "FileManager.h"

#include <QProcess>

class MakefileBasedProjectBuilder : public ProjectBuilder
{
    Q_OBJECT

private:
    MakefileBuilder* makefileBuilder;
    QProcess* process;

public:
    MakefileBasedProjectBuilder(OutputWriter* outputWriter, MakefileBuilder* makefileBuilder);
    virtual ~MakefileBasedProjectBuilder();

    virtual int Build(const Project& proj);
    virtual int Clean(const Project& proj);
    virtual int Rebuild(const Project& proj);

private slots:
    void ProcessStarted();
    void ProcessOutputReady();
    void ProcessErrorReady();
    void ProcessFinished(int errCode);
};

#endif // MAKEFILEBASEDPROJECTBUILDER_H
