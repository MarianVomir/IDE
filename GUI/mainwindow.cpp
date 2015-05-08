#include "ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
   // statusBar()->hide();
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

    converter = new JSONConverter();
    projectExplorer = new ProjectExplorer();
    projectExplorer->SetTree(ui->projectTree);
    projectExplorer->SetProjectFileConverter(converter);

    outputWriter = new ListOutputWriter(ui->outputWindow);
    projectBuilder = new MakefileBasedProjectBuilder(outputWriter);
    projectRunner = new ProjectRunner(projectBuilder, outputWriter);

    editor = new Editor(ui->tabEditor);

    connect(projectExplorer->GetTree(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnProjectExplorerDoubleClicked(QModelIndex)));
    connect(projectExplorer->GetTree(), SIGNAL(activated(QModelIndex)), this, SLOT(OnProjectExplorerDoubleClicked(QModelIndex)));

    connect(projectRunner, SIGNAL(runStarted()), this, SLOT(OnProjectInUse()));
    connect(projectRunner, SIGNAL(runFinished(int)), this, SLOT(OnProjectFinishedUse(int)));
    connect(projectBuilder, SIGNAL(buildStarted()), this, SLOT(OnProjectInUse()));
    connect(projectBuilder, SIGNAL(buildFinished(int)), this, SLOT(OnProjectFinishedUse(int)));
}
MainWindow::~MainWindow()
{
    delete editor;

    delete projectExplorer;
    delete converter;
    delete projectBuilder;
    delete ui;
}

void MainWindow::OnProjectExplorerDoubleClicked(QModelIndex index)
{
    QFileInfo info = this->projectExplorer->FileAt(index);
    if (info.isFile())
        this->editor->OpenFile(info.absoluteFilePath());
    if (info.isDir())
        this->projectExplorer->GetTree()->expand(index);
}

void MainWindow::SetNewProject(Project *project)
{
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
    openProjDialog.setModal(true);
    openProjDialog.exec();
}
void MainWindow::OnCloseProject()
{
    projectExplorer->CloseActiveProject();
}
void MainWindow::closeEvent(QCloseEvent* event)
{
    if (this->editor->AskSaveAll() == true)
    {
        // all files have been iterated through and have either been saved or discarded
        event->accept();
    }
    else
    {
        // if AskSaveAll returns false, it means the user clicked the Cancel button for a tab, therefore we don't close the app yet.
        event->ignore();
    }
}
void MainWindow::Exit()
{
    this->close();
}

// GUI Actions
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
        try
        {
            this->editor->OpenFile(files[i]);
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot read file", ex.Message());
        }
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
    const Project* project = projectExplorer->GetProject();
    if (project != NULL)
    {
        outputWriter->Clear();
        this->projectBuilder->Build(*project);
    }
}
void MainWindow::on_actionClean_triggered()
{
    const Project* project = projectExplorer->GetProject();
    if (project != NULL)
    {
        outputWriter->Clear();
        this->projectBuilder->Clean(*project);
    }
}
void MainWindow::on_actionRebuild_triggered()
{
    const Project* project = projectExplorer->GetProject();
    if (project != NULL)
    {
        outputWriter->Clear();
        this->projectBuilder->Rebuild(*project);
    }
}
void MainWindow::on_actionRun_triggered()
{
    const Project* project = projectExplorer->GetProject();
    if (project != NULL)
    {
        outputWriter->Clear();
        this->projectRunner->Run(*project);
    }
}

void MainWindow::OnProjectInUse()
{
    QMenu* projectMenu = ui->menuBar->findChildren<QMenu*>().at(1);
    projectMenu->setEnabled(false);
}

void MainWindow::OnProjectFinishedUse(int)
{
    QMenu* projectMenu = ui->menuBar->findChildren<QMenu*>().at(1);
    projectMenu->setEnabled(true);
}

void MainWindow::on_actionProject_Explorer_toggled(bool arg1)
{
    ui->projectTree->setVisible(arg1);
}

void MainWindow::on_actionOutputWindow_toggled(bool arg1)
{
    ui->outputWindow->setVisible(arg1);
}

void MainWindow::on_actionOpen_Settings_triggered()
{
    SettingsDialog settingsDialog(projectExplorer, this);
    settingsDialog.setModal(true);
    settingsDialog.exec();
}
