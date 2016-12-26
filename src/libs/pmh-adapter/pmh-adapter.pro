#
# PEG Markdown Highlight Adapter Static Libary Project for CuteMarkEd
#
# Github : https://github.com/cloose/CuteMarkEd
#

TARGET = pmh-adapter
TEMPLATE = lib
CONFIG += staticlib

QT += core gui widgets

CONFIG += c++11

SOURCES += \
    pmhmarkdownparser.cpp \
    styleparser.cpp

HEADERS  += \
    pmhmarkdownparser.h \
    styleparser.h \
    definitions.h

###################################################################################################
## DEPENDENCIES
###################################################################################################

#
# peg-markdown-highlight
#

INCLUDEPATH += $$PWD/../../../3rdparty/peg-markdown-highlight
