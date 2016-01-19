/**
 * $Id: xmlutils.h 498 2014-05-07 08:05:21Z klugeflo $
 * @file xmlutils.h
 * @brief Some utilities for XML parsing
 * @authors Peter Ittner, Florian Kluge <kluge@informatik.uni-augsburg.de>
 */

#ifndef XMLIO_XMLUTILS_H
#define XMLIO_XMLUTILS_H 1

/**
 * Includes
 */
#include <ios>
#include <iostream>
#include <string>
#include <libxml/parser.h>

using namespace std;

/**
 * Macro to get an xmlChar* from a std::string
 * @param a The std::string value to be converted
 * @return A const unsigned char pointer to the string
 */
#define STRTOXML(a)((xmlChar*)a.c_str())

namespace tmssim {
  
  typedef ios_base& (*format_function)(ios_base& str); // type for conciseness
  
  /**
   * Utility Class for XML operations
   */
  class XmlUtils {
    
  public:
    /**
     * Checks if the given node (tag-name) is matching the given string
     * @param cur The current node to check
	 * @param name The name to check against
	 * @return true, if the string matches the xml-tag, false otherwise
	 */
    static bool isNodeMatching(xmlNodePtr cur, const char* name);
    
    /**
     * Gets the generic value (content within XML-tags) of the given node
     * Tries to convert to return value into the right type
     * @param cur The current Node
     * @param doc The current document which contains the actual content
     * @return The value of XML element which is converted to the template paramter T
     */
    template<class T>
      static T getNodeValue(xmlNodePtr cur, xmlDocPtr doc);
    
    /**
     * Converts some XML-string to the given template parameter T
     * This method detects float-numbers, integer, binary, hex, octal and decimal notation
     * @param str The string to be converted
     * @return The value of the string
     */
    template<class T>
      static T convertFromXML(xmlChar* str);
    
    /**
     * Converts the given type to an std::string
     * @param val The value to be converted
     * @return The value the element in a std::string (use XMLCAST-Marco for writing a std::string to an xml-document)
     */
    template<class T>
      static std::string convertToXML(const T val);
    
    /**
     * Converts the given type to an std::string and formats the value with the given format-function
     * @param val The value to be converted
     * @param pFunc The format function to be used (hex, dec, oct)
     * @return The value the element in a std::string (use STRTOXML-Marco for writing a std::string to an xml-document)
     */
    template<class T>
      static std::string convertToXML(const T val, format_function pFunc );
  };

} // NS tmssim

#include <xmlio/xmlutils.tpp>

#endif /* !XMLIO_XMLUTILS_H */
