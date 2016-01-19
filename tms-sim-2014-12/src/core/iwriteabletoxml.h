/**
 * $Id: iwriteabletoxml.h 657 2014-08-28 19:21:40Z klugeflo $
 * @file iwriteabletoxml.h
 * @brief Interface to allow objects to serialize themselves into a XML file
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef IWRITEABLETOXML_H
#define IWRITEABLETOXML_H 1

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

namespace tmssim {

  /**
   * Interface to allow objects to serialize themselves into a XML file
   */
  class IWriteableToXML {
    
  public:
    
    //IWriteableToXML() {}
    
    /**
     * Write an object to a xml file by using the given xmlTextWriter
     * @param writer Pointer to the xmlTextWriter that sould be used for serz
     */
    virtual void write(xmlTextWriterPtr writer) const = 0;
    
    /**
     * D'tor
     */
    virtual ~IWriteableToXML() {}
  };

} // NS tmssim

#endif /* IWRITEABLETOXML_H */
