////////////////////////////////////////////////////////////////////////////////
// FILE: SourceLogger.h
// DATE: 2018-01-16
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines a SourceLogger class designed to handle logging messages from a
//       single source

#ifndef WILT_SOURCELOGGER_H
#define WILT_SOURCELOGGER_H

#include <string>

#include "LoggingLevel.h"

namespace wilt
{
  class LoggingManager;

  class SourceLogger
  {
  public:
    SourceLogger(LoggingManager* manager, const char* source);

    SourceLogger(const SourceLogger&) = default;
    SourceLogger(SourceLogger&&) = default;
    SourceLogger& operator=(const SourceLogger&) = default;
    SourceLogger& operator=(SourceLogger&&) = default;

    ~SourceLogger() {}

  public:
    // logging functions
    void fatal(const std::string& message) { log(LoggingLevel::FATAL, message); }
    void error(const std::string& message) { log(LoggingLevel::ERR0R, message); }
    void warn (const std::string& message) { log(LoggingLevel::WARN,  message); }
    void info (const std::string& message) { log(LoggingLevel::INFO,  message); }
    void debug(const std::string& message) { log(LoggingLevel::DEBUG, message); }
    void trace(const std::string& message) { log(LoggingLevel::TRACE, message); }

  private:
    void log(LoggingLevel level, const std::string& message);

  private:
    LoggingManager* _manager;
    const char* _source;

  }; // class SourceLogger

} // namespace wilt

#endif // !WILT_SOURCELOGGER_H
