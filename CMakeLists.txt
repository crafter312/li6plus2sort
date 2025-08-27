cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(li6plus2sort)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Setup directories.
set(SRC ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_INSTALL_LIBDIR ${PROJECT_BINARY_DIR}/lib)

# Set project sources
set(SOURCES Gobbi.cpp histo.cpp HINP.cpp silicon.cpp elist.cpp solution.cpp pid.cpp ZApar.cpp einstein.cpp losses.cpp loss2.cpp correl2.cpp parType.cpp calibrate.cpp Input.cpp)

list(TRANSFORM SOURCES PREPEND ${SRC}/)

# Locate the ROOT package and define a number of useful targets and variables
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist TreePlayer Core Imt Thread MultiProc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${ROOT_INCLUDE_DIRS})
message("ROOT include directory: ${ROOT_INCLUDE_DIRS}")
message("Available ROOT library components: ${ROOT_LIBRARIES}")

# Create an OBJECT library to handle the generated sources (with sim class)
add_library(Li6sortObjects OBJECT ${SOURCES})
set_target_properties(Li6sortObjects PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(Li6sortObjects ROOT::RIO ROOT::Tree ROOT::Hist ROOT::TreePlayer ROOT::Core ROOT::Imt ROOT::Thread ROOT::MultiProc)

# Create 6Li(2+2)->alpha+p+n executable
add_executable(sort ${SRC}/sort.cpp $<TARGET_OBJECTS:Li6sortObjects>)
target_link_libraries(sort ROOT::RIO ROOT::Tree ROOT::Hist ROOT::TreePlayer ROOT::Core ROOT::Imt ROOT::Thread ROOT::MultiProc)
set_target_properties(sort PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
