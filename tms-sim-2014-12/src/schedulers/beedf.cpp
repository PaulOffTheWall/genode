/**
 * $Id: beedf.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file beedf.cpp
 * @brief Implementation of best effort EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */
/**
 * $Id: beedf.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * Timing Models Scheduling Simulator
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/beedf.h>

using namespace std;

namespace tmssim {

  static const std::string myId = "BEEDFScheduler";
  
  
  BEEDFScheduler::BEEDFScheduler()
    : OEDFMinScheduler() { //EDFScheduler() {
  }
  
  
  BEEDFScheduler::~BEEDFScheduler() {
  }
  
  
  const string& BEEDFScheduler::getId(void) const {
    return myId;
  }
  
  
  double BEEDFScheduler::calcValue(int time, const Job *job) const {
    return job->getPossibleExecValue(time) / job->getExecutionTime();
  }
  
} // NS tmssim
