# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(taskmodels_SOURCES
	dbptask.cpp
	mkctask.cpp
	mkmonitor.cpp
	mktask.cpp
	periodictask.cpp
  periodicloadtask.cpp
	sporadictask.cpp
	sptask.cpp
	)

#add_library(taskmodels SHARED ${taskmodels_SOURCES})
add_library(taskmodels OBJECT ${taskmodels_SOURCES})

#install(TARGETS taskmodels DESTINATION ${LIBDIR})

#file(GLOB taskmodels_HEADERS "*.h")
#install(FILES ${taskmodels_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/taskmodels)
