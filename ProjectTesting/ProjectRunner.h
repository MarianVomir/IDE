#ifndef PROJECTRUNNER_H
#define PROJECTRUNNER_H

#include "ProjectBuilder.h"
#include "OutputWriter.h"

#include <QProcess>

class ProjectRunner : public QObject
{
    Q_OBJECT

protected:
    ProjectBuilder* projectBuilder;
    OutputWriter* outputWriter;
    QProcess* process;

private slots:
    void OnProcessStarted();
    void OnProcessFinished(int);

signals:
    void runStarted();
    void runFinished(int);

public:
    void Run(const Project &proj, const QString& commandLineArgs = "");

    ProjectRunner(ProjectBuilder* projectBuilder, OutputWriter* outputWriter);
    ~ProjectRunner();
};

#endif // PROJECTRUNNER_H
