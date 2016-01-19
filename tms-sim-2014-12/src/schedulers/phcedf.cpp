/**
 * $Id: phcedf.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file phcedf.cpp
 * @brief Implementation of history-cognisant overload EDF scheduler
 * (weight with remaining execution time)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/phcedf.h>

using namespace std;

namespace tmssim {
  
  static const std::string myId = "PHCEDFScheduler";
  
  
  PHCEDFScheduler::PHCEDFScheduler()
    : OEDFMaxScheduler() {
  }
  
  
  PHCEDFScheduler::~PHCEDFScheduler() {
  }
  
  
  const string& PHCEDFScheduler::getId(void) const {
    return myId;
  }
  
  
  double PHCEDFScheduler::calcValue(int time, const Job *job) const {
    return job->getPossibleHistoryValue(time);
  }
  
} // NS tmssim
