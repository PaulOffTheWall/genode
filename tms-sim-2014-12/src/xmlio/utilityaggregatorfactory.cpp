/**
 * $Id: utilityaggregatorfactory.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file utilityaggregatorfactory.cpp
 * @brief Building of utility aggregators from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/utilityaggregatorfactory.h>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <exception>
#include <stdexcept>

#include <utils/tlogger.h>
#include <xmlio/uaexpbuilder.h>
#include <xmlio/uameanbuilder.h>
#include <xmlio/uawmeanbuilder.h>
#include <xmlio/uawmkbuilder.h>

using namespace std;

namespace tmssim {

  UtilityAggregatorFactory::UtilityAggregatorFactory() {
    mBuilder = map<string, IElementBuilder<UtilityAggregator>*>();
    map<string, IElementBuilder<UtilityAggregator>*>::iterator it = mBuilder.begin();
    mBuilder.insert(it, pair<string, IElementBuilder<UtilityAggregator>*>("uaexp", new UAExpBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<UtilityAggregator>*>("uamean", new UAMeanBuilder()));
    //		mBuilder.insert(it, pair<string, IElementBuilder<UtilityAggregator>*>("uawindow", new UAWindowBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<UtilityAggregator>*>("uawmean", new UAWMeanBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<UtilityAggregator>*>("uawmk", new UAWMKBuilder()));
  }
  
  bool UtilityAggregatorFactory::accept(xmlDocPtr doc, xmlNodePtr cur) {
    const xmlChar* strPtr = cur->name;
    //tDebug() << "Checking " << (char*)strPtr;
    string s((char*) strPtr);
    return mBuilder.find(s) != mBuilder.end();
  }
  
  UtilityAggregator* UtilityAggregatorFactory::getElement(xmlDocPtr doc, xmlNodePtr cur) {
    //tDebug() << "getElement: " << cur->name;
    try {
      return mBuilder.at((const char*) cur->name)->build(doc, cur);
    } catch (out_of_range& ex) {
      //if (mBuilder.find() == mBuilder.end()) {
      //int keyCount = 1;//mBuilder.count((const char*) cur->name);
      //if (keyCount == 0) {
      tError() << "Key is NULL";
      return NULL;
      
      stringstream ss;
      ss << "Key in map not found: " << cur->name << endl;
      string error = ss.str();
      throw invalid_argument(error);
      //}
    }
  }
  
  UtilityAggregatorFactory::~UtilityAggregatorFactory() {
    for (map<string, IElementBuilder<UtilityAggregator>*>::iterator it = mBuilder.begin(); it != mBuilder.end(); ++it) {
      delete it->second;
    }
  }
  
} // NS tmssim
