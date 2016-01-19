/**
 * $Id: sporadictask.cpp 815 2014-12-15 13:40:52Z klugeflo $
 * @file sporadictask.cpp
 * @brief Implementation of sporadic task class
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/sporadictask.h>
#include <xmlio/xmlutils.h>

#include <cstdlib>

#include <iostream>
using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "sporadictask";
  
  
  SporadicTask::SporadicTask(unsigned int _id, int _minPeriod, int _et,
			     unsigned int _seed, int _ct, UtilityCalculator* __uc, UtilityAggregator* __ua, double _probability,
			     int _offset, int _prio)
    : Task(_id, _et, _ct, __uc, __ua, _prio),
      minPeriod(_minPeriod), /*ct(_ct),*/ offset(_offset),
      lastUtility(0), historyUtility(1), activationPending(false), seed(_seed),
      probability(_probability), baseSeed(_seed) {
    
  }
  
  
  SporadicTask::SporadicTask(unsigned int _id, int _minPeriod, int _et, unsigned int _seed, UtilityCalculator* __uc, UtilityAggregator* __ua, int _prio)
    : Task(_id, _et, _minPeriod, __uc, __ua, _prio),
      minPeriod(_minPeriod), /*ct(_minPeriod),*/ offset(0),
      lastUtility(0), historyUtility(1), activationPending(false), seed(_seed),
      probability(0.5), baseSeed(_seed) { //, m_iPrio(_prio) {
  }


  SporadicTask::SporadicTask(const SporadicTask &rhs)
    : Task(rhs), minPeriod(rhs.minPeriod), offset(rhs.offset),
      lastUtility(0), historyUtility(1), activationPending(false),
      seed(rhs.seed), probability(rhs.probability), baseSeed(rhs.seed)
  {
  }
  
  /*
    Task* SporadicTask::clone(void) const {
    return SporadicTask(id, p, et, ct, o);
    }
  */
  int SporadicTask::startHook(int now) {
    seed = baseSeed;
    return now + offset;
  }
  

  Job* SporadicTask::spawnHook(int now) {
    return new Job(this, activations, now, executionTime, now + absDeadline, getPriority());
  }
  
  
  int SporadicTask::getNextActivationOffset(int now) {
    unsigned int rval;
    int i = 0;
    //cout << "ST " << *this << " prob: " << probability << " mp: " << minPeriod << endl;
    while ( (rval=rand_r(&seed)) < (RAND_MAX * probability)) {
      //cout << "\trval=" << rval << " (RAND_MAX=" << RAND_MAX << ")" << endl;
      ++i;
    }
    
    return minPeriod + i;
  }
  
  
  bool SporadicTask::completionHook(Job *job, int now) {
    lastUtility = calcExecValue(job, now); // TODO: use new utility classes!
    advanceHistory();
    delete job;
    return true;
  }
  
  
  bool SporadicTask::cancelHook(Job *job) {
    lastUtility = 0;
    advanceHistory();
    delete job;
    return true;
  }
  
  
  std::ostream& SporadicTask::print(std::ostream& ost) const {
    ost << getIdString() << "(" << minPeriod << "/" << executionTime << "/" << absDeadline << ")";
    ost << " [" << lastUtility << ";" << historyUtility << "]";
    return ost;
  }
  
  
  double SporadicTask::getLastExecValue(void) const {
    return lastUtility;
  }
  
  
  double SporadicTask::getHistoryValue(void) const {
    return historyUtility;
  }
  
  
  double SporadicTask::calcExecValue(const Job *job, int complTime) const {
    if (complTime <= job->getAbsDeadline()) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  
  double SporadicTask::calcHistoryValue(const Job *job, int complTime) const {
    return (historyUtility + calcExecValue(job, complTime)) / 2;
  }
  
  
  double SporadicTask::calcFailHistoryValue(const Job *job) const {
    return historyUtility/2;
  }
  
  
  void SporadicTask::advanceHistory(void) {
    historyUtility = (historyUtility + lastUtility) / 2;
    //std::cout << "Advanced history to " << historyUtility << std::endl;
  }
  
  
  char SporadicTask::getShortId(void) const {
    return 'S';
  }
  
  Task* SporadicTask::clone() const {
    /*
    Task* t = new SporadicTask(id,minPeriod,et,seed,ct,this->getUC()->clone(),this->getUA()->clone(),probability,offset,priority);
    return t;
    */
    return new SporadicTask(*this);
  }

  /*
  void SporadicTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "sporadictask");
    xmlTextWriterWriteElement(writer, (xmlChar*)"id", STRTOXML(XmlUtils::convertToXML<int>(getId())));
    xmlTextWriterWriteElement(writer, (xmlChar*)"executiontime", STRTOXML(XmlUtils::convertToXML<int>(this->executionTime)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"criticaltime", STRTOXML(XmlUtils::convertToXML<int>(this->absDeadline)));
    
    this->getUC()->write(writer);
    this->getUA()->write(writer);
    
    xmlTextWriterWriteElement(writer, (xmlChar*)"priority", STRTOXML(XmlUtils::convertToXML<int>(this->priority)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"minperiod", STRTOXML(XmlUtils::convertToXML<int>(this->minPeriod)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"seed", STRTOXML(XmlUtils::convertToXML<unsigned int>(this->seed,std::hex)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"probability", STRTOXML(XmlUtils::convertToXML<double>(this->probability)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    xmlTextWriterEndElement(writer);
  }
  */
  
  const std::string& SporadicTask::getClassElement() {
    return ELEM_NAME;
  }


  int SporadicTask::writeData(xmlTextWriterPtr writer) {
    Task::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"minperiod", STRTOXML(XmlUtils::convertToXML<int>(this->minPeriod)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"seed", STRTOXML(XmlUtils::convertToXML<unsigned int>(this->seed,std::hex)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"probability", STRTOXML(XmlUtils::convertToXML<double>(this->probability)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"offset", STRTOXML(XmlUtils::convertToXML<int>(this->offset)));
    return 0;
  }
  
} // NS tmssim
