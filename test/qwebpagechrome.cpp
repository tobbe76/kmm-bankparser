#include "qwebpagechrome.h"
#include <QDebug>

QString QWebPageChrome::userAgentForUrl222(const QUrl &url) const
{
    qDebug() << "Set user agent for " << url;
    return "Mozilla/5.0 (X11; Linux x86_64; rv:10.0) Gecko/20100101 Firefox/10.0";
}

void QWebPageChrome::javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID)
{
    qDebug() << "javaScriptConsoleMessage: " <<  sourceID << ":" << lineNumber << "  " << message;
}
