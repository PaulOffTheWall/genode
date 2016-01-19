/**
 * $Id: ucfirmrtbuilder.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file ucfirmrtbuilder.cpp
 * @brief Build firm RT UC from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/ucfirmrtbuilder.h>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/ucfirm.h>

namespace tmssim {
  
  UtilityCalculator* UCFirmRTBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UtilityCalculator";
    
    // No further information is needed to build the object
    return new UCFirmRT();
  }
  
} // NS tmssim
