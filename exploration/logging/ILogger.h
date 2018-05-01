////////////////////////////////////////////////////////////////////////////////
// FILE: LoggingManager.h
// DATE: 2018-01-14
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines a ILogger class that declares the interfaces for a logger

#ifndef WILT_ILOGGER_H
#define WILT_ILOGGER_H

#include <cstdint>

#include "LoggingLevel.h"

namespace wilt
{
  class ILogger
  {
  public:
    virtual ~ILogger() {};

    // @brief  called with a message to log along with relevant context
    // @param  level - the severity of the message
    // @param  source - where the message originated
    // @param  message - the message to log
    // @param  ns - a nanosecond timestamp of the occurrence
    // @return message was logged successfully
    //
    // @note   shall be called sequentially with monotonically increasing 
    //         nanoseconds
    // @note   only one call shall be executed at a time
    virtual bool log(LoggingLevel level, const char* source, const char* message, std::uint64_t ns) = 0;

  }; // class ILogger

} // namespace wilt

#endif // !WILT_ILOGGER_H
