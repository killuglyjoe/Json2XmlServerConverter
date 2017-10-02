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
