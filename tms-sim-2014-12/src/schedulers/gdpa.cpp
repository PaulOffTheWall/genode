/**
 * $Id: gdpa.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file gdpa.cpp
 * @brief Guaranteed Dynamic Priority Assigment Scheduler
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <schedulers/gdpa.h>
#include <utils/tlogger.h>

#include <cassert>

namespace tmssim {

  static const std::string myId = "GDPAScheduler";

  GDPAScheduler::GDPAScheduler()
    : EDFScheduler(), readyQueueChanged(false) {
  }


  GDPAScheduler::~GDPAScheduler() {
    // ALDScheduler will cleanup the mySchedule list
    // BUT: not every job may be in this list!
  }


  bool GDPAScheduler::enqueueJob(Job *job) {
    assert(job->getTask() != NULL);
    //assert((long long)job->getTask() < 0x800000000000LL);
    readyQueue.push_back(job);
    readyQueueChanged = true;
  }


  bool GDPAScheduler::hasPendingJobs(void) const {
    return readyQueue.size() > 0;
  }


  int GDPAScheduler::schedule(int now, ScheduleStat& scheduleStat) {
    list<Job*>::iterator it;
    // Check feisibility of all jobs
    it = readyQueue.begin();
    while (it != readyQueue.end()) {
      if ((*it)->getLatestStartTime() < now) {
	Job* cancel = *it;
	it = readyQueue.erase(it);
	tDebug() << "Removing Job " << *cancel;
	scheduleStat.cancelled.push_back(cancel);
      }
      else {
	++it;
      }
    }
    // sort by shortest distance
    list<Job*> sdfList;
    for (it = readyQueue.begin();
	 it != readyQueue.end(); ++it) {
      Job* job = *it;
      int distance = job->getTask()->getDistance();
      list<Job*>::iterator ins = sdfList.begin();
      while ( ins != sdfList.end()
	      && *ins != NULL
	      && (*ins)->getTask()->getDistance() <= distance ) {
	ins++;
      }
      sdfList.insert(ins, job);
    }
    // create feasible EDF schedule
    mySchedule.clear();
    for (it = sdfList.begin(); it != sdfList.end(); it++) {
      Job* job = *it;
      std::list<Job*>::iterator ins = mySchedule.begin();
      while ( ins != mySchedule.end()
	      && *ins != NULL
	      && (*ins)->getAbsDeadline() <= job->getAbsDeadline() ) {
	ins++;
      }
      ins = mySchedule.insert(ins, job);
      // now check feasibility
      const Job* fjob = checkEDFSchedule(now);
      if (fjob != NULL) {
	mySchedule.erase(ins);
      }
    }
    readyQueueChanged = false;
    //scheduleChanged = true;
    notifyScheduleChanged();
    return 0;
  }


  const std::string& GDPAScheduler::getId(void) const {
    return myId;
  }


  void GDPAScheduler::jobFinished(Job *job) {
    list<Job*>::iterator it;
    it = readyQueue.begin();
    while (it != readyQueue.end() && *it != job)
      ++it;
    if (it != readyQueue.end()) { // found
      readyQueue.erase(it);
    }
  }


} // NS tmssim
