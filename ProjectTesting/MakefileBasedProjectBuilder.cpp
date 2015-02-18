#include "MakefileBasedProjectBuilder.h"

MakefileBasedProjectBuilder::MakefileBasedProjectBuilder(MakefileBuilder *makefileBuilder)
{
    this->makefileBuilder = makefileBuilder;
}

void MakefileBasedProjectBuilder::Build(const Project& proj)
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
