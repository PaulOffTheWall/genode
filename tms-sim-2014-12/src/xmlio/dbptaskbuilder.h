/**
 * $Id: dbptaskbuilder.h 678 2014-09-09 14:12:20Z klugeflo $
 * @file dbptaskbuilder.h
 * @brief
 * @author Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_DBPTASKBUILDER_H
#define XMLIO_DBPTASKBUILDER_H 1

#include <core/scobjects.h>
#include <xmlio/ielementbuilder.h>

namespace tmssim {

  /**
   * Builds a MBPTask object from the given XML-document.
   */
  class DbpTaskBuilder: public IElementBuilder<Task> {
    
  public:

    /**
     * Builds a SporadicTask object
     * @return A pointer to the created Task
     */
    Task* build(xmlDocPtr doc, xmlNodePtr cur);
  };

} // NS tmssim

#endif // !XMLIO_DBPTASKBUILDER_H
