cmake_minimum_required (VERSION 3.7)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the root and call cmake from there")
endif()

project (resolver)

#compiler requirement
set (CMAKE_CXX_STANDARD 14)

set (XTSSLIB_SOURCES_DIR "${PROJECT_SOURCE_DIR}/xtsslib" CACHE PATH "source path to xtsslib dependancy, default to the case that submodule is initialized")

include ("${XTSSLIB_SOURCES_DIR}/utility.cmake")

# the version number
set (RESOLVER_VERSION_MAJOR 0)
set (RESOLVER_VERSION_MINOR 1)

find_boost_lib("program_options")

# shortcut for directories used for the compilation
set (RESOLVER_SERVER_DIR PATH ${PROJECT_SOURCE_DIR}/resolver_server)
set (GENERATED_SOURCES_DIR PATH ${PROJECT_SOURCE_DIR}/generated_sources)
set (JSONCPP_SOURCES_DIR PATH ${PROJECT_SOURCE_DIR}/jsoncpp_amalgamated)

# cmake settings
configure_file (
	"${PROJECT_SOURCE_DIR}/resolver_config.hpp.in"
	"${PROJECT_SOURCE_DIR}/generated_sources/resolver_config.hpp"
)

# directories where to search headers
include_directories("${GENERATED_SOURCES_DIR}")
include_directories("${RESOLVER_SERVER_DIR}")
include_directories("${JSONCPP_SOURCES_DIR}")
include_directories("${XTSSLIB_SOURCES_DIR}")
include_directories("${Boost_INCLUDE_DIRS}")
link_directories("${Boost_LIBRARY_DIRS}")
# sources in the generated_sources directory
set (GENERATED_SOURCES ${GENERATED_SOURCES_DIR}/resolver_config.hpp)

# sources in the jsoncpp directory
set (JSONCPP_SOURCES ${JSONCPP_SOURCES_DIR}/jsoncpp.cpp
	${JSONCPP_SOURCES_DIR}/json/json.h
	${JSONCPP_SOURCES_DIR}/json/json-forwards.h
	)	

# sources in the resolver_server directory
set (RESOLVER_SOURCES 
	${RESOLVER_SERVER_DIR}/main.cpp
	${RESOLVER_SERVER_DIR}/data.hpp
	${RESOLVER_SERVER_DIR}/data.cpp
	${RESOLVER_SERVER_DIR}/reference.hpp
	${RESOLVER_SERVER_DIR}/reference.cpp
	${RESOLVER_SERVER_DIR}/data_parser.hpp
	${RESOLVER_SERVER_DIR}/data_parser.cpp
	${RESOLVER_SERVER_DIR}/data_dumper.hpp
	${RESOLVER_SERVER_DIR}/data_dumper.cpp
	${RESOLVER_SERVER_DIR}/game_resolver.cpp
	${RESOLVER_SERVER_DIR}/game_resolver.hpp
)


add_executable(resolver_server ${RESOLVER_SOURCES} ${GENERATED_SOURCES} ${JSONCPP_SOURCES})
target_link_libraries(resolver_server ${Boost_LIBRARIES})
