/**
 * $Id: taskfactory.cpp 699 2014-09-17 15:00:38Z klugeflo $
 * @file taskfactory.cpp
 * @brief Creation of tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <xmlio/taskfactory.h>

#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>

#include <utils/tlogger.h>
#include <xmlio/sporadictaskbuilder.h>
#include <xmlio/periodictaskbuilder.h>
#include <xmlio/mkctaskbuilder.h>
#include <xmlio/sptaskbuilder.h>
#include <xmlio/dbptaskbuilder.h>

using namespace std;

namespace tmssim {

  TaskFactory::TaskFactory() {
    mBuilder = map<string, IElementBuilder<Task>*>();
    map<string, IElementBuilder<Task>*>::iterator it = mBuilder.begin();
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("periodictask", new PeriodicTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("sporadictask", new SporadicTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("mkctask", new MKCTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("dbptask", new DbpTaskBuilder()));
    mBuilder.insert(it, pair<string, IElementBuilder<Task>*>("sptask", new SPTaskBuilder()));
    
  }
  
  TaskFactory::~TaskFactory() {
    for (map<string, IElementBuilder<Task>*>::iterator it = mBuilder.begin();
	 it != mBuilder.end(); ++it) {
      
      delete it->second;
      
    }
  }
  
  
  bool TaskFactory::accept(xmlDocPtr doc, xmlNodePtr cur) {
    const xmlChar* strPtr = cur->name;
    string s((char*) strPtr);
    return mBuilder.find(s) != mBuilder.end();
  }
  
  Task* TaskFactory::getElement(xmlDocPtr doc, xmlNodePtr cur) {
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

} // NS tmssim
