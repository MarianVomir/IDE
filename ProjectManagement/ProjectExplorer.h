#ifndef PROJECTEXPLORER_H
#define PROJECTEXPLORER_H

#include "Project.h"
#include "ProjectManagement/ProjectDirModel.h"
#include "FileManager.h"
#include "ProjectManagement/ProjectFileConverter.h"

#include <QTreeView>
#include <QDirModel>
#include <QMessageBox>
#include <QString>
#include <QMenu>
#include <QPoint>


#include <QQueue>

class ProjectExplorer : QObject
{
    Q_OBJECT

private:
    const static QString newFolderBaseName;
    const static QString newFileBaseName;

    Project* project;
    QMenu* rightClickMenu;

    QTreeView* projectTree;
    QDirModel* projectModel;
    ProjectFileConverter* converter;

private slots:
    void OnProjectTreeRightClick(QPoint p);

    void OnProjectNewFileClicked();
    void OnProjectNewSubfolderClicked();
    void OnProjectNewFileAtRootClicked();
    void OnProjectNewFolderAtRootClicked();
    void OnProjectDeleteFileClicked();
    void OnProjectDeleteFolderClicked();
    void OnRenameClicked();
    QModelIndex GetTreeSelectedIndex();

public:
    ProjectExplorer();
    virtual ~ProjectExplorer();

    void CloseActiveProject();
    void SetActiveProject(Project* proj);
    void SetTree(QTreeView* tree);
    void SetProjectFileConverter(ProjectFileConverter* converter);
    const Project* GetProject() const;
    const QTreeView* GetTree() const;

    QFileInfo FileAt(QModelIndex index);

public slots:
    void Refresh();

private:
    void CreateFile(const QString& filePath);
    void CreateDir(const QString& dirPath);
    void DeleteFile(const QString& filePath);
    void DeleteDir(const QString& dirPath);
};

#endif // PROJECTEXPLORER_H
