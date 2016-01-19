/**
 * $Id: deadlinemonitor.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file deadlinemonitor.cpp
 * @brief Implementation of Deadline Monitor
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/deadlinemonitor.h>
#include <utils/tlogger.h>

using namespace std;

namespace tmssim {

  DeadlineMonitor::DeadlineMonitor() {
  }


  DeadlineMonitor::~DeadlineMonitor() {
  }

  
  void DeadlineMonitor::addJob(const Job* job) {
    if (job == NULL) {
      // might also throw some error
      return;
    }
    list<const Job*>::iterator it = jobs.begin();
    while ( it != jobs.end()
	    && *it != NULL
	    && (*it)->getLatestStartTime() <= job->getLatestStartTime()) {
      it++;
    }
    jobs.insert(it, job);
  }

  
  const Job* DeadlineMonitor::check(int now) {
    if (jobs.size() == 0)
      return NULL;
    const Job* job = jobs.front();
    if (job->getLatestStartTime() < now) {
      jobs.pop_front();
      return job;
    }
    else {
      return NULL;
    }
  }


  const Job* DeadlineMonitor::jobExecuted(const Job* job) {
    list<const Job*>::iterator it = jobs.begin();
    while ( it != jobs.end() && *it != job ) {
      it++;
    }
    if (it == jobs.end())
      return NULL; // finished job not found

    // job execution can only increase the latest start time, so we need only
    // to look at jobs that are at the back of the list
    list<const Job*>::iterator itJob = it;
    it++; // goto next

    if ( (*it)->getLatestStartTime() >= job->getLatestStartTime() ) {
      return job;
    }
    // else we need to reorder...
    it = jobs.erase(itJob);
    while ( it != jobs.end() && *it != NULL
	    && (*it)->getLatestStartTime() < job->getLatestStartTime() ) {
      it++;
    }
    jobs.insert(it, job);
    return job;
  }


  const Job* DeadlineMonitor::jobFinished(const Job* job) {
    list<const Job*>::iterator it = jobs.begin();
    while ( it != jobs.end() && *it != job ) {
      it++;
    }
    if (it == jobs.end())
      return NULL;
    jobs.erase(it);
    //tDebug() << "DLMon removed finished job " << job << " " << *job;
    return job;
  }


} // NS tmssim
