#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QFileDialog>
#include <QCompleter>
#include <QDirModel>

#include "FileManager.h"
#include "Global.h"
#include "ProjectManagement/ProjectExplorer.h"
#include "Settings/SettingsManager.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(ProjectExplorer* projectExplorer, QWidget *parent = 0);
    ~SettingsDialog();

private slots:
    void on_btn_OK_clicked();
    void on_btn_Cancel_clicked();

    void OnBrowseCompilerClicked();
    void OnBrowseDebuggerClicked();
    void OnBrowseMakeClicked();
    void OnBrowseCompilerDefaultClicked();
    void OnBrowseDebuggerDefaultClicked();
    void OnBrowseMakeDefaultClicked();

private:
    Ui::SettingsDialog *ui;
    ProjectExplorer* projectExplorer;
    Project* proj;
    QCompleter* completer;
    QDirModel* model;

    QFileDialog* fileDialog;

    void Load();
    void Save();
};

#endif // SETTINGSDIALOG_H
