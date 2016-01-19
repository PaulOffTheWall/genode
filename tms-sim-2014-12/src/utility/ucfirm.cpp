/**
 * $Id: ucfirm.cpp 788 2014-12-09 12:59:15Z klugeflo $
 * @file ucfirm.cpp
 * @brief Implementation of firm real-time utility calculator
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>, Peter Ittner
 */

#include <utility/ucfirm.h>

namespace tmssim {

  static const string ELEM_NAME = "ucfirmrt";

  UCFirmRT::UCFirmRT() {
  }
  
  
  UCFirmRT::~UCFirmRT(void) {
  }
  
  
  double UCFirmRT::calcUtility(const Job *job, int complTime) const {
    if (complTime <= job->getAbsDeadline()) {
      return 1;
    }
    else {
      return 0;
    }
  }
  
  UtilityCalculator* UCFirmRT::clone() const {
    UtilityCalculator* uc = new UCFirmRT();
    return uc;
  }
  
  void UCFirmRT::write(xmlTextWriterPtr writer) const {
    xmlTextWriterStartElement(writer,(xmlChar*) "ucfirmrt");
    xmlTextWriterEndElement(writer);
  }


  const std::string& UCFirmRT::getClassElement() {
    return ELEM_NAME;
  }


  int UCFirmRT::writeData(xmlTextWriterPtr writer) {
    UtilityCalculator::writeData(writer);
    return 0;
  }

} // NS tmssim
