add_subdirectory(cxxopts)
add_subdirectory(mINI)

set(COREIR_PRODUCT ${CMAKE_BINARY_DIR}/bin/coreir)

add_custom_command(OUTPUT ${COREIR_PRODUCT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/coreir
    COMMAND "$(MAKE)"
    COMMAND "$(MAKE)" install "prefix=${CMAKE_BINARY_DIR}"
)

add_custom_target(coreir-build
    DEPENDS ${COREIR_PRODUCT}
)

add_library(coreir INTERFACE)

target_link_libraries(
    coreir
    INTERFACE -lcoreir-aetherlinglib -lcoreir-c -lcoreir-rtlil -lcoreir-commonlib -lcoreir-ice40 -lcoreir
)

target_include_directories(
    coreir
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/coreir/include
)
