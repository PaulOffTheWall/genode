# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(schedulers_SOURCES
	ald.cpp
	beedf.cpp
	dbpedf.cpp
	dvdedf.cpp
	edf.cpp
	fpp.cpp
	gdpa.cpp
	gdpas.cpp
	hcedf.cpp
	mkuedf.cpp
	phcedf.cpp
	oedf.cpp
	oedfmax.cpp
	oedfmin.cpp
	)

#add_library(schedulers SHARED ${schedulers_SOURCES})
add_library(schedulers OBJECT ${schedulers_SOURCES})

#install(TARGETS schedulers DESTINATION ${LIBDIR})

#file(GLOB schedulers_HEADERS "*.h")
#install(FILES ${schedulers_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/schedulers)
