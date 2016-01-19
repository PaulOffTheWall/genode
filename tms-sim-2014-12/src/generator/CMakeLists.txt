# $Id: CMakeLists.txt 815 2014-12-15 13:40:52Z klugeflo $

set(generator_SOURCES
        mkgenerator.cpp
        )

add_library(mkgenerator OBJECT ${generator_SOURCES})

add_executable(generator generator.cpp)

target_link_libraries(generator
  #  mkgenerator schedulers xmlio taskmodels utility utils core
  tms
  ${LIBXML2_LIBRARIES}
)

install(TARGETS generator DESTINATION ${BIN_INSTALL_DIR})

#file(GLOB generator_HEADERS "*.h")
#install(FILES ${generator_HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/generator)
