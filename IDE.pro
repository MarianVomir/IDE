#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T20:05:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IDE
TEMPLATE = app

INCLUDEPATH += /usr/lib/llvm-3.4/include/
LIBS += -L/usr/lib/llvm-3.4/lib -lclang
CONFIG += c++11

SOURCES += main.cpp\
    FileManager.cpp \
    Testing/Tester_FileManager.cpp \
    GUI/mainwindow.cpp \
    GUI/newprojectdialog.cpp \
    Project.cpp \
    ProjectManagement/ProjectExplorer.cpp \
    ProjectManagement/ProjectDirModel.cpp \
    ProjectManagement/JSONConverter.cpp \
    GUI/openprojectdialog.cpp \
    Editing/DefaultEditorPage.cpp \
    Editing/DefaultEditorPageCreator.cpp \
    Editing/EditorPageFactory.cpp \
    Editing/Editor.cpp \
    Editing/EditorPageCreator.cpp \
    Editing/EditorPage.cpp \
    Editing/CEditorPageCreator.cpp \
    Editing/Utility/LineNumberArea.cpp \
    Editing/CEditorPage.cpp \
    Editing/Utility/CLexic.cpp \
    Editing/Utility/CSyntaxHighlighter.cpp \
    ProjectTesting/MakefileBasedProjectBuilder.cpp \
    ProjectTesting/ListOutputWriter.cpp \
    ProjectTesting/ProjectBuilder.cpp \
    ProjectTesting/ProjectRunner.cpp \
    GUI/settingsdialog.cpp \
    Editing/Core/CParser.cpp \
    GUI/VisualStyles.cpp \
    Global.cpp \
    Settings/SettingsManager.cpp

HEADERS  += \
    FileManager.h \
    Exceptions.h \
    Testing/Tester_FileMan.h \
    Testing/Tester_FileManager.h \
    GUI/mainwindow.h \
    GUI/newprojectdialog.h \
    Project.h \
    ProjectManagement/ProjectExplorer.h \
    ProjectManagement/ProjectDirModel.h \
    ProjectManagement/JSONConverter.h \
    ProjectManagement/ProjectFileConverter.h \
    GUI/openprojectdialog.h \
    Editing/EditorPage.h \
    Editing/DefaultEditorPage.h \
    Editing/EditorPageCreator.h \
    Editing/DefaultEditorPageCreator.h \
    Editing/EditorPageFactory.h \
    Editing/Editor.h \
    Editing/CEditorPageCreator.h \
    Editing/Utility/LineNumberArea.h \
    Editing/CEditorPage.h \
    Editing/Utility/CLexic.h \
    Editing/Utility/CSyntaxHighlighter.h \
    ProjectTesting/MakefileBasedProjectBuilder.h \
    ProjectTesting/ProjectBuilder.h \
    ProjectTesting/OutputWriter.h \
    ProjectTesting/ListOutputWriter.h \
    ProjectTesting/ProjectRunner.h \
    GUI/settingsdialog.h \
    Editing/Core/CParser.h \
    Editing/Core/DiagnosticDTO.h \
    GUI/VisualStyles.h \
    Global.h \
    Settings/SettingsManager.h

FORMS    += \
    GUI/mainwindow.ui \
    GUI/newprojectdialog.ui \
    GUI/openprojectdialog.ui \
    GUI/settingsdialog.ui
