/**
 * $Id: mkmonitor.cpp 691 2014-09-14 10:35:58Z klugeflo $
 * @file mkmonitor.cpp
 * @brief Monitoring of (m,k)-firm condition
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mkmonitor.h>

#include <cassert>

namespace tmssim {

  MKMonitor::MKMonitor(unsigned int _m, unsigned int _k, unsigned int _init)
    : m(_m), k(_k), pos(0), violations(0) {
    vals = new unsigned int[k];
    sum = 0;
    for (int i = 0; i < k; ++i) {
      vals[i] = _init;
      sum += _init;
    }
  }


  MKMonitor::~MKMonitor() {
    delete[] vals;
  }


  void MKMonitor::push(unsigned int _val) {
    unsigned int tmp = vals[pos];
    vals[pos] = _val;
    sum -= tmp;
    sum += _val;
    ++pos;
    if (pos >= k)
      pos = 0;
    if (sum < m)
      ++violations;
    assert(pos < k);
  }


  int MKMonitor::getCurrentSum() const {
    return sum;
  }


  const unsigned int* MKMonitor::getVals() const {
    return vals;
  }


  unsigned int MKMonitor::getPos() const {
    return pos;
  }


  unsigned int MKMonitor::getViolations() const {
    return violations;
  }


} // NS tmssim
