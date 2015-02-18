#-------------------------------------------------
#
# Project created by QtCreator 2015-02-09T20:05:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IDE
TEMPLATE = app


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
    Editing/Utility/LineNumberArea.cpp

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
    Editing/Utility/LineNumberArea.h

FORMS    += \
    GUI/mainwindow.ui \
    GUI/newprojectdialog.ui \
    GUI/openprojectdialog.ui
