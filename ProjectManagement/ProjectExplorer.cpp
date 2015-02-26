#include "ProjectExplorer.h"

const QString ProjectExplorer::newFolderBaseName = "NewFolder";
const QString ProjectExplorer::newFileBaseName = "NewFile";

ProjectExplorer::ProjectExplorer()
{
    project = NULL;
    projectTree = NULL;
    converter = NULL;
    rightClickMenu = new QMenu(NULL);
    projectModel = new QDirModel(NULL);
    watcher = NULL;
}

void ProjectExplorer::AddPathsToWatcher(const QString& root)
{
    if (projectModel != NULL)
        projectModel->refresh();

    QFileInfo info(root);

    if (info.isFile() || !info.exists())
        return;
    else
    {
        for (QString& entry : FileManager::GetFolderContents(root))
        {
            if (QFileInfo(entry).isDir())
                watcher->addPath(entry);
        }
    }
}

ProjectExplorer::~ProjectExplorer()
{
    delete projectModel;
    delete rightClickMenu;
}

void ProjectExplorer::CloseActiveProject()
{
    if (this->projectModel != NULL)
    {
        this->projectTree->setModel(NULL);
        delete this->projectModel;
    }
    if (this->project != NULL)
        delete this->project;

    this->projectModel = NULL;
    this->project = NULL;
}
void ProjectExplorer::SetActiveProject(Project *project)
{
    delete watcher;
    this->project = project;

    if (!FileManager::Exists(project->Root()))
        FileManager::CreateDir(project->Root());

    if (!FileManager::Exists(project->Root() + "src/"))
        FileManager::CreateDir(project->Root() + "src/");
    if (!FileManager::Exists(project->Root() + "obj/"))
        FileManager::CreateDir(project->Root() + "obj/");
    if (!FileManager::Exists(project->Root() + "bin/"))
        FileManager::CreateDir(project->Root() + "bin/");

    if (!FileManager::Exists(project->Root() + project->Name() + ".proj"))
        FileManager::CreateFile(project->Root() + project->Name() + ".proj");

    watcher = new QFileSystemWatcher();
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(AddPathsToWatcher(QString)));
    watcher->addPath(project->Root() + "src/");

    if (this->projectModel != NULL)
    {
        delete this->projectModel;
        this->projectModel = NULL;
    }

    this->projectModel = new ProjectDirModel(NULL, project->Name());
    this->projectModel->setReadOnly(false);
    this->projectModel->setSorting(QDir::DirsLast | QDir::Name);

    this->projectTree->setModel(projectModel);

    this->projectTree->setRootIndex(projectModel->index(project->Root() + "src/"));
    this->projectTree->setContextMenuPolicy(Qt::CustomContextMenu);

    for (int i = 1; i < projectModel->columnCount(); i++)
        this->projectTree->setColumnHidden(i, true);

    this->converter->ProjectToFile(this->project, this->project->Root() + this->project->Name() + ".proj");
}
void ProjectExplorer::SetTree(QTreeView* tree)
{
    if (this->projectTree != NULL)
    {
        disconnect(this->projectTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnProjectTreeRightClick(QPoint)));
    }

    this->projectTree = tree;

    if (this->projectTree != NULL)
    {
        //this->projectTree->setDragDropMode(QAbstractItemView::NoDragDrop);
        this->projectTree->setEditTriggers(QTreeView::NoEditTriggers);
        connect(this->projectTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(OnProjectTreeRightClick(QPoint)));
    }
}
void ProjectExplorer::SetProjectFileConverter(ProjectFileConverter *converter)
{
    this->converter = converter;
}
const Project* ProjectExplorer::GetProject() const
{
    return this->project;
}
QTreeView* ProjectExplorer::GetTree()
{
    return this->projectTree;
}

