#ifndef MAKEFILEBUILDER_H
#define MAKEFILEBUILDER_H

#include <QString>
#include <sstream>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

#include "Project.h"

using namespace std;

class MakefileBuilder
{
public:
    virtual QString BuildMakefile(const Project& project) = 0;
};

#endif // MAKEFILEBUILDER_H
