
isEmpty(PREFIX): PREFIX = $$PWD/../../out/$${OS_NAME}-$${QMAKE_HOST.arch}-$${DEBUG_MODE}

mac {
} else:win32 {
    # app
    target.path = $${PREFIX}
    target.files = $$OUT_PWD/$${DEBUG_MODE}/$${TARGET}.exe
    INSTALLS += target

    # libs
    libs.path = $${PREFIX}
    libs.files = $${CUTE_MARK_ED_LIBS}
    INSTALLS += libs
    
    # deploy
    DEPLOY_BINS += $${PREFIX}/$${TARGET}.exe
    deploy.extra = windeployqt --$${DEBUG_MODE} --dir $${PREFIX} $${DEPLOY_BINS}
    deploy.path = $${PREFIX}
    INSTALLS += deploy
} else {
}