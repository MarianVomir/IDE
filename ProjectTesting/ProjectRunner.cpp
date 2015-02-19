#include "ProjectRunner.h"

ProjectRunner::ProjectRunner(ProjectBuilder *projectBuilder, OutputWriter *outputWriter)
    : projectBuilder(projectBuilder), outputWriter(outputWriter)
{
    process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
}
ProjectRunner::~ProjectRunner()
{

}

void ProjectRunner::Run(const Project& proj, const QString& commandLineArgs)
{
    int exitCode =projectBuilder->Build(proj);
    if (exitCode != 0)
    {
        outputWriter->WriteLine("Cannot run project: Build has failed!");
        return;
    }

    QString command = QString("xterm -hold -T ") + proj.Name() + " -e " + proj.Root() + "bin/" + proj.Name();
    outputWriter->WriteLine("Preparing to start: " + proj.Name());
    outputWriter->WriteLine("Starting: " + command);
    process->start(command + " " + commandLineArgs);

}


