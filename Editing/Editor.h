#ifndef EDITOR_H
#define EDITOR_H

#include <QTabWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QPaintEvent>
#include <QFileSystemWatcher>

#include "EditorPageFactory.h"
#include "FileManager.h"

class Editor : QWidget
{
    Q_OBJECT

private:
    QTabWidget* tabWidget;
    QFileSystemWatcher* watcher;

    QString GetFileExtension(const QString &filePath);

public:
    Editor(QTabWidget* tabWidget);
    ~Editor();

    bool AskSaveAll();

private slots:
    void CurrentDocChanged();
    void OnFileChanged(QString filePath);

private:
    EditorPage* CreateTab(const QString& filePath);
    void CreateBlankTab();
    int GetTabContainingFile(const QString& filePath);

public slots:
    void NewFile();
    /**
    Creates a new tab with default properties and sets focus on it
    **/

    void OpenFile(const QString& filePath);
    /**
    Opens a file in a new tab. If the file is already opened, sets focus on the tab containing that file.
    **/

    bool SaveFile();
    /**
    Saves the current tab's file if it is already on disk. If not on disk, calls SaveFileAs
    **/

    bool SaveFileAs();
    /**
    Prompts to save file with a new name.
    If the extension has changed, the page is replaced with the corresponding type
    or the default page (if the extension is not supported).
    **/

    void CloseTab(int index);
    /**
    Closes a tab. Prompts for save if file is changed.
    **/
};

#endif // EDITOR_H
