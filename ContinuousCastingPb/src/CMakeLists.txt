IF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  message("TRACE on")
  add_definitions(-DWITH_TRACE_MODEL)
ELSE ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  message("TRACE off")
ENDIF ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/src
  ${ARTIS_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS})

LINK_DIRECTORIES()

ADD_EXECUTABLE(cc-simulator-main evalCC.hpp evalCC.cpp solution.hpp cluster.hpp cluster.cpp crane.hpp crane.cpp
  gantry_crane.hpp gantry_crane.cpp generator.hpp generator.cpp
  graph_manager.hpp models.hpp models.cpp main.cpp run_out_table.hpp
  run_out_table.cpp stack.hpp stack.cpp stock.hpp stock.cpp utils/rand.cpp
  utils/rand.hpp)

TARGET_LINK_LIBRARIES(cc-simulator-main pthread)
