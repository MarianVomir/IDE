#ifndef OPENPROJECTDIALOG_H
#define OPENPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QCompleter>
#include <QDirModel>
#include <QMessageBox>

#include "mainwindow.h"
#include "ProjectManagement/ProjectFileConverter.h"

namespace Ui {
class OpenProjectDialog;
}

class OpenProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenProjectDialog(QWidget *parent,  ProjectFileConverter* converter);
    ~OpenProjectDialog();

    int exec();

private slots:
    void OnBrowseMakeClicked();
    void OnBrowseDebuggerClicked();
    void OnBrowseCompilerClicked();
    void OnOkButtonClicked();
    void on_btn_Cancel_clicked();

private:
    Ui::OpenProjectDialog *ui;
    QCompleter* completer;
    QDirModel* model;
    ProjectFileConverter* converter;
    Project* project;
    QString projFilePath;
};

#endif // OPENPROJECTDIALOG_H
