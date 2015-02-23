#include "openprojectdialog.h"
#include "ui_openprojectdialog.h"

OpenProjectDialog::OpenProjectDialog(QWidget *parent, ProjectFileConverter* converter) :
    QDialog(parent),
    ui(new Ui::OpenProjectDialog)
{
    ui->setupUi(this);
    this->converter = converter;

    project = NULL;

    connect(ui->btn_BrowseDebugger, SIGNAL(clicked()), this, SLOT(OnBrowseDebuggerClicked()));
    connect(ui->btn_BrowseCompiler, SIGNAL(clicked()), this, SLOT(OnBrowseCompilerClicked()));
    connect(ui->btn_BrowseMake, SIGNAL(clicked()), this, SLOT(OnBrowseMakeClicked()));

    connect(ui->btn_OK, SIGNAL(clicked()), this, SLOT(OnOkButtonClicked()));

    completer = new QCompleter(this);
    model = new QDirModel(completer);

    completer->setModel(model);

    ui->txt_Compiler->setCompleter(completer);
    ui->txt_Make->setCompleter(completer);
    ui->txt_Debugger->setCompleter(completer);
}
int OpenProjectDialog::exec()
{
    QFileDialog filePicker;
    filePicker.setFileMode(QFileDialog::AnyFile);
    filePicker.setNameFilter(QString("Project files (*.proj)"));
    filePicker.setDirectory(QDir::homePath());

    bool fileHasBeenSelected = false;
    while (1)
    {
        projFilePath = filePicker.getOpenFileName();
        if (projFilePath.size() > 0)
        {
            QFileInfo info(projFilePath);
            if (!info.fileName().endsWith(".proj")) // file name does not end with .proj
            {
                QMessageBox::warning(
                            NULL,
                            "Not a valid file",
                            "Project file must have '.proj' extension"
                        );
            }
            else
            {
                fileHasBeenSelected = true;
                break;
            }
        }
        else
        {
            break;
        }
    }

    project = converter->FileToProject(projFilePath);

    ui->txt_ProjectName->setText(project->Name());
    ui->txt_ProjectRoot->setText(project->Root());
    ui->txt_Compiler->setText(project->Compiler());
    ui->txt_Debugger->setText(project->Debugger());
    ui->txt_Make->setText(project->MakeUtility());
    ui->txt_CompilerFlags->setText(project->CompilerFlags());
    ui->txt_LinkerFlags->setText(project->LinkerFlags());

    if (!fileHasBeenSelected) // no .proj file was selected, therefore it makes no sense to display the dialog
    {
        return QDialog::Rejected;
    }

    return QDialog::exec(); // call the standard exec to display the dialog and return Accepted or Rejected based on what QDialog::exec() returns
}

void OpenProjectDialog::OnBrowseCompilerClicked()
{
    QString compiler = QFileDialog::getOpenFileName(this, "Browse Compiler Executable", ui->txt_Compiler->text().trimmed());

    if (compiler.size() != 0)
    {
        ui->txt_Compiler->setText(compiler);
    }
}

void OpenProjectDialog::OnBrowseDebuggerClicked()
{
    QString debugger = QFileDialog::getOpenFileName(this, "Browse Debugger Executable", ui->txt_Debugger->text().trimmed());
    if (debugger.size() != 0)
    {
        ui->txt_Debugger->setText(debugger);
    }
}

void OpenProjectDialog::OnBrowseMakeClicked()
{
    QString make = QFileDialog::getOpenFileName(this, "Browse Make Executable", ui->txt_Make->text().trimmed());

    if (make.size() != 0)
    {
        ui->txt_Make->setText(make);
    }
}

void OpenProjectDialog::OnOkButtonClicked()
{
    QString projectName = ui->txt_ProjectName->text().trimmed();
    QString projectRoot = ui->txt_ProjectRoot->text().trimmed();
    QString compilerFlags = ui->txt_CompilerFlags->text().trimmed();
    QString linkerFlags = ui->txt_LinkerFlags->text().trimmed();
    QString debugger = ui->txt_Debugger->text().trimmed();
    QString compiler = ui->txt_Compiler->text().trimmed();
    QString make = ui->txt_Make->text().trimmed();

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

    delete project;
    project = new Project(projectName, projectRoot, compiler, debugger, compilerFlags, linkerFlags, make);
    MainWindow* mw = static_cast<MainWindow*>(this->parent());
    try
    {
        mw->SetOpenProject(project);
        this->accept();
    }
    catch (Exception& e)
    {
        QMessageBox::warning(NULL, "Cannot create project", e.Message());
    }
}

OpenProjectDialog::~OpenProjectDialog()
{
    delete ui;
}

void OpenProjectDialog::on_btn_Cancel_clicked()
{
    this->reject();
}
