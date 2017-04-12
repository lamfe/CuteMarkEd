#
# Hoedown library
#
# Github : https://github.com/hoedown/hoedown
#

TARGET = hoedown
TEMPLATE = lib

CONFIG -= qt

# compile output is unreadable with -Wall
CONFIG += warn_off

SRC_ROOT = $$PWD/../../../3rdparty/hoedown.git

DEF_FILE = $${SRC_ROOT}/hoedown.def

SOURCES += \
    $$files($${SRC_ROOT}/src/*.c)

HEADERS += \
    $$files($${SRC_ROOT}/src/*.h)
