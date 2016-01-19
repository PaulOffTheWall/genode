/**
 * $Id: uawmk.cpp 677 2014-09-09 12:49:34Z klugeflo $
 * @file uawmk.cpp
 * @brief Utility Aggregator for utility-base (m,k)-firm scheduling
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <utility/uawmk.h>

#include <xmlio/xmlutils.h>

namespace tmssim {

  static const string ELEM_NAME = "uawmk";

  UAWMK::UAWMK(unsigned int _m, unsigned int _k)
    : UAWindow(_k),
      m(_m), k(_k), currentSum(_k)
  {
  }


  UAWMK::UAWMK(const UAWMK& rhs)
    : UAWindow(rhs), m(rhs.m), k(rhs.k), currentSum(rhs.currentSum)
  {
  }


  UAWMK::~UAWMK() {
  }


  double UAWMK::getCurrentUtility(void) const {
    //return currentSum / getSize() * k / m;
    // actually, it's the above, but getSize()==k
    return currentSum / m;
  }


  double UAWMK::predictUtility(double u) const {
    double s = currentSum;
    // need to subtract oldest entry
    s -= oldest();
    s += u;
    return s / m;
  }

    
  void UAWMK::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "uawmk");
    xmlTextWriterWriteElement(writer, (xmlChar*)"m", STRTOXML(XmlUtils::convertToXML<size_t>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"k", STRTOXML(XmlUtils::convertToXML<size_t>(this->k)));
    xmlTextWriterEndElement(writer);
  }

    
  UtilityAggregator* UAWMK::clone() const {
    return new UAWMK(*this);
  }


  void UAWMK::addHook(double u) {
    currentSum -= oldest();
    UAWindow::addHook(u);
    currentSum += u;
  }


  const std::string& UAWMK::getClassElement() {
    return ELEM_NAME;
  }


  int UAWMK::writeData(xmlTextWriterPtr writer) {
    UAWindow::writeData(writer);
    xmlTextWriterWriteElement(writer, (xmlChar*)"m", STRTOXML(XmlUtils::convertToXML<size_t>(this->m)));
    xmlTextWriterWriteElement(writer, (xmlChar*)"k", STRTOXML(XmlUtils::convertToXML<size_t>(this->k)));
    return 0;
  }


} // NS tmssim
