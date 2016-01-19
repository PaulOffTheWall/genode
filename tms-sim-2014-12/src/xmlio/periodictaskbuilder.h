/**
 * $Id: periodictaskbuilder.h 501 2014-05-07 14:16:17Z klugeflo $
 * @file periodictaskbuilder.h
 * @brief Building of periodic tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_PERIODICTASKBUILDER_H
#define XMLIO_PERIODICTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a PeriodicTask object from the given XML-document.
   */
  class PeriodicTaskBuilder: public IElementBuilder<Task> {
    
  public:
    /**
     * Builds a PeriodicTask object
     * @param doc The XML-document
     * @param cur The current position within the document
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };
  
} // NS tmssim

#endif /* XMLIO_PERIODICTASKBUILDER_H */
