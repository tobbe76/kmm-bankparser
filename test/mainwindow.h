#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebElement>
#include <bankparser.h>
#include "kmmaccountinfo.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public slots:
    void epa();
    void epa2();
    void accountFinished(MyMoneyStatement* s);

private slots:
    void webView_loadFinished(bool arg1);

    void on_testStartButton_clicked();

    void on_mapAccount_clicked();

private:
    Ui::MainWindow *ui;
    void clickElement(QWebElement element);
    QString readFile(QString file);
    void testParser1(QList<KmmAccountInfo> &accounts);
    bool first;
    int nr;
};

#endif // MAINWINDOW_H
