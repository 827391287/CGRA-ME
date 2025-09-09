configure_file(LoopParser.py ${CMAKE_BINARY_DIR}/script/LoopParser.py COPYONLY)

if(NOT BUILD_LLVM_PASSES)
    return()
endif()

# find the latest version of LLVM available on the system
set(CMAKE_FIND_PACKAGE_SORT_ORDER NATURAL)
set(CMAKE_FIND_PACKAGE_SORT_DIRECTION DEC)
find_package(llvm REQUIRED CONFIG NAMES LLVM)
message(STATUS "Using LLVM config at ${llvm_CONFIG}")


cmake_policy(SET CMP0057 NEW)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}") # not needed in new-ish versions
include(AddLLVM)

add_llvm_pass_plugin(llvm-pass-dfggen
    DFGGeneration.cpp
)

target_include_directories(llvm-pass-dfggen
    SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(llvm-pass-dfggen
    PRIVATE cgra-me
)

target_compile_options(llvm-pass-dfggen PRIVATE -fexceptions)
