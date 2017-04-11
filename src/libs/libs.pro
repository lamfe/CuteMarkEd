
TEMPLATE = subdirs

SUBDIRS += \
    discount \
    hunspell \
    peg-markdown-highlight \
    pmh-adapter

win32 {
    with_hoedown {
        message("3rdparty: Build hoedown markdown converter library")
        DEFINES += ENABLE_HOEDOWN
        SUBDIRS += hoedown
    }
}

pmh-adapter.depends = peg-markdown-highlight
