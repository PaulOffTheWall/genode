/**
 * $Id: sptaskbuilder.h 501 2014-05-07 14:16:17Z klugeflo $
 * @file sptaskbuilder.h
 * @brief Build tmssim::SPTask from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_SPTASKBUILDER_H
#define XMLIO_SPTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a SPTask object from the given XML-document.
   */
  class SPTaskBuilder: public IElementBuilder<Task> {
    
  public:
    
    /**
     * Builds a SporadicTask object
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };
  
} // NS tmssim

#endif /* !XMLIO_SPTASKBUILDER_H */
