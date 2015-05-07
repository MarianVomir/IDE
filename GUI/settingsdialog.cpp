#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    /*
    General Tab
    - dunno what to put here, fonts maybe if i have time
    */
    try
    {
        Load();
    }
    catch (FileSystemException& e)
    {
        QMessageBox::warning(NULL, "Cannot load settings", e.Message());
    }

    // get data from qsettings
    /*
    Default data
    - use QSettings or something to store it
    */
}

void SettingsDialog::Load()
{
    if (Global::ActiveProject != NULL)
    {
        Project* proj = Global::ActiveProject;
        ui->txt_Compiler->setText(proj->Compiler());
        ui->txt_Make->setText(proj->MakeUtility());
        ui->txt_Debugger->setText(proj->Debugger());
        ui->txt_LinkerFlags->setText(proj->LinkerFlags());
        ui->txt_CompilerFlags->setText(proj->CompilerFlags());
    }

    QString fileContents;
    QFile settingsFile("settings");
    bool failed = false;
    if (!settingsFile.exists())
    {
        failed = true;
    }
    else
    {
        fileContents = FileManager::ReadFile("settings");
    }

    QString compiler, linker, make, compilerFlags, linkerFlags;

    if (failed)
    {
        compiler = "";
        linker = "";
        make = "";
        compilerFlags = "";
        linkerFlags = "";
    }
    else
    {
        QJsonDocument doc(QJsonDocument::fromJson(fileContents.toLocal8Bit()));
        QJsonObject jsonObject = doc.object();

        compiler = jsonObject["compiler_default"].toString();
        linker = jsonObject["debugger_default"].toString();
        make = jsonObject["make_default"].toString();
        compilerFlags = jsonObject["compiler_default"].toString();
        linkerFlags = jsonObject["linkerFlags_default"].toString();
    }

    ui->txt_Compiler_Default->setText(compiler);
    ui->txt_Debugger_Default->setText(linker);
    ui->txt_Make_Default->setText(make);
    ui->txt_CompilerFlags_Default->setText(compilerFlags);
    ui->txt_LinkerFlags_Default->setText(linkerFlags);
}

void SettingsDialog::Save()
{
    Project* proj = Global::ActiveProject;
    QJsonDocument jsonDoc;
    if (proj != NULL)
    {
        QJsonObject projObject;

        projObject["compiler"] = ui->txt_Compiler->text();
        projObject["debugger"] = ui->txt_Debugger->text();
        projObject["make"] = ui->txt_Make->text();
        projObject["compilerFlags"] = ui->txt_CompilerFlags->text();
        projObject["linkerFlags"] = ui->txt_LinkerFlags->text();

        jsonDoc.setObject(projObject);

        proj->SetCompiler(ui->txt_Compiler->text());
        proj->SetDebugger(ui->txt_Debugger->text());
        proj->SetMakeUtility(ui->txt_Make->text());
        proj->SetCompilerFlags(ui->txt_CompilerFlags->text());
        proj->SetLinkerFlags(ui->txt_LinkerFlags->text());

        FileManager::WriteFile(Global::ActiveProject->Root() + Global::ActiveProject->Name() + ".proj", jsonDoc.toJson());
    }

    QJsonObject defaultObject;

    defaultObject["compiler_default"] = ui->txt_Compiler_Default->text();
    defaultObject["debugger_default"] = ui->txt_Debugger_Default->text();
    defaultObject["make_default"] = ui->txt_Make_Default->text();
    defaultObject["compilerFlags_default"] = ui->txt_CompilerFlags_Default->text();
    defaultObject["linkerFlags_default"] = ui->txt_LinkerFlags_Default->text();

    jsonDoc.setObject(defaultObject);

    FileManager::WriteFile("settings", jsonDoc.toJson());
    Global::defaultProjectValues = defaultObject;
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_btn_OK_clicked()
{
    try
    {
        this->Save();
    }
    catch (FileSystemException& e)
    {
        QMessageBox::warning(NULL, "Cannot save settings", e.Message());
    }
    this->accept();
}

void SettingsDialog::on_btn_Cancel_clicked()
{
    this->reject();
}
