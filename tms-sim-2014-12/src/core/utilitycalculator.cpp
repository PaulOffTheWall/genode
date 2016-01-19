/**
 * $Id: utilitycalculator.cpp 677 2014-09-09 12:49:34Z klugeflo $
 * @file utilitycalculator.cpp
 * @brief Implementation of the UtilityCalculator superclass
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/scobjects.h>

namespace tmssim {
  /*  
  UtilityCalculator::UtilityCalculator() {
  }
  */

  /**
   * Even though this function is declared abstract we need to provide
   * an implementation to ensure correct compilation/linking of subclasses
   */
  UtilityCalculator::~UtilityCalculator(void) {
  }


  int UtilityCalculator::writeData(xmlTextWriterPtr writer) {
    return 0;
  }

} // NS tmssim
