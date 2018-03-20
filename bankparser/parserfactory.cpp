#include "parserfactory.h"
#include "sebparser.h"
#include "swedbankparser.h"
#include "norwegianparser.h"
#include "handelsbankenparser.h"
#ifdef TEST_BANK
#include "testparser.h"
#endif

static SebParser *sebparser = NULL;
static NorwegianParser *norwegianparser = NULL;
static SwedbankParser *swedbankparser = NULL;
static HandelsbankenParser *handelsbankenParser = NULL;
#ifdef TEST_BANK
static TestParser *testParser1 = NULL;
static TestParser *testParser2 = NULL;
#endif

BankParser* ParserFactory::getParser(const QString& name)
{
  if(name == "SEB")
  {
    if(sebparser == NULL)
      sebparser = new SebParser();
    return sebparser;
  }
  else if(name == "NORWEGIAN")
  {
    if(norwegianparser == NULL)
      norwegianparser = new NorwegianParser();
    return norwegianparser;
  }
  else if(name == "SWEDBANK")
  {
    if(swedbankparser == NULL) {
      qDebug() << "create begin";
      swedbankparser = new SwedbankParser();
      qDebug() << "create done";
    }
    return swedbankparser;
  }
  else if(name == "HANDELSBANKEN")
  {
    if(handelsbankenParser == NULL)
      handelsbankenParser = new HandelsbankenParser();
    return handelsbankenParser;
  }
#ifdef TEST_BANK
  else if(name == "TESTBANK1")
  {
    if(testParser1 == NULL)
      testParser1 = new TestParser();
    return testParser1;
  }
  else if(name == "TESTBANK2")
  {
    if(testParser2 == NULL)
      testParser2 = new TestParser();
    return testParser2;
  }
#endif
  return NULL;
}

QPixmap ParserFactory::getPixmap(const QString& name)
{
  if(name == "SEB")
  {
      return QPixmap(":/files/logo/Seb.png");
  }
  else if(name == "NORWEGIAN")
  {
      return QPixmap(":/files/logo/Norwegian.png");
  }
  else if(name == "SWEDBANK")
  {
      return QPixmap(":/files/logo/Swedbank.png");
  }
  else if(name == "HANDELSBANKEN")
  {
      return QPixmap(":/files/logo/Handelsbanken.png");
  }
#ifdef TEST_BANK
  else if(name == "TESTBANK1")
  {
      return QPixmap(":/files/logo/Seb.png");
  }
  else if(name == "TESTBANK2")
  {
      return QPixmap(":/files/logo/Seb.png");
  }
#endif
  return QPixmap();
}

QIcon ParserFactory::getIcon(const QString& name)
{
  if(name == "SEB")
  {
      return QIcon(":/files/logo/Seb_small.png");
  }
  else if(name == "NORWEGIAN")
  {
      return QIcon(":/files/logo/Norwegian_small.png");
  }
  else if(name == "SWEDBANK")
  {
      return QIcon(":/files/logo/Swedbank_small.png");
  }
  else if(name == "HANDELSBANKEN")
  {
      return QIcon(":/files/logo/Handelsbanken_small.png");
  }
#ifdef TEST_BANK
  else if(name == "TESTBANK1")
  {
      return QIcon(":/files/logo/Seb_small.png");
  }
  else if(name == "TESTBANK2")
  {
      return QIcon(":/files/logo/Seb_small.png");
  }
#endif
  return QIcon();
}

QStringList ParserFactory::supportedParsers()
{
  QStringList list;
  list << "SEB" << "NORWEGIAN" << "SWEDBANK" << "HANDELSBANKEN";
#ifdef TEST_BANK
  list << "TESTBANK1" << "TESTBANK2";
#endif
  return list;
}
