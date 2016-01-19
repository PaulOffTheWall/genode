/**
 * $Id: ielementbuilder.h 499 2014-05-07 11:24:34Z klugeflo $
 * @file ielementbuilder.h
 * @brief
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef IELEMENTBUILDER_H
#define IELEMENTBUILDER_H 1

/**
 * Includes
 */
#include <libxml/parser.h>

namespace tmssim {

/**
 * Template interface class to build build a generic object parsed out of a XML-document
 */
template <class T>
class IElementBuilder {

public:

	/**
	 * Builds an element
	 * @param doc The parsed document
	 * @param cur The current position within the document
	 * @return A pointer to the created element
	 */
	virtual T* build(xmlDocPtr doc, xmlNodePtr cur) = 0;

	/**
	 * D'tor
	 */
	virtual ~IElementBuilder() {}
};

} // NS tmssim

#endif /* IELEMENTBUILDER_H */
