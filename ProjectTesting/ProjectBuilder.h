#ifndef PROJECTBUILDER_H
#define PROJECTBUILDER_H

#include <QObject>

#include "Project.h"
#include "OutputWriter.h"

class ProjectBuilder : public QObject
{
    Q_OBJECT

signals:
    void buildStarted();
    void buildFinished(int);

public:
    virtual int Build(const Project& proj) = 0;
    virtual int Clean(const Project& proj) = 0;
    virtual int Rebuild(const Project& proj) = 0;

    ProjectBuilder();
    virtual ~ProjectBuilder() = 0;
};

#endif // PROJECTBUILDER_H
