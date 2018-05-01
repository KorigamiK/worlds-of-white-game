#include <ctime>
#include <iomanip>

#include "StreamLogger.h"
using namespace wilt;

StreamLogger::StreamLogger(std::ostream& stream)
  : _stream{ stream } 
{ }

StreamLogger::~StreamLogger()
{

}

inline bool StreamLogger::log(LoggingLevel level, const char* source, const char* message, std::uint64_t ns)
{
  auto l = "?FEWIDT"[(int)level];

  std::time_t s = ns / 1000000000;
  int ms = (ns % 1000000000) / 1000000;
  std::tm time;
  gmtime_s(&time, &s);

  // YYYY-MM-DDTHH:MM:SS [?] Source: Message
  auto fill = _stream.fill();
  _stream
    << std::put_time(&time, "%Y-%m-%dT%H:%M:%S.") << std::setw(3) << std::setfill('0') << ms
    << "Z [" << l << "] "
    << source << ": "
    << message << '\n';
  _stream.fill(fill);

  return _stream.good();
}
