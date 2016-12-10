cmake_minimum_required (VERSION 3.7)
project (resolver)

# the version number
set (RESOLVER_VERSION_MAJOR 0)
set (RESOLVER_VERSION_MINOR 1)

set (resolver_server_dir ${PROJECT_SOURCE_DIR}/resolver_server)

# cmake settings
configure_file (
	"${PROJECT_SOURCE_DIR}/resolver_config.h.in"
	"${PROJECT_SOURCE_DIR}/generated_sources/resolver_config.h"
)

include_directories("${PROJECT_SOURCE_DIR}/generated_sources")
include_directories("${resolver_server_dir}")

add_executable(resolver_server ${resolver_server_dir}/main.cpp)
