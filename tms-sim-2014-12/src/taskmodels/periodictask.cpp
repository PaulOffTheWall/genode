/**
 * $Id: periodictask.cpp 815 2014-12-15 13:40:52Z klugeflo $
 * @file periodictask.cpp
 * @brief Implementation of periodic task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/periodictask.h>
#include <xmlio/xmlutils.h>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "periodictask";

  PeriodicTask::PeriodicTask(unsigned int _id, int _period, int _et, int _ct,
			     UtilityCalculator* __uc, UtilityAggregator* __ua,
			     int o, int _prio)
    : Task(_id, _et, _ct, __uc, __ua, _prio),
      period(_period), offset(o), lastUtility(0), historyUtility(1) {
  }
  

  PeriodicTask::PeriodicTask(const PeriodicTask& rhs)
    : Task(rhs),
      period(rhs.period), offset(0), lastUtility(0), historyUtility(1) {
  }
  
  
  int PeriodicTask::startHook(int now) {
    return now + offset;
  }
  
  
  Job* PeriodicTask::spawnHook(int now) {
    return new Job(this, activations, now, executionTime, now + absDeadline, getPriority());
  }
  
  
  int PeriodicTask::getNextActivationOffset(int now) {
    return period;
  }
  
  
  bool PeriodicTask::completionHook(Job *job, int now) {
    lastUtility = calcExecValue(job, now); // TODO: REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  bool PeriodicTask::cancelHook(Job *job) {
    lastUtility = 0; // REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  std::ostream& PeriodicTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << period << "/" << executionTime << "/" << absDeadline << ")";
    ost << " [" << lastUtility << ";" << historyUtility << "]";
    return ost;
  }
  
  
  double PeriodicTask::getLastExecValue(void) const {
    return lastUtility;
  }
  
  
  double PeriodicTask::getHistoryValue(void) const {
    return historyUtility;
  }
  
  
  double PeriodicTask::calcExecValue(const Job *job, int complTime) const {
  if (complTime <= job->getAbsDeadline()) {
    return 1;
  }
  else {
    return 0;
  }
}


  double PeriodicTask::calcHistoryValue(const Job *job, int complTime) const {
    return (historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double PeriodicTask::calcFailHistoryValue(const Job *job) const {
    return historyUtility/2;
  }
  
  
  void PeriodicTask::advanceHistory(void) {
    historyUtility = (historyUtility + lastUtility) / 2;
  }
  
  
  char PeriodicTask::getShortId(void) const {
    return 'P';
  }


  Task* PeriodicTask::clone() const {
    return new PeriodicTask(*this);
  }

  /*
  void PeriodicTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "periodictask");
    xmlTextWriterWriteElement(writer, (xmlChar*)"id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*)"executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterEndElement(writer);
  }
  */

  const std::string& PeriodicTask::getClassElement() {
    return ELEM_NAME;
  }


  int PeriodicTask::writeData(xmlTextWriterPtr writer) {
    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(offset)));
    return 0;
  }

  /*
  int PeriodicTask::getPeriod() const { return period; }
  int PeriodicTask::getOffset const { return offset; }
  */
} // NS tmssim
