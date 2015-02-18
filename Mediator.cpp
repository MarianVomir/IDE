#include "Mediator.h"

void Mediator::SetMainWindow(MainWindow* mainWindow)
{
    this->mainWindow = mainWindow;
}
void Mediator::SetProjectExplorer(ProjectExplorer* projectExplorer)
{
    this->projectExplorer = projectExplorer;
}
void Mediator::SetActiveProject(Project* project)
{
    //this->project = project;
    try
    {
       // projectExplorer->CloseActiveProject();
        projectExplorer->SetActiveProject(project);
    }
    catch (Exception& e)
    {
        QMessageBox::warning(NULL, "Cannot create project", e.Message());
    }
}

void Mediator::OnNewProject()
{
    NewProjectDialog newProjDiag(this);
    newProjDiag.setModal(true);
    newProjDiag.exec();
}

Mediator::Mediator()
{
    this->project = NULL;
    this->projectExplorer = NULL;
    this->mainWindow = NULL;
}

Mediator::~Mediator()
{

}

