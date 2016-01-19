/**
 * $Id: sptask.cpp 815 2014-12-15 13:40:52Z klugeflo $
 * @file sptask.cpp
 * @brief Implementation of sporadic periodic task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/sptask.h>

#include <xmlio/xmlutils.h>
#include <cstdlib>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "sptask";


  SPTask::SPTask(unsigned int _id, int _period, int _et, unsigned int _seed,
		 int _ct, UtilityCalculator* __uc, UtilityAggregator* __ua,
		 double _probability, int _offset, int _prio)
    : Task(_id, _et, _ct, __uc, __ua, _prio),
      period(_period), offset(_offset),
      lastUtility(0), historyUtility(1), activationPending(false), seed(_seed),
      probability(_probability), baseSeed(_seed)
  {    
  }
  
  
  SPTask::SPTask(unsigned int _id, int _period, int _et, unsigned int _seed,
		 UtilityCalculator* __uc, UtilityAggregator* __ua, int _prio)
    : Task(_id, _et, _period, __uc, __ua, _prio),
      period(_period), offset(0),
      lastUtility(0), historyUtility(1), activationPending(false), seed(_seed),
      probability(0.5), baseSeed(_seed)
  {
  }


  SPTask::SPTask(const SPTask &rhs)
    : Task(rhs), period(rhs.period), offset(rhs.offset),
      lastUtility(0), historyUtility(1), activationPending(false),
      seed(rhs.seed), probability(rhs.probability), baseSeed(rhs.seed)
  {
  }
  
  
  int SPTask::startHook(int now) {
    seed = baseSeed;
    return now + offset;
  }
  
  
  Job* SPTask::spawnHook(int now) {
    return new Job(this, activations, now, executionTime, now + absDeadline, getPriority());
  }
  
  
  int SPTask::getNextActivationOffset(int now) {
    unsigned int rval;
    int i = 1;
    while ( (rval=rand_r(&seed)) < (RAND_MAX * probability)) {
      ++i;
    }
    return i*period;
  }
  
  
  bool SPTask::completionHook(Job *job, int now) {
    lastUtility = calcExecValue(job, now); // TODO: use new utility classes!
    advanceHistory();
    delete job;
    return true;
  }

  
  bool SPTask::cancelHook(Job *job) {
    lastUtility = 0;
    advanceHistory();
    delete job;
    return true;
  }
  

  std::ostream& SPTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << period << "/" << executionTime << "/" << absDeadline << ")";
    ost << " [" << lastUtility << ";" << historyUtility << "]";
    return ost;
  }
  
  
  double SPTask::getLastExecValue(void) const {
    return lastUtility;
  }
  
  
  double SPTask::getHistoryValue(void) const {
    return historyUtility;
  }
  
  
  double SPTask::calcExecValue(const Job *job, int complTime) const {
    if (complTime <= job->getAbsDeadline()) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  
  double SPTask::calcHistoryValue(const Job *job, int complTime) const {
    return (historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double SPTask::calcFailHistoryValue(const Job *job) const {
    return historyUtility/2;
  }
  
  
  void SPTask::advanceHistory(void) {
    historyUtility = (historyUtility + lastUtility) / 2;
    //std::cout << "Advanced history to " << historyUtility << std::endl;
  }
  
  
  char SPTask::getShortId(void) const {
    return 'p';
  }

  //Task* t = new PeriodicTask(this->getId(),this->period,this->getEt(),ct,this->getUC().clone(),this->getUA().clone(),this->offset,this->getPrio());

  Task* SPTask::clone() const {
    /*
    Task* t = new SPTask(this->getId(), this->period, this->getEt(),
			 this->seed, this->ct, this->getUC()->clone(),
			 this->getUA()->clone(), this->probability,
			 this->offset, this->getPriority());
    return t;
    */
    return new SPTask(*this);
  }

  /*
  void SPTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "sporadicperiodictask");
    xmlTextWriterWriteElement(writer, (xmlChar*)"id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*)"executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    
    xmlTextWriterWriteElement(writer, (xmlChar*)"priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"seed", STRTOXML(XmlUtils::convertToXML<unsigned int>(this->seed, std::hex)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"probability", STRTOXML(XmlUtils::convertToXML<double>(this->probability)));
    xmlTextWriterEndElement(writer);
  }
  */
  
  const std::string& SPTask::getClassElement() {
    return ELEM_NAME;
  }


  int SPTask::writeData(xmlTextWriterPtr writer) {
    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"period", STRTOXML(XmlUtils::convertToXML<int>(this->period)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"seed", STRTOXML(XmlUtils::convertToXML<unsigned int>(this->seed, std::hex)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"probability", STRTOXML(XmlUtils::convertToXML<double>(this->probability)));
    return 0;
  }
  
} // NS tmssim
