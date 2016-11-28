#include "logger.h"

#include <iomanip>

using wilt::Logger;

std::ostream* Logger::stream_ = nullptr;
Logger::Level Logger::level_ = Logger::Level::WARN;
wilt::Ring<Logger::Message> Logger::queue_ = wilt::Ring<Logger::Message>(512);

void Logger::process()
{
  static char levels[] = "DIWEF";

  Logger::Message msg;
  while (queue_.try_read(msg))
  {
    std::tm tm;
    gmtime_s(&tm, &msg.time);

    // YYYY-MM-DD HH:MM:SS [LEVEL] component: message
    *stream_ << std::put_time(&tm, "%F %T");
    *stream_ << " [" << levels[msg.level] << "] ";
    *stream_ << msg.component << ": ";
    *stream_ << msg.message << '\n';
  }
}

void Logger::log(Logger::Level level, const char* component, const char* message)
{
  if (level >= level_ && stream_ != nullptr)
    queue_.write({std::time(nullptr), level, component, message});
}
