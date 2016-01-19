/**
 * $Id: ald.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file ald.cpp
 * @brief Abstract List Dispatch scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/ald.h>
#include <utils/tlogger.h>

#include <cassert>
#include <iostream>
using namespace std;

namespace tmssim {

  ALDScheduler::ALDScheduler()
    : Scheduler(), currentJob(NULL), scheduleChanged(false) {
  }


  ALDScheduler::~ALDScheduler() {
    list<Job*>::iterator it = mySchedule.begin();
    while (it != mySchedule.end()) {
      Job* job = *it;
      tWarn() << "ALDScheduler destroys unfinished job " << *job;
      //<< " in instance of " << getId();
      delete job;
      it = mySchedule.erase(it);
      //++it;
    }
  }


  Job* ALDScheduler::removeJob(Job *job) {
    list<Job*>::iterator it = mySchedule.begin();
    while (it != mySchedule.end() && *it != job)
      it++;
   if (it == mySchedule.end())
      return NULL;
    mySchedule.erase(it);
    notifyJobRemoved(job);
    return job;
  }


  Job* ALDScheduler::dispatch(int now, DispatchStat& dispatchStat) {
    printSchedule();
    Job* prevJob = currentJob;
    if (currentJob == NULL || scheduleChanged) { // have to get (possibly) new job
      if (mySchedule.size() > 0) {
	currentJob = mySchedule.front();
	if (prevJob != NULL && prevJob != currentJob) {
	  prevJob->preempt();
	}
      }
      else {
	currentJob = NULL;
      }
    }
    // now we know which job is to be executed next (->currentJob)

    if (currentJob == NULL) { // no job -> idle
      dispatchStat.idle = true;
      return NULL;
    }
    else {
      tDebug() << "\tExecuting job " << *currentJob;
      bool fin = currentJob->execStep();
      dispatchStat.executed = currentJob;
      if (fin) { // (currentJob->etRemain == 0) {
	Job* finishedJob = currentJob;
	assert(finishedJob == mySchedule.front());
	mySchedule.pop_front();
	if (finishedJob->getAbsDeadline() <= now) {
	  dispatchStat.dlMiss = true;
	}
	tDebug() << "Finished job " << *finishedJob << " @ " << finishedJob << " finished.";
	dispatchStat.finished = finishedJob;
	jobFinished(finishedJob);
	currentJob = NULL;
	return finishedJob;
      }
      else {
	return NULL;
      }
    }
  }


  bool ALDScheduler::hasPendingJobs(void) const {
    return mySchedule.size() > 0;
  }


  void ALDScheduler::printSchedule() const {
    for (list<Job*>::const_iterator it = mySchedule.begin();
	 it != mySchedule.end(); ++it) {
      tDebug() << "\t" << **it;
    }
  }


  void ALDScheduler::jobFinished(Job *job) {
  }


  void ALDScheduler::notifyJobRemoved(const Job* removedJob) {
    if (removedJob == currentJob) {
      currentJob = NULL;
    }
  }


  void ALDScheduler::notifyScheduleChanged() {
    scheduleChanged = true;
  }

} // NS tmssim
