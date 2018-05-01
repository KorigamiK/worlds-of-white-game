////////////////////////////////////////////////////////////////////////////////
// FILE: LoggingManager.h
// DATE: 2018-01-14
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines a LoggingManager class designed to handle logging messages 

#ifndef WILT_LOGGINGMANAGER_H
#define WILT_LOGGINGMANAGER_H

#include <memory>
#include <utility>
#include <string>

#include "LoggingLevel.h"
#include "ILogger.h"
#include "SourceLogger.h"

namespace wilt
{
  class LoggingManager
  {
  public:
    LoggingManager();

    LoggingManager(const LoggingManager&) = delete;
    LoggingManager(LoggingManager&&) = delete;
    LoggingManager& operator=(const LoggingManager&) = delete;
    LoggingManager& operator=(LoggingManager&&) = delete;

    ~LoggingManager();

  public:
    void setLevel(LoggingLevel level);

    template <class Logger, class... Args>
    void setLogger(Args&&... args)
    {
      logger = std::make_unique<Logger>(std::forward<Args>(args)...);
    }

  public:
    void log(LoggingLevel level, const char* source, const std::string& message);

  public:
    SourceLogger createLogger(const char* source);

  private:
    std::unique_ptr<ILogger> logger;
    LoggingLevel level;

  }; // class LoggingManager

  extern LoggingManager logging;

} // namespace wilt

#endif // !WILT_LOGGINGMANAGER_H
