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
    $$files($${SRC_ROOT}/src/*.c)

HEADERS += \
    $$files($${SRC_ROOT}/src/*.h)

macx:QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/../Frameworks/
