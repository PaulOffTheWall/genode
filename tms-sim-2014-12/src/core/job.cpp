/**
 * $Id: job.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file job.cpp
 * @brief Implementation of a TMS job
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

//#include <core/scobjects.h>
#include <core/job.h>
#include <core/task.h>

#include <cassert>
//#include <iostream>
#include <sstream>

using namespace std;

namespace tmssim {

  Job::Job(Task* task, unsigned int jid, int _activationTime, int _executionTime, int _absDeadline, int _priority)
    : myTask(task), jobId(jid), activationTime(_activationTime), executionTime(_executionTime), absDeadline(_absDeadline), priority(_priority),
      etRemain(_executionTime), preemptions(0)
  {
    assert(task != NULL);
    updateLatestStartTime();
  }
  
  
  int Job::getActivationTime(void) const {
    return activationTime;
  }
  
  
  int Job::getExecutionTime(void) const {
    return executionTime;
  }
  
  
  int Job::getAbsDeadline(void) const {
    return absDeadline;
  }
  
  
  int Job::getRemainingExecutionTime(void) const {
    return etRemain;
  }
  
  
  Task* Job::getTask(void) const {
    return myTask;
  }
  
  int Job::getPriority(){
    return priority;
  }
  
  bool Job::execStep(void) {
    etRemain--;
    updateLatestStartTime();
    if (etRemain <= 0) return true;
    else return false;
  }
  
  int Job::getLatestStartTime(void) const {
    return latestStartTime;
  }

  void Job::updateLatestStartTime(void) {
    latestStartTime = absDeadline - etRemain;
  }


  void Job::preempt() {
    ++preemptions;
  }
  
  
  int Job::getPreemptions() const {
    return preemptions;
  }
  
  
  double Job::getPossibleExecValue(int startTime) const {
    return myTask->getPossibleExecValue(this, startTime);
  }
  
  
  double Job::getPossibleHistoryValue(int startTime) const {
    return myTask->getPossibleHistoryValue(this, startTime);
  }
  
  
  double Job::getPossibleFailHistoryValue() const {
    return myTask->getPossibleFailHistoryValue(this);
  }


  string Job::getIdString(void) const {
    ostringstream oss;
    unsigned int tid = myTask->getId();
    unsigned int jid = jobId;
    oss << "J" << tid << "," << jid;
    return oss.str();
  }

  
  void Job::setPriority(int p){
    priority = p;
  }
  

  ostream& operator << (std::ostream& ost, const Job& job) {
    ost << job.getIdString() << "(" << job.etRemain
	<< "/" << job.executionTime << "-" << job.absDeadline << " P " << job.priority
	<< " S " << job.latestStartTime << ") " << job.myTask->strState();
    return ost;
  }
  
} // NS tmssim