QFileInfo ProjectExplorer::FileAt(QModelIndex index)
{
    if (projectModel != NULL && index.isValid())
        return this->projectModel->fileInfo(index);
    else
        return QFileInfo();
}
void ProjectExplorer::OnProjectTreeRightClick(QPoint p)
{
    rightClickMenu->clear();
    QModelIndex index = projectTree->indexAt(p);
    if (projectModel != NULL)
    {
        if (index.isValid())
        {
            rightClickMenu->addAction("Rename", this, SLOT(OnRenameClicked()));
            if (projectModel->isDir(index))
            {
                rightClickMenu->addAction("New File", this, SLOT(OnProjectNewFileClicked()));
                rightClickMenu->addAction("Add Subfolder", this, SLOT(OnProjectNewSubfolderClicked()));
                rightClickMenu->addAction("Delete Folder", this, SLOT(OnProjectDeleteFolderClicked()));
            }
            else
            {
                rightClickMenu->addAction("Delete File", this, SLOT(OnProjectDeleteFileClicked()));
            }
        }
        else
        {
            rightClickMenu->addAction("New File", this, SLOT(OnProjectNewFileAtRootClicked()));
            rightClickMenu->addAction("New Folder", this, SLOT(OnProjectNewFolderAtRootClicked()));
            projectTree->selectionModel()->setCurrentIndex(QModelIndex(), QItemSelectionModel::Clear);
        }
    }

    rightClickMenu->exec(projectTree->mapToGlobal(p) + QPoint(5, 20));
}

void ProjectExplorer::OnProjectNewFileClicked()
{
    QModelIndex index = this->GetTreeSelectedIndex();

    if (index.isValid())
    {
        QFileInfo info = projectModel->fileInfo(index);

        this->CreateFile(info.filePath() + "/" + newFileBaseName);
        projectModel->refresh();
        //projectTree->expand(index);
    }

    //projectTree->expand(index);
}
void ProjectExplorer::OnProjectNewSubfolderClicked()
{
    QModelIndex index = this->GetTreeSelectedIndex();
    if (index.isValid())
    {
        QFileInfo info = projectModel->fileInfo(index);

        if (info.isDir())
        {
            this->CreateDir(info.filePath() + "/" + newFolderBaseName);
        }

       projectModel->refresh();
       /* if (!projectTree->isExpanded(index))*/
      // projectTree->expand(index);


    }
}
void ProjectExplorer::OnProjectNewFileAtRootClicked()
{
    this->CreateFile(project->Root() + "src/" + newFileBaseName);
}
void ProjectExplorer::OnProjectNewFolderAtRootClicked()
{
    this->CreateDir(project->Root() + "src/" + newFolderBaseName);
}
void ProjectExplorer::OnProjectDeleteFileClicked()
{
    QModelIndex index = this->GetTreeSelectedIndex();

    if(index.isValid())
    {
        QFileInfo info = projectModel->fileInfo(index);
        this->DeleteFile(info.filePath());
    }
}
void ProjectExplorer::OnProjectDeleteFolderClicked()
{
    QModelIndex index = this->GetTreeSelectedIndex();

    if (index.isValid())
    {
        QFileInfo info = projectModel->fileInfo(index);
        if (info.isDir())
        {
            this->DeleteDir(info.filePath());
        }
    }
}
void ProjectExplorer::OnRenameClicked()
{
    QModelIndex index = this->GetTreeSelectedIndex();
    if (index.isValid())
        projectTree->edit(index);
    projectModel->refresh();
}
void ProjectExplorer::Refresh()
{
    if (projectModel != NULL)
    {
        projectModel->refresh();
    }
}
QModelIndex ProjectExplorer::GetTreeSelectedIndex()
{
    QModelIndex index;
    QModelIndexList selectedIndexes = projectTree->selectionModel()->selectedIndexes();
    if (selectedIndexes.size() > 0)
    {
        index = selectedIndexes[0];
    }

    return index;
}

