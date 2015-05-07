#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>

class SettingsManager
{
private:
    SettingsManager();
    ~SettingsManager();
public:
    static void Load();
    static void Save();
};

#endif // SETTINGSMANAGER_H
