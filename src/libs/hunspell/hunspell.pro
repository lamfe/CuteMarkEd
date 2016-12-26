#-------------------------------------------------
#
# Project created by QtCreator 2011-07-04T16:26:46
#
#-------------------------------------------------

TEMPLATE = lib
TARGET = hunspell

QT -= core gui

DEFINES += HUNSPELL_LIBRARY
DEFINES += BUILDING_LIBHUNSPELL

CONFIG += precompile_header

OBJECTS_DIR += temp

SRC_ROOT = $$PWD/../../../3rdparty/hunspell/src

INCLUDEPATH += ./ \
    $${SRC_ROOT}/hunspell \
    $${SRC_ROOT}/parsers \
    $${SRC_ROOT}/tools \
    $${SRC_ROOT}/win_api

SOURCES += \
    $${SRC_ROOT}/hunspell/suggestmgr.cxx \
    $${SRC_ROOT}/hunspell/replist.cxx \
    $${SRC_ROOT}/hunspell/phonet.cxx \
    $${SRC_ROOT}/hunspell/hunzip.cxx \
    $${SRC_ROOT}/hunspell/hunspell.cxx \
    $${SRC_ROOT}/hunspell/hashmgr.cxx \
    $${SRC_ROOT}/hunspell/filemgr.cxx \
    $${SRC_ROOT}/hunspell/dictmgr.cxx \
    $${SRC_ROOT}/hunspell/csutil.cxx \
    $${SRC_ROOT}/hunspell/affixmgr.cxx \
    $${SRC_ROOT}/hunspell/affentry.cxx \
# parsers
    $${SRC_ROOT}/parsers/firstparser.cxx \
    $${SRC_ROOT}/parsers/htmlparser.cxx \
    $${SRC_ROOT}/parsers/latexparser.cxx \
    $${SRC_ROOT}/parsers/manparser.cxx \
    $${SRC_ROOT}/parsers/textparser.cxx \

HEADERS +=\
    $${SRC_ROOT}/hunspell/w_char.hxx \
    $${SRC_ROOT}/hunspell/suggestmgr.hxx \
    $${SRC_ROOT}/hunspell/replist.hxx \
    $${SRC_ROOT}/hunspell/phonet.hxx \
    $${SRC_ROOT}/hunspell/langnum.hxx \
    $${SRC_ROOT}/hunspell/hunzip.hxx \
    $${SRC_ROOT}/hunspell/hunvisapi.h \
    $${SRC_ROOT}/hunspell/hunspell.hxx \
    $${SRC_ROOT}/hunspell/hunspell.h \
    $${SRC_ROOT}/hunspell/htypes.hxx \
    $${SRC_ROOT}/hunspell/hashmgr.hxx \
    $${SRC_ROOT}/hunspell/filemgr.hxx \
    $${SRC_ROOT}/hunspell/dictmgr.hxx \
    $${SRC_ROOT}/hunspell/csutil.hxx \
    $${SRC_ROOT}/hunspell/baseaffix.hxx \
    $${SRC_ROOT}/hunspell/atypes.hxx \
    $${SRC_ROOT}/hunspell/affixmgr.hxx \
    $${SRC_ROOT}/hunspell/affentry.hxx \
    $${SRC_ROOT}/win_api/config.h \
# parsers
    $${SRC_ROOT}/parsers/firstparser.hxx \
    $${SRC_ROOT}/parsers/htmlparser.hxx \
    $${SRC_ROOT}/parsers/manparser.hxx \
    $${SRC_ROOT}/parsers/latexparser.hxx \
    $${SRC_ROOT}/parsers/textparser.hxx \

!static: {
  win32:RC_FILE = $${SRC_ROOT}/win_api/Hunspell.rc
}

OTHER_FILES +=\
    $${SRC_ROOT}/hunspell/license.myspell \
    $${SRC_ROOT}/hunspell/license.hunspell \
    $${SRC_ROOT}/hunspell/utf_info.cxx
