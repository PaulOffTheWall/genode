/**
 * $Id: oedf.cpp 798 2014-12-11 11:17:53Z klugeflo $
 * @file oedf.cpp
 * @brief EDF scheduler with optional execution to resolve overloads
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/oedf.h>

#include <utils/logger.h>

namespace tmssim {

  //static const std::string myId = "OEDFScheduler";
  
  
  OEDFScheduler::OEDFScheduler()
    : EDFScheduler() {
  }
  
  
  OEDFScheduler::~OEDFScheduler() {
  }
  
  
  // what if cancelled job is currently being executed?!?
  // -> sanitise currentJob!!!1elf
  int OEDFScheduler::schedule(int now, ScheduleStat& scheduleStat) {
    LOG(3) << "Running OEDFScheduler::schedule";

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
      list<Job*>::iterator current;
      double currentVal = getComparisonNeutral();
      int time = now; // FIXME: evolve???
      list<Job*>::iterator it;
      for (it = mySchedule.begin();
	   it != mySchedule.end() && *it != missJob; ++it) {
	Job* job = *it;
	double val = calcValue(time, job);
	time += job->getExecutionTime();
	LOG(3) << "\tChecking " << *job << " with value " << val;
	if (!isCancelCandidate(val))
	  continue;
	if (compare(currentVal, val)) {
	  current = it;
	  currentVal = val;
	  LOG(4) << "New candidate utility found: " << val;
	}
      }
      if (*it == missJob) {
	// Check wether we can remove missJob
	double val = calcValue(time, missJob);
	if (compare(currentVal, val)) {
	  current = it;
	  currentVal = val;
	  LOG(3) << "New min-utility found: " << val;
	}
      }
      
      if (currentVal != getComparisonNeutral()) {
	Job* cancel = *current;
	LOG(3) << "Removing Job " << *cancel;
	notifyJobRemoved(cancel);
	mySchedule.erase(current);
	scheduleStat.cancelled.push_back(cancel);
	notifyScheduleChanged();
      }
      lastFoundJob = missJob;
    }
    return 0;
  }


} // NS tmssim
