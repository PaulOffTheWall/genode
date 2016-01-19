/**
 * $Id: oedfmin.cpp 795 2014-12-11 11:02:52Z klugeflo $
 * @file oedfmin.cpp
 * @brief Implementaiton of EDF with cancellation, minimum value cancel policy
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/oedfmin.h>
#include <utils/logger.h>

using namespace std;

namespace tmssim {

  OEDFMinScheduler::OEDFMinScheduler()
    : OEDFScheduler() {
  }
  
  
  OEDFMinScheduler::~OEDFMinScheduler() {
  }
  
  
  double OEDFMinScheduler::getComparisonNeutral() const {
    return DBL_MAX;
  }
  
  bool OEDFMinScheduler::compare(double current, double candidate) const {
    return candidate < current;
  }

  /*
  // what if cancelled job is currently being executed?!?
  // -> sanitise currentJob!!!1elf
  int OEDFMinScheduler::schedule(int now, ScheduleStat& scheduleStat) {
    //if (!scheduleChanged)
    //return 0;

    // check schedule head(s) wether they will keep their deadline
    Job* toJob; // timed-out
    while ( mySchedule.size() > 0 && (toJob = mySchedule.front())->getLatestStartTime() < now) {
      notifyJobRemoved(toJob);
      mySchedule.pop_front();
      scheduleStat.cancelled.push_back(toJob);
      notifyScheduleChanged();
    }

    const Job* missJob;
    const Job* lastFoundJob = NULL;
    while ( (missJob = checkEDFSchedule(now)) != NULL && missJob != lastFoundJob) {
      LOG(3) << "Found DL-Miss job " << *missJob;
      list<Job*>::iterator min;
      double minVal = DBL_MAX;
      int time = now;
      list<Job*>::iterator it;
      for (it = mySchedule.begin();
	   it != mySchedule.end() && *it != missJob; ++it) {
	Job* job = *it;
	double val = calcValue(time, job);
	if (!isCancelCandidate(val))
	  continue;
	if (val < minVal) {
	  min = it;
	  minVal = val;
	  LOG(4) << "New min-utility found: " << val;
	}
      }
      if (*it == missJob) {
	// Check wether we can remove missJob
	double val = calcValue(time, missJob);
	if (val < minVal) {
	  min = it;
	  minVal = val;
	  LOG(3) << "New min-utility found: " << val;
	}
      }
      
      if (minVal != DBL_MAX) {
	Job* cancel = *min;
	LOG(3) << "Removing Job " << *cancel;
	//if (cancel = currentJob)
	//currentJob = NULL;
	notifyJobRemoved(cancel);
	mySchedule.erase(min);
	scheduleStat.cancelled.push_back(cancel);
	//scheduleChanged = true;
	notifyScheduleChanged();
      }
      lastFoundJob = missJob;
    }
    return 0;
  }
  */
  
} // NS tmssim
