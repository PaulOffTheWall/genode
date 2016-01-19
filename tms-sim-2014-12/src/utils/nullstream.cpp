/**
 * $Id: nullstream.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file nullstream.cpp
 * @brief Stream with no output
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <nullstream.h>

#include <mutex>

using namespace std;

namespace tmssim {

  int NullStreambuf::overflow(int c) {
    static std::mutex nsbmtx;
    std::lock_guard<std::mutex> lck(nsbmtx);
    setp(dummyBuffer, dummyBuffer + sizeof(dummyBuffer));
    return (c == traits_type::eof()) ? '\0' : c;
  }

  static NullStreambuf nsb;

  ostream nullStream(&nsb);
  

} // NS tmssim
