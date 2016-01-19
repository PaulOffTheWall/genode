/**
 * $Id: oedfmax.cpp 795 2014-12-11 11:02:52Z klugeflo $
 * @file oedfmax.cpp
 * @brief Implementation of overload EDF scheduler (maximum value removal)
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 * @todo Replace tDebug etc. by LOG(...)
 */

#include <cassert>
#include <cfloat>
#include <climits>
#include <cstddef>
#include <list>

#include <iostream>

#include <schedulers/oedfmax.h>

#include <utils/logger.h>

using namespace std;

namespace tmssim {

  //static const std::string myId = "OEDFMaxScheduler";

  
  OEDFMaxScheduler::OEDFMaxScheduler()
    : OEDFScheduler() {
  }
  
  
  OEDFMaxScheduler::~OEDFMaxScheduler() {
  }

  double OEDFMaxScheduler::getComparisonNeutral() const {
    return DBL_MIN;
  }
  
  bool OEDFMaxScheduler::compare(double current, double candidate) const {
    return candidate > current;
  }

  
  // what if cancelled job is currently being executed?!?
  // -> sanitise currentJob!!!1elf
  /*
  int OEDFMaxScheduler::schedule(int now, ScheduleStat& scheduleStat) {
    LOG(3) << "Running OEDFMaxScheduler::schedule";

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
      list<Job*>::iterator max;
      double maxVal = DBL_MIN;
      int time = now;
      list<Job*>::iterator it;
      for (it = mySchedule.begin();
	   it != mySchedule.end() && *it != missJob; ++it) {
	Job* job = *it;
	double val = calcValue(time, job);
	LOG(3) << "\tChecking " << *job << " with value " << val;
	if (!isCancelCandidate(val))
	  continue;
	if (val > maxVal) {
	  max = it;
	  maxVal = val;
	  LOG(4) << "New max-utility found: " << val;
	}
      }
      if (*it == missJob) {
	// Check wether we can remove missJob
	double val = calcValue(time, missJob);
	if (val > maxVal) {
	  max = it;
	  maxVal = val;
	  LOG(3) << "New max-utility found: " << val;
	}
      }
      if (maxVal != DBL_MIN) {
	Job* cancel = *max;
	LOG(3) << "Removing Job " << *cancel;
	//assert(cancel != currentJob);
	//if (cancel = currentJob)
	//currentJob = NULL;
	notifyJobRemoved(cancel);
	mySchedule.erase(max);
	//internalCancelJob(cancel);
	//if (scheduleStat != NULL) {
	scheduleStat.cancelled.push_back(cancel);
	//}
	//scheduleChanged = true;
	notifyScheduleChanged();
      }
      lastFoundJob = missJob;
    }
    return 0;
  }
  */

} // NS tmssim
