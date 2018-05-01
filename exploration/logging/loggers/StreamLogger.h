////////////////////////////////////////////////////////////////////////////////
// FILE: StreamLogger.h
// DATE: 2018-01-16
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines a StreamLogger class that creates an ILogger that writes 
//       messages to an std::ostream

#ifndef WILT_STREAMLOGGER_H
#define WILT_STREAMLOGGER_H

#include <iostream>

#include "../ILogger.h"

namespace wilt
{
  class StreamLogger : public ILogger
  {
  public:
    StreamLogger(std::ostream& stream);

    StreamLogger(const StreamLogger&) = delete;
    StreamLogger(StreamLogger&&) = delete;
    StreamLogger& operator=(const StreamLogger&) = delete;
    StreamLogger& operator=(StreamLogger&&) = delete;

    ~StreamLogger();

  public:
    // ILogger functions
    bool log(LoggingLevel level, const char* source, const char* message, std::uint64_t ns);

  private:
    std::ostream& _stream;

  }; // class StreamLogger

} // namespace wilt

#endif // !WILT_STREAMLOGGER_H
