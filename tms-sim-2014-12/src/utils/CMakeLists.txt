# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(utils_SOURCES
	kvfile.cpp
	logger.cpp
	nullstream.cpp
	tlogger.cpp
	)

#add_library(utils SHARED ${utils_SOURCES})
add_library(utils OBJECT ${utils_SOURCES})

#install(TARGETS utils DESTINATION ${LIBDIR})

#file(GLOB utils_HEADERS "*.h")
#install(FILES ${utils_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/utils)
