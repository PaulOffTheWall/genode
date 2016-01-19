/**
 * $Id: uamean.cpp 677 2014-09-09 12:49:34Z klugeflo $
 * @file uamean.cpp
 * @brief Implementation of aggregator with equal weight
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uamean.h>

namespace tmssim {

  static const string ELEM_NAME = "uawmean";


  UAMean::UAMean() {
  }
  
  
  UAMean::~UAMean(void) {
  }
  
  
  double UAMean::getCurrentUtility(void) const {
    return getMeanUtility();
  }
  
  
  double UAMean::predictUtility(double u) const {
    double nextU = getTotal() + u;
    int nextC = getCount() + 1;
    return nextU / nextC;
  }
  
  
  void UAMean::addHook(double u) {
    // do nothing
  }
  
  void UAMean::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uamean");
    xmlTextWriterEndElement(writer);
  }
  
  UtilityAggregator* UAMean::clone() const {
    UtilityAggregator* ua = new UAMean();
    return ua;
  }
  

  const std::string& UAMean::getClassElement() {
    return ELEM_NAME;
  }


  int UAMean::writeData(xmlTextWriterPtr writer) {
    return UtilityAggregator::writeData(writer);
  }


} // NS tmssim

