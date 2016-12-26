TEMPLATE = subdirs

SUBDIRS += \
    peg-markdown-highlight

win32 {
    SUBDIRS += \
        discount

    with_hoedown {
        message("3rdparty: Build hoedown markdown converter library")
        DEFINES += ENABLE_HOEDOWN
        SUBDIRS += hoedown
    }
}
