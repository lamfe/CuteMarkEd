#
# Hunspell library
#
# Github : https://github.com/hunspell/hunspell.git
#

TARGET = hunspell
TEMPLATE = lib

include(../../global.pri)

CONFIG -= qt

DEFINES += HUNSPELL_LIBRARY
DEFINES += BUILDING_LIBHUNSPELL

CONFIG += precompile_header

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
