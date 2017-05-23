#ifndef DEBUGWEBPAGE_H
#define DEBUGWEBPAGE_H

#include <QWebPage>

class DebugWebPage : public QWebPage
{
    Q_OBJECT
public:
    DebugWebPage(QObject* parent = 0);
    QString userAgentForUrl222(const QUrl& url) const;
    void javaScriptConsoleMessage(const QString & message, int lineNumber, const QString & sourceID);
};

#endif
