////////////////////////////////////////////////////////////////////////////////
// FILE: LoggingManager.h
// DATE: 2018-01-14
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines a LoggingLevel enum for the severity of logging messages 

#ifndef WILT_LOGGINGLEVEL_H
#define WILT_LOGGINGLEVEL_H

#include <cstdint>

namespace wilt
{
  enum class LoggingLevel : std::int32_t
  {
    NONE,  // only used to set logging level
    FATAL, // the program cannot continue safely
    ERR0R, // something is wrong and needs to be addressed
    WARN,  // something could be wrong
    INFO,  // information for general use
    DEBUG, // information useful for debugging
    TRACE  // information for detailed program execution

  }; // enum LoggingLevel

} // namespace wilt

#endif // !WILT_LOGGINGLEVEL_H
