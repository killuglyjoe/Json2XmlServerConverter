#-------------------------------------------------
#
# Project created by QtCreator 2017-09-29T21:39:01
#
#-------------------------------------------------

QT -= core
QT -= gui

TARGET = QNweb

CONFIG += c++98 console
CONFIG -= app_bundle qml_debug

#Build pathes
BUILDDIR = Compiled

linux-g++:TARGET = QNweb
linux-g++: BUILDDIR = Compiled/Linux32

linux-g++-64:TARGET = QNweb64
linux-g++-64: BUILDDIR = Compiled/Linux64

#DESTDIR += $$PWD/Distr

win32:!wince*: BUILDDIR = Compiled/Win32

win64:!wince*: BUILDDIR = Compiled/Win64#???

wince*: BUILDDIR = Compiled/WinCE

android: BUILDDIR = Compiled/Android

#UI_DIR      = $$BUILDDIR/Ui
#MOC_DIR     = $$BUILDDIR/MOC
OBJECTS_DIR = $$BUILDDIR/OBJ

#From Qt we need it's own makefile
MAKEFILE = QtMakefile

TEMPLATE = app

include($$PWD/convutils/rapidjson/rapidjson.pri)
include($$PWD/convutils/rapidxml/rapidxml.pri)

SOURCES += \
    nweb.cpp \
#    main.cpp \
#    convutils/xml2json.cpp \
#    convutils/json2xml.cpp

HEADERS += \
#    convutils/xml2json.hpp \
#    convutils/json2xml.hpp
