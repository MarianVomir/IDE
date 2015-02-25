#include "MakefileBasedProjectBuilder.h"

QString MakefileBasedProjectBuilder::BuildMakefile(const Project& project)
{
    std::stringstream ss;
    ss << "compiler=" << project.Compiler().toStdString() << "\n";
    ss << "cFlags=" << project.CompilerFlags().toStdString() << "\n";
    ss << "lFlags=" << project.LinkerFlags().toStdString() << "\n";

    ss << "all: " << project.Name().toStdString() << "\n";
    ss << project.Name().toStdString() << " : ";

    QString projectRoot = project.Root();
    QString projectSrcDir = projectRoot + "src/";

    QStringList validSourceFileList;
    QStringList objectFileList;
    QString objectFile = "";

    QStringList sourceFiles;
    QStringList sourceDirs;
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

    for (QString fileName : sourceFiles)
    {
        if (fileName.size() > 2)
        {
            if (fileName[fileName.size() - 1] == 'c' && fileName[fileName.size() - 2] == '.') // found a .c source file
            {
                int index = fileName.indexOf(projectSrcDir);
                if (index != 0)
                {
                    // file omitted because it is not in the project's source directory
                }
                else
                {
                    fileName.replace(0, projectRoot.size(), "");
                    objectFile = fileName;
                    objectFile.replace(0, QString("src/").size(), "obj/");
                    objectFile[objectFile.size() - 1] = 'o'; // .o extension

                    objectFileList << objectFile;
                    validSourceFileList << fileName;
                }
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

    for (QString objectFile : objectFileList)
    {
        ss << objectFile.toStdString() << " ";
    }
    ss << "\n";
    ss << "\t" << "$(compiler) $(lFlags)" << " -o " << (QString("bin/") + project.Name()).toStdString() << " ";

    for (QString objectFile : objectFileList)
    {
        ss << objectFile.toStdString() << " ";
    }
    ss << "\n";

    for (int i = 0; i < objectFileList.size(); i++)
    {
        ss << objectFileList[i].toStdString() << " : " << validSourceFileList[i].toStdString() << "\n";
        ss << "\t" << "$(compiler) -Wall -g $(cFlags)" << " -c " << validSourceFileList[i].toStdString() << " -o " << objectFileList[i].toStdString() << "\n";
    }

    ss << ".PHONY: clean rm all" << "\n";

    ss << "clean:\n\trm -rf  obj/*.o bin/" + project.Name().toStdString();

    return QString(ss.str().c_str());
}

MakefileBasedProjectBuilder::MakefileBasedProjectBuilder(OutputWriter* outputWriter)
    : ProjectBuilder(outputWriter)
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
    for (QString& line : QString(process->readAll()).split('\n'))
    {
        if (line.trimmed() != "")
            outputWriter->WriteLine(line.trimmed());
    }
    outputWriter->WriteLine(QString("Process Finished: Error Code: %1").arg(errCode));
}

int MakefileBasedProjectBuilder::Build(const Project& proj)
{
    emit buildStarted();

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

    QString makefile = this->BuildMakefile(proj);

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

    emit buildFinished(process->exitCode());

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
