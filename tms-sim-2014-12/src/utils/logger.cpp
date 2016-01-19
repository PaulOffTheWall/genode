/**
 * $Id: logger.cpp 694 2014-09-15 15:22:39Z klugeflo $
 * @file logger.cpp
 * @brief Simple logging facility
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utils/logger.h>
#include <utils/nullstream.h>

#include <iostream>
#include <cstdlib>

using namespace std;

int tmssim::detail::llogger::globalLevel = 1;

namespace tmssim {

#if 0
  Logger::Logger(int _logLevel, std::ostream& _os)
      : logLevel(_logLevel), os(_os) {
    }


  std::ostream& Logger::getStream(int _logLevel) {
    if (_logLevel <= logLevel)
      return os;
    else
      return nullStream;
  }


  int Logger::getLogLevel() const {
    return logLevel;
  }


  void Logger::setLogLevel(int _logLevel) {
    logLevel = _logLevel;
  }


  Logger noLog(-1, nullStream);
  Logger* noLogPtr = &noLog;
#endif
#if 0

  /**
   * Initialize Singleton
   */
  Logger* Logger::_instance = 0;
  
  Logger::Logger() {
    this->_currentLogLevel = DEBUG;
  }
  
  Logger::Logger(const Logger::LogLevel level) {
    this->_currentLogLevel = level;
  }
  
  Logger::Logger(const Logger& otherLogger) {
    this->_currentLogLevel = otherLogger._currentLogLevel;
  }
  
  void Logger::setLogLevel(LogLevel level) {
    Logger::getInstance()->_currentLogLevel = level;
  }
  
  Logger::~Logger() {
    
  }
  
  Logger* Logger::getInstance() {
    if (_instance == NULL) {
      _instance = new Logger();
    }
    return _instance;
  }
  
  void Logger::clear() {
    if(_instance != NULL) {
      delete _instance;
    }
  }
  
  void Logger::log(const string& message, LogLevel level) {
    if (_currentLogLevel <= level) {
      if (level == Logger::ERROR) {
	cerr << message << endl;
      } else {
	cout << message << endl;
      }
    }
  }
  
  void Logger::debug(const string& message) {
    Logger::getInstance()->log(message, Logger::DEBUG);
  }
  
  void Logger::error(const string& message) {
	Logger::getInstance()->log(message, Logger::ERROR);
  }
  
  void Logger::info(const string& message) {
    Logger::getInstance()->log(message, Logger::INFO);
  }
  
  void Logger::fatal(const string& message) {
    Logger::getInstance()->log(message, Logger::ERROR);
    exit(1);
  }

#endif // 0



} // NS tmssim
