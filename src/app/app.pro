#-------------------------------------------------
#
# Project created by QtCreator 2013-03-16T11:40:15
#
#-------------------------------------------------

TARGET = cutemarked
TEMPLATE = app

include(../global.pri)

QT += core gui printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
win32: QT += winextras
qtHaveModule(webkitwidgets) {
    QT += webkitwidgets
    DEFINES += WITH_QTWEBENGINE=0
} else {
    QT += webenginewidgets webchannel
    DEFINES += WITH_QTWEBENGINE=1
}

CONFIG += c++11

unix: !macx {
  CONFIG += link_pkgconfig
}

TRANSLATIONS += \
    translations/cutemarked_cs.ts \
    translations/cutemarked_da.ts \
    translations/cutemarked_de.ts \
    translations/cutemarked_el.ts \
    translations/cutemarked_es.ts \
    translations/cutemarked_fr.ts \
    translations/cutemarked_hu_HU.ts \
    translations/cutemarked_id.ts \
    translations/cutemarked_ja.ts \
    translations/cutemarked_pl.ts \
    translations/cutemarked_pt_BR.ts \
    translations/cutemarked_ru.ts \
    translations/cutemarked_zh_CN.ts

win32: RC_FILE = cutemarked.rc
macx {
    ICON = resources/app-icon.icns
    QMAKE_INFO_PLIST = Info.plist
}

INCLUDEPATH += $$PWD

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    markdown_editor.cpp \
    controls/line_number_area.cpp \
    controls/active_label.cpp \
    controls/file_explorer_widget.cpp \
    controls/find_replace_widget.cpp \
    controls/recent_files_menu.cpp \
    html_preview_generator.cpp \
    markdown_highlighter.cpp \
    highlight_worker_thread.cpp \
    markdown_manipulator.cpp \
    export_pdf_dialog.cpp \
    export_html_dialog.cpp \
    html_highlighter.cpp \
    options.cpp \
    options_dialog.cpp \
    hunspell/spellchecker.cpp \
    controls/language_menu.cpp \
    table_tool_dialog.cpp \
    image_tool_dialog.cpp \
    snippet_completer.cpp \
    snippets_table_model.cpp \
    about_dialog.cpp \
    statusbar_widget.cpp

macx {
    SOURCES += \
        hunspell/spellchecker_macx.cpp
} else:win32 {
    SOURCES += \
        hunspell/spellchecker_win.cpp
} else {
    SOURCES += \
        hunspell/spellchecker_unix.cpp
}

HEADERS += \
    mainwindow.h \
    markdown_editor.h \
    controls/line_number_area.h \
    controls/active_label.h \
    controls/file_explorer_widget.h \
    controls/find_replace_widget.h \
    controls/recent_files_menu.h \
    html_preview_generator.h \
    markdown_highlighter.h \
    highlight_worker_thread.h \
    markdown_manipulator.h \
    export_pdf_dialog.h \
    export_html_dialog.h \
    html_highlighter.h \
    options.h \
    options_dialog.h \
    hunspell/spellchecker.h \
    controls/language_menu.h \
    table_tool_dialog.h \
    image_tool_dialog.h \
    snippet_completer.h \
    snippets_table_model.h \
    about_dialog.h \
    statusbar_widget.h \
    save_file_adapter.h

FORMS += \
    mainwindow.ui \
    controls/file_explorer_widget.ui \
    controls/find_replace_widget.ui \
    export_pdf_dialog.ui \
    export_html_dialog.ui \
    options_dialog.ui \
    table_tool_dialog.ui \
    image_tool_dialog.ui \
    about_dialog.ui

RESOURCES += \
    resources.qrc \
    translations.qrc

OTHER_FILES += \
    template.html \
    template_presentation.html \
    cutemarked.desktop \
    syntax.html \
    cutemarked.rc \
    syntax_cs.html \
    syntax_da.html \
    syntax_de.html \
    syntax_el.html \
    syntax_es.html \
    syntax_hu_HU.html \
    syntax_id.html \
    syntax_ja.html \
    syntax_pt_BR.html \
    syntax_zh_CN.html \
    styles/solarized-dark.css \
    styles/markdown.css \
    styles/github.css \
    styles/clearness-dark.css \
    styles/clearness.css \
    styles/byword-dark.css \
    styles/solarized-light.css \
    markdown-snippets.json \
    builtin-htmlpreview-themes.json

