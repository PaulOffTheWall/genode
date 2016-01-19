/**
 * $Id: ucfirmrtbuilder.h 501 2014-05-07 14:16:17Z klugeflo $
 * @file ucfirmrtbuilder.h
 * @brief Build firm RT UC from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_UCFIRMRTBUILDER_H
#define XMLIO_UCFIRMRTBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {
  
  /**
   * Builds a UCFirmRT object from the given XML-document.
   */
  class UCFirmRTBuilder: public IElementBuilder<UtilityCalculator> {
    
  public:
    /**
     * Builds a UtilityCalculator
     * @param doc The XML-document
     * @param cur The current position within the document
     * @return A pointer to the created UtilityCalculator
     */
    UtilityCalculator* build(xmlDocPtr doc, xmlNodePtr cur);
  };

} // NS tmssim

#endif /* !XMLIO_UCFIRMRTBUILDER_H */
