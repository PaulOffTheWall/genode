# $Id: CMakeLists.txt 800 2014-12-11 16:27:29Z klugeflo $

set(tms-sim_SOURCES
        tms-sim.cpp
        )

add_executable(tms-sim ${tms-sim_SOURCES})

target_link_libraries(tms-sim
	#schedulers xmlio taskmodels utility core utils
	tms
	${LIBXML2_LIBRARIES}
	)

install(TARGETS tms-sim DESTINATION ${BIN_INSTALL_DIR})
