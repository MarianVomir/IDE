#include "Editor.h"

Editor::Editor(QTabWidget *tabWidget)
    : tabWidget(tabWidget)
{
    while (tabWidget->count()) delete tabWidget->widget(0);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));

    watcher = new QFileSystemWatcher(this);

    connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(OnFileChanged(QString)));
}
int Editor::GetTabContainingFile(const QString &filePath)
{
    for (int i = 0; i < tabWidget->count(); i++)
    {
        if (tabWidget->widget(i)->property("filePath").toString() == filePath)
        {
            return i;
        }
    }

    return -1;
}
void Editor::OnFileChanged(QString filePath)
{
    QMessageBox box;
    box.setText("File " + filePath + " has been moved or deleted, keep in editor?");
    box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    box.setDefaultButton(QMessageBox::Yes);

    int answer = box.exec();

    int tabIndex = GetTabContainingFile(filePath);
    if (tabIndex != -1)
    {
        if (answer == QMessageBox::Yes)
        {
            MarkSaveStatus(tabIndex, false);
        }
        else if (answer == QMessageBox::No)
        {
            CloseTab(tabIndex);
        }
    }
}

Editor::~Editor()
{
    while (tabWidget->count()) delete tabWidget->widget(0);
    disconnect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));

    delete watcher;
}

void Editor::CurrentDocChanged()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    int index = tabWidget->currentIndex();
    if (page->document()->isUndoAvailable()) // document was changed and undo is available => document's contents have indeed changed
    {
        MarkSaveStatus(index, false);
    }
    else // if all changes have been undone, document can be considered as "not modified", thus we remove the *
    {
        MarkSaveStatus(index, true);
    }
}

void Editor::MarkSaveStatus(int tabIndex, bool isSaved)
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->widget(tabIndex));
    if (isSaved)
    {
        tabWidget->setTabText(tabIndex, QFileInfo(page->property("fileName").toString()).fileName());
    }
    else
    {
        tabWidget->setTabText(tabIndex, QFileInfo(page->property("fileName").toString()).fileName() + "*");
    }
}

void Editor::CreateTab(const QString& filePath)
{
    QFileInfo info(filePath);
    QString fileExtension = "";
    QString fileName = "";
    QString fileContents = "";
    if (info.exists())
    {
        fileName = info.fileName();

        if (fileName.size() > 0)
        {
            for (int i = fileName.size(); i >= 1; i--) // i >= 1 to not consider hidden files as extensions themselves, such as '.profiles', for example
            {
                if (fileName[i] == '.') // found the last dot
                {
                    fileExtension = fileName.right(fileName.size() - i - 1);
                    break;
                }
            }
        }
        try
        {
            fileContents = FileManager::ReadFile(filePath);
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot read file", ex.Message());
        }
    }

    EditorPage* page = EditorPageFactory::CreateEditorPage(fileExtension);
    int index = tabWidget->addTab(page, fileName);
    page->setProperty("filePath", info.absoluteFilePath());
    page->setProperty("fileName", info.fileName());
    page->setPlainText(fileContents);
    connect(page->document(), SIGNAL(contentsChanged()), this, SLOT(CurrentDocChanged()));
    tabWidget->setCurrentIndex(index);
    tabWidget->widget(index)->setWindowModified(true);

    watcher->addPath(page->property("filePath").toString());
    page->setFocus();

}

void Editor::CreateBlankTab()
{
    EditorPage* page = EditorPageFactory::CreateEditorPage(""); // create default page
    int index = tabWidget->addTab(page, "");
    page->setProperty("filePath", QString(""));
    page->setProperty("fileName", "(Unsaved)");
    page->setPlainText("");
    connect(page->document(), SIGNAL(contentsChanged()), this, SLOT(CurrentDocChanged()));
    tabWidget->setCurrentIndex(index);
    page->setFocus();
}
void Editor::NewFile()
{
    CreateBlankTab();
}

void Editor::OpenFile(const QString &filePath)
{
    for (int i = 0; i < tabWidget->count(); i++)
    {
        QString path = tabWidget->widget(i)->property("filePath").toString();

        if (path == filePath)
        {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }

    CreateTab(filePath);
}

void Editor::SaveFile()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    if (page == NULL)
        return;

    QString filePath = page->property("filePath").toString();
    if (filePath == "")
        this->SaveFileAs();
    else
    try
    {
        FileManager::WriteFile(filePath, page->toPlainText());
        page->document()->setModified(false);
        tabWidget->setTabText(tabWidget->currentIndex(), QFileInfo(filePath).fileName());
    }
    catch (FileSystemException& ex)
    {
        QMessageBox::warning(NULL, "Cannot write to file", ex.Message());
    }
}

void Editor::SaveFileAs()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    if (page == NULL)
        return;

    QString file = QFileDialog::getSaveFileName(this, "Save File As...", QDir::homePath());
    try
    {
        QFileInfo info(file);
        FileManager::WriteFile(info.absoluteFilePath(), page->toPlainText());
        page->setProperty("filePath", info.absoluteFilePath());
        page->setProperty("fileName", info.fileName());
        page->document()->setModified(false);
        tabWidget->setTabText(tabWidget->currentIndex(), page->property("fileName").toString());

        watcher->addPath(info.absoluteFilePath());
    }
    catch (FileSystemException& ex)
    {
        QMessageBox::warning(NULL, "Cannot write to file", ex.Message());
    }
}

void Editor::CloseTab(int index)
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    if (page == NULL)
        return;

    if (page->document()->isModified())
    {
        QMessageBox box;
        box.setText("File was modified. Save changes before closing?");
        box.setStandardButtons(QMessageBox::Save|QMessageBox::Cancel|QMessageBox::Discard);
        box.setDefaultButton(QMessageBox::Save);

        int answer = box.exec();
        if (answer == QMessageBox::Save)
        {
            SaveFile();
        }
        else if (answer == QMessageBox::Cancel)
        {
            return;
        }
    }
    watcher->removePath(page->property("filePath").toString());
    delete tabWidget->widget(index);
}
