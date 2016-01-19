# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(core_SOURCES
	deadlinemonitor.cpp
	job.cpp
	scheduler.cpp
	simulation.cpp
	stat.cpp
	statistics.cpp
	task.cpp
	utilityaggregator.cpp
	utilitycalculator.cpp
	writeabletoxml.cpp
	)

#add_library(core SHARED ${core_SOURCES})
add_library(core OBJECT ${core_SOURCES})

#install(TARGETS core DESTINATION ${LIBDIR})


#file(GLOB core_HEADERS "*.h")
#install(FILES ${core_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/core)
