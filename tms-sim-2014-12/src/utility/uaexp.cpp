/**
 * $Id: uaexp.cpp 677 2014-09-09 12:49:34Z klugeflo $
 * @file uaexp.cpp
 * @brief Implementation of aggregator with exponentially decreasing weight
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uaexp.h>
#include <xmlio/xmlutils.h>

namespace tmssim {


  static const string ELEM_NAME = "uaexp";

  UAExp::UAExp(double _w)
    : weight(_w), currentValue(1) {
    // TODO: we need to ensure that 0 < _w < 1!
  }
  
  
  UAExp::~UAExp(void) {
  }
  
  
  double UAExp::getCurrentUtility(void) const {
    return currentValue;
  }
  
  
  double UAExp::predictUtility(double u) const {
    return calcValue(u);
  }
  
  
  void UAExp::addHook(double u) {
    currentValue = calcValue(u);
  }
  
  
  double UAExp::calcValue(double u) const {
    return ( (1 - weight) * currentValue ) + weight * u;
  }
  
  double UAExp::getWeight() const {
    return this->weight;
  }
  
  void UAExp::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uaexp");
    xmlTextWriterWriteElement(writer, (xmlChar*)"weight", STRTOXML(XmlUtils::convertToXML<double>(this->weight)));
    xmlTextWriterEndElement(writer);
  }

  UtilityAggregator* UAExp::clone() const {
    UtilityAggregator* ua = new UAExp(this->weight);
    return ua;
  }


  const std::string& UAExp::getClassElement() {
    return ELEM_NAME;
  }


  int UAExp::writeData(xmlTextWriterPtr writer) {
    UtilityAggregator::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"weight", STRTOXML(XmlUtils::convertToXML<double>(this->weight)));
    return 0;
  }
  
} // NS tmssim
