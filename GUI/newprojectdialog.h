#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QCompleter>
#include <QDirModel>

#include "mainwindow.h"
#include "Project.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget* parent);
    ~NewProjectDialog();

private:
    Ui::NewProjectDialog *ui;
    QCompleter* completer;
    QDirModel* model;

    QFileDialog* fileDialog;

public slots:
    void OnPathChanged(QString s);
    void OnBrowseContainingFolderClicked();
    void OnBrowseCompilerClicked();
    void OnBrowseDebuggerClicked();
    void OnBrowseMakeClicked();
    void OnOkButtonClicked();
private slots:
    void on_btn_Cancel_clicked();
};

#endif // NEWPROJECTDIALOG_H
