# Tested CMake versions:
cmake_minimum_required(VERSION 3.14...3.28)

# Project name, description, language, etc.
project(cache-manager
	DESCRIPTION "Cache Manager"
    LANGUAGES CXX)

# Use CXX 17+.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version numbers:
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# Platform specifics:
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
	# xxx
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	# xxx
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux|BSD")
	# xxx
endif()

# Add external libraries.
# GTest testing framework.
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing() # enable testing

# JSON library.
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# TBB
include(FetchContent)
FetchContent_Declare(
  tbb
  GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
  # xxx update
  GIT_TAG v2021.12.0
  GIT_SHALLOW ON
)
set(TBB_TEST OFF CACHE BOOL "" FORCE)
set(TBB_STRICT OFF CACHE BOOL "" FORCE)
set(TBB_BUILD ON CACHE BOOL "" FORCE)
set(TBB_FIND_PACKAGE OFF CACHE BOOL "" FORCE)
set(TBB_CPF ON CACHE BOOL "" FORCE)
set(TBB_INSTALL OFF CACHE BOOL "" FORCE)
set(TBB_INSTALL_VARS OFF CACHE BOOL "" FORCE)
set(TBB_VALGRIND_MEMCHECK OFF CACHE BOOL "" FORCE)
#set(TBB_VALGRIND_MEMCHECK ON CACHE BOOL "" FORCE)
set(TBB_DISABLE_HWLOC_AUTOMATIC_SEARCH ON CACHE BOOL "" FORCE)
set(TBB_ENABLE_IPO ON CACHE BOOL "" FORCE)
set(TBB_CONTROL_FLOW_GUARD OFF CACHE BOOL "" FORCE)
#set(TBB_CONTROL_FLOW_GUARD ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(tbb)

# Executable(s):
add_executable(cache-manager)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -fsanitize=thread -O1 -fno-diagnostics-color")

# Debug build compiler flags.
#set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall -fsanitize=address -fno-diagnostics-color")
#set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g -Wall -fsanitize=thread -O1 -fno-diagnostics-color")

# Release build compiler flags.
set(CMAKE_CXX_FLAGS_RELEASE_INIT "")

# Add source files to target(s).
target_sources(cache-manager PRIVATE src/main.cpp
	src/main.cpp
)

# Add include directories to target(s).
# Local include directory.
target_include_directories(cache-manager PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/include/
)

# Link libraries to target(s).
target_link_libraries(cache-manager PRIVATE
	GTest::gtest_main
	nlohmann_json::nlohmann_json
	TBB::tbb
)

include(GoogleTest)
gtest_discover_tests(cache-manager)

# Copy necessary directories into build directory, for executable root access.
add_custom_command(TARGET cache-manager PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config/* $<TARGET_FILE_DIR:cache-manager>)

# Build documentation with Doxygen.
# Only build documentation for release build.
if (CMAKE_BUILD_TYPE MATCHES "^[Rr]elease")
	option(BUILD_DOC "Build documentation" ON)
	# check if doxygen is installed
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
	    # set input and output files
	    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
	    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	    # request to configure the file
	    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
	    message(STATUS "Doxygen build started")
	    # NOTE: ALL builds doc together with app - all targets
	    add_custom_target(doc_doxygen ALL
	        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
	        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	        COMMENT "Generating API documentation with Doxygen"
	        VERBATIM)
	else (DOXYGEN_FOUND)
	    message(STATUS "Doxygen needs to be installed to generate doxygen documentation")
	endif (DOXYGEN_FOUND)
endif()
