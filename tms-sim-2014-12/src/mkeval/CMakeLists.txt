# $Id: CMakeLists.txt 800 2014-12-11 16:27:29Z klugeflo $

set(mkeval_SOURCES
        eval.cpp
	mkeval.cpp
	utilisationstatistics.cpp
        )

add_executable(mkeval ${mkeval_SOURCES})

target_link_libraries(mkeval
	#mkgenerator schedulers xmlio taskmodels utility core utils
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	)

install(TARGETS mkeval DESTINATION ${BIN_INSTALL_DIR})


set(runts_SOURCES
	mkeval.cpp
	runts.cpp
	)

add_executable(runts ${runts_SOURCES})

target_link_libraries(runts
	#mkgenerator schedulers xmlio taskmodels utility core utils
	tms
	${LIBXML2_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
	)

install(TARGETS runts DESTINATION ${BIN_INSTALL_DIR})
