/**
 * $Id: mktask.cpp 752 2014-10-02 15:12:16Z klugeflo $
 * @file mktask.h
 * @brief General task with (m,k)-firm real-time constraints
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <taskmodels/mktask.h>
#include <utils/logger.h>
#include <utils/tlogger.h>

#include <cassert>

using namespace std;

namespace tmssim {

  static const string ELEM_NAME = "mktask";

  MKTask::MKTask(unsigned int _id, int _period, int _et, int _ct,
		 UtilityCalculator* _uc, UtilityAggregator* _ua,
		 unsigned int _priority, unsigned int _m, unsigned int _k,
		 unsigned int _spin)
    : PeriodicTask(_id, _period, _et, _ct, _uc, _ua, 0, _priority),
      m(_m), k(_k), spin(_spin), monitor(_m, _k)
  {
    //monitor = new MKMonitor(m, k);
  }


  MKTask::MKTask(const MKTask& rhs)
    : PeriodicTask(rhs),
      m(rhs.m), k(rhs.k), spin(rhs.spin), monitor(rhs.m, rhs.k)
  {
    //monitor = new MKMonitor(m, k);
  }


  MKTask::MKTask(const MKTask* rhs)
    : PeriodicTask(*rhs),
      m(rhs->m), k(rhs->k), spin(rhs->spin), monitor(rhs->m, rhs->k)
  {
    //monitor = new MKMonitor(m, k);
  }


  MKTask::~MKTask() {
    //delete monitor;
  }


  int MKTask::getDistance() const {
    /*const unsigned int* vals = monitor.getVals();
      unsigned int pos = monitor.getPos();*/
    int l = calcL(m);
    assert(l >= 0);
    int distance = k - l + 1;
    return distance;
  }


  int MKTask::calcL(int n, const unsigned int* vals, unsigned int pos) const {
    if (n > k) {
      tError() << "Invalid parameter n=" << n << " > " << k << "!";
      return -1;
    }
    unsigned int p;
    if (pos == 0)
      p = k - 1;
    else
      p = pos - 1;

    unsigned int ctr = 0;
    unsigned int i;
    for (i = 0; i < k; ++i) {
      if (vals[p] == 1)
	ctr++;
      if (ctr == n)
	break;

      if (p == 0)
	p = k - 1;
      else
	--p;
    }
    return i + 1;
  }


  int MKTask::calcL(int n) const {
    return calcL(n, monitor.getVals(), monitor.getPos());
  }


  bool MKTask::completionHook(Job *job, int now) {
    monitor.push(1);
    PeriodicTask::completionHook(job, now);
  }


  bool MKTask::cancelHook(Job *job) {
    monitor.push(0);
    if (monitor.getCurrentSum() < m) {
      LOG(4) << "(" << m << "," << k << ") condition violated by job " << job->getIdString() << "(k=" << monitor.getCurrentSum() << ")";
    }
    PeriodicTask::cancelHook(job);
  }


  const MKMonitor& MKTask::getMonitor() const {
    return monitor;
  }


  const std::string& MKTask::getClassElement() {
    return ELEM_NAME;
  }


  int MKTask::writeData(xmlTextWriterPtr writer) {
    PeriodicTask::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*) "m", STRTOXML(XmlUtils::convertToXML<int>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "k", STRTOXML(XmlUtils::convertToXML<int>(this->k)));
    xmlTextWriterWriteElement(writer, (xmlChar*) "spin", STRTOXML(XmlUtils::convertToXML<int>(this->spin)));
    return 0;
  }


  std::ostream& MKTask::print(std::ostream& ost) const {
    PeriodicTask::print(ost);
    ost << " (" << m << "," << k << ") s=" << spin;
    return ost;
  }


  char MKTask::getShortId(void) const {
    return 'M';
  }


  std::string MKTask::strState() const {
    ostringstream oss;
    oss << "{" << monitor.getCurrentSum() << "/" << k << "|" << getUA()->getCurrentUtility() << "->" << getUA()->predictUtility(0) << "}";
    return oss.str();
  }

} // NS tmssim
