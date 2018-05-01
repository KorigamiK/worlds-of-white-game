#include "FileLogger.h"
using namespace wilt;

FileLogger::FileLogger(const std::string& path)
  : _stream{ path }
  , _streamLogger{ _stream }
{ }

FileLogger::~FileLogger()
{

}

bool FileLogger::log(LoggingLevel level, const char* source, const char* message, std::uint64_t ns)
{
  return _streamLogger.log(level, source, message, ns);
}
