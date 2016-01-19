/**
 * $Id: hcedf.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file hcedf.cpp
 * @brief Implementation of history-cognisant overload EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/hcedf.h>

using namespace std;

namespace tmssim {

  static const std::string myId = "HCEDFScheduler";
  
  
  HCEDFScheduler::HCEDFScheduler()
    : OEDFMaxScheduler() { //EDFScheduler() {
  }
  
  
  HCEDFScheduler::~HCEDFScheduler() {
  }
  
  
  const string& HCEDFScheduler::getId(void) const {
    return myId;
  }
  
  
  double HCEDFScheduler::calcValue(int time, const Job *job) const {
    return job->getPossibleHistoryValue(time) * job->getRemainingExecutionTime();
  }
  
} // NS tmssim
