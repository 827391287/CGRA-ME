find_package(BISON)
find_package(FLEX)

set(gen_dir ${CMAKE_CURRENT_SOURCE_DIR}/generated)
file(MAKE_DIRECTORY ${gen_dir})

set(dot_tab_cc ${gen_dir}/dot.tab.cc)
set(dot_y_cc   ${gen_dir}/dot.y.cc)

if (BISON_FOUND)
    bison_target(dotparser_parser dot.y ${dot_tab_cc})
else()
    set(BISON_dotparser_parser_OUTPUTS ${dot_tab_cc})
endif()

if (FLEX_FOUND)
    flex_target(dotparser_scanner dot.l ${dot_y_cc})
else()
    set(FLEX_dotparser_scanner_OUTPUTS ${dot_y_cc})
endif()

if (BISON_FOUND AND FLEX_FOUND)
    add_flex_bison_dependency(dotparser_scanner dotparser_parser)
endif()

add_library(
    dotparser
    OBJECT
    ${BISON_dotparser_parser_OUTPUTS}
    ${FLEX_dotparser_scanner_OUTPUTS}
)

target_link_libraries_for_object_libs(
    dotparser
    cgrame-project-wide-interface
)

set_target_properties(
    dotparser PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-effc++")

add_library(
    dotparser_tests
    OBJECT
    dotparser_tests.cpp
)

target_link_libraries_for_object_libs(
    dotparser_tests
    cgrame-project-wide-interface
)
