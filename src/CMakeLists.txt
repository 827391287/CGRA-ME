# GCC 7.0 added support for the c++17 standard attributes:
#     nodiscard, maybe_unused, fallthrough etc
# Earlier versions ignore them (and provide compiler-specific replacements),
#     but produce warnings about not recognising trem.
# I think this is neater than defining a macro or something,
#     allowing our code to be cleaner & more standard
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0.0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-attributes")
endif()

add_subdirectory(archs)
add_subdirectory(core)
add_subdirectory(dotparser)
add_subdirectory(llvm-passes)
add_subdirectory(modules)
add_subdirectory(run)

add_library(cgra-me
    $<TARGET_OBJECTS:archs>
    $<TARGET_OBJECTS:core>
    $<TARGET_OBJECTS:dotparser>
    $<TARGET_OBJECTS:modules>
)

add_library(cgra-me_static STATIC
    $<TARGET_OBJECTS:archs>
    $<TARGET_OBJECTS:core>
    $<TARGET_OBJECTS:dotparser>
    $<TARGET_OBJECTS:modules>
)

target_link_libraries(cgra-me
    PUBLIC cgrame-project-wide-interface
)

target_link_libraries(cgra-me_static
    PUBLIC cgrame-project-wide-interface
)

target_link_libraries(cgra-me
    PUBLIC coreir
)
target_link_libraries(cgra-me_static
    PUBLIC coreir
)

if(CGRAME_USE_GOOGLE_OR_TOOLS)
target_link_libraries(cgra-me
    PRIVATE ortools::ortools
)
target_link_libraries(cgra-me_static
    PRIVATE ortools::ortools
)
endif()

if(Gurobi_FOUND)
    target_link_libraries(cgra-me
        PRIVATE gurobi::cxx
    )
    target_link_libraries(cgra-me_static
        PRIVATE gurobi::cxx
    )
endif()
