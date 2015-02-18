#include "MakefileBasedProjectBuilder.h"

MakefileBasedProjectBuilder::MakefileBasedProjectBuilder(MakefileBuilder *makefileBuilder)
{
    this->makefileBuilder = makefileBuilder;
}

void MakefileBasedProjectBuilder::Build(const Project& proj)
{
    QString makefile = makefileBuilder->BuildMakefile(proj);

    try
    {
        FileManager::WriteFile(proj.Root() + "/" + proj.Name() + ".makefile", makefile);
    }
    catch (FileSystemException& ex)
    {
        // show errors while creating makefile
    }

    QProcess* process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory(proj.Root());

    process->start(proj.MakeUtility() + " -f " + proj.Name() + ".makefile " + proj.Name());

    if (!process->waitForFinished())
        qDebug() << "Make failed: " << process->errorString();
    else
        qDebug() << "Make output: " << process->readAll();
}
void MakefileBasedProjectBuilder::Clean(const Project& proj)
{
    QProcess* process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory(proj.Root());

    process->start(proj.MakeUtility() + " -f " + proj.Name() + ".makefile " + " clean "); // make target "clean"

    if (!process->waitForFinished())
        qDebug() << "Make failed: " << process->errorString();
    else
        qDebug() << "Make output: " << process->readAll();
}
void MakefileBasedProjectBuilder::Rebuild(const Project& proj)
{
    this->Clean(proj);
    this->Build(proj);
}
