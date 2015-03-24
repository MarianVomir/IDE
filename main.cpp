#include "GUI/mainwindow.h"
#include "Mediator.h"
#include "ProjectManagement/JSONConverter.h"
#include "Editing/EditorPageFactory.h"

#include "GUI/settingsdialog.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>

#include "Testing/Tester_FileManager.h"

int main(int argc, char *argv[])
{
    // Add supported file extensions
    EditorPageFactory::Creators().insert("", new DefaultEditorPageCreator()); // default (plain text)
    EditorPageFactory::Creators().insert("c", new CEditorPageCreator()); // C language (.c's)
    EditorPageFactory::Creators().insert("h", new CEditorPageCreator()); // C headers (same as .c's)

    QApplication a(argc, argv);
/*    MainWindow w;
    w.showMaximized();
    */

    SettingsDialog diag;
    diag.show();

    return a.exec();
}
