add_executable(cgrame cgrame.cpp)
target_link_libraries(cgrame cgra-me cxxopts mINI)

set(default_config_file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/mapper_config.ini)

add_custom_command(
	OUTPUT ${default_config_file}
	COMMAND cgrame "--gen-config-file=${default_config_file}"
	DEPENDS cgrame
	COMMENT "Generating default mapper config ${default_config_file}"
)

add_custom_target(
	default-configs
	ALL
	DEPENDS ${default_config_file}
)

add_executable(cgrame-tests
	cgrame_tests.cpp
)

target_link_libraries(cgrame-tests
	cgra-me
)
