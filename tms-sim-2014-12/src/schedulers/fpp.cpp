/**
 * $Id: fpp.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file fpp.cpp
 * @brief Fixed Priority Preemptive Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/fpp.h>
#include <utils/tlogger.h>

#include <cassert>
#include <iostream>
using namespace std;

namespace tmssim {

  static const std::string myId = "FPPScheduler";
  
  FPPScheduler::FPPScheduler() : ALDScheduler() {
  }


  FPPScheduler::~FPPScheduler() {
  }

    
  bool FPPScheduler::enqueueJob(Job *job) {
    tDebug() << "Enqueueing job " << *job << " @ " << job << " T@ " << job->getTask();
    assert(job->getTask() != NULL);
    assert((long long)job->getTask() < 0x800000000000LL);
    std::list<Job*>::iterator it = mySchedule.begin();
    while ( it != mySchedule.end()
	    && *it != NULL
	    && (*it)->getPriority() <= job->getPriority() ) {
      it++;
    }
    mySchedule.insert(it, job);
    //scheduleChanged = true;
    notifyScheduleChanged();
    dlmon.addJob(job);
    return true;
  }


  int FPPScheduler::schedule(int now, ScheduleStat& scheduleStat) {
    printSchedule();
    Job* missJob = NULL;
    while ( (missJob = (Job*)dlmon.check(now)) != NULL) {
      scheduleStat.cancelled.push_back((Job*)missJob);
      removeJob(missJob);
    }
    return 0;
  }

  Job* FPPScheduler::dispatch(int now, DispatchStat& dispatchStat) {
    Job* job = ALDScheduler::dispatch(now, dispatchStat);
    const Job* fin = dispatchStat.finished;
    if (fin != NULL) {
      if (fin != dlmon.jobFinished(fin)) {
	tError() << "Finished job " << job << " " << *job << " not found in dlmon!";
      }
      else {
	tDebug() << "Removed finished job " << job << " " << *job << " from dlmon!";
      }
    }
    return job;
  }


  const std::string& FPPScheduler::getId(void) const {
    return myId;
  }



} // NS tmssim
