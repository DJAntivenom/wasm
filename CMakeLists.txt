# wasm project definition
cmake_minimum_required(VERSION 3.31)
project("wasm" VERSION 0.1.0 LANGUAGES C CXX)

# include helpers
include("cmake/AddCompileOption.cmake")

# add executable. This is the core of the project
set(WASM_TARGET_NAME "${PROJECT_NAME}")
add_executable(${WASM_TARGET_NAME})

# setup project
add_subdirectory("src")

# add tests
option(WASM_COMPILE_TESTS "Whether tests should be built" ON)
if(WASM_COMPILE_TESTS)
    include(CTest)
    add_subdirectory("test")
endif()

# set compile features
target_compile_features(${WASM_TARGET_NAME} PUBLIC cxx_std_17)
set_target_properties(${WASM_TARGET_NAME} PROPERTIES C_EXTENSIONS OFF CXX_EXTENSIONS OFF)
enable_supported_compiler_flag(${WASM_TARGET_NAME} FLAGS "-Wall" "-Wextra")

# Force colored output for the ninja generator
if(CMAKE_GENERATOR STREQUAL "Ninja")
  enable_supported_compiler_flag(${WASM_TARGET_NAME}
                                 PUBLIC
                                 FLAGS "-fcolor-diagnostics" "-fdiagnostics-color=always"
  )
  enable_supported_compiler_flag(${WASM_TARGET_NAME}
                                 PUBLIC CHECK_C_FLAGS
                                 FLAGS "-fcolor-diagnostics" "-fdiagnostics-color=always"
  )
endif()
