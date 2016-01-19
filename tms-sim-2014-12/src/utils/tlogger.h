/**
 * $Id: tlogger.h 701 2014-09-18 16:23:03Z klugeflo $
 * @file tlogger.h
 * @brief Stream-based logging
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_TLOGGER_H
#define UTILS_TLOGGER_H 1

#include <mutex>
#include <iostream>
#include <cassert>
#include <sstream>
#include <ctime>
#include <ostream>

namespace tmssim {

#if 0
  // was a nice idea, but doesn't work well with multithreading
  /**
   * we need a null stream, therefore we use the implementation by James Kanze,
   * see http://stackoverflow.com/questions/8243743/is-there-a-null-stdostream-implementation-in-c-or-libraries
   */
  class _NullStreambuf : public std::streambuf {
    char dummyBuffer[ 64 ];
  protected:
    virtual int overflow(int c) {
      setp(dummyBuffer, dummyBuffer + sizeof(dummyBuffer));
      return (c == traits_type::eof()) ? '\0' : c;
    }
  };
  
  /*
    // fails to compile in rdbuf, might stem from private inheritance
  class _NullOStream : private _NullStreambuf, public std::ostream {
  public:
  _NullOStream() : std::ostream(this) {}
    _NullStreambuf* rdbuf() const { return this; }
  };
  */

  /**
   * Logger class that provides std::ostreams
   */
  class TLogger {
  public:
    /**
     * Log levels
     */
    enum LogLevel {
      NONE = 0, ERROR, WARN, INFO, DEBUG
    };

    static void init(TLogger::LogLevel _level);
    static void setLogLevel(TLogger::LogLevel _level);
    static void clear();

    static std::ostream& error();
    static std::ostream& warn();
    static std::ostream& info();
    static std::ostream& debug();

  private:
    static TLogger* getInstance();

    static TLogger* instance;

    TLogger();
    ~TLogger();
    void _setLogLevel(TLogger::LogLevel _level);

    _NullStreambuf nsb;
    std::ostream nullStream;
    LogLevel logLevel;

    std::ostream* sError;
    std::ostream* sWarn;
    std::ostream* sInfo;
    std::ostream* sDebug;

  };

#define tError (TLogger::error())
#define tWarn (TLogger::warn())
#define tInfo (TLogger::info())
#define tDebug (TLogger::debug())

#endif // 0


  /*
    based on http://stackoverflow.com/a/4455454
  */

  namespace detail {
    template<class Ch, class Tr, class A>
      class no_output {
    private:
      struct null_buffer {
	template<class T>
	null_buffer &operator<<(const T &) {
	  return *this;
	}
      };
    public:
      typedef null_buffer stream_buffer;
      
    public:
      void operator()(const stream_buffer &) {
      }
    };
    
    template<class Ch, class Tr, class A>
      class output_to_clog {
    public:
      typedef std::basic_ostringstream<Ch, Tr, A> stream_buffer;
    public:
      void operator()(const stream_buffer &s) {
	static std::mutex mtx;
	std::lock_guard<std::mutex> lck(mtx);
	//std::clog << now() << ": " << s.str() << std::endl;
	std::clog << s.str() << std::endl;
      }
      
    private:
      static std::string now() {
	char buf[64];
	const time_t tm = time(0);  
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tm));
	return buf;
      }
    };
    
    template<template <class Ch, class Tr, class A> class OutputPolicy, class Ch = char, class Tr = std::char_traits<Ch>, class A = std::allocator<Ch> >
      class tlogger {
      typedef OutputPolicy<Ch, Tr, A> output_policy;
    public:
      ~tlogger() {
	output_policy()(m_SS);
      }
    public:
      template<class T>
      tlogger &operator<<(const T &x) {
	m_SS << x;
	return *this;
      }
    private:
      typename output_policy::stream_buffer m_SS;
    };
  } // NS detail


#define TLL_NONE  0
#define TLL_ERROR 1
#define TLL_WARN  2
#define TLL_INFO  3
#define TLL_DEBUG 4

#ifndef TLOGLEVEL
#define TLOGLEVEL TLL_WARN
#endif


#if TLOGLEVEL >= TLL_ERROR
  class tError : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tError : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_ERROR

#if TLOGLEVEL >= TLL_WARN
  class tWarn : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tWarn : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_WARN

#if TLOGLEVEL >= TLL_INFO
  class tInfo : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tInfo : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_INFO

#if TLOGLEVEL >= TLL_DEBUG
  class tDebug : public detail::tlogger<detail::output_to_clog> {
  };
#else
  class tDebug : public detail::tlogger<detail::no_output> {
  };
#endif // >= TLL_DEBUG
  /*
#define tError TError()
#define tWarn TWarn()
#define tInfo TInfo()
#define tDebug TDebug()
  */
  /*
#define tError cout
#define tWarn cout
#define tInfo cout
#define tDebug cout
  */


} // NS tmssim

#endif // !UTILS_TLOGGER_H
