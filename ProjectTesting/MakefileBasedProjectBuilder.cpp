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
    QMap<QString, QStringList> headerDependencies;

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
                    QStringList headers = GetUserIncludedHeaders(fileName);

                    fileName.replace(0, projectRoot.size(), "");
                    objectFile = fileName;
                    objectFile.replace(0, QString("src/").size(), "obj/");
                    objectFile[objectFile.size() - 1] = 'o'; // .o extension

                    objectFileList << objectFile;
                    validSourceFileList << fileName;

                    headerDependencies[fileName] = headers;
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
    ss << "\t" << "$(compiler) " << "-o " << (QString("bin/") + project.Name()).toStdString() << " ";

    for (QString objectFile : objectFileList)
    {
        ss << objectFile.toStdString() << " ";
    }
    ss << "$(lFlags)\n";

    for (int i = 0; i < objectFileList.size(); i++)
    {
        ss << objectFileList[i].toStdString() << " : " << validSourceFileList[i].toStdString() << " ";


       /* for (auto header : headerDependencies[validSourceFileList[i]])
            ss << header.toStdString() << " ";
*/

        ss << "\n";
        string debugFlag = ""; // "-g" - when debugger is implemented
        ss << "\t" << string("$(compiler) -Wall ") + debugFlag << " -c " << validSourceFileList[i].toStdString() << " -o " << objectFileList[i].toStdString() << " $(cFlags)\n";
    }

 /*   for (auto headerList : headerDependencies.values())
    {
        for (auto header : headerList)
        {
            ss << header.toStdString() << " : " << "\n\t\n";
        }
    }
*/
    ss << ".PHONY: clean rm all find" << "\n";

    ss << "clean:\n\tfind obj/ -name '*.o' -type f -delete";
    ss << "\n\trm bin/" + project.Name().toStdString();
    return QString(ss.str().c_str());
}

// Not used right now
QStringList MakefileBasedProjectBuilder::GetUserIncludedHeaders(const QString &cFile)
{
    QStringList headers;
    static QRegExp userDefHeaderRegex("#include +\"(.+)\"");
    userDefHeaderRegex.setMinimal(true);

    try
    {
        QString text = FileManager::ReadFile(cFile);

        int index = 0;
        int offset = 0;

        while (index != -1)
        {
            index = userDefHeaderRegex.indexIn(text, offset, QRegExp::CaretAtOffset);
            if (index == -1)
                break;

            QString header = userDefHeaderRegex.cap(1);

            header = QDir(QFileInfo(cFile).absolutePath()).relativeFilePath(header);

            headers.append(header);
            offset += userDefHeaderRegex.matchedLength();
        }

    }
    catch (FileSystemException& e)
    {
        qDebug() << QString("Cannot read file to determine header dependencies: ") << e.Message();
    }

    return headers;
}

MakefileBasedProjectBuilder::MakefileBasedProjectBuilder(OutputWriter* outputWriter)
    : outputWriter(outputWriter)
{
    process = new QProcess();
    process->setProcessChannelMode(QProcess::MergedChannels);
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

    if (proj.Compiler() == "" || proj.MakeUtility() == "")
    {
        emit buildFinished(-1);
        return -1;
    }

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
        qDebug() << "Could not create makefile";
        return QProcess::FailedToStart;
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
    if (proj.MakeUtility() == "")
        return -1;

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
}
