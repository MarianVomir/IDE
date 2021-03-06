#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QProcess>

#include "GUI/mainwindow.h"
#include "ProjectManagement/JSONConverter.h"
#include "Editing/EditorPageFactory.h"
#include "GUI/settingsdialog.h"
#include "Testing/Tester_FileManager.h"

int main(int argc, char *argv[])
{
    // Add supported file extensions

    auto cPageCreator = new CEditorPageCreator();
    EditorPageFactory::Creators().insert("", new DefaultEditorPageCreator()); // default (plain text)
    EditorPageFactory::Creators().insert("c", cPageCreator); // C language (.c's)
    EditorPageFactory::Creators().insert("h", cPageCreator); // C headers (same as .c's)

    try
    {
        SettingsManager::LoadProjectDefaults();
    }
    catch (Exception&)
    {
        qDebug() << "Cannot load default project settings from file.";
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    //w.setStyleSheet(Global::Visual.WindowStyle);
    return a.exec();
}
