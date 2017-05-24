h#-------------------------------------------------
#
# Project created by QtCreator 2015-11-26T17:34:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ffmpeg-ui
TEMPLATE = app

## shared_ptr

CONFIG  +=  c++11
CONFIG   -= x86_64

CONFIG+= static

RC_FILE = myapp.rc

SOURCES +=  main.cpp \
            mainwindow.cpp \
            mloading.cpp \
            mfileinfo.cpp \
            process/mcodecs.cpp \
            process/mfile.cpp \
            process/mpixfmt.cpp \
            process/mencode.cpp

HEADERS += \
            mainwindow.hh \
            mloading.hh \
            mfileinfo.hh \
            process/mcodecs.hh \
            process/mfile.hh \
            process/mpixfmt.hh \
            process/mencode.hh

FORMS   +=  mainwindow.ui

TRANSLATIONS = software_en.ts software_fr.ts

RESOURCES += \
    pictures.qrc
