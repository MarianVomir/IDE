#include "ProjectRunner.h"

ProjectRunner::ProjectRunner(ProjectBuilder *projectBuilder, OutputWriter *outputWriter)
    : projectBuilder(projectBuilder), outputWriter(outputWriter)
{
    process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);

    connect(process, SIGNAL(started()), this, SLOT(OnProcessStarted()));
    connect(process, SIGNAL(finished(int)), this, SLOT(OnProcessFinished(int)));
}
ProjectRunner::~ProjectRunner()
{

}

void ProjectRunner::Run(const Project& proj, const QString& commandLineArgs)
{
    int exitCode = projectBuilder->Build(proj);
    if (exitCode != 0)
    {
        outputWriter->WriteLine("Cannot run project: Build has failed!");
        return;
    }

    outputWriter->WriteLine("Preparing to start: " + proj.Name());
    QString command = QString("xterm -hold -T ") + proj.Name() + " -e ./runner " + proj.Root() + "bin/" + proj.Name() + " " + commandLineArgs;

    outputWriter->WriteLine("Starting: " + command);
    process->start(command);
}

void ProjectRunner::OnProcessStarted()
{
    outputWriter->WriteLine("Program has started.");
    emit runStarted();
}
void ProjectRunner::OnProcessFinished(int exitCode)
{
    outputWriter->WriteLine(QString("Program has finished."));
    emit runFinished(exitCode);
}
