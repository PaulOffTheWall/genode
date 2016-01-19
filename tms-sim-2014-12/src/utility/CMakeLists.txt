# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(utility_SOURCES
	uaexp.cpp
	uamean.cpp
	uawindow.cpp
	uawmean.cpp
	uawmk.cpp
	ucfirm.cpp
	)

#add_library(utility SHARED ${utility_SOURCES})
add_library(utility OBJECT ${utility_SOURCES})

#install(TARGETS utility DESTINATION ${LIBDIR})

#file(GLOB utility_HEADERS "*.h")
#install(FILES ${utility_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/utility)
