/**
 * $Id: uawmeanbuilder.h 501 2014-05-07 14:16:17Z klugeflo $
 * @file uawmeanbuilder.h
 * @brief Build tmssim::UAWMean from XML
 * @authors Peter Ittner Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_UAWMEANBUILDER_H
#define XMLIO_UAWMEANBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a UAWMean object from the given XML-document.
   */
  class UAWMeanBuilder: public IElementBuilder<UtilityAggregator> {
    
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

#endif /* !XMLIO_UAWMEANBUILDER_H */
