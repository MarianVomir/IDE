#ifndef MAKEFILEBASEDPROJECTBUILDER_H
#define MAKEFILEBASEDPROJECTBUILDER_H

#include <sstream>

#include <QDebug>
#include <QProcess>

#include "ProjectTesting/ProjectBuilder.h"
#include "OutputWriter.h"
#include "Exceptions.h"
#include "FileManager.h"

class MakefileBasedProjectBuilder : public ProjectBuilder
{
    Q_OBJECT

private:
    QProcess* process;
    OutputWriter* outputWriter;

public:
    MakefileBasedProjectBuilder(OutputWriter* outputWriter);
    virtual ~MakefileBasedProjectBuilder();

    virtual int Build(const Project& proj);
    virtual int Clean(const Project& proj);
    virtual int Rebuild(const Project& proj);

private slots:
    void ProcessStarted();
    void ProcessOutputReady();
    void ProcessErrorReady();
    void ProcessFinished(int errCode);

private:
    QString BuildMakefile(const Project& proj);
    QStringList GetUserIncludedHeaders(const QString& cFile);
};

#endif // MAKEFILEBASEDPROJECTBUILDER_H
