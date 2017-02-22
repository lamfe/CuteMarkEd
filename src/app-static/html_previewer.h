
#ifndef ___HEADFILE_3A106946_25FF_4B54_A3E5_ECE5E8C8A2AF_
#define ___HEADFILE_3A106946_25FF_4B54_A3E5_ECE5E8C8A2AF_

#if WITH_QTWEBENGINE
#   include <QWebEngineView>
#else
#   include <QWebView>
#endif

#if WITH_QTWEBENGINE
class HtmlPreviewer : public QWebEngineView
#else
class HtmlPreviewer : public QWebView
#endif
{
    Q_OBJECT

public:
    HtmlPreviewer(QWidget *parent = nullptr);
    ~HtmlPreviewer();
};

#endif
