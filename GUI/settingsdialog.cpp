#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    /*
    General Tab
    - dunno what to put here, fonts maybe if i have time
    */

    /*
    Project Tab
    - get data from current project
    - set data in boxes
    */

    /*
    Default data
    - use QSettings or something to store it
    */
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
