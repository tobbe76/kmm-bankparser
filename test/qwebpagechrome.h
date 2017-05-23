#ifndef QWEBPAGECHROME_H
#define QWEBPAGECHROME_H

#include <QWebPage>

class QWebPageChrome : public QWebPage
{
    Q_OBJECT
public:
    QString userAgentForUrl222(const QUrl& url) const;
    void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);
};

#endif // QWEBPAGECHROME_H
