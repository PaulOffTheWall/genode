/**
 * $Id: uawmeanbuilder.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file uawmeanbuilder.cpp
 * @brief Build tmssim::UAWMean from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uawmeanbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uawmean.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAWMeanBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAWMean";
    
    // Variables need to build the task
    size_t size = 0;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements
    while (cur != NULL) {
      
      if (XmlUtils::isNodeMatching(cur, "size")) {
	size = XmlUtils::getNodeValue<size_t>(cur, doc);
      } else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    // Create the entire UtilityAggregator from the parsed data
    tDebug() << "size=" << size;
    
    UAWindow* uaptr = new UAWMean(size);
    
    return uaptr;
  }
  
} // NS tmssim
