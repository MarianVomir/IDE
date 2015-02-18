#ifndef MEDIATOR_H
#define MEDIATOR_H

#include "Project.h"
#include "GUI/newprojectdialog.h"
#include "GUI/mainwindow.h"
#include "ProjectManagement/ProjectExplorer.h"
#include "Exceptions.h"
#include <QMessageBox>

class MainWindow;
class Mediator
{
private:
    MainWindow* mainWindow;
    Project* project;
    ProjectExplorer* projectExplorer;

public:
    void SetMainWindow(MainWindow* mainWindow);
    void SetActiveProject(Project* project);
    void SetProjectExplorer(ProjectExplorer* projectExplorer);

    void OnNewProject();

    Mediator();
    ~Mediator();
};

#endif // MEDIATOR_H
