
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
      std::ostream* stream_;
      Level level_;
      wilt::Ring<Message> queue_;

    public:
      Logger(std::ostream* stream = nullptr, Level level = Level::WARN)
        : stream_(stream)
        , level_(level)
        , queue_(512)
      { }

      void setLevel(Level level) { level_ = level; }
      void setStream(std::ostream& stream) { stream_ = &stream; }

      void process();
      bool log(Level level, const char* component, const char* message);

    public:
      // SINGLETON INSTANCE
      static Logger instance;

      // STATIC FUNCTIONS
      static void debug(const char* component, const char* message) { instance.log(Level::DEBUG, component, message); }
      static void info (const char* component, const char* message) { instance.log(Level::INFO,  component, message); }
      static void warn (const char* component, const char* message) { instance.log(Level::WARN,  component, message); }
      static void error(const char* component, const char* message) { instance.log(Level::ERROR, component, message); }
      static void fatal(const char* component, const char* message) { instance.log(Level::FATAL, component, message); }

    }; // class Logger
  } // namespace common
} // namespace wilt

#endif // !WILT_LOGGER_H