void ProjectExplorer::CreateFile(const QString& filePath)
{
    QString path = filePath;
    uint i = 1;

    while (FileManager::Exists(path))
    {
        path = QString(filePath + "%1").arg(i);
        i++;
    }
    try
    {
        FileManager::CreateFile(path);
        projectModel->refresh();

        QQueue<QModelIndex> queue;

        queue.enqueue(projectModel->index(project->Root() + "/src/"));
        while (queue.size() > 0)
        {
            QModelIndex crtParent = queue.dequeue();
            for (int row = 0; row < projectModel->rowCount(crtParent); row++)
            {
                QModelIndex crtChild = projectModel->index(row, 0, crtParent);
                if (projectModel->hasChildren(crtChild))
                    queue.enqueue(crtChild);
                else
                {
                    if (!projectModel->isDir(crtChild))
                    {
                        if (path == projectModel->filePath(crtChild))
                        {
                            projectTree->expand(crtParent);
                            projectTree->selectionModel()->select(crtChild, QItemSelectionModel::ClearAndSelect);
                            projectTree->edit(crtChild);
                            queue.clear();
                        }
                    }
                }
            }
        }
    }
    catch (FileSystemException& ex)
    {
        QMessageBox::warning(NULL, "Cannot create file", ex.Message());
    }
}
void ProjectExplorer::CreateDir(const QString& dirPath)
{
    uint i = 1;
    QString path = dirPath;

    while (FileManager::Exists(path))
    {
        path = QString(dirPath + "%1").arg(i);
        i++;
    }

    try
    {
         FileManager::CreateDir(path);
         projectModel->refresh();

         QQueue<QModelIndex> queue;

         queue.enqueue(projectModel->index(project->Root() + "/src/"));
         while (queue.size() > 0)
         {
             QModelIndex crtParent = queue.dequeue();
             for (int row = 0; row < projectModel->rowCount(crtParent); row++)
             {
                 QModelIndex crtChild = projectModel->index(row, 0, crtParent);
                 if (projectModel->hasChildren(crtChild))
                     queue.enqueue(crtChild);
                 else
                 {
                     if (path == projectModel->filePath(crtChild))
                     {
                         projectTree->expand(crtParent);
                         projectTree->selectionModel()->select(crtChild, QItemSelectionModel::ClearAndSelect);
                         projectTree->edit(crtChild);
                         queue.clear();
                     }
                 }
             }
         }

         QModelIndex indexOfAddedDir = this->projectModel->index(path);
         projectTree->expand(indexOfAddedDir);
         if (indexOfAddedDir.isValid())
         {
            this->projectTree->selectionModel()->select(indexOfAddedDir, QItemSelectionModel::ClearAndSelect);
            this->projectTree->edit(indexOfAddedDir);
         }
    }
    catch (FileSystemException& ex)
    {
        QMessageBox::warning(NULL, "Cannot create folder", ex.Message());
    }
}
void ProjectExplorer::DeleteFile(const QString& filePath)
{
    QMessageBox::StandardButton answer = QMessageBox::question(
                NULL,
                "About to remove file!",
                "About to remove " + filePath + " PERMANENTLY. Continue?",
                QMessageBox::Yes | QMessageBox::No
            );
    if (answer == QMessageBox::Yes)
    {
        try
        {
            FileManager::DeleteFile(filePath);
            projectModel->refresh();
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot delete file", ex.Message());
        }
    }
}
void ProjectExplorer::DeleteDir(const QString& dirPath)
{
    QDir dir(dirPath);

    bool confirmRemove = true;

    if (dir.entryInfoList(QDir::NoDotAndDotDot|QDir::AllEntries).count() > 0) // dir is not empty
    {
        QMessageBox::StandardButton answer = QMessageBox::question(
                    NULL,
                    "About to remove dir!",
                    "Remove NON-EMPTY directory " + dirPath + " and all its contents PERMANENTLY?",
                    QMessageBox::Yes | QMessageBox::No
                );
        if (answer == QMessageBox::Yes)
        {
            confirmRemove = true;
        }
        else
        {
            confirmRemove = false;
        }
    }
    else // dir is empty
    {
        confirmRemove = true;
    }

    if (confirmRemove)
    {
        try
        {
            FileManager::DeleteDir(dirPath);
            projectModel->refresh();
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot delete folder", ex.Message());
        }
    }
}
