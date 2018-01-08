#ifndef DEBUGWEBPAGE_H
#define DEBUGWEBPAGE_H

#ifdef USE_WEBKIT
#include <QWebPage>

class DebugWebPage : public QWebPage
{
#else
#include <QWebEnginePage>

class DebugWebPage : public QWebEnginePage
{

#endif

    Q_OBJECT
public:
    DebugWebPage(QObject* parent = 0);
    QString userAgentForUrl222(const QUrl& url) const;
#ifdef USE_WEBKIT
    void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);
#else
    bool certificateError(const QWebEngineCertificateError &certificateError);
    void javaScriptConsoleMessage(JavaScriptConsoleMessageLevel level, const QString & message, int lineNumber, const QString & sourceID);
#endif
    void loadPage(const QUrl &page);
    QUrl getUrl();
    void runJavaScript(QString s);
};

#endif
