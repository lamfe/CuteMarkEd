
#if WITH_QTWEBENGINE
#   include <QWebEngineSettings>
#else
#   include <QWebFrame>
#endif

#include "html_previewer.h"


HtmlPreviewer::HtmlPreviewer(QWidget *parent)
#if WITH_QTWEBENGINE
    : QWebEngineView(parent)
#else
    : QWebView(parent)
#endif
{}

HtmlPreviewer::~HtmlPreviewer()
{}
