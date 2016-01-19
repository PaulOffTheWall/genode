/**
 * $Id: dbptask.cpp 815 2014-12-15 13:40:52Z klugeflo $
 * @file dbptask.h
 * @brief Task with distance-based priority assignment
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/dbptask.h>
#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <cassert>

using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "dbptask";

  DBPTask::DBPTask(unsigned int _id, int _period, int _et, int _ct,
		   UtilityCalculator* _uc, UtilityAggregator* _ua,
		   unsigned int _priority, unsigned int _m, unsigned int _k)
    : MKTask(_id, _period, _et, _ct, _uc, _ua, _priority, _m, _k)
  {
    tDebug() << "Created dbptask " << _id << " priority " << _priority << " task prio " << priority << "(" << m << "," << k << ")";
    tDebug() << "\tMonitorsum: " << monitor.getCurrentSum();
  }


  DBPTask::DBPTask(const DBPTask& rhs)
    : MKTask(rhs)
  {

  }


  DBPTask::DBPTask(const MKTask* rhs)
    : MKTask(rhs)
  {

  }


  DBPTask::~DBPTask() {

  }


  Job* DBPTask::spawnHook(int now) {
    int dbPriority = getDistance();
    Job* job = new Job(this, activations, now, executionTime, now + absDeadline, dbPriority);
    tDebug() << "Created dbpjob " << *job << " taskprio " << dbPriority;
    return job;

    return NULL;
  }


  bool DBPTask::completionHook(Job *job, int now) {
    MKTask::completionHook(job, now);
    //delete job;
  }


  bool DBPTask::cancelHook(Job *job) {
    MKTask::cancelHook(job);
    //delete job;
  }


  char DBPTask::getShortId(void) {
    return 'M';
  }

  /*
  void DBPTask::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer, (xmlChar*) "dbptask");
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
  
  Task* DBPTask::clone() const {
    return new DBPTask(*this);
  }


  const std::string& DBPTask::getClassElement() {
    return ELEM_NAME;
  }


  int DBPTask::writeData(xmlTextWriterPtr writer) {
    MKTask::writeData(writer);
    return 0;
  }

} // NS tmssim
