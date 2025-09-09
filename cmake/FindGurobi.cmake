
# First, try looking in various places for a GUROBI_HOME
if(DEFINED GUROBI_HOME)
    message(STATUS "CMake variable GUROBI_HOME already defined")
elseif(DEFINED ENV{GUROBI_HOME})
    message(STATUS "Using GUROBI_HOME environment variable")
    set(GUROBI_HOME "$ENV{GUROBI_HOME}")
else()
    message(STATUS "GUROBI_HOME environment variable not defined, searching for gurobi in ${CGRAME_DIR}/../")

    file(GLOB POSSIBLE_GUROBI_HOMES "${CGRAME_DIR}/../gurobi[0-9][0-9][0-9]*/")
    list(SORT POSSIBLE_GUROBI_HOMES)
    list(REVERSE POSSIBLE_GUROBI_HOMES)
    list(LENGTH POSSIBLE_GUROBI_HOMES NUM_GUROBI_HOMES)
    if (NUM_GUROBI_HOMES GREATER 0)
        list(GET POSSIBLE_GUROBI_HOMES 0 GUROBI_HOME)
    endif()
endif()

# Then, try to find all the libs & includes, etc.
if (DEFINED GUROBI_HOME)
    # Make sure there are no relative components (ie. '..'),
    #  which seem to cause CMake to convert it to a relative path when linking, sometimes!?
    get_filename_component(GUROBI_HOME "${GUROBI_HOME}" ABSOLUTE)

    message(STATUS "Using GUROBI_HOME=${GUROBI_HOME}")

    find_path(
        GUROBI_INCLUDE
        NAMES gurobi_c++.h
        PATHS "${GUROBI_HOME}/include"
        NO_DEFAULT_PATH # only search the in PATHS passed in
    )

    # can't use find_library, because the filename is dependent on version
    file(GLOB GUROBI_LIBRARY "${GUROBI_HOME}/lib/libgurobi.so.*")

    # try macos
    if (NOT GUROBI_LIBRARY)
        file(GLOB GUROBI_LIBRARY "${GUROBI_HOME}/lib/libgurobi*.dylib")
    endif()

    list(LENGTH GUROBI_LIBRARY NUM_GUROBI_LIBRARY)
    if(NUM_GUROBI_LIBRARY EQUAL 0)
        set(GUROBI_LIBRARY "GUROBI_LIBRARY-NOTFOUND")
    elseif(NUM_GUROBI_LIBRARY GREATER 1)
        message(WARNING "Multiple gurobi shared libraries found. Will use first one. Found ${GUROBI_LIBRARY}")
        list(GET GUROBI_LIBRARY 0 GUROBI_LIBRARY)
    endif()


    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # Search for the highest abi-versioned lib less than the compiler version
        foreach(abi_ver 8.5 5.2 4.8 4.2)  # should be in descending order (highest priority first)
            find_library(
                gurobi_cxxlib_for_abi_ver
                NAMES "libgurobi_g++${abi_ver}.a"
                PATHS ${GUROBI_HOME}/lib/
                NO_DEFAULT_PATH
                NO_CACHE
            )
            if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "${abi_ver}" AND gurobi_cxxlib_for_abi_ver)
                set(GUROBI_CXXLIB "${gurobi_cxxlib_for_abi_ver}" CACHE FILEPATH "Gurobi C++ static lib")
                break()
            endif()
        endforeach()
    endif()

    if (NOT GUROBI_CXXLIB)
        find_library(
            GUROBI_CXXLIB
            NAMES "libgurobi_c++.a"
            PATHS ${GUROBI_HOME}/lib/
            NO_DEFAULT_PATH
        )
    endif()

    message(STATUS "Using GUROBI_CXXLIB=${GUROBI_CXXLIB}")
else()
    message(STATUS "Unable to determine GUROBI_HOME")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Gurobi
    REQUIRED_VARS GUROBI_HOME GUROBI_LIBRARY GUROBI_CXXLIB GUROBI_INCLUDE
)

add_library(gurobi::main SHARED IMPORTED)
set_target_properties(
    gurobi::main PROPERTIES
    IMPORTED_LOCATION "${GUROBI_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${GUROBI_INCLUDE}"
)

add_library(gurobi::cxx STATIC IMPORTED)
set_target_properties(
    gurobi::cxx PROPERTIES
    IMPORTED_LOCATION "${GUROBI_CXXLIB}"
    INTERFACE_LINK_LIBRARIES "gurobi::main"
)