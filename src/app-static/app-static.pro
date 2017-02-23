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
    snippets/jsonsnippettranslator.cpp \
    snippets/snippetcollection.cpp \
    converter/discount_markdown_converter.cpp \
    converter/reveal_markdown_converter.cpp \
    spellchecker/dictionary.cpp \
    template/htmltemplate.cpp \
    template/presentationtemplate.cpp \
    themes/jsonthemetranslator.cpp \
    themes/stylemanager.cpp \
    themes/theme.cpp \
    themes/themecollection.cpp \
    completionlistmodel.cpp \
    datalocation.cpp \
    slidelinemapping.cpp \
    viewsynchronizer.cpp \
    revealviewsynchronizer.cpp \
    htmlpreviewcontroller.cpp \
    htmlviewsynchronizer.cpp \
    yamlheaderchecker.cpp \
    html_previewer.cpp

HEADERS += \
    snippets/snippet.h \
    snippets/jsonsnippettranslator.h \
    snippets/jsonsnippettranslatorfactory.h \
    snippets/snippetcollection.h \
    converter/markdown_converter.h \
    converter/markdown_document.h \
    converter/discount_markdown_converter.h \
    converter/reveal_markdown_converter.h \
    spellchecker/dictionary.h \
    template/template.h \
    template/htmltemplate.h \
    template/presentationtemplate.h \
    themes/jsonthemetranslator.h \
    themes/jsonthemetranslatorfactory.h \
    themes/stylemanager.h \
    themes/theme.h \
    themes/themecollection.h \
    completionlistmodel.h \
    datalocation.h \
    slidelinemapping.h \
    viewsynchronizer.h \
    revealviewsynchronizer.h \
    htmlpreviewcontroller.h \
    htmlviewsynchronizer.h \
    yamlheaderchecker.h \
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
