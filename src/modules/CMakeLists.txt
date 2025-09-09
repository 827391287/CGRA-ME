file(GLOB MODULES_SRC
    "*.cpp"
)

add_library(modules OBJECT ${MODULES_SRC})

add_library(modules-interface INTERFACE)
target_include_directories(modules-interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries_for_object_libs(
	modules
  PRIVATE modules-interface
  cgrame-project-wide-interface
)

set_target_properties(
    modules PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
