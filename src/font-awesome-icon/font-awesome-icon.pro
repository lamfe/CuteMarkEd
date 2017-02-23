#-------------------------------------------------
#
# Project created by QtCreator 2013-05-06T20:13:32
#
#-------------------------------------------------

QT       += core gui

TARGET = font-awesome-icon
TEMPLATE = lib
CONFIG += plugin

win32 {
   debug:DESTDIR = $$OUT_PWD/../app/debug/iconengines
   release:DESTDIR = $$OUT_PWD/../app/release/iconengines
}

SOURCES += \
    font_awesome_icon_engine_plugin.cpp \
    font_awesome_icon_engine.cpp

HEADERS += \
    font_awesome_icon_engine_plugin.h \
    font_awesome_icon_engine.h

OTHER_FILES += \
    font-awesome-icon.json \
    font-awesome.ttf

#unix:!symbian {
#    maemo5 {
#        target.path = /opt/usr/lib
#    } else {
#        target.path = /usr/lib
#    }
#    INSTALLS += target
#}

RESOURCES += \
    font-awesome-icon.qrc

## INSTALLATION

unix {
   # install plugin
   target.path = $$[QT_INSTALL_PLUGINS]/iconengines
   INSTALLS += target
   message("The project will be installed in $$[QT_INSTALL_PLUGINS]/iconengines")
}
