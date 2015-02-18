#include "SimpleMakefileBuilder.h"

SimpleMakefileBuilder::SimpleMakefileBuilder()
{

}

SimpleMakefileBuilder::~SimpleMakefileBuilder()
{

}

QString SimpleMakefileBuilder::BuildMakefile(const Project& project)
{
    QStringList sourceFiles;
    QStringList sourceDirs;
    QString projectRoot = project.Root();// + "/";
    QDir dir(projectRoot + "src");
    if (dir.exists())
    {
        QDirIterator dirIt(dir, QDirIterator::Subdirectories);
        while (dirIt.hasNext())
        {
            dirIt.next();
            QString path = dirIt.filePath();
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

    foreach (QString dir, sourceDirs)
    {
        if (dir.indexOf(projectRoot + "src") == 0) // dir is in project's src folder
        {
            dir.replace(0, QString(projectRoot + "src").size(), QString(projectRoot + "bin"));

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

    stringstream ss;
    ss << "compiler=" << project.Compiler().toStdString() << "\n";
    ss << "cFlags=" << project.CompilerFlags().toStdString() << "\n";
    ss << "lFlags=" << project.LinkerFlags().toStdString() << "\n";

    ss << "all: " << project.Name().toStdString() << "\n";
    ss << project.Name().toStdString() << " : ";

    QString projectSrcDir = projectRoot + "src/";

    QStringList validSourceFileList;
    QStringList objectFileList;
    QString objectFile = "";
    for (QString fileName : sourceFiles)
    {
        if (fileName.size() >= 2)
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
