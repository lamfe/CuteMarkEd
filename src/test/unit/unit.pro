#
# Unit Test Project for CuteMarkEd
#
# Github : https://github.com/cloose/CuteMarkEd
#

QT += testlib

TARGET = unittest
CONFIG += console testcase
CONFIG += c++11

SOURCES += \
    main.cpp \
    completion_list_model_test.cpp \
    snippet_test.cpp \
    json_snippet_translator_test.cpp \
    json_theme_translator_test.cpp \
    json_translator_factory_test.cpp \
    slideline_mapping_test.cpp \
    snippet_collection_test.cpp \
    dictionary_test.cpp \
    yaml_header_checker_test.cpp \
    theme_test.cpp \
    theme_collection_test.cpp \
    style_manager_test.cpp

HEADERS += \
    completion_list_model_test.h \
    snippet_test.h \
    json_snippet_translator_test.h \
    json_theme_translator_test.h \
    json_translator_factory_test.h \
    slideline_mapping_test.h \
    snippet_collection_test.h \
    dictionary_test.h \
    yaml_header_checker_test.h \
    theme_test.h \
    theme_collection_test.h \
    style_manager_test.h

target.CONFIG += no_default_install

#
# JSON configuration library
#
INCLUDEPATH += $$PWD/../../libs/jsonconfig

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
