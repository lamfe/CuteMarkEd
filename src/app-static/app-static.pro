#
# Application Static Libary Project for CuteMarkEd
#
# Github : https://github.com/cloose/CuteMarkEd
#

TARGET = app-static
TEMPLATE = lib

QT += gui
lessThan(QT_VERSION, 5.6) {
    QT += webkitwidgets
    DEFINES += WITH_QTWEBENGINE=0
} else {
    QT += webenginewidgets webchannel
    DEFINES += WITH_QTWEBENGINE=1
}

CONFIG += staticlib
CONFIG += c++11

INCLUDEPATH += $$PWD

SOURCES += \
    snippets/json_snippet_translator.cpp \
    snippets/snippet_collection.cpp \
    converter/discount_markdown_converter.cpp \
    converter/reveal_markdown_converter.cpp \
    spellchecker/dictionary.cpp \
    template/html_template.cpp \
    template/presentation_template.cpp \
    themes/json_theme_translator.cpp \
    themes/style_manager.cpp \
    themes/theme.cpp \
    themes/theme_collection.cpp \
    completion_list_model.cpp \
    data_location.cpp \
    slideline_mapping.cpp \
    view_synchronizer.cpp \
    reveal_view_synchronizer.cpp \
    html_preview_controller.cpp \
    html_view_synchronizer.cpp \
    yaml_header_checker.cpp \
    html_previewer.cpp

HEADERS += \
    snippets/snippet.h \
    snippets/json_snippet_translator.h \
    snippets/json_snippet_translator_factory.h \
    snippets/snippet_collection.h \
    converter/markdown_converter.h \
    converter/markdown_document.h \
    converter/discount_markdown_converter.h \
    converter/reveal_markdown_converter.h \
    spellchecker/dictionary.h \
    template/template.h \
    template/html_template.h \
    template/presentation_template.h \
    themes/json_theme_translator.h \
    themes/json_theme_translator_factory.h \
    themes/style_manager.h \
    themes/theme.h \
    themes/theme_collection.h \
    completion_list_model.h \
    data_location.h \
    slideline_mapping.h \
    view_synchronizer.h \
    reveal_view_synchronizer.h \
    html_preview_controller.h \
    html_view_synchronizer.h \
    yaml_header_checker.h \
    html_previewer.h

#unix:!symbian {
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
#    INSTALLS += target
#}

##################################################
# Dependencies
##################################################

#
# JSON configuration library
#
INCLUDEPATH += $$PWD/../libs/jsonconfig

#
# Discount library
#
INCLUDEPATH += $$PWD/../../3rdparty/discount $$PWD/../libs/discount

#
# Hoedown library
#
with_hoedown {
    message("app-static: Enable hoedown markdown converter support")

    DEFINES += ENABLE_HOEDOWN
    SOURCES += converter/hoedownmarkdownconverter.cpp
    HEADERS += converter/hoedownmarkdownconverter.h

    win32:INCLUDEPATH += $$PWD/../../3rdparty/hoedown
}
