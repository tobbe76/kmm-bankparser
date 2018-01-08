#ifndef CUSTOMWEBVIEW_H
#define CUSTOMWEBVIEW_H

#ifdef USE_WEBKIT
#include <QWebView>

class CustomWebView : public QWebView
{
public:
    CustomWebView(QWidget *parent = 0) : QWebView(parent) { }
};
#else
#include <QWebEngineView>

class CustomWebView : public QWebEngineView
{
public:
    CustomWebView(QWidget *parent = 0) : QWebEngineView(parent) { }
};
#endif

#endif
