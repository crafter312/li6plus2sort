cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

include(ExternalProject)

project(li6plus2sort)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("C++ compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Setup directories.
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_INSTALL_LIBDIR ${PROJECT_BINARY_DIR}/lib)

# Define dictionary file path
set(PCMFILE ${CMAKE_BINARY_DIR}/libTNLIB_rdict.pcm)
add_definitions(-DPCMFILE=\"${PCMFILE}\")

# Define shared library file path (this also forces the correct file name)
set(SOFILE ${CMAKE_BINARY_DIR}/libTNLIB.so)
add_definitions(-DSOFILE=\"${SOFILE}\")

# Set project sources
set(SOURCES SortConfig.cpp Gobbi.cpp histo.cpp HINP.cpp silicon.cpp elist.cpp solution.cpp pid.cpp ZApar.cpp einstein.cpp losses.cpp loss2.cpp correl2.cpp parType.cpp calibrate.cpp Input.cpp)
set(LIBHEADERS OutStructs.h)

list(TRANSFORM SOURCES PREPEND ${SRC}/)

# Locate the ROOT package and define a number of useful targets and variables
find_package(ROOT REQUIRED COMPONENTS RIO Tree Hist TreePlayer Core Imt Thread MultiProc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${ROOT_INCLUDE_DIRS})
message("ROOT include directory: ${ROOT_INCLUDE_DIRS}")
message("Available ROOT library components: ${ROOT_LIBRARIES}")

# Generate the dictionary using ROOT_GENERATE_DICTIONARY
include_directories(${SRC})
ROOT_GENERATE_DICTIONARY(G__li6plus2sort ${LIBHEADERS} MODULE li6plus2sort LINKDEF ${SRC}/LinkDef.h)

# Create a custom target for the dictionary generation
add_custom_target(generate_dict ALL DEPENDS ${CMAKE_BINARY_DIR}/G__li6plus2sort.cxx)

# Include the TexNeut library (TNLIB)
ExternalProject_Add(tnlib
	SOURCE_DIR ${CMAKE_SOURCE_DIR}/lib/TNLIB/src                     # Path to the source directory of the external project
	BINARY_DIR ${CMAKE_BINARY_DIR}/lib/TNLIB-build                   # Path to build directory
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/lib/TNLIB-install   # Installation path
		-DTNA_LIBRARY_ONLY=ON                                          # Build the library instead of the executables
	BUILD_COMMAND ${CMAKE_COMMAND} --build . --target TNLIB          # Custom build target
	INSTALL_COMMAND ""                                               # Skip the installation step
	BUILD_BYPRODUCTS ${CMAKE_BINARY_DIR}/lib/TNLIB-build/libTNLIB.so # Path to built library
)

add_library(TNLIB_IMPORTED STATIC IMPORTED GLOBAL)
set_target_properties(TNLIB_IMPORTED PROPERTIES
	IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/TNLIB-build/libTNLIB.so
	INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/lib/TNLIB/src"
)

# Ensure the external project is built before this target is used
add_dependencies(TNLIB_IMPORTED tnlib)

# Create an OBJECT library to handle the generated sources (with sim class)
add_library(li6plus2sort SHARED ${SOURCES} ${CMAKE_BINARY_DIR}/G__li6plus2sort.cxx)
set_target_properties(li6plus2sort PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(li6plus2sort TNLIB_IMPORTED ROOT::RIO ROOT::Tree ROOT::Hist ROOT::TreePlayer ROOT::Core ROOT::Imt ROOT::Thread ROOT::MultiProc)
set_target_properties(li6plus2sort PROPERTIES
	BUILD_RPATH "${CMAKE_BINARY_DIR}"
	INSTALL_RPATH "${CMAKE_BINARY_DIR}"
)

# Create sort executable
add_executable(sort ${SRC}/sort.cpp)
target_link_libraries(sort li6plus2sort TNLIB_IMPORTED ROOT::RIO ROOT::Tree ROOT::Hist ROOT::TreePlayer ROOT::Core ROOT::Imt ROOT::Thread ROOT::MultiProc)
set_target_properties(sort PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set_target_properties(sort PROPERTIES
	BUILD_RPATH "${CMAKE_BINARY_DIR}"
	INSTALL_RPATH "${CMAKE_BINARY_DIR}"
)

# Set up copying of TNLIB library after project is built
ExternalProject_Get_Property(tnlib BINARY_DIR)

add_custom_command(
	OUTPUT ${PCMFILE} ${CMAKE_BINARY_DIR}/libTNLIB.rootmap ${SOFILE}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${BINARY_DIR}/libTNLIB_rdict.pcm
		${PCMFILE}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${BINARY_DIR}/libTNLIB.rootmap
		${CMAKE_BINARY_DIR}/libTNLIB.rootmap
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${BINARY_DIR}/libTNLIB.so
		${SOFILE}
	DEPENDS tnlib
	COMMENT "Copying ROOT dictionary files and library from TNLIB build directory"
)

add_custom_target(copy_tnlib_dict_files ALL
    DEPENDS ${PCMFILE} ${CMAKE_BINARY_DIR}/libTNLIB.rootmap ${SOFILE}
)



