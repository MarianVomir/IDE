#include "ui_mainwindow.h"
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    converter = new JSONConverter();
    projectExplorer = new ProjectExplorer();
    projectExplorer->SetTree(ui->projectTree);
    projectExplorer->SetProjectFileConverter(converter);

    /*makeFileBuilder = new MakefileBuilder();
    tabEditor = new TabEditor(this);
    tabEditor->TabWidget(ui->tabEditor);
*/
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

    editor = new Editor(ui->tabEditor);
/*

    connect(tabEditor->TabWidget(), SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));
*/
    connect(projectExplorer->GetTree(), SIGNAL(doubleClicked(QModelIndex)), this, SLOT(OnProjectExplorerDoubleClicked(QModelIndex)));
}
MainWindow::~MainWindow()
{
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
}
void MainWindow::Exit()
{
    this->close();
}
/*
void MainWindow::SetMediator(Mediator* mediator)
{
    this->mediator = mediator;
}*/
/*
void MainWindow::SetProject(const Project& proj)
{
    try
    {
        projectExplorer->SetProject(proj);
    }
    catch (Exception& e)
    {
        QMessageBox::warning(this, "Cannot create project", e.what());
    }
}
void MainWindow::on_actionRefresh_Project_Explorer_triggered()
{
    projectExplorer->Refresh();
}
void MainWindow::on_actionQuit_triggered()
{
    this->close();
}
void MainWindow::on_actionNew_Project_triggered()
{
    NewProjectDialog newProjDiag(this);
    newProjDiag.setModal(true);
    newProjDiag.exec();
}

void MainWindow::CloseTab(int index)
{
    tabEditor->CloseTab(index);
}
void MainWindow::NewTab(QModelIndex index)
{
    QFileInfo info = projectExplorer->GetModel()->fileInfo(index);

    tabEditor->AddTab(info);
}

void MainWindow::on_actionBuild_triggered()
{
    const Project& project = projectExplorer->GetProject();

    QString makefile = makeFileBuilder->BuildMakefile(project);
    QFile file(project.DirPath() + project.Name() + "/" + project.Name() + ".makefile");
    file.open(QFile::WriteOnly);
    file.write(makefile.toStdString().c_str(), makefile.size());
    file.close();

    QProcess* process = new QProcess(this);
    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory(project.DirPath() + project.Name());

    process->start(project.MakePath() + " -f " + project.Name() + ".makefile " + project.Name());

    if (!process->waitForFinished())
        qDebug() << "Make failed: " << process->errorString();
    else
        qDebug() << "Make output: " << process->readAll();
}*/

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
