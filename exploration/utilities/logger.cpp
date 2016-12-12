#include "logger.h"

#include <iomanip>

using wilt::Logger;

Logger* Logger::instance = nullptr;

void Logger::process()
{
  static char levels[] = "DIWEF";

  Logger::Message msg;
  while (queue_.try_read(msg))
  {
    if (msg.level < level_ || stream_ == nullptr)
      continue;

    std::tm tm;
    gmtime_s(&tm, &msg.time);

    // YYYY-MM-DD HH:MM:SS [LEVEL] component: message
    *stream_ << std::put_time(&tm, "%F %T");
    *stream_ << " [" << levels[(int)msg.level] << "] ";
    *stream_ << msg.component << ": ";
    *stream_ << msg.message << '\n';
  }
}

bool Logger::log(Logger::Level level, const char* component, const char* message)
{
  return queue_.try_write({std::time(nullptr), level, component, message});
}
