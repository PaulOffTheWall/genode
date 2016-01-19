# $Id: CMakeLists.txt 802 2014-12-11 16:47:37Z klugeflo $

file(GLOB XSD "*.xsd")
file(GLOB XML "*.xml")

install(FILES ${XSD} DESTINATION ${XSD_INSTALL_DIR})
install(FILES ${XML} DESTINATION ${XML_INSTALL_DIR})


#  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html DESTINATION ${DOCDIR})
