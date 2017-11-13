#ifndef KMYMONEYPLUGIN_H
#define KMYMONEYPLUGIN_H

#include <QDate>

#define MyMoneyMoney QString

class MyMoneyStatement
{
public:

  enum EType { etNone = 0, etCheckings, etSavings, etInvestment, etCreditCard, etEnd };

  class Split
  {
  public:
    QString      m_strCategoryName;
    QString      m_strMemo;
    QString      m_accountId;
 //   MyMoneySplit::reconcileFlagE m_reconcile;
 //   MyMoneyMoney m_amount;

  };

  class Transaction
  {
  public:
    QDate m_datePosted;
    QString m_strPayee;
    QString m_strMemo;
    QString m_strNumber;
    QString m_strBankID;
    QString m_amount;
 //   MyMoneySplit::reconcileFlagE m_reconcile;

    // the following members are only used for investment accounts (m_eType==etInvestment)
    // eaNone means the action, shares, and security can be ignored.
    enum EAction { eaNone = 0, eaBuy, eaSell, eaReinvestDividend, eaCashDividend, eaShrsin, eaShrsout, eaStksplit, eaFees, eaInterest, eaEnd };
    EAction m_eAction;
  //  MyMoneyMoney m_shares;
  //  MyMoneyMoney m_fees;
  //  MyMoneyMoney m_price;
    QString m_strInterestCategory;
    QString m_strBrokerageAccount;
    QString m_strSymbol;
    QString m_strSecurity;
    QList<Split> m_listSplits;
  };

  struct Price {
    QDate m_date;
    QString m_strSecurity;
    QString m_strCurrency;
//    MyMoneyMoney m_amount;
  };

  struct Security {
    QString m_strName;
    QString m_strSymbol;
    QString m_strId;
  };

  QString m_strAccountName;
  QString m_strAccountNumber;
  QString m_strRoutingNumber;

  /**
   * The statement provider's information for the statement reader how to find the
   * account. The provider usually leaves some value with a key unique to the provider in the KVP of the
   * MyMoneyAccount object when setting up the connection or at a later point in time.
   * Using the KMyMoneyPlugin::KMMStatementInterface::account() method it can retrieve the
   * MyMoneyAccount object for this key. The account id of that account should be returned
   * here. If no id is available, leave it empty.
   */
  QString m_accountId;

  QString m_strCurrency;
  QDate m_dateBegin;
  QDate m_dateEnd;
  MyMoneyMoney m_closingBalance;
  EType m_eType;

  QList<Transaction> m_listTransactions;
  QList<Price> m_listPrices;
  QList<Security> m_listSecurities;

  bool m_skipCategoryMatching;


};

#endif // KMYMONEYPLUGIN_H
