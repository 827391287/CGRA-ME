add_library(cxxopts INTERFACE)

target_include_directories(cxxopts
    SYSTEM
    INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)
