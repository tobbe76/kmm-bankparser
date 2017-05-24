#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QUrl>
#include <QWebPage>
#include <QWebFrame>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QWebElement>
#include <QMouseEvent>
#include "sebparser.h"
#include "norwegianparser.h"
#include "logininterface.h"
#include "logindialog.h"
#include <QGraphicsRectItem>
#include "parserfactory.h"
#include "mapaccountdialog.h"
#include <QListWidgetItem>
#include "datesdialog.h"
#include "dateinterval.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

    first = true;
    ui->comboBox->addItems(ParserFactory::supportedParsers());
    ui->comboBox->addItem("RunAll");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::readFile(QString file)
{
    QFile f(file);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "Failed to open file " << file;
        return "";
    }
    QTextStream in(&f);

    return in.readAll();
}

void MainWindow::webView_loadFinished(bool arg1)
{
  qDebug() << "LoginDialog::webView_loadFinished start" << arg1;

//  currentParser->isLoginFinished();

  qDebug() << "LoginDialog::webView_loadFinished end";
}


void MainWindow::testParser1(QList<KmmAccountInfo>& accounts)
{
    DateInterval dateInterval;
    bool useForAll = false;

    for(int i = 0; i < accounts.size(); i++)
    {
        if(useForAll == false)
        {
            DatesDialog dates(accounts[i]);
            int res = dates.exec();

            if(res == QDialog::Rejected) {
                return;
            }

            dateInterval = dates.getDateInterval();
            useForAll = dates.getUseForAll();
        }

        BankParser* parser = ParserFactory::getParser(accounts[i].getMappedBank());

        connect(parser, SIGNAL(accountFinishedSignal(MyMoneyStatement*)),
                this, SLOT(accountFinished(MyMoneyStatement*)),
                Qt::UniqueConnection);
        qDebug() << "START PROCESS ACCOUNT";
        parser->processAccount(accounts[i], dateInterval);
    }
}

void MainWindow::accountFinished(MyMoneyStatement* s)
{
    qDebug() << "MainWindow accountFinished ";
    for(int i = 0; i < s->m_listTransactions.size(); i++)
    {
        QString apa;
        apa = s->m_listTransactions[i].m_datePosted.toString() + " " + s->m_listTransactions[i].m_amount + " " + s->m_listTransactions[i].m_strPayee + " " + s->m_listTransactions[i].m_strMemo + " " + s->m_listTransactions[i].m_strBankID;
        ui->transList->addItem(apa);
    }
    delete s;
}

void MainWindow::on_testStartButton_clicked()
{
    QList<KmmAccountInfo> accounts;
    QString name = ui->comboBox->currentText();
    if(name == "RunAll")
    {
        accounts.append(KmmAccountInfo("Hus", "5048 00123 45", "Seb", "ID3", QDate::currentDate(), QDate::currentDate(), "SEB", "50480037566"));
        accounts.append(KmmAccountInfo("Norwegiancc", "12345678", "Norwegian", "ID5", QDate::currentDate(), QDate::currentDate(), "NORWEGIAN", "12345678"));
    }
    else if(name == "SEB")
    {
        accounts.append(KmmAccountInfo("Hus", "5048 00123 45", "Seb", "ID3", QDate::currentDate(), QDate::currentDate(), name, "50480037566"));
    }
    else if(name == "NORWEGIAN")
    {
        accounts.append(KmmAccountInfo("Norwegiancc", "12345678", "Norwegian", "ID5", QDate::currentDate(), QDate::currentDate(), name, "12345678"));
    }
    else if(name == "HANDELSBANKEN")
    {
        accounts.append(KmmAccountInfo("HandelsBanken", "aa345678", "Norwegian", "ID4",
                                       QDate::currentDate(), QDate::currentDate(), name, "401942621"));
        accounts.append(KmmAccountInfo("HandelsBanken", "12345678", "Norwegian", "ID5",
                                       QDate::currentDate(), QDate::currentDate(), name, "401942511"));
        accounts.append(KmmAccountInfo("HandelsBanken", "45321", "Norwegian", "ID6",
                                       QDate::currentDate(), QDate::currentDate(), name, "72806508"));
        accounts.append(KmmAccountInfo("HandelsBanken", "aa321", "Norwegian", "ID7",
                                       QDate::currentDate(), QDate::currentDate(), name, "72806338"));
    }
    testParser1(accounts);
}

void MainWindow::on_mapAccount_clicked()
{
    KmmAccountInfo account("Hus", "5048 00123 45", "SEB", "ID3", QDate::currentDate(), QDate::currentDate(), "SEB", "50480037566");
    MapAccountDialog dlg(account);
    dlg.exec();
    qDebug() << "SELECTED " << dlg.getAccountNr() << "  " << dlg.getSelectedParser();
}
