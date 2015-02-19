#include "MakefileBasedProjectBuilder.h"

MakefileBasedProjectBuilder::MakefileBasedProjectBuilder(OutputWriter* outputWriter, MakefileBuilder *makefileBuilder)
    : ProjectBuilder(outputWriter), makefileBuilder(makefileBuilder)
{
    process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);

   /* connect(process, SIGNAL(started()), this, SLOT(ProcessStarted()));
    connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(ProcessOutputReady()));
    connect(process, SIGNAL(readyReadStandardError()), this, SLOT(ProcessErrorReady()));
    connect(process, SIGNAL(finished(int)), this, SLOT(ProcessFinished(int)));*/
}

MakefileBasedProjectBuilder::~MakefileBasedProjectBuilder()
{
    if (!process->waitForFinished())
        delete process;
}
void MakefileBasedProjectBuilder::ProcessStarted()
{
    this->outputWriter->Clear();
}
void MakefileBasedProjectBuilder::ProcessOutputReady()
{
    for (QString& line : QString(process->readAllStandardOutput()).split('\n'))
    {
        if (line.trimmed() != "")
            outputWriter->WriteLine(line);
    }
}
void MakefileBasedProjectBuilder::ProcessErrorReady()
{
    for (QString& line : QString(process->readAllStandardError()).split('\n'))
    {
        if (line.trimmed() != "")
            outputWriter->WriteLine(line.trimmed());
    }
}
void MakefileBasedProjectBuilder::ProcessFinished(int errCode)
{
    outputWriter->WriteLine(QString("Process Finished: Error Code: %1").arg(errCode));
}

int MakefileBasedProjectBuilder::Build(const Project& proj)
{
    QStringList sourceFiles;
    QStringList sourceDirs;
    QString projectRoot = proj.Root();
    QDir dir(projectRoot + "src");
    if (dir.exists())
    {
        QDirIterator dirIt(dir, QDirIterator::Subdirectories);
        while (dirIt.hasNext())
        {
            dirIt.next();
            QString path = dirIt.filePath();

            if (path.endsWith(".") || path.endsWith(".."))
                continue;

            QFileInfo info(path);

            if (!info.isDir())
            {
                sourceFiles << path;
            }
            else
            {
                sourceDirs << path;
            }
        }
    }

    QDir(projectRoot+"bin").mkpath(".");
    QDir(projectRoot+"obj").mkpath(".");

    foreach (QString dir, sourceDirs)
    {
        if (dir.indexOf(projectRoot + "src") == 0) // dir is in project's src folder
        {
            dir.replace(0, QString(projectRoot + "src").size(), QString(projectRoot + "obj"));

            if (dir.size() >= 2)
            {
                if (dir[dir.size() - 1] != '.')
                {
                    QDir d(dir);
                    d.mkpath(".");
                }
            }
        }
    }

   /* try
    {
        if (!FileManager::Exists(projectRoot + "bin/"))
            FileManager::CreateDir(projectRoot + "bin/");
    }
    catch (FileSystemException& ex)
    {
        qDebug() << "Something went wrong while creating bin folder";
    }*/

    QString makefile = makefileBuilder->BuildMakefile(proj);

    try
    {
        FileManager::WriteFile(proj.Root() + "/" + proj.Name() + ".makefile", makefile);
    }
    catch (FileSystemException& ex)
    {
        // show errors while creating makefile
    }
    process->setWorkingDirectory(proj.Root());
    process->start(proj.MakeUtility() + " -f " + proj.Name() + ".makefile " + proj.Name());

    if (process->waitForFinished())
    {
        for (QString& line : QString(process->readAll()).split('\n'))
        {
            if (line.trimmed() != "")
                outputWriter->WriteLine(line.trimmed());
        }
    }

    return process->exitCode();
}
int MakefileBasedProjectBuilder::Clean(const Project& proj)
{
    process->setWorkingDirectory(proj.Root());
    process->start(proj.MakeUtility() + " -f " + proj.Name() + ".makefile " + " clean "); // make target "clean"

    if (process->waitForFinished())
    {
        for (QString& line : QString(process->readAll()).split('\n'))
        {
            if (line.trimmed() != "")
                outputWriter->WriteLine(line.trimmed());
        }
    }
    return process->exitCode();
}
int MakefileBasedProjectBuilder::Rebuild(const Project& proj)
{
    Clean(proj);
    process->waitForFinished();
    Build(proj);
    return process->exitCode();
    /*process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory(proj.Root());

    process->start(proj.MakeUtility() + " -f " + proj.Name() + ".makefile " + " clean ");

    process->waitForFinished();

    process->start(proj.MakeUtility() + " -f " + proj.Name() + ".makefile " + proj.Name());*/
    /*
    this->Clean(proj);
    this->Build(proj);*/
}
