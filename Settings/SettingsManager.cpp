#include "SettingsManager.h"

SettingsManager::SettingsManager()
{

}

SettingsManager::~SettingsManager()
{

}

void SettingsManager::LoadProjectDefaults()
{
    QString fileContents;
    static QString fileName = "settings_projectdefault";
    QFile settingsFile(fileName);
    bool failed = false;
    if (!settingsFile.exists())
    {
        failed = true;
    }
    else
    {
        fileContents = FileManager::ReadFile(fileName);
    }

    QString compiler, linker, make, compilerFlags, linkerFlags;

    QJsonObject jsonObject;
    if (failed)
    {
        jsonObject["compiler_default"] = "";
        jsonObject["debugger_default"] = "";
        jsonObject["make_default"] = "";
        jsonObject["compiler_default"] = "";
        jsonObject["linkerFlags_default"] = "";
    }
    else
    {
        QJsonDocument doc(QJsonDocument::fromJson(fileContents.toLocal8Bit()));
        jsonObject = doc.object();

        compiler = jsonObject["compiler_default"].toString();
        linker = jsonObject["debugger_default"].toString();
        make = jsonObject["make_default"].toString();
        compilerFlags = jsonObject["compiler_default"].toString();
        linkerFlags = jsonObject["linkerFlags_default"].toString();
    }

    Global::defaultProjectValues = jsonObject;
}


void SettingsManager::SaveProjectDefaults()
{}

