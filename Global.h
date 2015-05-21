#ifndef GLOBAL_H
#define GLOBAL_H

#include <QJsonObject>

#include "Project.h"
#include "GUI/VisualStyles.h"

class Global
{
public:
    static QJsonObject defaultProjectValues;
    static VisualStyles Visual;

public:
    Global();
    ~Global();
};

#endif // GLOBAL_H
