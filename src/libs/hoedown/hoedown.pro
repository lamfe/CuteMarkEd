#
# Hoedown library
#
# Github : https://github.com/hoedown/hoedown
#

TARGET = hoedown
TEMPLATE = lib

QT -= core gui

SRC_ROOT = $$PWD/../../../3rdparty/hoedown

DEF_FILE = $${SRC_ROOT}/hoedown.def

SOURCES += \
    $${SRC_ROOT}/src/autolink.c \
    $${SRC_ROOT}/src/buffer.c \
    $${SRC_ROOT}/src/escape.c \
    $${SRC_ROOT}/src/html.c \
    $${SRC_ROOT}/src/html_blocks.c \
    $${SRC_ROOT}/src/html_smartypants.c \
    $${SRC_ROOT}/src/markdown.c \
    $${SRC_ROOT}/src/stack.c

HEADERS += \
    $${SRC_ROOT}/src/autolink.h \
    $${SRC_ROOT}/src/buffer.h \
    $${SRC_ROOT}/src/escape.h \
    $${SRC_ROOT}/src/html.h \
    $${SRC_ROOT}/src/markdown.h \
    $${SRC_ROOT}/src/stack.h
