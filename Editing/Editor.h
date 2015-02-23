#ifndef EDITOR_H
#define EDITOR_H

#include "EditorPageFactory.h"
#include <QTabWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QPaintEvent>
#include <QFileSystemWatcher>

#include "FileManager.h"
class Editor : QWidget
{
    Q_OBJECT

private:
    QTabWidget* tabWidget;
    QFileSystemWatcher* watcher;

public:
    Editor(QTabWidget* tabWidget);
    ~Editor();

    bool AskSaveAll();

private slots:
    void CurrentDocChanged();
    void OnFileChanged(QString filePath);

private:
    void CreateTab(const QString& filePath);
    void CreateBlankTab();
    int GetTabContainingFile(const QString& filePath);

public slots:
    void NewFile();
    void OpenFile(const QString& filePath);
    bool SaveFile();
    bool SaveFileAs();
    void CloseTab(int index);

};

#endif // EDITOR_H
