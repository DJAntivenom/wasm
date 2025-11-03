enable_testing()

# Add executable target for the test
add_executable(test_01
    test_01.cpp
)

target_link_libraries(test_01 PUBLIC wasm_library)
# set compile features
target_compile_features(test_01 PUBLIC cxx_std_23)
set_target_properties(test_01 PROPERTIES C_EXTENSIONS OFF CXX_EXTENSIONS OFF)
enable_supported_compiler_flag(test_01 FLAGS "-Wall" "-Wextra")

# Force colored output for the ninja generator
if(CMAKE_GENERATOR STREQUAL "Ninja")
  enable_supported_compiler_flag(test_01
                                 PUBLIC
                                 FLAGS "-fcolor-diagnostics" "-fdiagnostics-color=always"
  )
endif()

# Register the test with CTest
add_test(NAME Test01 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" COMMAND test_01)
