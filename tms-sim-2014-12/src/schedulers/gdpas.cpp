/**
 * $Id: gdpas.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file gdpas.cpp
 * @brief Simplified Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/gdpas.h>

#include <cassert>

#include <schedulers/edf.h>
#include <utils/logger.h>
#include <utils/tlogger.h>


namespace tmssim {

  static const std::string myId = "GDPA-SScheduler";


  GDPASScheduler::GDPASScheduler()
    : Scheduler(), readyListChanged(false), dispatchListsChanged(false), /*edfFeasible(false),*/ currentJob(NULL) {
  }


  GDPASScheduler::~GDPASScheduler() {
    // ALDScheduler will cleanup the mySchedule list
    // BUT: not every job may be in this list!
  }


  bool GDPASScheduler::enqueueJob(Job *job) {
    // lines 6-8
    assert(job != NULL);
    assert(job->getTask() != NULL);
    //LOG(-1) << "Enqueueing job " << job;
    readyList.push_back(job);
    readyListChanged = true;

    int distance = job->getTask()->getDistance();
    list<Job*>::iterator insSDF = sdfList.begin();
    while ( insSDF != sdfList.end()
	    && *insSDF != NULL
	    && (*insSDF)->getTask()->getDistance() <= distance ) {
      insSDF++;
    }
    sdfList.insert(insSDF, job);

    //Job* job = *it;
    std::list<Job*>::iterator insEDF = edfList.begin();
    while ( insEDF != edfList.end()
	    && *insEDF != NULL
	    && (*insEDF)->getAbsDeadline() <= job->getAbsDeadline() ) {
      insEDF++;
    }
    edfList.insert(insEDF, job);
    dispatchListsChanged = true;
  }


  Job* GDPASScheduler::removeJob(Job *job) {
    return NULL;
  }


  bool GDPASScheduler::hasPendingJobs(void) const {
    return readyList.size() > 0;
  }


  int GDPASScheduler::schedule(int now, ScheduleStat& scheduleStat) {  
    LOG(3) << "Creating new schedule?";
    if (readyListChanged) {
      LOG(3) << "\tYes! R: " << readyList.size()
	     << " E: " << edfList.size() << " S: " << sdfList.size();
      // lines 14-18
      list<Job*>::iterator it = readyList.begin();
      while (it != readyList.end()) {
	LOG(3) << "Checking job...";
	if ((*it)->getLatestStartTime() < now) {
	  Job* job = *it;
	  it = readyList.erase(it);
	  removeJobFromList(edfList, job);
	  removeJobFromList(sdfList, job);
	  scheduleStat.cancelled.push_back(job);
	  dispatchListsChanged = true;
	}
	else {
	  ++it;
	}
      }
    }
    return 0;
  }


  Job* GDPASScheduler::dispatch(int now, DispatchStat& dispatchStat) {
    Job* prevJob = currentJob;
    bool oldDlc = dispatchListsChanged;
    bool edfFeasible = false;
    if (currentJob == NULL || dispatchListsChanged) {
      // lines 19-23
      edfFeasible = EDFScheduler::checkEDFSchedule(now, edfList) == NULL;
      //currentJob = edfFeasible ? edfList.front() : sdfList.front();
      if (readyList.size() > 0) {
	if (edfFeasible) {
	  currentJob = edfList.front();
	}
	else {
	  currentJob = sdfList.front();
	}
      }
      else {
	currentJob = NULL;
      }
      dispatchListsChanged = false;
    }
    if (prevJob != NULL && prevJob != currentJob) {
      prevJob->preempt();
    }
    if (currentJob == NULL) {
      dispatchStat.idle = true;
      return NULL;
    }

    tDebug() << "\tExecuting job " << *currentJob;
    bool fin = currentJob->execStep();
    dispatchStat.executed = currentJob;
    if (fin) { // (currentJob->etRemain == 0) {
      Job* finishedJob = currentJob;
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


  const std::string& GDPASScheduler::getId(void) const {
    return myId;
  }


  void GDPASScheduler::jobFinished(Job *job) {
    // lines 9-12
    removeJobFromList(readyList, job);
    removeJobFromList(edfList, job);
    removeJobFromList(sdfList, job);
    readyListChanged = true;
    dispatchListsChanged = true;
    //feasibilityCheck();
  }


  void GDPASScheduler::feasibilityCheck(int now) {
    // lines 14-18
    /*
    list<Job*>::iterator it = readyList.begin();
    while (it != readyList.end()) {
      if ((*it)->getSt() < now) {
	Job* job = *it;
	it = readyList.erase(it);
	removeJobFromList(edfList, job);
	removeJobFromList(sdfList, job);
	queuesChanged = true;
      }
    }
    */
  }

  void GDPASScheduler::removeJobFromList(list<Job*>& jList, Job* job) {
    list<Job*>::iterator it = jList.begin();
    while (it != jList.end() && *it != job)
      it++;
    if (it != jList.end()) {
      jList.erase(it);
    }
  }


} // NS tmssim
