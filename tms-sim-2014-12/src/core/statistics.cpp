/**
 * $Id: statistics.cpp 691 2014-09-14 10:35:58Z klugeflo $
 * @file statistics.cpp
 * @brief management of schedule and dispatch statistics
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/scheduler.h>

namespace tmssim {

  ScheduleStat::ScheduleStat()
  //: cancelled()
  {
    //cancelled = new std::list<Job*>;
  }
  
  ScheduleStat::~ScheduleStat() {
    //delete cancelled;
    //cancelled = NULL;
  }
  
  DispatchStat::DispatchStat() 
    : executed(NULL), finished(NULL), dlMiss(false), idle(false) {
  }
  
} // NS tmssim