# translations
lrelease.input         = TRANSLATIONS
lrelease.output        = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
lrelease.commands      = $$[QT_INSTALL_BINS]/lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG       += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

###################################################################################################
## DEPENDENCIES
###################################################################################################

#
# JSON configuration library
#
INCLUDEPATH += $$PWD/../libs/jsonconfig

# Use internal static library: app-static
INCLUDEPATH += $$PWD/../app-static
LIBS += -L$$OUT_PWD/../app-static$${OUT_TAIL} -lapp-static

# discount
INCLUDEPATH += $$PWD/../../3rdparty/discount.git
LIBS += -L$$OUT_PWD/../libs/discount$${OUT_TAIL} -ldiscount

# pmh-adapter
INCLUDEPATH += $$PWD/../libs/pmh-adapter/..
LIBS += -L$$OUT_PWD/../libs/pmh-adapter$${OUT_TAIL} -lpmh-adapter

# peg-markdown-highlight
INCLUDEPATH += \
    $$PWD/../../3rdparty/peg-markdown-highlight.git \
    $$PWD/../libs/peg-markdown-highlight
LIBS += -L$$OUT_PWD/../libs/peg-markdown-highlight$${OUT_TAIL} -lpmh

# hunspell
INCLUDEPATH += $$PWD/../../3rdparty/hunspell.git/src
LIBS += -L$$OUT_PWD/../libs/hunspell$${OUT_TAIL} -lhunspell

unix:!macx {
  PKGCONFIG += hunspell
}

# hoedown
with_hoedown {
    DEFINES += ENABLE_HOEDOWN
    INCLUDEPATH += $$PWD/../../3rdparty/hoedown.git
    LIBS += -L$$OUT_PWD/../libs/hoedown$${OUT_TAIL} -lhoedown
}

## INSTALLATION

unix:!macx {
   isEmpty(PREFIX): PREFIX = /usr
   DATADIR = $${PREFIX}/share

   # install desktop file
   desktop.path = $${DATADIR}/applications
   desktop.files += cutemarked.desktop

   # install icons
   icon16.path = $${DATADIR}/icons/hicolor/16x16/apps
   icon16.files = resources/icons/16x16/cutemarked.png

   icon32.path = $${DATADIR}/icons/hicolor/32x32/apps
   icon32.files = resources/icons/32x32/cutemarked.png

   icon48.path = $${DATADIR}/icons/hicolor/48x48/apps
   icon48.files = resources/icons/48x48/cutemarked.png

   icon64.path = $${DATADIR}/icons/hicolor/64x64/apps
   icon64.files = resources/icons/64x64/cutemarked.png

   icon128.path = $${DATADIR}/icons/hicolor/128x128/apps
   icon128.files = resources/icons/128x128/cutemarked.png

   iconsvg.path = $${DATADIR}/icons/hicolor/scalable/apps
   iconsvg.files = resources/icons/scalable/cutemarked.svg

   # install application
   target.path = $${PREFIX}/bin

   INSTALLS += target desktop icon16 icon32 icon48 icon64 icon128 iconsvg
   message("The project will be installed in prefix $${PREFIX}")
}

################################################################################

mac {
    CUTE_MARK_ED_LIBS = \
        $$OUT_PWD/../libs/discount/libdiscount.1.dylib \
        $$OUT_PWD/../libs/hunspell/libhunspell.1.dylib
    with_hoedown {
        CUTE_MARK_ED_LIBS += $$OUT_PWD/../libs/hoedown/libhoedown.1.dylib
    }
} else:win32 {
    CUTE_MARK_ED_LIBS = \
        $$OUT_PWD/../libs/discount/$${DEBUG_MODE}/discount.dll \
        $$OUT_PWD/../libs/hunspell/$${DEBUG_MODE}/hunspell.dll
    with_hoedown {
        CUTE_MARK_ED_LIBS += $$OUT_PWD/../libs/hoedown/$${DEBUG_MODE}/hoedown.dll
    }
} else {
    CUTE_MARK_ED_LIBS = \
        $$OUT_PWD/../libs/discount/libdiscount.1.so \
        $$OUT_PWD/../libs/hunspell/libhunspell.1.so
    with_hoedown {
        CUTE_MARK_ED_LIBS += $$OUT_PWD/../libs/hoedown/libhoedown.1.so
    }
}

include(post_link.pri)

include(archive.pri)
