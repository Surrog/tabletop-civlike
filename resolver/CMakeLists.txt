cmake_minimum_required (VERSION 3.7)
project (resolver)

# the version number
set (RESOLVER_VERSION_MAJOR 0)
set (RESOLVER_VERSION_MINOR 1)

set (RESOLVER_SERVER_DIR ${PROJECT_SOURCE_DIR}/resolver_server)
set (GENERATED_SOURCES_DIR ${PROJECT_SOURCE_DIR}/generated_sources)
set (JSONCPP_SOURCES_DIR ${PROJECT_SOURCE_DIR}/jsoncpp_amalgamated)

# cmake settings
configure_file (
	"${PROJECT_SOURCE_DIR}/resolver_config.hpp.in"
	"${PROJECT_SOURCE_DIR}/generated_sources/resolver_config.hpp"
)

include_directories("${GENERATED_SOURCES_DIR}")
include_directories("${RESOLVER_SERVER_DIR}")
include_directories("${JSONCPP_SOURCES_DIR}")

set (GENERATED_SOURCES ${GENERATED_SOURCES_DIR}/resolver_config.hpp)

set (JSONCPP_SOURCES ${JSONCPP_SOURCES_DIR}/jsoncpp.cpp
	${JSONCPP_SOURCES_DIR}/json/json.h
	${JSONCPP_SOURCES_DIR}/json/json-forwards.h
	)

set (RESOLVER_SOURCES 
	${RESOLVER_SERVER_DIR}/main.cpp
	)

add_executable(resolver_server ${RESOLVER_SOURCES} ${GENERATED_SOURCES} ${JSONCPP_SOURCES})
