#include "logger.h"

#include <iomanip>
#include <ctime>

using wilt::Logger;

std::ostream* Logger::stream_ = nullptr;
Logger::Level Logger::level_ = Logger::Level::WARN;

void Logger::process()
{
  static char levels[] = "DIWEF";

  for (const auto& msg : queue_)
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
  {
    queue_.push_back({std::time(nullptr), level, component, message});
  }
}
