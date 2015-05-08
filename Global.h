#ifndef GLOBAL_H
#define GLOBAL_H

#include <QJsonObject>

#include "Project.h"

class Global
{
public:
    static QJsonObject defaultProjectValues;
    // visual styles

public:
    Global();
    ~Global();
};

#endif // GLOBAL_H
