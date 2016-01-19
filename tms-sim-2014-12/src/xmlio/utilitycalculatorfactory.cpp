/**
 * $Id: utilitycalculatorfactory.cpp 693 2014-09-14 20:32:48Z klugeflo $
 * @file utilitycalculatorfactory.cpp
 * @brief Creation of utility calculators from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/utilitycalculatorfactory.h>

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <exception>
#include <stdexcept>

#include <utils/tlogger.h>
#include <xmlio/ucfirmrtbuilder.h>

using namespace std;

namespace tmssim {

UtilityCalculatorFactory::UtilityCalculatorFactory() {
	mBuilder = map<string, IElementBuilder<UtilityCalculator>*>();
	map<string, IElementBuilder<UtilityCalculator>*>::iterator it = mBuilder.begin();
	mBuilder.insert(it, pair<string, IElementBuilder<UtilityCalculator>*>("ucfirmrt", new UCFirmRTBuilder()));
}

bool UtilityCalculatorFactory::accept(xmlDocPtr doc, xmlNodePtr cur) {
	const xmlChar* strPtr = cur->name;
	string s((char*) strPtr);
	return mBuilder.find(s) != mBuilder.end();
}

UtilityCalculator* UtilityCalculatorFactory::getElement(xmlDocPtr doc, xmlNodePtr cur) {
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

UtilityCalculatorFactory::~UtilityCalculatorFactory() {
  for (map<string, IElementBuilder<UtilityCalculator>*>::iterator it = mBuilder.begin(); it != mBuilder.end(); ++it) {
    delete it->second;
  }
}

} // NS tmssim
