#include "debugwebpage.h"
#include <QDebug>
#ifdef USE_WEBKIT
#include <QWebFrame>
#endif

DebugWebPage::DebugWebPage(QObject* parent) :
#ifdef USE_WEBKIT
    QWebPage(parent)
#else
    QWebEnginePage(parent)
#endif
{
}

QString DebugWebPage::userAgentForUrl222(const QUrl &url) const
{
    qDebug() << "Set user agent for " << url;
    return "Mozilla/5.0 (X11; Linux x86_64; rv:10.0) Gecko/20100101 Firefox/10.0";
}

#ifdef USE_WEBKIT
void DebugWebPage::javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID)
{
    qDebug() << "javaScriptConsoleMessage: " <<  sourceID << ":" << lineNumber << "  " << message;
}
#else
void DebugWebPage::javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString & message, int lineNumber, const QString & sourceID)
{
    Q_UNUSED(level);
    qDebug() << "javaScriptConsoleMessage: " <<  sourceID << ":" << lineNumber << "  " << message;
}
#endif

#ifdef USE_WEBENGINE
bool DebugWebPage::certificateError(const QWebEngineCertificateError &certificateError) {
    Q_UNUSED(certificateError);
    return true;
}
#endif
void DebugWebPage::loadPage(const QUrl &page) {
#ifdef USE_WEBENGINE
    load(page);
#else
    mainFrame()->load(page);
#endif
}

QUrl DebugWebPage::getUrl() {
#ifdef USE_WEBENGINE
    return url();
#else
    return mainFrame()->url();
#endif
}


void DebugWebPage::runJavaScript(QString s) {
#ifdef USE_WEBENGINE
    QWebEnginePage::runJavaScript(s);
#else
    mainFrame()->evaluateJavaScript(s);
#endif

}
