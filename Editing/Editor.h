#ifndef EDITOR_H
#define EDITOR_H

#include "EditorPageFactory.h"
#include <QTabWidget>
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QPaintEvent>

#include "FileManager.h"
class Editor : QWidget
{
    Q_OBJECT

private:
    QTabWidget* tabWidget;
public:
    Editor(QTabWidget* tabWidget);
    ~Editor();

private slots:
    void CurrentDocChanged();

private:
    void CreateTab(const QString& filePath);
    void CreateBlankTab();

public slots:
    void NewFile();
    void OpenFile(const QString& filePath);
    void SaveFile();
    void SaveFileAs();
    void CloseTab(int index);

};

#endif // EDITOR_H
