/**
 * $Id: ielementfactory.h 501 2014-05-07 14:16:17Z klugeflo $
 * @file ielementfactory.h
 * @brief Factory interface
 * @authors Peter ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_IELEMENTFACTORY_H
#define XMLIO_IELEMENTFACTORY_H 1

/**
 * Includes
 */
#include <libxml/parser.h>

namespace tmssim {

  /**
   * Template class the construct objects out of an XML-document
   */
  template <class T>
    class IElementFactory {
    
  public:
    /**
     * Gets a pointer to an element out of the factory, which is at the given position in the document
     * @param doc The XML-document
     * @param cur The current position within the XML-document
     * @return A pointer to an object that has been created
     */
    virtual T* getElement(xmlDocPtr doc, xmlNodePtr cur) = 0;
    
    /**
     * Checks if the factory is able to construct an element at the current document position
     * @param doc The XML document
     * @param cur The current document position
     * @return true, if this factory can construct an object, otherwise false
     */
    virtual bool accept(xmlDocPtr doc, xmlNodePtr cur) = 0;
    
    /**
     * D'tor
     */
    virtual ~IElementFactory() {}
  };
  
} // NS tmssim

#endif /* !XMLIO_IELEMENTFACTORY_H */
