#-------------------------------------------------
#
# Project created by QtCreator 2013-04-24T19:50:18
#
#-------------------------------------------------

CONFIG -= qt

TARGET = pmh
TEMPLATE = lib
CONFIG += staticlib

SRC_ROOT = $$PWD/../../../3rdparty/peg-markdown-highlight

INCLUDEPATH += \
    $${SRC_ROOT}

SOURCES += \
    pmh_parser.c \
    pmh_styleparser.c

HEADERS += \
    $${SRC_ROOT}/pmh_styleparser.h \
    $${SRC_ROOT}/pmh_parser.h \
    $${SRC_ROOT}/pmh_definitions.h
