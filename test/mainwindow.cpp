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
#include "qwebpagechrome.h"
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
//    QWebPage* page = new QWebPageChrome();
//    ui->webView->setPage(page);
 //   ui->webView->load(QUrl("https://datatables.net/examples/server_side/post.html")); // file:///home/zlartho/saved.html"));
 //   ui->webView->load(QUrl("https://dura-bb.mo.sw.ericsson.se/#page=commit"));
 //   ui->webView->load(QUrl("file:///home/zlartho/saved.html"));
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

void MainWindow::epa2()
{
    qDebug() << "EPA CALLED" << nr;
    nr++;
}

void MainWindow::epa()
{
    qDebug() << "EPA CALLED";
 //   QVariant res1 = ui->webView->page()->mainFrame()->evaluateJavaScript("$( document ).unbind(\"ajaxComplete\")");
/*
    QVariant res = ui->webView->page()->mainFrame()->evaluateJavaScript("var accDiv = document.getElementById(\"example\");"
                                                                        "var epa = [];"
                                                                        "var rows = accDiv.tBodies[0].rows;"
                                                                        "for(var i = 0; i < rows.length; i++) {"
                                                                        "  epa.push({Name:rows[i].cells[0].innerText, Date:rows[i].cells[1].innerText});"
                                                                        "}"
                                                                        "epa;");
    qDebug() << "APA " << res.typeName();
    qDebug() << "APA " << res.toDouble();
    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        qDebug() << "APA2 " << map["Name"].toString() << " " << map["Date"].toString() << " " << map["Sum"].toString() << " " << map["Extra"].toString();
    }

    QVariant res3 = ui->webView->page()->mainFrame()->evaluateJavaScript("$( document ).ajaxComplete(function( event, request, settings ) { console.log(\"ajax event\"); MainWindow.epa(); });");


  //  QVariant res4 = ui->webView->page()->mainFrame()->evaluateJavaScript("var event = document.createEvent('Event');"
   //                                                                      "event.initEvent('click', true, true);"
    //                                                                     "document.getElementById(\"example_next\").dispatchEvent(event);");
    */
}

#if 0
void MainWindow::on_runJavaScript_clicked()
{
    // SEB account table
    const QString sebAccountListParser =
            "var accDiv = document.getElementById(\"IKPMaster_MainPlaceHolder_ucAccountTable_pnlAccounts\");"
            "var tables = accDiv.getElementsByTagName(\"table\");"
            "var epa = [];"
            "for(var i = 0; i < tables.length; i++) {"
            "  var tab = tables[i].tBodies[0];"
            "  for(var j = 0; j < tab.rows.length; j++) { "
            "     var href = tab.rows[j].cells[0].getElementsByTagName(\"a\")[0].href;"
            "     epa.push({Link:href, Name:tab.rows[j].cells[0].innerText.trim(), Number:tab.rows[j].cells[1].innerText.trim()});"
            "   }"
            "}"
            "epa;";

   // Norwegian transactions
    const QString norwegianTransactionParser =
            "var accDiv = document.getElementById(\"transactions\");"
            "var tables = accDiv.getElementsByTagName(\"table\");"
            "var epa = [];"
            "var bodies = tables[0].tBodies;"
            "for(var i = 0; i < bodies.length; i++) {"
            "  var trans = bodies[i];"
            "  var date = trans.rows[0].cells[0].innerText;"
            "  var name = trans.rows[0].cells[1].getElementsByTagName(\"div\")[0].innerText;"
            "  var type = trans.rows[0].cells[1].getElementsByTagName(\"div\")[1].innerText;"
            "  var sum  = trans.rows[0].cells[4].innerText;"
            "  epa.push({Name:name, Date:date, Sum:sum, Extra:type});"
            "}"
            "epa;";

   // Seb transaction
   QString sebTransactionParser =
           "var accDiv = document.getElementById(\"IKPMaster_MainPlaceHolder_ucAccountEvents_DataTable\");"
           "var tables = accDiv.getElementsByTagName(\"table\");"
           "var epa = [];"
           "var rows = tables[0].tBodies[0].rows;"
           "for(var i = 2; i < rows.length; i = i +2) {"
           "  var trans = rows[i];"
           "  var date = trans.cells[1].innerText;"
           "  var name = trans.cells[2].innerText.split('/')[0];"
           "  var sum  = trans.cells[4].innerText;"
           "  if(sum.length == 0) {"
           "    sum = trans.cells[3].innerText;"
           "  }"
           "  var extra = \"Empty\";"
           "  if(rows[i+1].cells[1].getElementsByClassName(\"key-value-list\").length > 0) {"
           "    extra = rows[i+1].cells[1].getElementsByClassName(\"key-value-list\")[0].getElementsByTagName(\"span\")[0].innerText;"
           "  }"
           "  epa.push({Name:name, Date:date, Sum:sum, Extra:extra});"
           "}"
           "epa;";

   const QString pollDomUpdated =
           "var apatimes = 0; var prevapatimes = 0;"
           "function myEventListner(ev) {"
           "  apatimes++;"
           "}"
           "%1.addEventListener(\"DOMNodeInserted\", myEventListner, false);"
           "setTimeout(function myTimeoutFunc() { console.log(\"val \" + apatimes);"
           "                                      if(apatimes >0 && prevapatimes == apatimes) {"
           "                                        console.log(\"DONE\");"
           "                                        %1.removeEventListener(\"DOMNodeInserted\", myEventListner);"
           "                                        %2;"
           "                                      }"
           "                                      else {"
           "                                        prevapatimes = apatimes;"
           "                                        setTimeout(myTimeoutFunc, 100);"
           "                                      }"
           "}, 100)";

   const QString ajaxCompleteEvent =
           "$( document ).ajaxComplete(function( event, request, settings ) { %1 });";


   const QString clickEvent =
           "var event = document.createEvent('Event');"
           "event.initEvent('click', true, true);"
           "%1.dispatchEvent(event);";
/*
   QWebFrame* frame = new QWebFrame();

    if(first) {
        frame->addToJavaScriptWindowObject("MainWindow", this);
        this->nr = 0;
        first = false;
    }

    frame->evaluateJavaScript(pollDomUpdated.arg("document.getElementsByClassName(\"ebTable ebTable_striped ebTable_borderTop_none commitListTable\")[0]", "MainWindow.epa2()"));

    frame->evaluateJavaScript(clickEvent.arg("document.getElementsByClassName(\"ebPagination-nextAnchor\")[0]"));
    */
    /*

    qDebug() << "APA " << res.typeName();
     qDebug() << "APA " << res.toDouble();
    QList<QVariant> list = res.toList();
    for(int i = 0; i < list.size(); i++)
    {
        QMap<QString, QVariant> map = list[i].toMap();
        qDebug() << "APA2 " << map["Name"].toString() << " " << map["Date"].toString() << " " << map["Sum"].toString() << " " << map["Extra"].toString();
    }
    */

}
#endif

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
