
#ifndef WILT_LOGGER_H
#define WILT_LOGGER_H

#include <ctime>
#include <iostream>
#include <vector>

#include "ring.h"

namespace wilt
{
  inline namespace common
  {
    class Logger
    {
    public:
      enum Level
      {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
      };

    private:
      struct Message
      {
        std::time_t time;
        Level level;
        const char* component;
        const char* message;
      };

    private:
      static std::ostream* stream_;
      static Level level_;
      static wilt::Ring<Message> queue_;

      Logger() = delete;

    public:
      // STATIC FUNCTIONS

      static void setLevel(Level level) { level_ = level; }
      static void setStream(std::ostream& stream) { stream_ = &stream; }

      static void process();
      static void log(Level level, const char* component, const char* message);

      static void debug(const char* component, const char* message) { log(Level::DEBUG, component, message); }
      static void info (const char* component, const char* message) { log(Level::INFO,  component, message); }
      static void warn (const char* component, const char* message) { log(Level::WARN,  component, message); }
      static void error(const char* component, const char* message) { log(Level::ERROR, component, message); }
      static void fatal(const char* component, const char* message) { log(Level::FATAL, component, message); }

    }; // class Logger
  } // namespace common
} // namespace wilt

#endif // !WILT_LOGGER_H