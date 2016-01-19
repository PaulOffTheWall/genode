/**
 * $Id: utilityaggregator.cpp 685 2014-09-12 08:47:42Z klugeflo $
 * @file utilityaggregator.cpp
 * @brief Aggregation of job utilities into a task utility
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/utilityaggregator.h>
//#include <core/scobjects.h>

#include <cmath>
#include <iostream>

namespace tmssim {

  UtilityAggregator::UtilityAggregator()
    : utilitySum(0), utilityCount(0) {
  }
  
  
  UtilityAggregator::~UtilityAggregator(void) {
  }
  
  
  void UtilityAggregator::addUtility(double u) {
    if (std::isnan(u) || std::isinf(u)) {
      // we have a problem!
      // TODO: solve!
    }
    else {
      utilitySum += u;
      ++utilityCount;
    }
    addHook(u);
  }
  
  
  int UtilityAggregator::getCount(void) const {
    return utilityCount;
  }
  
  
  double UtilityAggregator::getTotal(void) const {
    return utilitySum;
  }
  
  
  double UtilityAggregator::getMeanUtility(void) const {
    return utilitySum/utilityCount;
  }
  
  
  std::ostream& operator << (std::ostream& ost, const UtilityAggregator& ua) {
    ost << "[" << ua.utilitySum << "/" << ua.utilityCount 
	<< "=" << ua.getMeanUtility() << "]";
    return ost;
  }


  int UtilityAggregator::writeData(xmlTextWriterPtr writer) {
    return 0;
  }
  
} // NS tmssim
