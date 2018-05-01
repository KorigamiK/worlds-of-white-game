#include "SourceLogger.h"
#include "LoggingManager.h"
using namespace wilt;

SourceLogger::SourceLogger(LoggingManager* manager, const char* source)
  : _manager{ manager }
  , _source{ source }
{ }

void SourceLogger::log(LoggingLevel level, const std::string& message)
{
  _manager->log(level, _source, message);
}
