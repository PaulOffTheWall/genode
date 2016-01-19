/**
 * $Id: edf.cpp 795 2014-12-11 11:02:52Z klugeflo $
 * @file edf.cpp
 * @brief Implementation of EDF scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/edf.h>

#include <cassert>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>


using namespace std;

namespace tmssim {

  static const std::string myId = "EDFScheduler";
  
  
  EDFScheduler::EDFScheduler() : ALDScheduler() {
  }
  
  
  EDFScheduler::~EDFScheduler() {
  }
  

  bool EDFScheduler::enqueueJob(Job *job) {
    assert(job->getTask() != NULL);
    assert((long long)job->getTask() < 0x800000000000LL);
    std::list<Job*>::iterator it = mySchedule.begin();
    while ( it != mySchedule.end()
	    && *it != NULL
	    && (*it)->getAbsDeadline() <= job->getAbsDeadline() ) {
      it++;
    }
    mySchedule.insert(it, job);
    //scheduleChanged = true;
    notifyScheduleChanged();
    return true;
  }


  int EDFScheduler::schedule(int now, ScheduleStat& scheduleStat) {
    bool firstJobValid = false;
    while (!firstJobValid) {
      Job* job = mySchedule.front();
      if (job->getLatestStartTime() < now) {
	scheduleStat.cancelled.push_back(job);
	mySchedule.pop_front();
      }
      else {
	firstJobValid = true;
      }
    }
    return 0;
  }


  const std::string& EDFScheduler::getId(void) const {
    return myId;
  }


  const Job* EDFScheduler::checkEDFSchedule(int now) const {
    int time = now;
    for (list<Job*>::const_iterator it = mySchedule.begin();
	 it != mySchedule.end(); ++it) {
      time += (*it)->getRemainingExecutionTime();
      if (time > (*it)->getAbsDeadline()) {
	return *it;
      }
    }
    return NULL;
  }


  const Job* EDFScheduler::checkEDFSchedule(int now, const list<Job*>& schedule) {
    int time = now;
    for (list<Job*>::const_iterator it = schedule.begin();
	 it != schedule.end(); ++it) {
      time += (*it)->getRemainingExecutionTime();
      if (time > (*it)->getAbsDeadline()) {
	return *it;
      }
    }
    return NULL;
  }
  
  /*
  std::ostream& operator << (std::ostream& ost, const EDFScheduler& scheduler) {
    scheduler.printSchedule(ost);
    return ost;
  }
  */
  
} // NS tmssim
