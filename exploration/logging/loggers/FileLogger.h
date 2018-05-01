////////////////////////////////////////////////////////////////////////////////
// FILE: FileLogger.h
// DATE: 2018-01-16
// AUTH: Trevor Wilson <kmdreko@gmail.com>
// DESC: defines a FileLogger class that creates an ILogger that writes 
//       messages to an std::ifstream

#ifndef WILT_FILELOGGER_H
#define WILT_FILELOGGER_H

#include <fstream>
#include <string>

#include "../ILogger.h"
#include "./StreamLogger.h"

namespace wilt
{
  class FileLogger : public ILogger
  {
  public:
    FileLogger(const std::string& path);

    FileLogger(const FileLogger&) = delete;
    FileLogger(FileLogger&&) = delete;
    FileLogger& operator=(const FileLogger&) = delete;
    FileLogger& operator=(FileLogger&&) = delete;

    ~FileLogger();

  public:
    // ILogger functions
    bool log(LoggingLevel level, const char* source, const char* message, std::uint64_t ns);

  private:
    std::ofstream _stream;
    StreamLogger _streamLogger;

  }; // class FileLogger

} // namespace wilt

#endif // !WILT_FILELOGGER_H
