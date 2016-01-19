/**
 * $Id: periodicloadtask.cpp
 * @file periodicloadtask.cpp
 * @brief Implementation of periodic load task class
 * @author Avinash Kundaliya <avinash.kundaliya@tum.de>
 */

#include <taskmodels/periodicloadtask.h>
#include <xmlio/xmlutils.h>

#include <iostream>
#include <string>
#include <stdlib.h>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "periodictask";

  PeriodicLoadTask::PeriodicLoadTask(unsigned int _id,
                                     int _period,
                                     int _et,
                                     int _ct,
                                     UtilityCalculator* __uc,
                                     UtilityAggregator* __ua,
                                     int o,
                                     int _prio,
                                     int _matrixSize)
    : Task(_id, _et, _ct, __uc, __ua, _prio),
      period(_period), offset(o), lastUtility(0), historyUtility(1), matrixSize(_matrixSize) {
  }
  

  PeriodicLoadTask::PeriodicLoadTask(const PeriodicLoadTask& rhs)
    : Task(rhs),
      period(rhs.period), offset(0), lastUtility(0), historyUtility(1) {
  }
  
  
  int PeriodicLoadTask::startHook(int now) {
    return now + offset;
  }
  
  
  Job* PeriodicLoadTask::spawnHook(int now) {
    return new Job(this, activations, now, executionTime, now + absDeadline, getPriority());
  }
  
  
  int PeriodicLoadTask::getNextActivationOffset(int now) {
    return period;
  }
  
  
  bool PeriodicLoadTask::completionHook(Job *job, int now) {
    lastUtility = calcExecValue(job, now); // TODO: REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  bool PeriodicLoadTask::cancelHook(Job *job) {
    lastUtility = 0; // REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  std::ostream& PeriodicLoadTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << period << "/" << executionTime << "/" << absDeadline << ")";
    ost << " [" << lastUtility << ";" << historyUtility << "]";
    return ost;
  }
  
  
  double PeriodicLoadTask::getLastExecValue(void) const {
    return lastUtility;
  }
  
  
  double PeriodicLoadTask::getHistoryValue(void) const {
    return historyUtility;
  }
  
  
  double PeriodicLoadTask::calcExecValue(const Job *job, int complTime) const {
  if (complTime <= job->getAbsDeadline()) {
    return 1;
  }
  else {
    return 0;
  }
}


  double PeriodicLoadTask::calcHistoryValue(const Job *job, int complTime) const {
    return (historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double PeriodicLoadTask::calcFailHistoryValue(const Job *job) const {
    return historyUtility/2;
  }
  
  
  void PeriodicLoadTask::advanceHistory(void) {
    historyUtility = (historyUtility + lastUtility) / 2;
  }
  
  
  char PeriodicLoadTask::getShortId(void) const {
    return 'P';
  }


  Task* PeriodicLoadTask::clone() const {
    return new PeriodicLoadTask(*this);
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

  const std::string& PeriodicLoadTask::getClassElement() {
    return ELEM_NAME;
  }


  int PeriodicLoadTask::writeData(xmlTextWriterPtr writer) {
    string tasks[] = {"hey", "namaste", "tumatmul"};
    string pkg = tasks[ rand() % 3 ]; // size of tasks array

    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"matrixSize", STRTOXML(XmlUtils::convertToXML<int>(matrixSize)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"pkg", STRTOXML(XmlUtils::convertToXML<string>(pkg)));
    return 0;
  }

  /*
  int PeriodicTask::getPeriod() const { return period; }
  int PeriodicTask::getOffset const { return offset; }
  */
} // NS tmssim
