#include "Editor.h"

Editor::Editor(QTabWidget *tabWidget)
    : tabWidget(tabWidget)
{
    while (tabWidget->count()) delete tabWidget->widget(0);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));
}

Editor::~Editor()
{
    while (tabWidget->count()) delete tabWidget->widget(0);
    disconnect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));
}

void Editor::CurrentDocChanged()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    int index = tabWidget->currentIndex();
    if (page->document()->isUndoAvailable()) // document was changed and undo is available => document's contents have indeed changed
    {
        tabWidget->setTabText(index, QFileInfo(page->property("filePath").toString()).fileName() + "*");
    }
    else // if all changes have been undone, document can be considered as "not modified", thus we remove the *
    {
        tabWidget->setTabText(index, QFileInfo(page->property("filePath").toString()).fileName());
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
    page->setPlainText(fileContents);
    connect(page->document(), SIGNAL(contentsChanged()), this, SLOT(CurrentDocChanged()));
    tabWidget->setCurrentIndex(index);
    tabWidget->widget(index)->setWindowModified(true);
    page->setFocus();
}
void Editor::CreateBlankTab()
{
    EditorPage* page = EditorPageFactory::CreateEditorPage(""); // create default page
    int index = tabWidget->addTab(page, "");
    page->setProperty("filePath", QString(""));
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
        FileManager::WriteFile(file, page->toPlainText());
        page->setProperty("filePath", file);
        page->document()->setModified(false);
        tabWidget->setTabText(tabWidget->currentIndex(), QFileInfo(file).fileName());
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
    delete tabWidget->widget(index);
}
