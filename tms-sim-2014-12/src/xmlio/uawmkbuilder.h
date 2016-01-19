/**
 * $Id: uawmkbuilder.h 671 2014-09-04 15:39:12Z klugeflo $
 * @file uawmkbuilder.h
 * @brief Build tmssim::UAWMK from XML
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_UAWMKBUILDER_H
#define XMLIO_UAWMKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a UAWMK object from the given XML-document.
   */
  class UAWMKBuilder: public IElementBuilder<UtilityAggregator> {
    
  public:
    /**
     * Builds a UtilityAggregator
     * @param doc The XML-document
     * @param cur The current position within the document
     * @return A pointer to the created taUtilityAggregatorsk
     */
    UtilityAggregator* build(xmlDocPtr doc, xmlNodePtr cur);
  };

} // NS tmssim

#endif // !XMLIO_UAWMKBUILDER_H
