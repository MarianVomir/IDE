#ifndef SIMPLEMAKEFILEBUILDER_H
#define SIMPLEMAKEFILEBUILDER_H

#include "ProjectTesting/MakefileBuilder.h"

class SimpleMakefileBuilder : public MakefileBuilder
{
public:
    virtual QString BuildMakefile(const Project& project);

    SimpleMakefileBuilder();
    ~SimpleMakefileBuilder();
};

#endif // SIMPLEMAKEFILEBUILDER_H
