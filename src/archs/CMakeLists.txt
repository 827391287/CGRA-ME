file(GLOB ARCHS_SRC
    "*.cpp"
)

add_library(archs OBJECT ${ARCHS_SRC})

target_link_libraries_for_object_libs(
    archs
  cgrame-project-wide-interface
)

set_target_properties(
    archs PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

