#include <chrono>
#include <cassert>

#include "LoggingManager.h"
using namespace wilt;

LoggingManager::LoggingManager()
  : level{ LoggingLevel::INFO }
{ }

LoggingManager::~LoggingManager()
{

}

void LoggingManager::setLevel(LoggingLevel level)
{
  this->level = level;
}

void LoggingManager::log(LoggingLevel level, const char* source, const std::string& message)
{
  assert(level != LoggingLevel::NONE);
  assert(source != nullptr);

  if (!logger || level > this->level)
    return;

  using namespace std::chrono;
  auto time = high_resolution_clock::now().time_since_epoch();
  auto ns = duration_cast<nanoseconds>(time).count();

  logger->log(level, source, message.c_str(), ns);
}

SourceLogger LoggingManager::createLogger(const char* source)
{
  return SourceLogger(this, source);
}

LoggingManager wilt::logging;
