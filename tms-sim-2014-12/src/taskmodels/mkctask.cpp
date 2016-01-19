/**
 * $Id: mkctask.cpp 815 2014-12-15 13:40:52Z klugeflo $
 * @file mkctask.cpp
 * @brief Implementation of (m,k)-firm real-time task for control applications
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mkctask.h>
#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <cmath>

#include <limits.h>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "mkctask";

  MKCTask::MKCTask(unsigned int _id, int _period, int _et, int _ct,
		   UtilityCalculator* _uc, UtilityAggregator* _ua,
		   unsigned int _priority, unsigned int _m, unsigned int _k,
		   unsigned int _spin, bool _useSpin)
    : MKTask(_id, _period, _et, _ct, _uc, _ua, _priority, _m, _k, _spin)
      //, a(0)
  {
    if (_useSpin)
      actSpin = spin;
    else
      actSpin = 0;
    //monitor = new MKMonitor(m, k);
    tDebug() << "Created mkctask " << _id << " priority " << _priority << " task prio " << priority << "(" << m << "," << k << ") @ " << this;
    tDebug() << "\tMonitorsum: " << monitor.getCurrentSum();
  }


  MKCTask::MKCTask(const MKCTask& rhs)
    : MKTask(rhs)
      //, a(0)
  {
    tDebug() << "Copied mkctask to " << this;
  }


  MKCTask::MKCTask(const MKTask* rhs)
    : MKTask(rhs)
      //, a(0)
  {
    //tDebug() << "Copied mkctask to " << this;
  }


  MKCTask::~MKCTask() {
    //delete monitor;
  }

  /**
   * we can use the activations field of the Task class instead of keeping
   * another counter a
   */
  Job* MKCTask::spawnHook(int now) {
    int _prio;
    //double tmp = floor(ceil( (double)(activations + actSpin) * (double)m / (double)k) * (double)k / (double)m);
    //if (int(tmp) == activations) { // mandatory
    if (isJobMandatory(activations)) {
      _prio = priority;
    }
    else { // optional
      _prio = INT_MAX;
    }
    Job* job = new Job(this, activations, now, executionTime, now + absDeadline, _prio);
    tDebug() << this << " Created mkjob " << *job /*<< " for tmp=" << tmp*/
	     << " a=" << activations << " taskprio " << priority
	     << " @ " << job << " t@ " << job->getTask();
    //++a;
    return job;
  }


  bool MKCTask::isJobMandatory(unsigned int jobId) const {
    double tmp = floor(ceil( (double)(jobId + actSpin) * (double)m / (double)k) * (double)k / (double)m);
    return tmp == jobId;
  }


  void MKCTask::enableSpin() {
    actSpin = spin;
  }


  void MKCTask::disableSpin() {
    actSpin = 0;
  }


  bool MKCTask::completionHook(Job *job, int now) {
    MKTask::completionHook(job, now);
    //delete job;
  }


  bool MKCTask::cancelHook(Job *job) {
    MKTask::cancelHook(job);
    //delete job;
  }


  char MKCTask::getShortId(void) {
    return 'M';
  }

  /*
  void MKCTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer, (xmlChar*) "mkctask");
    xmlTextWriterWriteElement(writer, (xmlChar*) "id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*) "executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    
    xmlTextWriterWriteElement(writer, (xmlChar*) "priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "m", STRTOXML(XmlUtils::convertToXML<int>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "k", STRTOXML(XmlUtils::convertToXML<int>(this->k)));
    xmlTextWriterEndElement(writer);
  }
  */
  
  Task* MKCTask::clone() const {
    return new MKCTask(*this);
  }


  const std::string& MKCTask::getClassElement() {
    return ELEM_NAME;
  }


  int MKCTask::writeData(xmlTextWriterPtr writer) {
    MKTask::writeData(writer);
    return 0;
  }

} // NS tmssim
