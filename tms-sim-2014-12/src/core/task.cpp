/**
 * $Id: task.cpp 815 2014-12-15 13:40:52Z klugeflo $
 * @file task.cpp
 * @brief Implementation of abstract task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <task.h>
#include <core/job.h>

#include <cassert>
#include <iostream>
#include <sstream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "task";

  Task::Task(unsigned int __id, int __et, int __ct,
	     UtilityCalculator* __uc, UtilityAggregator* __ua, int __priority)
    :
    id(_id), executionTime(_executionTime), absDeadline(_relDeadline), priority(_priority),
    activations(_activations), completions(_completions),
    cancellations(_cancellations), misses(_misses), preemptions(_preemptions),
    _id(__id), _executionTime(__et), _relDeadline(__ct), _uc(__uc), _ua(__ua), _activations(0),
    _completions(0), _cancellations(0), _misses(0), _preemptions(0),
    _execCancellations(0), _priority(__priority), _nextActivation(-1),
    _lastValue(1), delayCounter(0)//, currentSize(0)
  {
    assert(_uc != NULL);
    assert(_ua != NULL);
  }
  
  
  Task::~Task(void) {
    delete _uc;
    delete _ua;
  }


  Task::Task(const Task& rhs)
    :
    id(_id), executionTime(_executionTime), absDeadline(_relDeadline), priority(_priority),
    activations(_activations), completions(_completions),
    cancellations(_cancellations), misses(_misses), preemptions(_preemptions),
    _id(rhs._id), _executionTime(rhs._executionTime), _relDeadline(rhs._relDeadline),  _activations(0),
    _completions(0), _cancellations(0), _misses(0), _preemptions(0),
    _execCancellations(0), _priority(rhs._priority), _nextActivation(-1),
    _lastValue(1), delayCounter(0)//, currentSize(0)
  {
    //_uc(__uc), _ua(__ua),
    _uc = rhs._uc->clone();
    _ua = rhs._ua->clone();
  }
  
  
  void Task::setPriority(int prio) {
    cout << "Setting priority of task " << id << " to " << prio << endl;
    _priority = prio;
  }
  

  int Task::getPriority() const {
    return _priority;
  }

  
  unsigned int Task::getId(void) const {
    return id;
  }
  
  
  int Task::getExecutionTime(void) const {
    return _executionTime;
  }


  int Task::getRelativeDeadline(void) const {
    return _relDeadline;
  }
  

  int Task::getDistance() const {
    return 1;
  }

  
  void Task::start(int now) {
    _nextActivation = startHook(now);
    _activations = 0;
    _completions = 0;
    _cancellations = 0;
    _misses = 0;
    _preemptions = 0;
  }
  
  
  Job* Task::spawnJob(int now) {
    if (now >= _nextActivation) {
      Job* job = spawnHook(now);
      _nextActivation += getNextActivationOffset(now);
      _activations++;
      return job;
    }
    else {
      return NULL;
    }
  }
  
  
  void Task::completeJob(Job* job, int now) {
    if (job == NULL) return;
    _completions++;
    if (job->getAbsDeadline() < now) {
      _misses++;
    }
    _preemptions += job->getPreemptions();
    _lastValue = _uc->calcUtility(job, now);
    _ua->addUtility(_lastValue);
    record(_lastValue);
    //cout << "Complete " << *this << " " << *job << endl;
    completionHook(job, now);
  }
  
  
  void Task::cancelJob(Job* job) {
    if (job == NULL) return;
    _preemptions += job->getPreemptions();
    ++_cancellations;
    if (job->getRemainingExecutionTime() < job->getExecutionTime()) {
      ++_execCancellations;
    }
    _lastValue = 0;
    _ua->addUtility(_lastValue);
    record(_lastValue);
    //cout << "Cancel " << *this << " " << *job << endl;
    cancelHook(job);
  }
  
  
  std::ostream& Task::print(std::ostream& ost) const {
    ost << getIdString() << "(" << executionTime << ")";
    return ost;
  }
  

  std::string Task::strState() const {
    return "";
  }


  const std::string& Task::getClassElement() {
    return ELEM_NAME;
  }


  int Task::writeData(xmlTextWriterPtr writer) {
    xmlTextWriterWriteElement(writer, (xmlChar*)"id", STRTOXML(XmlUtils::convertToXML<int>(_id)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"executiontime", STRTOXML(XmlUtils::convertToXML<int>(_executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"criticaltime", STRTOXML(XmlUtils::convertToXML<int>(_relDeadline)));
    //_uc->write(writer);
    _uc->writeToXML(writer);
    //_ua->write(writer);
    _ua->writeToXML(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"priority", STRTOXML(XmlUtils::convertToXML<int>(_priority)));
    //xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    //xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    return 0;
  }

  
  std::ostream& operator << (std::ostream& ost, const Task& task) {
    return task.print(ost);
  }
  
  
  double Task::getPossibleExecValue(const Job *job, int startTime) const {
    return _uc->calcUtility(job, startTime + job->getRemainingExecutionTime());
  }
  
  
  double Task::getPossibleHistoryValue(const Job *job, int startTime) const {
    return _ua->predictUtility(_uc->calcUtility(job, startTime + job->getRemainingExecutionTime()));
  }
  
  
  double Task::getPossibleFailHistoryValue(const Job *job) const {
    return _ua->predictUtility(0);
  }
  
  
  int Task::getActivations(void) const {
    return _activations;
  }
  
  
  int Task::getCompletions(void) const {
    return _completions;
  }
  
  
  int Task::getCancellations(void) const {
    return _cancellations;
  }
  
  
  int Task::getMisses(void) const {
    return _misses;
  }
  
  
  int Task::getPreemptions(void) const {
    return _preemptions;
  }
  
  
  int Task::getExecCancellations(void) const {
    return _execCancellations;
  }
  
  
  const UtilityAggregator* Task::getUA() const {
    return _ua;
  }
  
  const UtilityCalculator* Task::getUC() const {
    return _uc;
  }
  
  
  std::string Task::getIdString(void) const {
    ostringstream oss;
    oss << "T" << getShortId() << id;
    return oss.str();
  }
    
  
  double Task::getLastExecValue(void) const {
    return _lastValue;
  }
  
  
  double Task::getHistoryValue(void) const {
    return _ua->getCurrentUtility();
  }
  
  
  void Task::record(double u) {
    if (u <= 0) {
      recordDelay();
    }
    else {
      recordNoDelay();
    }
  }
  

  const std::vector<int>& Task::getCounters(void) const {
    return delayCounters;
  }
  
  void Task::recordDelay() {
    ++delayCounter;
  }
  
  
  void Task::recordNoDelay() {
    if (delayCounter <= 0) {
      return;
    }
    if (delayCounters.size() < delayCounter) {
      delayCounters.resize(delayCounter, 0);
    }
    delayCounters[delayCounter-1]++;
    delayCounter = 0;
  }
  
  /*
    double Task::getPossibleExecValue(const Job *job, int startTime) const;
    double Task::getPossibleHistoryValue(const Job *job, int startTime) const;
    double Task::getPossibleFailHistoryValue(const Job *job) const;
  */
  
} // NS tmssim
