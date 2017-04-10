#
# Integration Test Project for CuteMarkEd
#
# Github : https://github.com/cloose/CuteMarkEd
#

TARGET = integrationtest

include(../../global.pri)

QT += testlib
QT += gui
lessThan(QT_VERSION, 5.6) {
    QT += webkitwidgets
    DEFINES += WITH_QTWEBENGINE=0
} else {
    QT += webenginewidgets webchannel
    DEFINES += WITH_QTWEBENGINE=1
}

CONFIG += console testcase
CONFIG += c++11

SOURCES += \
    discount_markdown_converter_test.cpp \
    html_preview_controller_test.cpp \
    html_template_test.cpp \
    json_snippet_file_test.cpp \
    json_theme_file_test.cpp \
    main.cpp \
    pmh_markdown_parser_test.cpp \
    reveal_markdown_converter_test.cpp \
    theme_collection_test.cpp

HEADERS += \
    discount_markdown_converter_test.h \
    html_preview_controller_test.h \
    html_template_test.h \
    json_snippet_file_test.h \
    json_theme_file_test.h \
    pmh_markdown_parser_test.h \
    reveal_markdown_converter_test.h \
    theme_collection_test.h

target.CONFIG += no_default_install

#
# JSON configuration library
#
INCLUDEPATH += $$PWD/../../libs/jsonconfig

#
# Add search paths below /usr/local for Mac OSX
#
macx {
  LIBS += -L/usr/local/lib
  INCLUDEPATH += /usr/local/include
}

##################################################
# Use internal static library: app-static
##################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../app-static/release/ -lapp-static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../app-static/debug/ -lapp-static
else:unix: LIBS += -L$$OUT_PWD/../../app-static/ -lapp-static

INCLUDEPATH += $$PWD/../../app-static
DEPENDPATH += $$PWD/../../app-static

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/release/libapp-static.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/debug/libapp-static.a
else:win32-msvc*:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/release/app-static.lib
else:win32-msvc*:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../app-static/debug/app-static.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../app-static/libapp-static.a

#
# PEG Markdown Highlight adapter library
#
LIBS += -L$$OUT_PWD/../../libs/pmh-adapter$${OUT_TAIL} -lpmh-adapter

INCLUDEPATH += $$PWD/../../libs/pmh-adapter

win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/pmh-adapter/$${DEBUG_MODE}/libpmh-adapter.a
else:win32-msvc*: PRE_TARGETDEPS += $$OUT_PWD/../../libs/pmh-adapter/$${DEBUG_MODE}/pmh-adapter.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../libs/pmh-adapter/libpmh-adapter.a

#
# peg-markdown-highlight
#
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../../3rdparty/peg-markdown-highlight/release/ -lpmh
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../../3rdparty/peg-markdown-highlight/debug/ -lpmh
else:unix: LIBS += -L$$OUT_PWD/../../../3rdparty/peg-markdown-highlight/ -lpmh

INCLUDEPATH += $$PWD/../../../3rdparty/peg-markdown-highlight

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/peg-markdown-highlight/release/libpmh.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/peg-markdown-highlight/debug/libpmh.a
else:win32-msvc*:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/peg-markdown-highlight/release/pmh.lib
else:win32-msvc*:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/peg-markdown-highlight/debug/pmh.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../../3rdparty/peg-markdown-highlight/libpmh.a

#
# Discount library
#
INCLUDEPATH += $$PWD/../../../3rdparty/
LIBS += -L$$OUT_PWD/../../libs/discount$${OUT_TAIL} -ldiscount

#win32-g++: PRE_TARGETDEPS += $$OUT_PWD/../../libs/discount/$${DEBUG_MODE}/libdiscount.a
#else:win32-msvc*: PRE_TARGETDEPS += $$OUT_PWD/../../libs/discount/$${DEBUG_MODE}/discount.lib
#else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../libs/discount/libdiscount.a

#
# hoedown
#
with_hoedown {
    DEFINES += ENABLE_HOEDOWN
    SOURCES += hoedown_markdown_converter_test.cpp
    HEADERS += hoedown_markdown_converter_test.h

    INCLUDEPATH += $$PWD/../../../3rdparty/hoedown

    LIBS += -L$$OUT_PWD/../../libs/hoedown$${OUT_TAIL} -lhoedown

    #win32: PRE_TARGETDEPS += $$OUT_PWD/../../libs/hoedown/$${DEBUG_MODE}/libhoedown.a
}
