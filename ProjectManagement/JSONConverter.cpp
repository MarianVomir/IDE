#include "JSONConverter.h"

void JSONConverter::ProjectToFile(const Project *proj, const QString &filePath)
{
    QJsonObject projObject;
    projObject["name"] = proj->Name();
    projObject["compiler"] = proj->Compiler();
    projObject["debugger"] = proj->Debugger();
    projObject["make"] = proj->MakeUtility();
    projObject["compilerFlags"] = proj->CompilerFlags();
    projObject["linkerFlags"] = proj->LinkerFlags();

    QJsonDocument jsonDoc(projObject);
    /*
    QFile projFile(filePath);
    projFile.write(jsonDoc.toJson());
    projFile.close();
    */
    FileManager::WriteFile(filePath, QString(jsonDoc.toJson()));
}

Project *JSONConverter::FileToProject(const QString &filePath)
{
    QFile projFile(filePath);
    if (!projFile.exists())
    {
        throw Exception("The project file was not found!");
    }
    if (projFile.open(QFile::ReadOnly) == false)
    {
        throw Exception("Cannot read project information from project file! Is the file locked?");
    }

    QString fileContents = FileManager::ReadFile(filePath);

    projFile.close();
    QJsonDocument doc(QJsonDocument::fromJson(fileContents.toLocal8Bit()));
    QJsonObject projObject = doc.object();

    Project* proj = new Project(
            projObject["name"].toString(),
            QFileInfo(projFile).path() + "/", // project root is assumed to be current location of the .proj file
            projObject["compiler"].toString(),
            projObject["debugger"].toString(),
            projObject["compilerFlags"].toString(),
            projObject["linkerFlags"].toString(),
            projObject["make"].toString()
            );
    return proj;
}

JSONConverter::JSONConverter()
{
}

JSONConverter::~JSONConverter()
{
}

