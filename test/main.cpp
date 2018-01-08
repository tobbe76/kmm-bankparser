#include <QApplication>
#include "mainwindow.h"
//#include <QNetworkProxyFactory>

int main(int argc, char *argv[])
{
 //   QNetworkProxyFactory::setUseSystemConfiguration(false);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
