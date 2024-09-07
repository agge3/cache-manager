# cmake tested versions
cmake_minimum_required(VERSION 3.22...3.27)

# project name, description, and lang
project(cache-manager
	DESCRIPTION "Cache manager"
    LANGUAGES CXX)

# set version numbers
set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# declare exe
# xxx should be a library instead, and executable should be running test cases
add_executable(cache-manager)

# add src
target_sources(cache-manager PRIVATE src/main.cpp
	src/util.cpp
	src/test.cpp
)

# include dir
target_include_directories(cache-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    )
target_include_directories(cache-manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    )

# build doc with doxygen
option(BUILD_DOC "Build documentation" ON)
# check if doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/conf/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message(STATUS "Doxygen build started")
    # NOTE: ALL builds doc together with app - all targets
    add_custom_target(doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
else (DOXYGEN_FOUND)
    message(STATUS "Doxygen needs to be installed to generate doxygen documentation")
endif (DOXYGEN_FOUND)
