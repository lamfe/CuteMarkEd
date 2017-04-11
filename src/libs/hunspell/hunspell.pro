#-------------------------------------------------
#
# Project created by QtCreator 2011-07-04T16:26:46
#
#-------------------------------------------------

TARGET = hunspell
TEMPLATE = lib

QT -= core gui

DEFINES += HUNSPELL_LIBRARY
DEFINES += BUILDING_LIBHUNSPELL

CONFIG += precompile_header

OBJECTS_DIR += temp

SRC_ROOT = $$PWD/../../../3rdparty/hunspell.git

INCLUDEPATH += \
    $${SRC_ROOT}/src/hunspell \
    $${SRC_ROOT}/src/parsers \
    $${SRC_ROOT}/src/tools

SOURCES += \
    $$files($${SRC_ROOT}/src/hunspell/*.cxx) \
    $$files($${SRC_ROOT}/src/parsers/*.cxx) \

HEADERS += \
    $$files($${SRC_ROOT}/src/hunspell/*.hxx) \
    $$files($${SRC_ROOT}/src/hunspell/*.h) \
    $$files($${SRC_ROOT}/src/parsers/*.hxx)

OTHER_FILES += \
    $${SRC_ROOT}/license.myspell \
    $${SRC_ROOT}/license.hunspell \
    $${SRC_ROOT}/src/hunspell/utf_info.cxx
