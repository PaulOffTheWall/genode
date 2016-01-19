/**
 * $Id: xmlutils.cpp 498 2014-05-07 08:05:21Z klugeflo $
 * @file xmlutils.cpp
 * @brief Some utilities for XML parsing
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/xmlutils.h>

namespace tmssim {
  
  bool XmlUtils::isNodeMatching(xmlNodePtr cur, const char* name) {
    //		 Unterscheidung Complex Type - Simple Type
    //		if (xmlFirstElementChild(cur) == NULL) {
    //			cout << cur->name << " is a Simple Type" << endl;
    //		} else {
    //			cout << cur->name << " is a Complex Type" << endl;
    //		}
    
    return (xmlStrEqual(cur->name, (const xmlChar*) name));
  }
  
  
} // NS tmssim
