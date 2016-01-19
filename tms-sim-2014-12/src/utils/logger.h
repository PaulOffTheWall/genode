/**
 * $Id: logger.h 703 2014-09-19 12:42:22Z klugeflo $
 * @file logger.h
 * @brief Simple logging facility
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <utils/nullstream.h>

#include <string>
#include <iostream>

#include <mutex>
#include <cassert>
#include <sstream>
#include <ctime>
#include <ostream>

namespace tmssim {

#if 0
  // is not thread safe

  /**
   * @brief Logging of execution traces.
   * Use the #LOG macro for easy access of the log stream.
   */
  class Logger {
  public:
    /**
     * @brief C'tor.
     *
     * @param _logLevel loglevel
     * @param _os output stream to write to
     */
    Logger(int _logLevel = 0, std::ostream& _os = std::cout);


    /**
     * @brief Get a stream to log into
     *
     * Output is only written to the object's output stream if the object's
     * loglevel is greater or each to paramater.
     * @param _logLevel level of the output - output is only written if this
     * value is less or equal to the object's loglevel
     */
    std::ostream& getStream(int _logLevel);


    /**
     * @brief get the object's loglevel
     * @return loglevel
     */
    int getLogLevel() const;
    /**
     * @brief set the object's loglevel
     * @param _logLevel
     */
    void setLogLevel(int _logLevel);
  private:
    int logLevel;
    std::ostream& os;
  };


  extern Logger noLogger;
  extern Logger* noLogPtr;


  /**
   * Use this macro to access a #Logger object's stream.
   * @param loggerptr pointer to a Logger object
   * @param loglevel loglevel of the output
   * @return an ostream object
   */
#define LOG(loggerptr, loglevel) loggerptr->getStream(loglevel)

#endif // 0

#if 0
  // don't like this anymore
  /**
   * Utility Class to support logging with different levels
   */
  class Logger {
  public:
    /**
     * Describe the severity of a message that should be logged by the log-function
     */
    enum LogLevel {
      DEBUG = 0, INFO = 1, ERROR = 2, FATAL = 3, NONE = 4
    };
    
  private:
    
    /**
     * Singleton instance
     */
    static Logger* _instance;
    
    /**
     * The current log level
     */
    LogLevel _currentLogLevel;
    
    /**
     * Private Methods
     */
  private:
    /**
     * Creates a new instance of a the Logger
     */
    Logger();
    
    /**
     * Creates a customized logger with the given level
     * @param level The level to log
     */
    Logger(const Logger::LogLevel level);
    
    /**
     * Copy-Constructor.
     * @param otherotherLogger
     */
    Logger(const Logger& otherLogger);
    
  public:
    
    /**
     * Destroys the instance and frees the memory. Is called in the clear()-method.
     */
    ~Logger();
    
    /**
     * Gets an instance of the class. If there is already one, you will get that.
     * @return an instance of the class.
     */
    static Logger* getInstance();
    
    /**
     * Releases all the allocated memory of the singleton object.
     */
    void clear();
    
    /**
     * Logs the given message to the proper output stream, if the log level is supported
     * @param message The message to log
     * @param level The level(criticality) of the message
     */
    void log(const std::string& message, LogLevel level);
    
    /**
     * Logs the given message with debug prioriry/level
     * @param message The message to log
     */
    static void debug(const std::string& message);
    
    /**
     * Logs the given message with info prioriry/level
     * @param message The message to log
     */
    static void info(const std::string& message);
    
    /**
     * Logs the given message with error prioriry/level
     * @param message The message to log
     */
    static void error(const std::string& message);
    
    /**
     * Logs the given message with fatal prioriry/level
     * The program is aborted with  exit-code 1 after logging the message
     * @param message The message to log
     */
    static void fatal(const std::string& message);
    
    /**
     * Sets the loglevel to log.
     * NONE: nothing will be logged
     * FATAL: Log only Fatals
     * ERROR: Log only Fatals and Errors
     * INFO: Log only Infos, Errors and Fatals
     * DEBUG: Log Infos, Errors, Fatals and Debug-Messages
     * @param level The level to log
     */
    static void setLogLevel(LogLevel level);
  };

#endif // 0


  namespace detail {
    //template<class Ch, class Tr, class A>
    class output_policy {
    public:
      virtual void operator()(const std::/*basic_*/ostringstream/*<Ch, Tr, A>*/ &s) = 0;
    };
    
    //template<class Ch, class Tr, class A>
    class lno_output : public output_policy/*<Ch, Tr, A>*/ {
    private:
      struct null_buffer : public std::/*basic_*/ostringstream/*<Ch, Tr, A>*/ {
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
    
    //template<class Ch, class Tr, class A>
    class loutput_to_clog : public output_policy/*<Ch, Tr, A>*/ {
    public:
      typedef std::/*basic_*/ostringstream/*<Ch, Tr, A>*/ stream_buffer;
    public:
      void operator()(const stream_buffer &s) {
	static std::mutex mtx;
	std::lock_guard<std::mutex> lck(mtx);
	std::clog << s.str() << std::endl;
      }
      
    private:
      /*
      static std::string now() {
	char buf[64];
	const time_t tm = time(0);  
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&tm));
	return buf;
      }
      */
    };
    
    //template<class Ch = char, class Tr = std::char_traits<Ch>, class A = std::allocator<Ch> >
    class llogger {
      //typedef OutputPolicy<Ch, Tr, A> output_policy;
    public:
      llogger(int level) {
	myLevel = level;
	if (myLevel <= globalLevel) {
	  buffer = new loutput_to_clog::stream_buffer;
	}
	else {
	  buffer = new lno_output::stream_buffer;
	}
      }

      static void setLevel(int level) {
	globalLevel = level;
	//std::cout << "Global log level @ " << level << std::endl;
      }

      ~llogger() {
	if (myLevel <= llogger::globalLevel) {
	  loutput_to_clog/*<Ch, Tr, A>*/()(*buffer);
	}
	delete buffer;
      }

    public:
      template<class T>
	llogger &operator<<(const T &x) {
	*buffer << x;
	return *this;
      }
    private:
      static int globalLevel;
      int myLevel;
      std::/*basic_*/ostringstream/*<Ch, Tr, A>*/* buffer;
      //typename output_policy::stream_buffer m_SS;
    };
  } // NS detail

  typedef detail::llogger/*<char, std::char_traits<char>, std::allocator<char> >*/ logger; 
#define LOG(level) logger(level)
  
} // NS tmssim

#endif /* !UTILS_LOGGER_H */
