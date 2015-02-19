#ifndef PROJECTBUILDER_H
#define PROJECTBUILDER_H

#include "Project.h"

#include "OutputWriter.h"

#include <QObject>

class ProjectBuilder : public QObject
{
    Q_OBJECT

protected:
    OutputWriter* outputWriter;

public:
    virtual void Build(const Project& proj) = 0;
    virtual void Clean(const Project& proj) = 0;
    virtual void Rebuild(const Project& proj) = 0;

    ProjectBuilder(OutputWriter* outputWriter) : outputWriter(outputWriter) {}
    virtual ~ProjectBuilder() = 0;
};

#endif // PROJECTBUILDER_H
