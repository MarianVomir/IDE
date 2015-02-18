#include "ui_mainwindow.h"
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->project = NULL;

    ui->setupUi(this);
    converter = new JSONConverter();
    projectExplorer = new ProjectExplorer();
    projectExplorer->SetTree(ui->projectTree);
    projectExplorer->SetProjectFileConverter(converter);

    makefileBuilder = new SimpleMakefileBuilder();
    projectBuilder = new MakefileBasedProjectBuilder(makefileBuilder);

    editor = new Editor(ui->tabEditor);

    QList<int> sizes;
    sizes.append(200);
    sizes.append(900);
    ui->splitter->setSizes(sizes);

    sizes.clear();
    sizes.append(800);
    sizes.append(200);
    ui->splitter_2->setSizes(sizes);

    ui->mainToolBar->setFloatable(false);
    ui->mainToolBar->setMovable(false);

    connect(projectExplorer->GetTree(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnProjectExplorerDoubleClicked(QModelIndex)));
}
MainWindow::~MainWindow()
{
    delete projectExplorer;
    delete converter;
    delete projectBuilder;
    delete makefileBuilder;
    delete editor;

    delete ui;
}

void MainWindow::OnProjectExplorerDoubleClicked(QModelIndex index)
{
    QFileInfo info = this->projectExplorer->FileAt(index);
    if (info.isFile())
        this->editor->OpenFile(info.absoluteFilePath());
}

void MainWindow::SetProjectExplorer(ProjectExplorer* projectExplorer)
{
    this->projectExplorer = projectExplorer;
}

void MainWindow::SetNewProject(Project *project)
{
    this->project = project;
    if (FileManager::Exists(project->Root()) && FileManager::IsDir(project->Root())) // projectDir.exists())
    {
        QMessageBox::StandardButton answer = QMessageBox::question(
                 NULL,
                 "Warning",
                 "Directory already exists, files may be overridden, create project?",
                 QMessageBox::Yes | QMessageBox::No
                 );

        if (answer == QMessageBox::No)
        {
            return;
        }
    }
    projectExplorer->SetActiveProject(project);
}
void MainWindow::SetOpenProject(Project* project)
{
    this->project = project;
    projectExplorer->SetActiveProject(project);
}

void MainWindow::OnNewProject()
{
    NewProjectDialog newProjDiag(this);
    newProjDiag.setModal(true);
    newProjDiag.exec();
}
void MainWindow::OnOpenProject()
{
    OpenProjectDialog openProjDialog(this, this->converter);
    if (openProjDialog.FileHasBeenSelected())
    {
        openProjDialog.setModal(true);
        openProjDialog.exec();
    }
}
void MainWindow::OnCloseProject()
{
    projectExplorer->CloseActiveProject();
    delete this->project;
    this->project = NULL;
}
void MainWindow::Exit()
{
    this->close();
}
void MainWindow::on_actionNew_Project_triggered()
{
    this->OnNewProject();
}

void MainWindow::on_actionOpen_Project_triggered()
{
    this->OnOpenProject();
}

void MainWindow::on_actionExit_triggered()
{
    this->Exit();
}

void MainWindow::on_actionClose_Project_triggered()
{
    this->OnCloseProject();
}

void MainWindow::on_actionRefreshProjectExplorer_triggered()
{
    this->projectExplorer->Refresh();
}

void MainWindow::on_actionOpen_File_triggered()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Open File(s)", QDir::homePath());
    for (int i = 0; i < files.size(); i++)
    {
        this->editor->OpenFile(files[i]);
    }
}

void MainWindow::on_actionSave_File_triggered()
{
    this->editor->SaveFile();
}

void MainWindow::on_actionSave_File_As_triggered()
{
    this->editor->SaveFileAs();
}

void MainWindow::on_actionNew_File_triggered()
{
    this->editor->NewFile();
}

void MainWindow::on_actionBuild_triggered()
{
    if (this->project != NULL)
        this->projectBuilder->Build(*this->project);
}

void MainWindow::on_actionClean_triggered()
{
    if (this->project != NULL)
        this->projectBuilder->Clean(*this->project);
}

void MainWindow::on_actionRebuild_triggered()
{
    if (this->project != NULL)
        this->projectBuilder->Rebuild(*this->project);
}
