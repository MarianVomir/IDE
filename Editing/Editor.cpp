#include "Editor.h"

Editor::Editor(QTabWidget *tabWidget)
    : tabWidget(tabWidget)
{
    while (tabWidget->count()) delete tabWidget->widget(0);
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));

    watcher = new QFileSystemWatcher(this);

    connect(watcher, SIGNAL(fileChanged(QString)), this, SLOT(OnFileChanged(QString)));
}
Editor::~Editor()
{
    while (tabWidget->count())
    {
        tabWidget->setCurrentIndex(0);
        delete tabWidget->widget(0);
    }
    disconnect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(CloseTab(int)));

    delete watcher;
}

bool Editor::AskSaveAll()
{
    for (int i = 0; i < tabWidget->count(); i++)
    {
        EditorPage* page = static_cast<EditorPage*>(tabWidget->widget(i));

        if (page->property("saved").toBool() == false || page->property("onDisk").toBool() == false)
        {
            tabWidget->setCurrentIndex(i);

            QMessageBox box;
            box.setText("Do you want to save " + page->property("filePath").toString() + " before exiting?");
            box.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Yes);

            int answer = box.exec();

            if (answer == QMessageBox::Yes)
            {
                if (SaveFile() == false)
                    return false;
            }
            else if (answer == QMessageBox::Cancel)
            {
                return false;
            }
        }
    }

    return true;
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
            EditorPage* page = static_cast<EditorPage*>(tabWidget->widget(tabIndex));

            page->setProperty("saved", false);
            page->setProperty("onDisk", false);
            tabWidget->setTabText(tabIndex, page->property("fileName").toString() + "*");
        }
        else if (answer == QMessageBox::No)
        {
            CloseTab(tabIndex);
        }
    }
}

void Editor::CurrentDocChanged()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    int index = tabWidget->currentIndex();

    bool onDisk = page->property("onDisk").toBool();
    bool isUndoAvailable = page->document()->isUndoAvailable();

    if (!onDisk)
    {
        // put asterisk
        tabWidget->setTabText(index, page->property("fileName").toString() + "*");
        page->setProperty("saved", false);
    }
    else
    {
        if (!isUndoAvailable)
        {
            // remove asterisk
            tabWidget->setTabText(index, page->property("fileName").toString());
            page->setProperty("saved", true);
        }
        else
        {
            // put asterisk
            tabWidget->setTabText(index, page->property("fileName").toString() + "*");
            page->setProperty("saved", false);
        }
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
                    fileExtension = fileName.right(fileName.size() - i - 1); // get file extension, given the last dot's position
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
    page->setProperty("saved", true);
    page->setProperty("onDisk", true);

    page->setPlainText(fileContents);
    connect(page->document(), SIGNAL(contentsChanged()), this, SLOT(CurrentDocChanged()));

    tabWidget->setCurrentIndex(index);

    watcher->addPath(page->property("filePath").toString());

    page->setFocus();

}

void Editor::CreateBlankTab()
{
    EditorPage* page = EditorPageFactory::CreateEditorPage(""); // create default page
    if (page == NULL)
        return;

    int index = tabWidget->addTab(page, "");

    page->setProperty("filePath", QString(""));
    page->setProperty("fileName", "(Unsaved)");
    page->setProperty("saved", false);
    page->setProperty("onDisk", false);

    page->setPlainText("");

    tabWidget->setTabText(index, page->property("fileName").toString() + "*");

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

bool Editor::SaveFile()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    if (page == NULL)
        return false;

    QString filePath = page->property("filePath").toString();
    bool saved = page->property("saved").toBool();
    bool onDisk = page->property("onDisk").toBool();

    if (filePath != "" && saved && onDisk)
    {
        return true;
    }
    else if (filePath != "" && !saved)
    {
        try
        {
            watcher->removePath(filePath);
            FileManager::WriteFile(filePath, page->toPlainText());
            watcher->addPath(filePath);

            page->setProperty("saved", true);
            page->setProperty("onDisk", true);

            page->document()->setModified(false);
            tabWidget->setTabText(tabWidget->currentIndex(), page->property("fileName").toString());

            return true;
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot write to file", ex.Message());
            return false;
        }
    }
    else if (filePath == "")
    {
        return this->SaveFileAs();
    }

    return false;
}

bool Editor::SaveFileAs()
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->currentWidget());
    if (page == NULL)
        return false;

    QString file = QFileDialog::getSaveFileName(this, "Save File As...", QDir::homePath());
    if (file != "")
    {
        try
        {
            QFileInfo info(file);
            watcher->removePath(info.absoluteFilePath());
            FileManager::WriteFile(info.absoluteFilePath(), page->toPlainText());
            watcher->addPath(info.absoluteFilePath());
            page->setProperty("filePath", info.absoluteFilePath());
            page->setProperty("fileName", info.fileName());
            page->setProperty("saved", true);
            page->setProperty("onDisk", true);

            page->document()->setModified(false);
            tabWidget->setTabText(tabWidget->currentIndex(), page->property("fileName").toString());

            watcher->addPath(info.absoluteFilePath());

            return true;
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot write to file", ex.Message());

            return false;
        }
    }
    else
    {
        return false;
    }
}

void Editor::CloseTab(int index)
{
    EditorPage* page = static_cast<EditorPage*>(tabWidget->widget(index));
    if (page == NULL)
        return;

    QString filePath = page->property("filePath").toString();
    bool saved = page->property("saved").toBool();
    bool onDisk = page->property("onDisk").toBool();

    if (filePath != "" && saved && onDisk)
    {
        delete page;
    }
    else if (filePath != "" && !saved)
    {
        try
        {
            QMessageBox box;
            box.setText("Save changes before closing?");
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
             watcher->removePath(page->property("filePath").toString());
            delete page;
        }
        catch (FileSystemException& ex)
        {
            QMessageBox::warning(NULL, "Cannot write to file", ex.Message());
        }
    }
    else if (filePath == "")
    {
        QMessageBox box;
        box.setText("Save changes before closing?");
        box.setStandardButtons(QMessageBox::Save|QMessageBox::Cancel|QMessageBox::Discard);
        box.setDefaultButton(QMessageBox::Save);

        int answer = box.exec();
        if (answer == QMessageBox::Save)
        {
            QString file = QFileDialog::getSaveFileName(this, "Save File As...", QDir::homePath());
            if (file != "")
            {
                try
                {
                    QFileInfo info(file);
                    FileManager::WriteFile(info.absoluteFilePath(), page->toPlainText());

                    delete page;
                }
                catch (FileSystemException& ex)
                {
                    QMessageBox::warning(NULL, "Cannot write to file", ex.Message());
                }
            }
            else
            {
                return;
            }
        }
        else if (answer == QMessageBox::Cancel)
        {
            return;
        }
        else
        {
            delete page;
        }
    }
}
