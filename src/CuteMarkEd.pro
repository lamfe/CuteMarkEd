#-------------------------------------------------
#
# Project created by QtCreator 2013-03-16T11:40:15
#
#-------------------------------------------------

TEMPLATE = subdirs

CONFIG += c++11

SUBDIRS = \
    libs \
    app-static \
    app \
    font-awesome-icon \
    test

# build order: libs -> app-static -> app & test
app.depends = libs app-static
test.depends = libs app-static
