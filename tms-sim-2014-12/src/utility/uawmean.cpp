/**
 * $Id: uawmean.cpp 677 2014-09-09 12:49:34Z klugeflo $
 * @file uawmean.cpp
 * @brief Implementation of aggregator over sliding window with average calculation
 * @authors Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/uawmean.h>

#include <xmlio/xmlutils.h>

namespace tmssim {

  static const string ELEM_NAME = "uawmean";

  UAWMean::UAWMean(size_t _size)
    : UAWindow(_size), currentSum(0)
  {
  }
  
  
  UAWMean::~UAWMean(void) {
  }
  
  
  double UAWMean::getCurrentUtility(void) const {
    return currentSum / getSize();
  }
  
  
  double UAWMean::predictUtility(double u) const {
    double s = currentSum;
    // need to subtract oldest entry
    s -= oldest();
    s += u;
    return s / getSize();
  }
  
  
  void UAWMean::addHook(double u) {
    currentSum -= oldest();
    UAWindow::addHook(u);
    currentSum += u;
  }


  UtilityAggregator* UAWMean::clone() const {
    UtilityAggregator* ua = new UAWMean(this->getSize());
    return ua;
  }

  
  void UAWMean::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uawmean");
    xmlTextWriterWriteElement(writer, (xmlChar*)"size", STRTOXML(XmlUtils::convertToXML<size_t>(this->getSize())));
    xmlTextWriterEndElement(writer);
  }


  const std::string& UAWMean::getClassElement() {
    return ELEM_NAME;
  }


  int UAWMean::writeData(xmlTextWriterPtr writer) {
    UAWindow::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"size", STRTOXML(XmlUtils::convertToXML<size_t>(this->getSize())));
    return 0;
  }

} // NS tmssim
