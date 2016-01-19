/**
 * $Id: uawmkbuilder.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file uawmkbuilder.cpp
 * @brief Build tmssim::UAWMK from XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/uawmkbuilder.h>

#include <string>
#include <sstream>
#include <iostream>

#include <utils/tlogger.h>
#include <xmlio/xmlutils.h>
#include <utility/uawmk.h>

using namespace std;

namespace tmssim {

  UtilityAggregator* UAWMKBuilder::build(xmlDocPtr doc, xmlNodePtr cur) {
    tDebug() << "Building a UAWMK";
    
    // Variables need to build the UtilityAggregator
    unsigned int m = 0;
    unsigned int k = 0;
    
    // Iterate through the document to get the needed data
    cur = cur->xmlChildrenNode;
    // Iterate over all children elements
    while (cur != NULL) {
      
      if (XmlUtils::isNodeMatching(cur, "m")) {
	m = XmlUtils::getNodeValue<size_t>(cur, doc);
      }
      else if (XmlUtils::isNodeMatching(cur, "k")) {
	k = XmlUtils::getNodeValue<size_t>(cur, doc);
      }
      else {
	//tError() << "Unknown Node Tag: " << cur->name;
      }
      
      cur = cur->next;
    }
    return new UAWMK(m,k);
  }

} // NS tmssim
