/**
 * $Id: mkctaskbuilder.h 699 2014-09-17 15:00:38Z klugeflo $
 * @file mkctaskbuilder.h
 * @brief Build tmssim::MKCTask from XML
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_MKCTASKBUILDER_H
#define XMLIO_MKCTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a MKCTask object from the given XML-document.
   */
  class MKCTaskBuilder: public IElementBuilder<Task> {
    
  public:

    /**
     * Builds a MKCTask object
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };

} // NS tmssim

#endif /* !XMLIO_MKCTASKBUILDER_H */
