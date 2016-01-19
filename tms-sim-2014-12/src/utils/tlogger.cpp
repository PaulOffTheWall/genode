/**
 * $Id: tlogger.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file tlogger.cpp
 * @brief Stream-based logging
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

//#include <utils/tlogger.h>

#include <cstdlib>

using namespace std;

namespace tmssim {

#if 0

  TLogger* TLogger::instance = NULL;


  void TLogger::init(TLogger::LogLevel _level) {
    if (instance != NULL) {
      cerr << "TLogger already initialised!" << endl;
      abort();
    }
    instance = new TLogger();
    instance->_setLogLevel(_level);
  }


  void TLogger::setLogLevel(TLogger::LogLevel _level) {
    getInstance()->_setLogLevel(_level);
  }


  void TLogger::clear() {
    if (TLogger::instance != NULL) {
      delete TLogger::instance;
      instance = NULL;
    }
  }


  ostream& TLogger::error() {
    return *getInstance()->sError;
  }


  ostream& TLogger::warn() {
    return *getInstance()->sWarn;
  }


  ostream& TLogger::info() {
    return *getInstance()->sInfo;
  }


  ostream& TLogger::debug() {
    return *getInstance()->sDebug;
  }
  

  TLogger* TLogger::getInstance() {
    if (TLogger::instance == NULL) {
      cerr << "First call TLogger::init!" << endl;
      abort();
    }
    return TLogger::instance;
  }


  TLogger::TLogger()
    : nullStream(&nsb), logLevel(TLogger::NONE),
      sError(&nullStream), sWarn(&nullStream),
      sInfo(&nullStream), sDebug(&nullStream)
  {
  }


  TLogger::~TLogger() {
  }


  void TLogger::_setLogLevel(TLogger::LogLevel _level) {

    if (_level < NONE || _level > DEBUG) {
      cerr << "Invalid loglevel: " << _level << "!" << endl;
      abort();
    }

    logLevel = _level;

    switch (logLevel) { 
    case DEBUG:
      sDebug = &cout;
    case INFO:
      sInfo = &cout;
    case WARN:
      sWarn = &cout;
    case ERROR:
      sError = &cout;
    case NONE:
      (0);
    }

    switch (logLevel) {
    case NONE:
      sError = &nullStream;
    case ERROR:
      sWarn = &nullStream;
    case WARN:
      sInfo = &nullStream;
    case INFO:
      sDebug = &nullStream;
    case DEBUG:
      (0);
    }

  }

#endif // 0

} // NS tmssim
