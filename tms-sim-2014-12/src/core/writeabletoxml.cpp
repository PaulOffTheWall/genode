/**
 * $Id: writeabletoxml.cpp 811 2014-12-12 15:55:09Z klugeflo $
 * @file writeabletoxml.cpp
 * @brief Implementation of basic write2xml functionality
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#include <core/writeabletoxml.h>


using namespace std;

namespace tmssim {

  WriteableToXML::~WriteableToXML() {
  }


  int WriteableToXML::writeToXML(xmlTextWriterPtr writer) {
    string elem = getClassElement();
    
    xmlTextWriterStartElement(writer,(xmlChar*) elem.c_str());

    writeData(writer);
    
    xmlTextWriterEndElement(writer);

    return 0;
  }


} // NS tmssim
