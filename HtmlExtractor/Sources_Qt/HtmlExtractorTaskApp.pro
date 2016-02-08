QT += core webkitwidgets xml
QT -= gui

CONFIG += c++11

TARGET = HtmlExtractorTaskApp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    blockanalyzer.cpp \
    outputformatter.cpp \
    webpageextractor.cpp \
    webkithtmldomparser.cpp \
    textsavertofile.cpp \
    htmlsourceproviderfromweb.cpp \
    analyzeroptions.cpp

HEADERS += \
    interfaces.h \
    blockanalyzer.h \
    outputformatter.h \
    webpageextractor.h \
    webkithtmldomparser.h \
    textsavertofile.h \
    htmlsourceproviderfromweb.h \
    analyzeroptions.h
