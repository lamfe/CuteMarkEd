#
# Discount by David Parsons
#
# Github : https://github.com/Orc/discount
# Webpage: http://www.pell.portland.or.us/~orc/Code/discount/
#

TARGET = discount
TEMPLATE = lib

QT       -= core gui

# compile output is unreadable with -Wall
CONFIG += warn_off

SRC_ROOT = $$PWD/../../../3rdparty/discount

DEF_FILE = discount.def

INCLUDEPATH += \
    $$PWD \
    $${SRC_ROOT}

SOURCES += \
    $${SRC_ROOT}/mkdio.c \
    $${SRC_ROOT}/markdown.c \
    $${SRC_ROOT}/dumptree.c \
    $${SRC_ROOT}/generate.c \
    $${SRC_ROOT}/resource.c \
    $${SRC_ROOT}/docheader.c \
    version.c \
    $${SRC_ROOT}/toc.c \
    $${SRC_ROOT}/css.c \
    $${SRC_ROOT}/xml.c \
    $${SRC_ROOT}/Csio.c \
    $${SRC_ROOT}/xmlpage.c \
    $${SRC_ROOT}/basename.c \
    $${SRC_ROOT}/emmatch.c \
    $${SRC_ROOT}/github_flavoured.c \
    $${SRC_ROOT}/setup.c \
    $${SRC_ROOT}/tags.c \
    $${SRC_ROOT}/html5.c \
    $${SRC_ROOT}/flags.c

HEADERS += \
    mkdio.h \
    $${SRC_ROOT}/markdown.h \
    $${SRC_ROOT}/tags.h \
    config.h \
    config_win.h \
    config_mac.h
