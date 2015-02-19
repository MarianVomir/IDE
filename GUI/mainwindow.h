#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirModel>
#include <QCompleter>
#include <QLineEdit>
#include <QSplitter>
#include <QTextDocument>
#include <QTextEdit>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QProcess>

#include "Project.h"

#include "ProjectManagement/ProjectExplorer.h"
#include "ProjectManagement/ProjectFileConverter.h"
#include "ProjectManagement/JSONConverter.h"

#include "Editing/Editor.h"

#include "ProjectTesting/ProjectBuilder.h"
#include "ProjectTesting/MakefileBasedProjectBuilder.h"
#include "ProjectTesting/MakefileBuilder.h"
#include "ProjectTesting/SimpleMakefileBuilder.h"
#include "ProjectTesting/ListOutputWriter.h"

#include "newprojectdialog.h"
#include "openprojectdialog.h"

namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void SetProjectExplorer(ProjectExplorer* projectExplorer);
    void SetOpenProject(Project *project);
    void SetNewProject(Project *project);

private slots:

    void on_actionNew_Project_triggered();

    void on_actionOpen_Project_triggered();

    void on_actionExit_triggered();

    void on_actionClose_Project_triggered();

    void on_actionRefreshProjectExplorer_triggered();

    void OnProjectExplorerDoubleClicked(QModelIndex index);

    void on_actionOpen_File_triggered();

    void on_actionSave_File_triggered();

    void on_actionSave_File_As_triggered();

    void on_actionNew_File_triggered();

    void on_actionBuild_triggered();

    void on_actionClean_triggered();

    void on_actionRebuild_triggered();

private:
    Ui::MainWindow *ui;

    ProjectExplorer* projectExplorer;
    ProjectFileConverter* converter;
    Editor* editor;
    ProjectBuilder* projectBuilder;
    MakefileBuilder* makefileBuilder;
    OutputWriter* outputWriter;

public:
    void OnNewProject();
    void OnOpenProject();
    void OnCloseProject();
    void Exit();

};

#endif // MAINWINDOW_H
