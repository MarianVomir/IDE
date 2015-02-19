﻿#ifndef EDITOR_H
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

private slots:
    void CurrentDocChanged();
    void OnFileChanged(QString filePath);

private:
    void CreateTab(const QString& filePath);
    void CreateBlankTab();
    int GetTabContainingFile(const QString& filePath);
    void MarkSaveStatus(int tabIndex, bool isSaved);

public slots:
    void NewFile();
    void OpenFile(const QString& filePath);
    void SaveFile();
    void SaveFileAs();
    void CloseTab(int index);

};

#endif // EDITOR_H