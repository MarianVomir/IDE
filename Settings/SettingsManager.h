#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QFile>
#include <QJsonDocument>

#include "FileManager.h"
#include "Global.h"

class SettingsManager
{
private:
    static QString projectDefaultSettingsFile;

    SettingsManager();
    ~SettingsManager();

public:
    static void LoadProjectDefaults();
    static void SaveProjectDefaults(const QJsonObject &defaultObject);
};

#endif // SETTINGSMANAGER_H
