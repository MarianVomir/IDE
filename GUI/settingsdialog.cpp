#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(ProjectExplorer* projectExplorer, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog), projectExplorer(projectExplorer)
{
    ui->setupUi(this);

    connect(ui->btn_BrowseDebugger, SIGNAL(clicked()), this, SLOT(OnBrowseDebuggerClicked()));
    connect(ui->btn_BrowseCompiler, SIGNAL(clicked()), this, SLOT(OnBrowseCompilerClicked()));
    connect(ui->btn_BrowseMake, SIGNAL(clicked()), this, SLOT(OnBrowseMakeClicked()));
    connect(ui->btn_BrowseDebugger_Default, SIGNAL(clicked()), this, SLOT(OnBrowseDebuggerDefaultClicked()));
    connect(ui->btn_BrowseCompiler_Default, SIGNAL(clicked()), this, SLOT(OnBrowseCompilerDefaultClicked()));
    connect(ui->btn_BrowseMake_Default, SIGNAL(clicked()), this, SLOT(OnBrowseMakeDefaultClicked()));

    completer = new QCompleter(this);
    model = new QDirModel(completer);

    completer->setModel(model);
    ui->txt_Compiler->setCompleter(completer);
    ui->txt_Make->setCompleter(completer);
    ui->txt_Debugger->setCompleter(completer);
    ui->txt_Compiler_Default->setCompleter(completer);
    ui->txt_Make_Default->setCompleter(completer);
    ui->txt_Debugger_Default->setCompleter(completer);

    try
    {
        Load();
    }
    catch (FileSystemException& e)
    {
        QMessageBox::warning(NULL, "Cannot load settings", e.Message());
    }
}

void SettingsDialog::Load()
{
    proj = this->projectExplorer->GetProject();

    if (proj != NULL)
    {
        ui->txt_Compiler->setText(proj->Compiler());
        ui->txt_Make->setText(proj->MakeUtility());
        ui->txt_Debugger->setText(proj->Debugger());
        ui->txt_LinkerFlags->setText(proj->LinkerFlags());
        ui->txt_CompilerFlags->setText(proj->CompilerFlags());
    }
    else
    {
        ui->txt_Compiler->setEnabled(false);
        ui->txt_Make->setEnabled(false);
        ui->txt_Debugger->setEnabled(false);
        ui->txt_LinkerFlags->setEnabled(false);
        ui->txt_CompilerFlags->setEnabled(false);
        ui->btn_BrowseCompiler->setEnabled(false);
        ui->btn_BrowseDebugger->setEnabled(false);
        ui->btn_BrowseMake->setEnabled(false);
    }

    ui->txt_Compiler_Default->setText(Global::defaultProjectValues["compiler_default"].toString());
    ui->txt_Debugger_Default->setText(Global::defaultProjectValues["debugger_default"].toString());
    ui->txt_Make_Default->setText(Global::defaultProjectValues["make_default"].toString());
    ui->txt_CompilerFlags_Default->setText(Global::defaultProjectValues["compilerFlags_default"].toString());
    ui->txt_LinkerFlags_Default->setText(Global::defaultProjectValues["linkerFlags_default"].toString());
}

void SettingsDialog::Save()
{
    Project* proj = projectExplorer->GetProject();
    QJsonDocument jsonDoc;
    if (proj != NULL)
    {
        QJsonObject projObject;

        projObject["name"] = proj->Name();
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

        FileManager::WriteFile(proj->Root() + proj->Name() + ".proj", jsonDoc.toJson());
    }

    QJsonObject defaultObject;

    defaultObject["compiler_default"] = ui->txt_Compiler_Default->text();
    defaultObject["debugger_default"] = ui->txt_Debugger_Default->text();
    defaultObject["make_default"] = ui->txt_Make_Default->text();
    defaultObject["compilerFlags_default"] = ui->txt_CompilerFlags_Default->text();
    defaultObject["linkerFlags_default"] = ui->txt_LinkerFlags_Default->text();

    jsonDoc.setObject(defaultObject);

    Global::defaultProjectValues = defaultObject;
    SettingsManager::SaveProjectDefaults();
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

void SettingsDialog::OnBrowseCompilerClicked()
{
    QString compiler = QFileDialog::getOpenFileName(this, "Browse Compiler Executable", ui->txt_Compiler->text().trimmed());

    if (compiler.size() != 0)
    {
        ui->txt_Compiler->setText(compiler);
    }
}
void SettingsDialog::OnBrowseDebuggerClicked()
{
    QString debugger = QFileDialog::getOpenFileName(this, "Browse Debugger Executable", ui->txt_Debugger->text().trimmed());
    if (debugger.size() != 0)
    {
        ui->txt_Debugger->setText(debugger);
    }
}
void SettingsDialog::OnBrowseMakeClicked()
{
    QString make = QFileDialog::getOpenFileName(this, "Browse Make Executable", ui->txt_Make->text().trimmed());

    if (make.size() != 0)
    {
        ui->txt_Make->setText(make);
    }
}

void SettingsDialog::OnBrowseCompilerDefaultClicked()
{
    QString compiler = QFileDialog::getOpenFileName(this, "Browse Compiler Executable", ui->txt_Compiler->text().trimmed());

    if (compiler.size() != 0)
    {
        ui->txt_Compiler_Default->setText(compiler);
    }
}
void SettingsDialog::OnBrowseDebuggerDefaultClicked()
{
    QString debugger = QFileDialog::getOpenFileName(this, "Browse Debugger Executable", ui->txt_Debugger->text().trimmed());
    if (debugger.size() != 0)
    {
        ui->txt_Debugger_Default->setText(debugger);
    }
}
void SettingsDialog::OnBrowseMakeDefaultClicked()
{
    QString make = QFileDialog::getOpenFileName(this, "Browse Make Executable", ui->txt_Make->text().trimmed());

    if (make.size() != 0)
    {
        ui->txt_Make_Default->setText(make);
    }
}
