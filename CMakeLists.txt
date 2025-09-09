cmake_minimum_required(VERSION 3.1)

project(
  cgra-me
  VERSION 2.0.0
  LANGUAGES C CXX
)

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_EXTENSIONS OFF)
set (BUILD_SHARED_LIBS ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CGRAME_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Gurobi)

# link to this INTERFACE library to get all the CGRA-ME headers &  libraries
add_library(cgrame-project-wide-interface INTERFACE)

# include directories for our code
target_include_directories(
  cgrame-project-wide-interface
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# include directories for third party code.
target_include_directories(
  cgrame-project-wide-interface
  SYSTEM # no warnings
  INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
  INTERFACE ${CMAKE_BINARY_DIR}/include
  INTERFACE $<TARGET_PROPERTY:coreir,INTERFACE_INCLUDE_DIRECTORIES>
  INTERFACE $<TARGET_PROPERTY:mINI,INTERFACE_INCLUDE_DIRECTORIES>
)

# -Wno-maybe-uninitialized can generates false positives, at least on g++5.4
target_compile_options(
  cgrame-project-wide-interface
  INTERFACE
  ${CGRAME_CXX_FLAGS} -Wall -Wextra -Weffc++ -Wno-maybe-uninitialized -ggdb -g3
)

if(Gurobi_FOUND)
  target_compile_definitions(
    cgrame-project-wide-interface
    INTERFACE USE_GUROBI
  )
  target_include_directories(
    cgrame-project-wide-interface
    SYSTEM # no warnings
    INTERFACE $<TARGET_PROPERTY:gurobi::cxx,INTERFACE_INCLUDE_DIRECTORIES>
  )
endif()

# make sure third party stuff is built first
# eg. generated headers
add_dependencies(
  cgrame-project-wide-interface
  coreir-build
)

target_link_libraries(
  cgrame-project-wide-interface
  INTERFACE -L${CMAKE_BINARY_DIR}/lib
  INTERFACE -lpthread
)

if(CGRAME_USE_GOOGLE_OR_TOOLS)
target_include_directories(
  cgrame-project-wide-interface
  SYSTEM # no warnings
  INTERFACE $<TARGET_PROPERTY:ortools::ortools,INTERFACE_INCLUDE_DIRECTORIES>
)
add_dependencies(
  cgrame-project-wide-interface
  ortools::ortools
)
target_compile_definitions(
  cgrame-project-wide-interface
  INTERFACE CGRAME_USE_GOOGLE_OR_TOOLS
)
endif()

# behaves like target_link_libraries of CMake 3.12
# supports PUBLIC PRIVATE or INTERFACE before each entry, but no other functionality
function(target_link_libraries_for_object_libs olib_target)
  SET(ARGN_LIST ${ARGN}) # rest of arguments after olib_target
  list(LENGTH ARGN_LIST ARGN_LIST_SIZE)
  MATH(EXPR ARGN_LIST_MAX_INDEX ${ARGN_LIST_SIZE}-1)
  foreach(i RANGE 0 ${ARGN_LIST_MAX_INDEX})
    list(GET ARGN_LIST ${i} ARG)
    MATH(EXPR i_MINUS_ONE ${i}-1)
    list(GET ARGN_LIST ${i_MINUS_ONE} PREV_ARG)
    set(VISIBILITY PRIVATE)
    if (ARG MATCHES "PRIVATE|PUBLIC|INTERFACE")
      # skip, and pick it up in the next iteration
    else()
      if (i GREATER 0 AND PREV_ARG MATCHES "PRIVATE|PUBLIC|INTERFACE")
        set(VISIBILITY ${PREV_ARG})
      endif()
      get_property(all_include_dirs TARGET ${ARG} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
      get_property(sys_include_dirs TARGET ${ARG} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
      foreach(dir IN LISTS all_include_dirs)
        list(FIND sys_include_dirs ${dir} _index)
        if (${_index} GREATER -1)
          target_include_directories(${olib_target} SYSTEM ${VISIBILITY} ${dir})
        else()
          target_include_directories(${olib_target} ${VISIBILITY} ${dir})
        endif()
      endforeach()
      target_compile_definitions(${olib_target}
        ${VISIBILITY} $<TARGET_PROPERTY:${ARG},INTERFACE_COMPILE_DEFINITIONS>
      )
      add_dependencies(${olib_target} ${ARG})
    endif()
  endforeach()
endfunction()

add_subdirectory(thirdparty)
add_subdirectory(src)

option (BUILD_LLVM_PASSES
"Build Loop extraction llvm passes. Requires LLVM" ON)

option (USE_ADL1
"Use new parser for ADL" ON)
