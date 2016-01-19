/**
 * $Id: dvdedf.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file dvdedf.cpp
 * @brief Implementation of dynamic value density scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/dvdedf.h>

using namespace std;

namespace tmssim {

  static const std::string myId = "DVDEDFScheduler";
  
  
  DVDEDFScheduler::DVDEDFScheduler()
    : OEDFMinScheduler() {
  }
  
  
  DVDEDFScheduler::~DVDEDFScheduler() {
  }
  
  
  const string& DVDEDFScheduler::getId(void) const {
    return myId;
  }
  
  
  double DVDEDFScheduler::calcValue(int time, const Job *job) const {
    return job->getPossibleExecValue(time) / job->getRemainingExecutionTime();
  }
  
} // NS tmssim
