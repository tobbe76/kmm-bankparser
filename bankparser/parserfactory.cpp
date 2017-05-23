#include "parserfactory.h"
#include "sebparser.h"
#include "swedbankparser.h"
#include "norwegianparser.h"
#include "handelsbankenparser.h"

static SebParser *sebparser = NULL;
static NorwegianParser *norwegianparser = NULL;
static SwedbankParser *swedbankparser = NULL;
static HandelsbankenParser *handelsbankenParser = NULL;

BankParser* ParserFactory::getParser(QString name)
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
    if(swedbankparser == NULL)
      swedbankparser = new SwedbankParser();
    return swedbankparser;
  }
  else if(name == "HANDELSBANKEN")
  {
    if(handelsbankenParser == NULL)
      handelsbankenParser = new HandelsbankenParser();
    return handelsbankenParser;
  }
  return NULL;
}

QStringList ParserFactory::supportedParsers()
{
  QStringList list;
  list << "SEB" << "NORWEGIAN" << "SWEDBANK" << "HANDELSBANKEN";
  return list;
}
