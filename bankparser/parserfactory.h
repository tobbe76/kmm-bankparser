#include "bankparser.h"
#include "sebparser.h"
#include <QString>

class ParserFactory
{
 public:
  static BankParser* getParser(const QString &name);

  static QStringList supportedParsers();
};
