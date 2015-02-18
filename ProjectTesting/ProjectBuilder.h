#ifndef PROJECTBUILDER_H
#define PROJECTBUILDER_H

#include "Project.h"

class ProjectBuilder
{
public:
    virtual void Build(const Project& proj) = 0;
    virtual void Clean(const Project& proj) = 0;
    virtual void Rebuild(const Project& proj) = 0;
};

#endif // PROJECTBUILDER_H
