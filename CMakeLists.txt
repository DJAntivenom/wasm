# wasm project definition
cmake_minimum_required(VERSION 3.31)
project("wasm" VERSION 0.1.0 LANGUAGES C CXX)

# include helpers
include("cmake/AddCompileOption.cmake")

# add executable. This is the core of the project
set(WASM_TARGET_NAME "${PROJECT_NAME}")
add_library(wasm_library)
add_executable(${WASM_TARGET_NAME})
target_link_libraries(${WASM_TARGET_NAME} PUBLIC wasm_library)

# setup project
add_subdirectory("src")

# add tests
option(WASM_COMPILE_TESTS "Whether tests should be built" ON)
if(WASM_COMPILE_TESTS)
    include(CTest)
    add_subdirectory("test")
endif()

# set compile features
target_compile_features(wasm_library PUBLIC cxx_std_23)
set_target_properties(wasm_library PROPERTIES C_EXTENSIONS OFF CXX_EXTENSIONS OFF)
enable_supported_compiler_flag(wasm_library FLAGS "-Wall" "-Wextra")

# Force colored output for the ninja generator
if(CMAKE_GENERATOR STREQUAL "Ninja")
  enable_supported_compiler_flag(wasm_library
                                 PUBLIC
                                 FLAGS "-fcolor-diagnostics" "-fdiagnostics-color=always"
  )
endif()
