# Tested CMake versions:
cmake_minimum_required(VERSION 3.14...3.27)

set(TARGET_SYSTEM_LINUX TRUE)
set(TARGET_SYSTEM_WIN32 FALSE)

# Project name, description, language, etc.
project(cache-manager
	DESCRIPTION "Cache Manager"
    LANGUAGES CXX)

# Use CXX 17+.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Version numbers:
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

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

# Executable(s):
add_executable(cache-manager)

# Debug build compiler flags.
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall")

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
)

include(GoogleTest)
gtest_discover_tests(cache-manager)

# Copy necessary directories into build directory, for executable root access.
add_custom_command(TARGET cache-manager PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/external/milestone4/ $<TARGET_FILE_DIR:cache-manager>)

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
