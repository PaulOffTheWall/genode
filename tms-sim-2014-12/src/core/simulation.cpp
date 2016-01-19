/**
 * $Id: simulation.cpp 795 2014-12-11 11:02:52Z klugeflo $
 * @file simulation.cpp
 * @brief Implementation of simulation class
 * @author Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/simulation.h>
//#include <core/stat.h>
#include <utils/tlogger.h>

#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>


using namespace std;

namespace tmssim {

  
  SimulationResults::SimulationResults(Taskset* ts)
    : simulatedTime(0),
      success(false),
      activations(0),
      completions(0),
      cancellations(0),
      execCancellations(0),
      misses(0),
      preemptions(0),
      usum(0),
      esum(0),
      cancelSteps(0),
      idleSteps(0),
      taskset(ts),
      schedulerId("")
  {
  }


  SimulationResults::SimulationResults(const SimulationResults& rhs)
    : simulatedTime(rhs.simulatedTime),
      success(rhs.success),
      activations(rhs.activations),
      completions(rhs.completions),
      cancellations(rhs.cancellations),
      execCancellations(rhs.execCancellations),
      misses(rhs.misses),
      preemptions(rhs.preemptions),
      usum(rhs.usum),
      esum(rhs.esum),
      cancelSteps(rhs.cancelSteps),
      idleSteps(rhs.idleSteps),
      taskset(rhs.taskset),
      schedulerId(rhs.schedulerId)
  {
  }


  std::ostream& operator<< (std::ostream& out, const SimulationResults& stats) {
    out << "Act: " << stats.activations
	<< " Compl: " << stats.completions
	<< " Canc: " << stats.cancellations
	<< " Ecanc: " << stats.execCancellations
	<< " Miss: " << stats.misses
	<< " Preempt: " << stats.preemptions
	<< " CancelS: " << stats.cancelSteps
	<< " IdleS: " << stats.idleSteps << endl;
    out << "U_Sys/ [" << stats.usum
	<< "/" << stats.esum
	<< "=" << ((float)stats.usum / stats.esum) << "]" << endl;
    return out;
  }


  Simulation::Simulation(Taskset* _taskset, Scheduler* _scheduler,
			 int _steps) : //, Logger* _logger) :
    taskset(_taskset), scheduler(_scheduler), steps(_steps),// logger(_logger),
    stats(_taskset)
    
  {
    cancelSteps = 0;
    idleSteps = 0;
    stats.schedulerId = scheduler->getId();
  }


  Simulation::~Simulation() {
    delete scheduler;
    for (Taskset::iterator it = taskset->begin();
	 it != taskset->end(); ++it) {
      delete *it;
    }
    delete taskset;
  }

  
  const SimulationResults& Simulation::run() {
    
    //Stat* statistics = new Stat();
    //statistics->setVerbose(true); // FIXME: remove!
    //statistics->setSchedulerId(scheduler->getId());

    LOG(0) << "######## Running Simulation with scheduler " << scheduler->getId();
    LOG(0) << "\tAim are " << steps << " iterations";

    
    initCounters();
    for (size_t taskNum = 0; taskNum< (*taskset).size(); taskNum++) {
      (*taskset)[taskNum]->start(0);
    }
    unsigned int time = 0;
    bool schedSuccess = true;
    for (time = 0; time < steps; time++) {
      LOG(1) << "\n\nT : " << time;
      doActivations(time, scheduler);
      schedSuccess = doExecutions(time, scheduler/*, statistics*/);
      if (!schedSuccess) {
	LOG(0) << "Executions failed in regular time step " << time;
	break;
      }
    }
    if (schedSuccess) {
      while (scheduler->hasPendingJobs()) {
	bool bdisp = doExecutions(time, scheduler/*, statistics*/);
        ++time;
        if (!bdisp) {
	  LOG(0) << "Executions failed in during cleanup in step " << time;
	  break;
	}
      }
    }
    LOG(0) << "Totally simulated time: " << time;
    
    printExecStats(scheduler->getId());
    printDelayCounters();
    
    // Save the statistics in a other object
    //storeStatistics(statistics);
    
    calculateStatistics();
    stats.simulatedTime = time;
    stats.success = schedSuccess;
    return stats;
  }


  const Taskset* Simulation::getTaskset() const {
    return taskset;
  }

  
  void Simulation::initCounters(void) {
    /*for (int i = 0; i < CANCEL_SLOTS; ++i) {
      cancelCtr[i] = 0;
      }*/
    cancelSteps = 0;
    idleSteps = 0;
    cancelStepList.clear();
  }

  
  bool Simulation::doActivations(int time, Scheduler *sched/*, ostream& log*/) {
    tDebug() << "\nActivations [" << time << "]";
    bool rv = false;
    list<Job*> actList;
    for (size_t i = 0; i < taskset->size(); i++) {
      Job* job = NULL;
      job = (*taskset)[i]->spawnJob(time);
      if (job != NULL) {
	sched->enqueueJob(job);
	actList.push_back(job);
	rv = true;
      }
    }
    if (actList.size() > 0) {
      ostringstream oss;
      oss << "A@" << time << " :";
      ostringstream osb;
      osb << "\t";
      //log << "A@" << time << " :";
      for (list<Job*>::iterator it = actList.begin(); it != actList.end(); ++it) {
	oss << " {" << *(*it) << "}";
	osb << *it << " ";
      }
      LOG(1) << oss.str();
      LOG(2) << osb.str();
    }
    return rv;
  }

  
  bool Simulation::doExecutions(int time, Scheduler *sched/*, ostream& log, Stat* statPtr*/) {
    tDebug() << "Executions [" << time << "]";

    // Schedule
    ScheduleStat scStat;
    int scrv = sched->schedule(time, scStat);
    if (scrv != 0) {
      LOG(1) << "Schedule failed: " << scrv;
      return false;
    }
    
    if (scStat.cancelled.size() > 0) {
      int ctr = 0;
      ostringstream oss;
      oss << "C@" << time << " :";
      for (list<Job*>::iterator it = scStat.cancelled.begin(); it != scStat.cancelled.end(); ++it) {
	Job* cjob = *it;
	//cout << "\tcanceling job " << cjob << " " << *cjob;
	oss << " {" << *cjob << "}";
	Task *task = cjob->getTask();
	task->cancelJob(cjob);
	ctr++;
      }
      LOG(1) << oss.str();
      //assert(ctr <= CANCEL_SLOTS);
      //++cancelCtr[ctr - 1];
      ++cancelSteps;
      //statPtr->addToCancelStepList(time);
      cancelStepList.push_back(time);
    }
    
    // Dispatch
    Job* job = NULL;
    DispatchStat dispStat;
    job = sched->dispatch(time, dispStat);
    ostringstream oss;
    oss << "E@" << time << " : ";
    if (dispStat.idle) {
      oss << "I";
      ++idleSteps;
    }
    else {
      if (dispStat.executed != NULL)
	oss << "[" << *dispStat.executed << "] ";
      else
	oss << "EXEC FAIL";
    }
    
    if ((long int) job < 0) {
      oss << "\tDispatching failed: " << (long int) job;
      return false;
    }
    else if (job != NULL) { // equiv to dispStat->finished != NULL
      assert(job == dispStat.finished);
      oss << "(F";
      if (dispStat.dlMiss) {
	oss << ",M";
      }
      oss << ") ";
      LOG(1) << oss.str();
      Task *task = job->getTask();
      task->completeJob(job, time);
      return true;
    }
    else {
      LOG(1) << oss.str();
      return true;
    }
  }


  void Simulation::calculateStatistics() {
    for (size_t i = 0; i < taskset->size(); i++) {
      Task* task = (*taskset)[i];
      stats.activations += task->getActivations();
      stats.completions += task->getCompletions();
      stats.cancellations += task->getCancellations();
      stats.execCancellations += task->getExecCancellations();
      stats.misses += task->getMisses();
      stats.preemptions += task->getPreemptions();
      stats.usum += task->getUA()->getTotal();
      stats.esum += task->getUA()->getCount();
    }
    stats.cancelSteps = cancelSteps;
    stats.idleSteps = idleSteps;
  }
  
  /*
    // don't use this anymore
  void Simulation::storeStatistics(Stat* stat) {
    stat->setSchedulerId(scheduler->getId());
    
    // foreach task store statistics
    for (size_t i = 0; i < taskset->size(); i++) {
      Task* task = (*taskset)[i];
      stat->addTaskId(task->getIdString());
      stat->setActivations(task->getIdString(),task->getActivations());
      stat->setCompletions(task->getIdString(),task->getCompletions());
      stat->setCancellations(task->getIdString(),task->getCancellations());
      stat->setExecCancellations(task->getIdString(),task->getExecCancellations());
      stat->setMisses(task->getIdString(),task->getMisses());
      stat->setPreemptions(task->getIdString(),task->getPreemptions());
      // TODO: SET task->getUA()
      stat->setUtilitySums(task->getIdString(),task->getUA()->getTotal());
      stat->setUtiltyCounts(task->getIdString(), task->getUA()->getCount());
      stat->setMeanUtilities(task->getIdString(),task->getUA()->getMeanUtility());
      stat->setHistoryValues(task->getIdString(),task->getHistoryValue());
      
      // TODO: Check if these values equal sum of the utilities
      stat->setSumOfUtilities(task->getUA()->getTotal());
      stat->setSumOfUtilityCounts(task->getUA()->getCount());
      
      // TODO: Implement
      vector<int> test = task->getCounters();
    }
    
    // TODO: Make better (not just store it in simulation object)
    stat->setIdleSteps(this->idleSteps);
  }
  */
  
  void Simulation::printExecStats(const string& head) {
    int activations = 0, completions = 0, cancellations = 0, execCancellations = 0, misses = 0, preemptions = 0;
    double usum = 0;
    int esum = 0;
    LOG(0) << head;
    LOG(0) << "# Task [hv] {act|compl|canc/ecanc|miss|preempt} [UA]";
    for (size_t i = 0; i < taskset->size(); i++) {
      Task* task = (*taskset)[i];
      LOG(0) << task->getIdString()
	     << " {" << task->getActivations()
	     << "|" << task->getCompletions()
	     << "|" << task->getCancellations()
	     << "/" << task->getExecCancellations()
	     << "|" << task->getMisses()
	     << "|" << task->getPreemptions()
	     << "} " << *(task->getUA())
	;
      activations += task->getActivations();
      completions += task->getCompletions();
      cancellations += task->getCancellations();
      execCancellations += task->getExecCancellations();
      misses += task->getMisses();
      preemptions += task->getPreemptions();
      usum += task->getUA()->getTotal();
      esum += task->getUA()->getCount();
    }
    LOG(0) << "Act: " << activations << " Compl: " << completions << " Canc: " << cancellations << " Ecanc: " << execCancellations << " Miss: " << misses << " Preempt: " << preemptions;
    LOG(0) << "U_Sys/ [" << usum << "/" << esum << "=" << (usum / esum) << "]";
    
    LOG(0) << "Cancel statistics (how often got $i tasks cancelled in one time step?):";
      // FIXME: write to stringstream
    ostringstream osscs;
    /*for (int i = 0; i < CANCEL_SLOTS; ++i) {
      osscs << "CC[" << (i + 1) << "]=" << cancelCtr[i] << " ";
      }*/
    LOG(1) << osscs.str();
    LOG(0) << "Total steps with cancellations: " << cancelSteps;
      // FIXME: write to stringstream
    ostringstream osscc;
    osscc << "CancelSteps: ";
    if (cancelStepList.size() > 0)
      for (list<int>::iterator it = cancelStepList.begin(); it != cancelStepList.end(); ++it) {
	osscc << *it << " ";
      }
    LOG(1) << osscc.str();
    LOG(0) << "Idle steps: " << idleSteps;
  }

  
  void Simulation::printDelayCounters(void) {
    LOG(0) << "DelayCounters:";
    for (size_t i = 0; i < taskset->size(); i++) {
      const vector<int>& delayCounters = (*taskset)[i]->getCounters();
      
      ostringstream oss;
      oss << "Task " << (*taskset)[i]->getId() << ": ";
      
      if(delayCounters.size() > 0) {
	
	for (size_t j = 0; j < delayCounters.size(); ++j) {
	  if (delayCounters[j] != 0) {
	    oss << " (" << (j + 1) << "," << delayCounters[j] * (j + 1) << ")";
	  }
            }
	oss << " ";
	oss << "MAX: " << delayCounters.size();
	
      } else {
	oss << "None";
      }
      
      LOG(0) << oss.str();
    }
  }

  
} // NS tmssim
