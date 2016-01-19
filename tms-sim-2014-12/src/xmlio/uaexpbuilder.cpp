/**
 * $Id: uaexpbuilder.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file uaexpbuilder.cpp
 * @brief Build exponential utility aggregator from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uaexpbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uaexp.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAExpBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAExp";
    
    // Variables need to build the UtilityAggregator
    double weight = 0;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements
    while (cur != NULL) {
      
      if (XmlUtils::isNodeMatching(cur, "weight")) {
	weight = XmlUtils::getNodeValue<double>(cur, doc);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    // Create the entire UtilityAggregator from the parsed data
    tDebug() << "with weight=" << weight;
    
    UAExp* uaptr = new UAExp(weight);
    return uaptr;
    
  }
  
} // NS tmssim
