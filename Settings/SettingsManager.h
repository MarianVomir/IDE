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
    SettingsManager();
    ~SettingsManager();

public:
    static void LoadProjectDefaults();
    static void SaveProjectDefaults();
};

#endif // SETTINGSMANAGER_H
