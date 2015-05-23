#include "Editor.h"

Editor::Editor(QTabWidget *tabWidget)
    : tabWidget(tabWidget)
{
    fileDialog = new QFileDialog(this);
    fileDialog->setStyleSheet(Global::Visual.WindowStyle);

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
    delete fileDialog;
}

bool Editor::AskSaveAll()
{
    for (int i = 0; i < tabWidget->count(); i++)
    {
        EditorPage* page = static_cast<EditorPage*>(tabWidget->widget(i));

        if (page->property("saved").toBool() == false || page->property("onDisk").toBool() == false)
        {
            tabWidget->setCurrentIndex(i);

            QString filePath = page->property("filePath").toString();
            QString message = "";
            if (filePath == "")
            {
                message = "File has not been saved yet. Save before exiting?";
            }
            else
            {
                message = "Do you want to save changes to " + filePath + " before exiting?";
            }

            QMessageBox box;
            box.setText(message);
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

QString Editor::GetFileExtension(const QString& filePath)
{
    QString fileExtension = "";
    QString fileName = "";

    fileName = QFileInfo(filePath).fileName();

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

    return fileExtension;
}

EditorPage* Editor::CreateTab(const QString& filePath)
{
    QFileInfo info(filePath);
    QString fileExtension = "";
    QString fileName = "";
    QString fileContents = "";

    if (info.exists())
    {
        fileName = info.fileName();
        fileExtension = this->GetFileExtension(filePath);

        fileContents = FileManager::ReadFile(filePath);
    }

    EditorPage* page = EditorPageFactory::CreateEditorPage(fileExtension, &fileContents);
    if (page == NULL)
        return NULL;

    //page->setPlainText(fileContents);

    page->setProperty("filePath", info.absoluteFilePath());
    page->setProperty("fileName", info.fileName());
    page->setProperty("saved", true);
    page->setProperty("onDisk", true);

    //connect(page->document(), SIGNAL(contentsChanged()), this, SLOT(CurrentDocChanged()));

    return page;
}

void Editor::CreateBlankTab()
{
    QString fileExtension = "";
    QString fileName = "(Unsaved)";
    QString fileContents = "";

    EditorPage* page = EditorPageFactory::CreateEditorPage(fileExtension, &fileContents); // create default page with no text
    if (page == NULL)
        return;

    page->setProperty("filePath", QString(""));
    page->setProperty("fileName", fileName);
    page->setProperty("saved", false);
    page->setProperty("onDisk", false);

    page->setPlainText(fileContents);

    //connect(page->document(), SIGNAL(contentsChanged()), this, SLOT(CurrentDocChanged()));

    int index = tabWidget->addTab(page, "");

    tabWidget->setTabText(index, page->property("fileName").toString() + "*");
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
            tabWidget->widget(i)->setFocus();
            return;
        }
    }

    EditorPage* page = CreateTab(filePath);

    int index = tabWidget->addTab(page, QFileInfo(filePath).fileName());
    tabWidget->setTabText(index, page->property("fileName").toString());
    tabWidget->setCurrentIndex(index);

    connect(page, SIGNAL(textChanged()), this, SLOT(CurrentDocChanged()));
    page->setFocus();

    watcher->addPath(page->property("filePath").toString());
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

    QString file = fileDialog->getSaveFileName(this, "Save File As...", QDir::homePath(), "", 0, QFileDialog::DontUseNativeDialog);

    if (file != "")
    {
        try
        {
            QFileInfo info(file);
            watcher->removePath(info.absoluteFilePath());
            FileManager::WriteFile(info.absoluteFilePath(), page->toPlainText());
            watcher->addPath(info.absoluteFilePath());

            QString ext1 = this->GetFileExtension(page->property("filePath").toString());
            QString ext2 = this->GetFileExtension(file);
            if (ext1 != ext2)
            {
                EditorPage* newPage = this->CreateTab(file);

                tabWidget->insertTab(tabWidget->indexOf(page), newPage, newPage->property("fileName").toString());

                delete page;

                page = newPage;
            }
            else
            {
                page->setProperty("filePath", info.absoluteFilePath());
                page->setProperty("fileName", info.fileName());
                page->setProperty("saved", true);
                page->setProperty("onDisk", true);

                page->document()->setModified(false);

                tabWidget->setTabText(tabWidget->currentIndex(), page->property("fileName").toString());

                watcher->addPath(info.absoluteFilePath());
            }

            connect(page, SIGNAL(textChanged()), this, SLOT(CurrentDocChanged()));
            page->setFocus();

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
        watcher->removePath(page->property("filePath").toString());
        disconnect(page, SIGNAL(textChanged()), this, SLOT(CurrentDocChanged()));
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
                if (SaveFile() == false)
                {
                    return;
                }
            }
            else if (answer == QMessageBox::Cancel)
            {
                return;
            }

            watcher->removePath(page->property("filePath").toString());
            disconnect(page, SIGNAL(textChanged()), this, SLOT(CurrentDocChanged()));
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
            QString file = fileDialog->getSaveFileName(this, "Save File As...", QDir::homePath());
            if (file != "")
            {
                try
                {
                    QFileInfo info(file);
                    FileManager::WriteFile(info.absoluteFilePath(), page->toPlainText());

                    disconnect(page, SIGNAL(textChanged()), this, SLOT(CurrentDocChanged()));
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
            disconnect(page, SIGNAL(textChanged()), this, SLOT(CurrentDocChanged()));
            delete page;
        }
    }
}
