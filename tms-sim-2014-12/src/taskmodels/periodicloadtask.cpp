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

  PeriodicLoadTask::PeriodicLoadTask(
        unsigned int id,
        int period,
        int executionTime,
        int relDeadline,
        UtilityCalculator* utilityCalculator,
        UtilityAggregator* utilityAggregator,
        int offset,
        int priority,
        const GenodeConfig& config
  ) :
    Task(id, executionTime, relDeadline, utilityCalculator, utilityAggregator, priority),
    _period(period), _offset(offset), _lastUtility(0.0), _historyUtility(1.0), _config(config)
  {
  }

  PeriodicLoadTask::PeriodicLoadTask(const PeriodicLoadTask& rhs)
    : Task(rhs),
      _period(rhs._period), _offset(0), _lastUtility(0), _historyUtility(1) {
  }
  
  
  int PeriodicLoadTask::startHook(int now) {
    return now + _offset;
  }
  
  
  Job* PeriodicLoadTask::spawnHook(int now) {
    return new Job(this, activations, now, executionTime, now + absDeadline, getPriority());
  }
  
  
  int PeriodicLoadTask::getNextActivationOffset(int now) {
    return _period;
  }
  
  
  bool PeriodicLoadTask::completionHook(Job *job, int now) {
    _lastUtility = calcExecValue(job, now); // TODO: REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  bool PeriodicLoadTask::cancelHook(Job *job) {
    _lastUtility = 0; // REMOVE
    advanceHistory();
    delete job;
    return true;
  }
  
  
  std::ostream& PeriodicLoadTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << _period << "/" << executionTime << "/" << absDeadline << ")";
    ost << " [" << _lastUtility << ";" << _historyUtility << "]";
    return ost;
  }
  
  
  double PeriodicLoadTask::getLastExecValue(void) const {
    return _lastUtility;
  }
  
  
  double PeriodicLoadTask::getHistoryValue(void) const {
    return _historyUtility;
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
    return (_historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double PeriodicLoadTask::calcFailHistoryValue(const Job *job) const {
    return _historyUtility/2;
  }
  
  
  void PeriodicLoadTask::advanceHistory(void) {
    _historyUtility = (_historyUtility + _lastUtility) / 2;
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
    int quota = 2 * 1024 * 1024;
    if (pkg == "tumatmul")
    {
      quota = 8 * 1024 * 1024;
    }

    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(_period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(_offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"quota", STRTOXML(XmlUtils::convertToXML<int>(quota)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"pkg", STRTOXML(XmlUtils::convertToXML<string>(pkg)));

    xmlTextWriterStartElement(writer, (xmlChar*)"config");
    xmlTextWriterWriteElement(writer, (xmlChar*)"matrixSize", STRTOXML(XmlUtils::convertToXML<int>(_config.matrixSize)));
    xmlTextWriterEndElement(writer);
    return 0;
  }

  /*
  int PeriodicTask::getPeriod() const { return period; }
  int PeriodicTask::getOffset const { return offset; }
  */
} // NS tmssim
