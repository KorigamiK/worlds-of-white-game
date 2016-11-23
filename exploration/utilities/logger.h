
#ifndef WILT_LOGGER_H
#define WILT_LOGGER_H

#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cassert>

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
      }; // enum level

    private:
      std::ostream* os;
      Level min;

      Logger(Level min, std::ostream* os) : os{ os }, min{ min } { }

      void log_(Level level, const char* component, const char* message)
      {
        if (!os || level < min)
          return;

        static char levels[] = "DIWEF";

        std::time_t time = std::time(nullptr);
        std::tm tm;
        gmtime_s(&tm, &time);
        *os << std::put_time(&tm, "%FT%TZ") << " [" << levels[level] << "] " << component << ": " << message << '\n';
      }

    private:
      static Logger* instance;

    public:
      static void init(Level level = Level::INFO, std::ostream* o = nullptr)
      {
        static Logger logger(level, o);
        instance = &logger;
      }

      static void log(Level level, const char* component, const char* message)
      {
        assert(instance);

        instance->log_(level, component, message);
      }

      static void debug(const char* component, const char* message) { log(Level::DEBUG, component, message); }
      static void info (const char* component, const char* message) { log(Level::INFO,  component, message); }
      static void warn (const char* component, const char* message) { log(Level::WARN,  component, message); }
      static void error(const char* component, const char* message) { log(Level::ERROR, component, message); }
      static void fatal(const char* component, const char* message) { log(Level::FATAL, component, message); }

    }; // class Logger
  } // namespace common
} // namespace wilt

wilt::Logger* wilt::Logger::instance = nullptr;

#endif // !WILT_LOGGER_H