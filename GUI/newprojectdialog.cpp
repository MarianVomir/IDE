#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

NewProjectDialog::NewProjectDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

    connect(ui->txt_ProjectName, SIGNAL(textChanged(QString)), this, SLOT(OnPathChanged(QString)));
    connect(ui->txt_ContainingFolder, SIGNAL(textChanged(QString)), this, SLOT(OnPathChanged(QString)));

    connect(ui->btn_BrowseContainingFolder, SIGNAL(clicked()), this, SLOT(OnBrowseContainingFolderClicked()));
    connect(ui->btn_BrowseDebugger, SIGNAL(clicked()), this, SLOT(OnBrowseDebuggerClicked()));
    connect(ui->btn_BrowseCompiler, SIGNAL(clicked()), this, SLOT(OnBrowseCompilerClicked()));
    connect(ui->btn_BrowseMake, SIGNAL(clicked()), this, SLOT(OnBrowseMakeClicked()));

    connect(ui->btn_OK, SIGNAL(clicked()), this, SLOT(OnOkButtonClicked()));

    completer = new QCompleter(this);
    model = new QDirModel(completer);

    completer->setModel(model);

    ui->txt_ContainingFolder->setCompleter(completer);
    ui->txt_Compiler->setCompleter(completer);
    ui->txt_Make->setCompleter(completer);
    ui->txt_Debugger->setCompleter(completer);
}
NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

void NewProjectDialog::OnPathChanged(QString s)
{
    Q_UNUSED(s);

    QString projName = ui->txt_ProjectName->text();//.trimmed();
    QString containingFolder = ui->txt_ContainingFolder->text();//.trimmed();
    if (projName.trimmed().size() == 0 || containingFolder.trimmed().size() == 0)
    {
        ui->lbl_ProjectRoot->setText(QString("Containing folder and project name must not be empty!"));
        ui->btn_OK->setEnabled(false);
    }
    else if (projName.trimmed().contains(" ") || projName.trimmed().contains("/") || projName.trimmed().contains("\\"))
    {
        ui->lbl_ProjectRoot->setText(QString("Project name must not contain spaces or slashes!"));
        ui->btn_OK->setEnabled(false);
    }
    else if (!containingFolder.startsWith("/"))
    {
        ui->lbl_ProjectRoot->setText(QString("Containing folder path must be absolute (Must start with '/')"));
        ui->btn_OK->setEnabled(false);
    }
    else
    {
        ui->txt_ContainingFolder->setText(containingFolder);
        containingFolder.replace('\\', '/');
        if (containingFolder[containingFolder.size() - 1] != '/' && containingFolder[containingFolder.size() -1] != '\\')
        {
            containingFolder += '/';
        }

        ui->lbl_ProjectRoot->setText(containingFolder + projName + "/");
        ui->btn_OK->setEnabled(true);
    }
}
void NewProjectDialog::OnBrowseContainingFolderClicked()
{
    QString folderName = QFileDialog::getExistingDirectory(this, "Browse File", ui->txt_ContainingFolder->text().trimmed());

    if (folderName.size() != 0)
    {
        ui->txt_ContainingFolder->setText(folderName + "/");
    }
}
void NewProjectDialog::OnBrowseCompilerClicked()
{
    QString compiler = QFileDialog::getOpenFileName(this, "Browse Compiler Executable", ui->txt_Compiler->text().trimmed());

    if (compiler.size() != 0)
    {
        ui->txt_Compiler->setText(compiler);
    }
}
void NewProjectDialog::OnBrowseDebuggerClicked()
{
    QString debugger = QFileDialog::getOpenFileName(this, "Browse Debugger Executable", ui->txt_Debugger->text().trimmed());
    if (debugger.size() != 0)
    {
        ui->txt_Debugger->setText(debugger);
    }
}
void NewProjectDialog::OnBrowseMakeClicked()
{
    QString make = QFileDialog::getOpenFileName(this, "Browse Make Executable", ui->txt_Make->text().trimmed());

    if (make.size() != 0)
    {
        ui->txt_Make->setText(make);
    }
}
void NewProjectDialog::OnOkButtonClicked()
{
    QString containingFolder = ui->txt_ContainingFolder->text().trimmed();
    QString projectName = ui->txt_ProjectName->text().trimmed();
    QString debugger = ui->txt_Debugger->text().trimmed();
    QString compiler = ui->txt_Compiler->text().trimmed();
    QString make = ui->txt_Make->text().trimmed();

    containingFolder.replace('\\', '/');

    if (containingFolder[containingFolder.size() - 1] != '/' && containingFolder[containingFolder.size() -1] != '\\')
    {
        containingFolder += '/';
    }

    if (compiler.size() == 0 || make.size() == 0)
    {
        QMessageBox::StandardButton answer = QMessageBox::question(
                    NULL,
                    "Compiler or make not present",
                    "If you do not specify a compiler and a make utility, you will not be able to build the project. Create project anyway?",
                    QMessageBox::Yes | QMessageBox::No
                );
        if (answer == QMessageBox::No)
        {
            return;
        }
    }

    Project* project = new Project(
                projectName,
                ui->lbl_ProjectRoot->text().trimmed(),
                compiler,
                debugger,
                ui->txt_CompilerFlags->text().trimmed(),
                ui->txt_LinkerFlags->text().trimmed(),
                ui->txt_Make->text().trimmed()
                );

    MainWindow* mw = static_cast<MainWindow*>(this->parent());
    try
    {
        mw->SetNewProject(project);
        this->accept();
    }
    catch (Exception& e)
    {
        QMessageBox::warning(NULL, "Cannot create project", e.Message());
    }
}

void NewProjectDialog::on_btn_Cancel_clicked()
{
    this->reject();
}
