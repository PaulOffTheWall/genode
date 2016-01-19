/**
 * $Id: sporadictaskbuilder.h 501 2014-05-07 14:16:17Z klugeflo $
 * @file sporadictaskbuilder.h
 * @brief Building of sporadic tasks from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_SPORADICTASKBUILDER_H
#define XMLIO_SPORADICTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a SporadicTask object from the given XML-document.
   */
  class SporadicTaskBuilder: public IElementBuilder<Task> {
    
  public:
    
    /**
     * Builds a SporadicTask object
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };
  
} // NS tmssim

#endif /* !XMLIO_SPORADICTASKBUILDER_H */
