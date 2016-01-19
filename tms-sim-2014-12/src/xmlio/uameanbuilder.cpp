/**
 * $Id: uameanbuilder.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file uameanbuilder.cpp
 * @brief Build tmssim::UAMean from XML
 * @authors Peter Ittner Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uameanbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uamean.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAMeanBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAMean";
    
    // No further information is needed to build the object
    UAMean* uaptr = new UAMean();
    return uaptr;
  }
  
} // NS tmssim
