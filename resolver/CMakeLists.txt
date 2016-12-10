cmake_minimum_required (VERSION 3.7)
project (resolver)

# the version number
set (RESOLVER_VERSION_MAJOR 0)
set (RESOLVER_VERSION_MINOR 1)

set (RESOLVER_SERVER_DIR ${PROJECT_SOURCE_DIR}/resolver_server)
set (GENERATED_SOURCES_DIR ${PROJECT_SOURCE_DIR}/generated_sources)

# cmake settings
configure_file (
	"${PROJECT_SOURCE_DIR}/resolver_config.hpp.in"
	"${PROJECT_SOURCE_DIR}/generated_sources/resolver_config.hpp"
)

include_directories("${GENERATED_SOURCES_DIR}")
include_directories("${RESOLVER_SERVER_DIR}")

set (GENERATED_HEADER ${GENERATED_SOURCES_DIR}/resolver_config.hpp)

set (RESOLVER_SOURCES 
	${GENERATED_HEADER}
	${RESOLVER_SERVER_DIR}/main.cpp
	)

add_executable(resolver_server ${RESOLVER_SOURCES})
