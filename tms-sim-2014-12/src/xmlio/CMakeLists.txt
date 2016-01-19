# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(xmlio_SOURCES
	dbptaskbuilder.cpp
	taskfactory.cpp
	tasksetreader.cpp
	tasksetwriter.cpp
	utilityaggregatorfactory.cpp
	utilitycalculatorfactory.cpp
	xmlutils.cpp
	mkctaskbuilder.cpp
	periodictaskbuilder.cpp
	sporadictaskbuilder.cpp
	sptaskbuilder.cpp
	uaexpbuilder.cpp
	uameanbuilder.cpp
	uawmeanbuilder.cpp
	uawmkbuilder.cpp
	ucfirmrtbuilder.cpp
	)

#add_library(xmlio SHARED ${xmlio_SOURCES})
add_library(xmlio OBJECT ${xmlio_SOURCES})

#install(TARGETS xmlio DESTINATION ${LIBDIR})

#file(GLOB xmlio_HEADERS "*.h")
#install(FILES ${xmlio_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/xmlio)
