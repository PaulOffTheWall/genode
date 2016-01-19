/**
 * $Id: nullstream.h 693 2014-09-14 20:32:48Z klugeflo $
 * @file nullstream.h
 * @brief Stream with no output
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef UTILS_NULLSTREAM_H
#define UTILS_NULLSTREAM_H 1

#include <iostream>
#include <mutex>

namespace tmssim {

  /**
   * we need a null stream, therefore we use the implementation by James Kanze,
   * see http://stackoverflow.com/questions/8243743/is-there-a-null-stdostream-implementation-in-c-or-libraries
   */
  class NullStreambuf : public std::streambuf {
  protected:
    virtual int overflow(int c);
  private:
    char dummyBuffer[ 64 ];
  };

  extern std::ostream nullStream;
  
  /*
  class _NullOStream : private NullStreambuf, public std::ostream {
  public:
    _NullOStream() : std::ostream(this) {}
    NullStreambuf* rdbuf() const { return this; }
  };
  */


} // NS tmssim

#endif // !UTILS_NULLSTREAM_H
