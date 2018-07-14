#-------------------------------------------------
#
# Project created by QtCreator 2017-12-19T14:48:46
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIleSystem
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    filesystem.cpp \
    editor.cpp \
    properties_dialog.cpp \
    login_dialog.cpp

HEADERS  += mainwindow.h \
    fcb.h \
    filesystem.h \
    editor.h \
    properties_dialog.h \
    login_dialog.h

FORMS    += mainwindow.ui \
    editor.ui \
    properties_dialog.ui \
    login_dialog.ui

DISTFILES += \
    pic/dir.ico \
    login.txt \
    pic/edit.ico

RESOURCES += \
    src.qrc
